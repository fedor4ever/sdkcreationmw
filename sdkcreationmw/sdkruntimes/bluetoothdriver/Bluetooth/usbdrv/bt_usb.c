/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
 *
*/


#include "bt_usb.h"

typedef NTSTATUS (*BtSendProc)(BtDevice* bt, const void * data, int len);

/**
 * Synchronously sends an HCI command packet.
 */
STATIC NTSTATUS BT_SendHciCommand(BtDevice* bt, const void * data, int len)
{
    struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST request;
    URB* urb = (URB*)&request;
    BT_TRACE("HCI Command sent:");
    BT_DUMP(data,len);

    /* build the request */
    UsbBuildVendorRequest(urb,URB_FUNCTION_CLASS_DEVICE,sizeof(request), 
        USBD_TRANSFER_DIRECTION_OUT,0,0,0,BT_HCI_COMMAND_ENDPOINT,(void*)data,
        NULL,len,NULL);

    /* submit the URB and wait for completion */
    //BT_SelectInterface(bt, BT_HCI_COMMAND_INTERFACE);
    return BT_CallUSBD(bt,NULL,urb);
}

/**
 * Synchronously sends ACL data packet.
 */
STATIC NTSTATUS BT_SendACL(BtDevice* bt, const void * data, int len)
{
    struct _URB_BULK_OR_INTERRUPT_TRANSFER request;
    URB* urb = (URB*)&request;
    BT_TRACE("ACL data sent:");
    BT_DUMP(data,len);

    UsbBuildInterruptOrBulkTransferRequest(urb, sizeof(request),
        bt->aclPipeOut->PipeHandle, (void*)data, NULL, len,
        USBD_TRANSFER_DIRECTION_OUT, NULL);
    
    /* submit the URB and wait for completion */
    //BT_SelectInterface(bt, BT_ACL_DATA_INTERFACE);
    return BT_CallUSBD(bt,NULL,urb);
}

/**
 * Synchronously sends SCO data packet.
 */
STATIC NTSTATUS BT_SendSCO(BtDevice* bt, const void * data, int len)
{
    ASSMSG("SCO not implemented");
    BT_DUMP(data,len);
    //BT_SelectInterface(bt, BT_SCO_DATA_INTERFACE);
    //return BT_Send(bt, BT_HCI_SCO_OUT_ENDPOINT, data, len);
    return STATUS_UNSUCCESSFUL;
}

/**
 * Stores HCI packet in the output buffer. Submits an HCI packet if 
 * possible.
 */
NTSTATUS BT_Write(BtDevice* bt, const void* data, ULONG len)
{
    ULONG fullPacketSize = 0;
    NTSTATUS status = STATUS_SUCCESS;
    BtSendProc sendProc = NULL;
    ASSERT(len > 0);
    if (len > 0) {
        MUTEX_Lock(&bt->common.mutex);
        if (BUFFER_Put(&bt->writeBuffer, data, len, False)) {
            do {
                const UCHAR* pkt = BUFFER_Access(&bt->writeBuffer);
                int size = BUFFER_Size(&bt->writeBuffer);

                BtPacketInfo pinfo;
                BtPacketType ptype = bt->codec->decode(&pinfo, pkt, size);
                if (ptype == BtPacketIncomplete) {
                    /* not enough data even to determine the packet length */
                    break;
                } else if (ptype == BtPacketInvalid) {
                    BT_TRACE("Invalid packet:");
                    BT_DUMP(pkt, size);
                    BUFFER_Clear(&bt->writeBuffer);
                    status = STATUS_INVALID_PARAMETER;
                    break;
                } else {
                    int total = pinfo.header + pinfo.length + pinfo.trailer;
                    if (total > size) {
                        /* incomplete packet, wait for the next write */
                        break;
                    } else {
                        BtSendProc sendProc = NULL;
                        switch (ptype) { 
                        case BtPacketHciCommand: 
                            sendProc = BT_SendHciCommand;
                            break;
                        case BtPacketAclData:
                            sendProc = BT_SendACL;
                            break;
                        case BtPacketScoData:
                            sendProc = BT_SendSCO;
                            break;
                        default:
                            BT_TRACE1("Unknown packet type: %d",ptype);
                            BT_DUMP(pkt, size);
                            break;
                        }
                        if (sendProc) {
                            UCHAR stackBuffer[HCI_PACKET_BUFFER_SIZE];
                            UCHAR* allocatedBuffer = NULL;
                            UCHAR* pbuf = NULL;
                            if (pinfo.length <= sizeof(stackBuffer)) {
                                pbuf = stackBuffer;
                            } else {
                                allocatedBuffer = MEM_Alloc(pinfo.length);
                                pbuf = allocatedBuffer;
                            }
                            if (pbuf) {
                                /* Copy the data before releasing mutex */
                                BUFFER_Skip(&bt->writeBuffer,pinfo.header);
                                BUFFER_Get(&bt->writeBuffer,pbuf,pinfo.length);
                                BUFFER_Skip(&bt->writeBuffer,pinfo.trailer);
                                MUTEX_Unlock(&bt->common.mutex);
                                status = sendProc(bt, pbuf, pinfo.length);
                                MEM_Free(allocatedBuffer);
                                if (NT_SUCCESS(status)) {
                                    MUTEX_Lock(&bt->common.mutex);
                                    continue;
                                } else {
                                    BT_UpdateEventMask(bt, SERIAL_EV_ERR);
                                    MUTEX_Lock(&bt->common.mutex);
                                    status = STATUS_UNSUCCESSFUL;
                                    break;
                                }
                            } else {
                                BUFFER_Skip(&bt->writeBuffer, total);
                                status = STATUS_INSUFFICIENT_RESOURCES;
                                break;
                            }
                        } else {
                            BUFFER_Skip(&bt->writeBuffer, total);
                        }
                    }
                }
            } while (BUFFER_Size(&bt->writeBuffer) > 0);
        } else {
            status = STATUS_INSUFFICIENT_RESOURCES;
        }
        MUTEX_Unlock(&bt->common.mutex);
    }
    BT_UpdateEventMask(bt, SERIAL_EV_TXEMPTY);
    return status;
}

/**
 * Reads one packet (or leftover from the previous packet) into the provided
 * buffer. Returns number of bytes stored in the destination buffer, zero if
 * read buffer is empty. The caller holds the device spinlock.
 */
STATIC ULONG BT_LockedReadChunk(BtDevice* bt, UCHAR* dest, ULONG maxlen)
{
    ULONG bytesStored = 0;
    if (maxlen > 0 && BUFFER_Size(&bt->inputQueue) > 0) {
        /* check for leftover from the previous read */
        ULONG available = BUFFER_Size(&bt->inputQueue);
        if (bt->leftover) {
            bytesStored = bt->leftover;
            if (bytesStored > available) bytesStored = available;
            if (bytesStored > maxlen) bytesStored = maxlen;
            BT_TRACE("Leftover:");
            BT_DUMP(BUFFER_Access(&bt->inputQueue), bytesStored);
            BUFFER_Get(&bt->inputQueue, dest, bytesStored);
            bt->leftover -= bytesStored;
        } else {
            /* There should be at least a packet header in the buffer */
            const UCHAR* data = BUFFER_Access(&bt->inputQueue);
            ULONG datalen = BUFFER_Size(&bt->inputQueue);
            BtPacketInfo pkt;
            BtPacketType type = bt->codec->decode(&pkt, data, datalen);
            if (type == BtPacketInvalid) {
                BT_TRACE1("HCI packet INVALID (%d bytes)",datalen);
                BUFFER_Clear(&bt->inputQueue);
            } else {
                ULONG fullPacketSize = pkt.header + pkt.length + pkt.trailer;
                bytesStored = fullPacketSize;
                if (bytesStored > available) bytesStored = available;
                if (bytesStored > maxlen) bytesStored = maxlen;
                BT_TRACE1("%s received:",pkt.name);
                BT_DUMP(data + pkt.header, bytesStored - pkt.header);
                BUFFER_Get(&bt->inputQueue, dest, bytesStored);
                bt->leftover = fullPacketSize - bytesStored;
            }
        }
    }
    return bytesStored;
}

/**
 * Read some data from the input queue into the provided buffer, up to maximum
 * length. Returns number of bytes stored in the destination buffer, zero if
 * read buffer is empty. The caller holds the device spinlock.
 */
ULONG BT_LockedRead(BtDevice* bt, UCHAR* dest, ULONG maxlen)
{
    ULONG total = 0;
    ULONG chunk;
    BT_FillInputQueue(bt);
    while ((chunk = BT_LockedReadChunk(bt, dest+total, maxlen-total)) > 0) {
        BT_FillInputQueue(bt);
        total += chunk;
    }
    return total;
}

/**
 * Stores HCI packet in the input buffer. Completes a read if possible
 */
STATIC BOOLEAN BT_IndicatePacket(BtDevice* bt, BtPacketType type, 
                                 const UCHAR* data, ULONG len)
{
    MUTEX_Lock(&bt->common.mutex);
    if (!QUEUE_IsEmpty(&bt->common.fileObjects) &&
        bt->codec->encode(&bt->readBuffer, type, data, len)) {

        /* look for request to complete */
        ULONG bytesRead;
        IRP* readIrpDone = NULL;
        void* dest;
        BT_FillInputQueue(bt);
        if (BUFFER_Size(&bt->inputQueue) > 0 && bt->readIrp) {
            IO_STACK_LOCATION* sp = IoGetCurrentIrpStackLocation(bt->readIrp);
            ULONG length = sp->Parameters.Read.Length;
            dest = (bt->readIrp->MdlAddress ? 
                MmGetSystemAddressForMdl(bt->readIrp->MdlAddress) :
                bt->readIrp->AssociatedIrp.SystemBuffer);
            bytesRead = BT_LockedRead(bt, dest, length);
            if (bytesRead > 0) {
                readIrpDone = bt->readIrp;
                bt->readIrp = NULL;
                readIrpDone->IoStatus.Status = STATUS_SUCCESS;
                readIrpDone->IoStatus.Information = bytesRead;
            }
        }
        MUTEX_Unlock(&bt->common.mutex);
        BT_UpdateEventMask(bt, 0);
        if (readIrpDone) {
            BT_TRACE2("read IRP %08X done, %d bytes",readIrpDone,bytesRead);
            BT_DUMP(dest, bytesRead);
            IoCompleteRequest(readIrpDone, IO_SERIAL_INCREMENT);
        }
        return TRUE;
    }
    MUTEX_Unlock(&bt->common.mutex);
    return FALSE;
}

/**
 * Retrieves the port status using IOCTL_INTERNAL_USB_GET_PORT_STATUS
 */
STATIC BOOLEAN BT_GetPortStatus(BtDevice* bt, ULONG* PortStatus)
{
    IRP* irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;

    *PortStatus = 0;
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_GET_PORT_STATUS,
        bt->TopOfStackDeviceObject, NULL, 0, NULL, 0, TRUE, &event, 
        &ioStatus);

    ASSERT(irp);
    if (irp) {
        NTSTATUS status;
        IO_STACK_LOCATION * nextStack = IoGetNextIrpStackLocation(irp);
        nextStack->Parameters.Others.Argument1 = PortStatus;
        status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
        if (status == STATUS_PENDING) {
           KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = ioStatus.Status;
        }

        if (status == STATUS_SUCCESS) {
            return TRUE;
        }

        BT_TRACE1("IOCTL_INTERNAL_USB_GET_PORT_STATUS failed, err %08X",
            status);
    }

    return FALSE;
}

/**
 * This routine sends an IOCTL_INTERNAL_USB_RESET_PORT synchronously 
 * down the stack.
 */
STATIC NTSTATUS BT_ResetPort(BtDevice* bt)
{
    IRP* irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

    BT_TRACE("resettings port");
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_RESET_PORT,
            bt->TopOfStackDeviceObject, NULL, 0, NULL, 0, TRUE, &event, 
            &ioStatus);

    ASSERT(irp);
    if (irp) {
        status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = ioStatus.Status;
        }

        if (!NT_SUCCESS(status)) {
            BT_TRACE1("IOCTL_INTERNAL_USB_RESET_PORT failed, err %08X",
                status);
        }
    }

    return status;
}

/**
 * Resets the data pipes
 */
STATIC BOOLEAN BT_ResetPipe(BtDevice* bt, const USBD_PIPE_INFORMATION* pipe)
{
    if (pipe) {
        struct _URB_PIPE_REQUEST reset;
        BT_TRACE1("aborting pipe %02X",pipe->EndpointAddress);

        RtlZeroMemory(&reset, sizeof(reset)); 
        reset.Hdr.Function = URB_FUNCTION_ABORT_PIPE;
        reset.Hdr.Length = sizeof(reset); 
        reset.PipeHandle = pipe->PipeHandle;
        if (BT_TimeCallUSBD(bt,NULL,((URB*)&reset),-1) == STATUS_SUCCESS) {

            /* NOTE: some host controller drivers damage reset.PipeHandle */
            BT_TRACE("resettings pipe");
            reset.Hdr.Function = URB_FUNCTION_RESET_PIPE;
            reset.PipeHandle = pipe->PipeHandle;
            if (BT_TimeCallUSBD(bt,NULL,((URB*)&reset),-1) == STATUS_SUCCESS) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * Resets the data pipes
 */
STATIC void BT_ResetPipes(BtDevice* bt)
{
    BT_ResetPipe(bt, bt->eventPipe);
    BT_ResetPipe(bt, bt->aclPipeIn);
    BT_ResetPipe(bt, bt->aclPipeOut);
    BT_ResetPipe(bt, bt->scoPipeIn);
    BT_ResetPipe(bt, bt->scoPipeOut);
}

/**
 * Resets the port or the pipe depending on the port status.
 */
void BT_Reset(BtDevice* bt)
{
    ULONG status;
    if (BT_GetPortStatus(bt, &status)) {
        if (status & USBD_PORT_CONNECTED) {
            if (status & USBD_PORT_ENABLED) {
                BT_ResetPipes(bt);
            }
            BT_ResetPort(bt);
        } else {
            BT_TRACE1("port not connected! status = %08X",status);
        }
    }
}

/**
 * Completion routine for the HCI event IRPs.
 */
STATIC NTSTATUS BT_UsbEventComplete(DEVICE_OBJECT* dev, IRP* irp, PVOID ctx)
{
    ULONG len = 0;
    BtTransfer* buffer = ctx;
    URB* urb = buffer->urb;
    BtDevice* bt = buffer->bt;
    UCHAR* data = urb->UrbBulkOrInterruptTransfer.TransferBuffer;
    NTSTATUS status = irp->IoStatus.Status;
    BOOLEAN receivedDataOK = FALSE;
    IO_STACK_LOCATION* irpSp;
    
    /* sanity check */
    ASSERT(bt->common.deviceObject == dev || !dev);
    ASSERT(buffer == bt->eventTx);
    ASSERT(data == buffer->data);

    /* check if we have received expected amount of data */
    if (status == STATUS_SUCCESS) {
        ASSERT(urb->UrbHeader.Status == USBD_STATUS_SUCCESS);
        len = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
        BT_TRACE2("event IRP %08X complete (%d bytes)", irp, len);
        if (len >= 2) {
            ULONG dataLen = data[1];
            ULONG packetLen = dataLen + 2;
            if (packetLen <= len) {
                len = packetLen;
                receivedDataOK = TRUE;
            } else {
                ASSMSG("SHORT EVENT PACKET!");
            }
        } else {
            ASSMSG("SHORT EVENT PACKET!");
        }
    } else {
        BT_TRACE2("HCI event request failed, error %08X (%08X)",status,
            urb->UrbHeader.Status);
        MUTEX_Lock(&bt->common.mutex);
        if (bt->eventIrp == irp) {
            bt->eventIrp = NULL;
        }
        buffer->urb = NULL;
        MUTEX_Unlock(&bt->common.mutex);
        MEM_Free(urb);
        IoFreeIrp(irp);
        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    /* check if the IRP has completed successfully */
    if (receivedDataOK) {
        BT_IndicatePacket(bt, BtPacketHciEvent, data, len);
    }

    /* recycle the Irp */
    IoInitializeIrp(irp, irp->Size, irp->StackCount);

    /* reinitialize the URB */
    RtlZeroMemory(urb,sizeof(urb->UrbBulkOrInterruptTransfer));
    UsbBuildInterruptOrBulkTransferRequest(urb, 
        sizeof(urb->UrbBulkOrInterruptTransfer),
        bt->eventPipe->PipeHandle, data, NULL, HCI_PACKET_BUFFER_SIZE,
        USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
        NULL);

    /* fill IRP stack location */
    irpSp = IoGetNextIrpStackLocation(irp);
    irpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    irpSp->Parameters.Others.Argument1 = urb;
    irpSp->Parameters.DeviceIoControl.IoControlCode = 
        IOCTL_INTERNAL_USB_SUBMIT_URB;

    /* fire next transfer */
    IoSetCompletionRoutine(irp,BT_UsbEventComplete,ctx,TRUE,TRUE,TRUE);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    if (!NT_SUCCESS(status)) {
        BT_TRACE1("failed to resubmit event request, error %08X",status);
        MUTEX_Lock(&bt->common.mutex);
        if (bt->eventIrp == irp) {
            bt->eventIrp = NULL;
        }
        buffer->urb = NULL;
        MUTEX_Unlock(&bt->common.mutex);
        MEM_Free(urb);
        IoFreeIrp(irp);
    }

    /* note that we must always return STATUS_MORE_PROCESSING_REQUIRED 
     * because we deallocate the IRP */
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * Creates and submits event request
 */
BOOLEAN BT_SubmitEventRequest(BtDevice* bt)
{
    USHORT urbSize = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    URB* urb = MEM_Alloc(urbSize);
    if (urb) {
        IRP* irp = BT_CreateUsbSubmitRequest(bt->TopOfStackDeviceObject,urb);
        if (irp) {
            NTSTATUS status;

            /* initialize the URB */
            RtlZeroMemory(urb, urbSize);
            UsbBuildInterruptOrBulkTransferRequest(urb,
                sizeof(urb->UrbBulkOrInterruptTransfer),
                bt->eventPipe->PipeHandle, bt->eventTx->data,
                NULL, HCI_PACKET_BUFFER_SIZE,
                USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK, 
                NULL);
    
            BT_TRACE3("allocated event IRP %08X, URB %08X, buf %08X", irp,
                urb, urb->UrbBulkOrInterruptTransfer.TransferBuffer);

            /* do we need any synchronization here? */
            ASSERT(!bt->eventIrp);
            bt->eventIrp = irp;
            bt->eventTx->urb = urb;
            IoSetCompletionRoutine(irp,BT_UsbEventComplete,bt->eventTx,
                TRUE,TRUE,TRUE);
            status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
            if (NT_SUCCESS(status)) {
                return TRUE;
            }

            BT_TRACE1("failed to submit event request, error %08X",status);
            ASSERT(!bt->eventIrp);
        }
        MEM_Free(urb);
    }
    return FALSE;
}

/**
 * Completion routine for the ACL data IRPs.
 */
STATIC NTSTATUS BT_UsbAclComplete(DEVICE_OBJECT* dev, IRP* irp, PVOID ctx)
{
    ULONG len = 0;
    BtTransfer* buffer = ctx;
    URB* urb = buffer->urb;
    BtDevice* bt = buffer->bt;
    UCHAR* data = urb->UrbBulkOrInterruptTransfer.TransferBuffer;
    NTSTATUS status = irp->IoStatus.Status;
    BOOLEAN receivedDataOK = FALSE;
    IO_STACK_LOCATION* irpSp;
    
    /* sanity check */
    ASSERT(bt->common.deviceObject == dev || !dev);
    ASSERT(buffer == bt->aclTx);
    ASSERT(data == buffer->data);

    /* check if we have received expected amount of data */
    if (status == STATUS_SUCCESS) {
        ASSERT(urb->UrbHeader.Status == USBD_STATUS_SUCCESS);
        len = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
        BT_TRACE2("ACL IRP %08X complete (%d bytes)", irp, len);
        if (len >= 4) {
            ULONG dataLen = data[2] + (data[3] << 8);
            ULONG packetLen = dataLen + 4;
            if (packetLen <= len) {
                len = packetLen;
                receivedDataOK = TRUE;
            } else {
                ASSMSG("SHORT ACL PACKET!");
            }
        } else {
            ASSMSG("SHORT ACL PACKET!");
        }
    } else {
        BT_TRACE2("ACL request failed, error %08X (%08X)",status,
            urb->UrbHeader.Status);
        MUTEX_Lock(&bt->common.mutex);
        if (bt->aclIrp == irp) {
            bt->aclIrp = NULL;
        }
        buffer->urb = NULL;
        MUTEX_Unlock(&bt->common.mutex);
        MEM_Free(urb);
        IoFreeIrp(irp);
        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    /* check if the IRP has completed successfully */
    if (receivedDataOK) {
        BT_IndicatePacket(bt, BtPacketAclData, data, len);
    }

    /* recycle the Irp */
    IoInitializeIrp(irp, irp->Size, irp->StackCount);

    /* reinitialize the URB */
    RtlZeroMemory(urb,sizeof(urb->UrbBulkOrInterruptTransfer));
    UsbBuildInterruptOrBulkTransferRequest(urb, 
        sizeof(urb->UrbBulkOrInterruptTransfer),
        bt->aclPipeIn->PipeHandle, data, NULL, HCI_PACKET_BUFFER_SIZE,
        USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
        NULL);

    /* fill IRP stack location */
    irpSp = IoGetNextIrpStackLocation(irp);
    irpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    irpSp->Parameters.Others.Argument1 = urb;
    irpSp->Parameters.DeviceIoControl.IoControlCode = 
        IOCTL_INTERNAL_USB_SUBMIT_URB;

    /* fire next transfer */
    IoSetCompletionRoutine(irp,BT_UsbAclComplete,ctx,TRUE,TRUE,TRUE);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    if (!NT_SUCCESS(status)) {
        BT_TRACE1("failed to resubmit ACL request, error %08X",status);
        MUTEX_Lock(&bt->common.mutex);
        if (bt->aclIrp == irp) {
            bt->aclIrp = NULL;
        }
        buffer->urb = NULL;
        MUTEX_Unlock(&bt->common.mutex);
        MEM_Free(urb);
        IoFreeIrp(irp);
    }

    /* note that we must always return STATUS_MORE_PROCESSING_REQUIRED 
     * because we deallocate the IRP */
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * Creates and submits ACL data request
 */
BOOLEAN BT_SubmitAclRequest(BtDevice* bt)
{
    USHORT urbSize = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    URB* urb = MEM_Alloc(urbSize);
    if (urb) {
        IRP* irp = BT_CreateUsbSubmitRequest(bt->TopOfStackDeviceObject,urb);
        if (irp) {
            NTSTATUS status;

            /* initialize the URB */
            RtlZeroMemory(urb, urbSize);
            UsbBuildInterruptOrBulkTransferRequest(urb,
                sizeof(urb->UrbBulkOrInterruptTransfer),
                bt->aclPipeIn->PipeHandle, bt->aclTx->data,
                NULL, HCI_PACKET_BUFFER_SIZE,
                USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK, 
                NULL);
    
            BT_TRACE3("allocated ACL IRP %08X, URB %08X, buf %08X", irp,
                urb, urb->UrbBulkOrInterruptTransfer.TransferBuffer);

            /* do we need any synchronization here? */
            ASSERT(!bt->aclIrp);
            bt->aclIrp = irp;
            bt->aclTx->urb = urb;
            IoSetCompletionRoutine(irp,BT_UsbAclComplete,bt->aclTx,
                TRUE,TRUE,TRUE);
            status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
            if (NT_SUCCESS(status)) {
                return TRUE;
            }

            BT_TRACE1("failed to submit ACL request, error %08X",status);
            ASSERT(!bt->aclIrp);
        }
        MEM_Free(urb);
    }
    return FALSE;
}

/**
 * Completion routine for the SCO data IRPs.
 */
STATIC NTSTATUS BT_UsbScoComplete(DEVICE_OBJECT* dev, IRP* irp, PVOID ctx)
{
    ULONG len = 0;
    BtTransfer* buffer = ctx;
    URB* urb = buffer->urb;
    BtDevice* bt = buffer->bt;
    UCHAR* data = urb->UrbIsochronousTransfer.TransferBuffer;
    NTSTATUS status = irp->IoStatus.Status;
    BOOLEAN receivedDataOK = FALSE;
    IO_STACK_LOCATION* irpSp;
    
    /* sanity check */
    ASSERT(bt->common.deviceObject == dev || !dev);
    ASSERT(buffer == bt->scoTx);
    ASSERT(data == buffer->data);

    /* check if we have received expected amount of data */
    if (status == STATUS_SUCCESS) {
        ASSERT(urb->UrbHeader.Status == USBD_STATUS_SUCCESS);
        len = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
        BT_TRACE2("SCO IRP %08X complete (%d bytes)", irp, len);
        if (len >= 3) {
            ULONG dataLen = data[2];
            ULONG packetLen = dataLen + 3;
            if (packetLen <= len) {
                len = packetLen;
                receivedDataOK = TRUE;
            } else {
                ASSMSG("SHORT SCO PACKET!");
            }
        } else {
            ASSMSG("SHORT SCO PACKET!");
        }
    } else {
        BT_TRACE2("SCO request failed, error %08X (%08X)",status,
            urb->UrbHeader.Status);
        MUTEX_Lock(&bt->common.mutex);
        if (bt->scoIrp == irp) {
            bt->scoIrp = NULL;
        }
        buffer->urb = NULL;
        MUTEX_Unlock(&bt->common.mutex);
        MEM_Free(urb);
        IoFreeIrp(irp);
        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    /* check if the IRP has completed successfully */
    if (receivedDataOK) {
        BT_IndicatePacket(bt, BtPacketScoData, data, len);
    }

    /* recycle the Irp */
    IoInitializeIrp(irp, irp->Size, irp->StackCount);

    /* fill IRP stack location */
    irpSp = IoGetNextIrpStackLocation(irp);
    irpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    irpSp->Parameters.Others.Argument1 = urb;
    irpSp->Parameters.DeviceIoControl.IoControlCode = 
        IOCTL_INTERNAL_USB_SUBMIT_URB;

    /* fire next transfer */
    IoSetCompletionRoutine(irp,BT_UsbScoComplete,ctx,TRUE,TRUE,TRUE);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    if (!NT_SUCCESS(status)) {
        BT_TRACE1("failed to resubmit SCO request, error %08X",status);
        MUTEX_Lock(&bt->common.mutex);
        if (bt->scoIrp == irp) {
            bt->scoIrp = NULL;
        }
        buffer->urb = NULL;
        MUTEX_Unlock(&bt->common.mutex);
        MEM_Free(urb);
        IoFreeIrp(irp);
    }

    /* note that we must always return STATUS_MORE_PROCESSING_REQUIRED 
     * because we deallocate the IRP */
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * Creates and submits SCO data request
 */
BOOLEAN BT_SubmitScoRequest(BtDevice* bt)
{
    ULONG numPackets;
    USHORT urbSize;
    URB* urb;
    ULONG packetSize = bt->scoPipeIn->MaximumPacketSize;
    if (!packetSize) return FALSE;

    numPackets = HCI_PACKET_BUFFER_SIZE/packetSize;
    urbSize = GET_ISO_URB_SIZE(numPackets);
    urb = MEM_Alloc(urbSize);

    ASSERT(HCI_PACKET_BUFFER_SIZE >= packetSize);
    if (urb) {
        IRP* irp = BT_CreateUsbSubmitRequest(bt->TopOfStackDeviceObject,urb);
        if (irp) {
            ULONG i;
            NTSTATUS status;

            /* initialize the URB */
            RtlZeroMemory(urb, urbSize);
            urb->UrbHeader.Function =  URB_FUNCTION_ISOCH_TRANSFER;
            urb->UrbHeader.Length = urbSize;
            urb->UrbIsochronousTransfer.PipeHandle = bt->scoPipeIn->PipeHandle;
            urb->UrbIsochronousTransfer.TransferBuffer = bt->scoTx->data;
            urb->UrbIsochronousTransfer.TransferBufferLength = numPackets*packetSize;
            urb->UrbIsochronousTransfer.TransferFlags = USBD_TRANSFER_DIRECTION_IN;
            urb->UrbIsochronousTransfer.NumberOfPackets = numPackets;
            for (i=0; i<numPackets; i++) {
                urb->UrbIsochronousTransfer.IsoPacket[i].Offset = i*packetSize;
            }
    
            BT_TRACE3("allocated SCO IRP %08X, URB %08X, buf %08X", irp,
                urb, urb->UrbIsochronousTransfer.TransferBuffer);

            /* do we need any synchronization here? */
            ASSERT(!bt->scoIrp);
            bt->scoIrp = irp;
            bt->scoTx->urb = urb;
            IoSetCompletionRoutine(irp,BT_UsbScoComplete,bt->scoTx,
                TRUE,TRUE,TRUE);
            status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
            if (NT_SUCCESS(status)) {
                return TRUE;
            }

            BT_TRACE1("failed to submit SCO request, error %08X",status);
            ASSERT(!bt->scoIrp);
        }
        MEM_Free(urb);
    }
    return FALSE;
}
