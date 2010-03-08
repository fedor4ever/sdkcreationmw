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
* Description:  Plug and Play events
 *
*/


#include "bt_usb.h"

/**
 * This routine is a completion routine. In this routine we set an event.
 * Since the completion routine returns STATUS_MORE_PROCESSING_REQUIRED, 
 * the Irps, which set this routine as the completion routine, should be 
 * marked pending.
 */
STATIC NTSTATUS BT_CompletionEvent(PDEVICE_OBJECT dev, PIRP irp, PVOID ctx)
{
    EVENT_Set(ctx);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS BT_SelectInterface(BtDevice * bt, UCHAR interfaceNumber)
{
    NTSTATUS status = STATUS_SUCCESS;
    if (bt->currentInterface != interfaceNumber) {
        struct _URB_SELECT_INTERFACE selectUnterfaceUrb;
        URB* urb = (URB*)&selectUnterfaceUrb;
        NTSTATUS status;

        BT_TRACE1("selecting interface #%u",interfaceNumber);
        UsbBuildSelectInterfaceRequest(urb, sizeof(selectUnterfaceUrb),
            bt->configurationHandle, interfaceNumber, 
            bt->interfaceInfo[interfaceNumber]->AlternateSetting);
        status = BT_CallUSBD(bt, NULL, urb);
        if (NT_SUCCESS(status)) {
            bt->currentInterface = interfaceNumber;
        }
    }
    return status;
}

/**
 * Retrieves a string descriptor from the device, given the string id.
 * The caller deallocates the descriptor. Returns NULL ib failure or if
 * string id is zero.
 */
STATIC USB_STRING_DESCRIPTOR * BT_GetUsbString(BtDevice * bt, UCHAR iString) 
{
    USB_STRING_DESCRIPTOR * sd = NULL;
    if (iString) {
        URB * urb = MEM_Alloc(sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
        if (urb) {
            NTSTATUS status;
            USB_STRING_DESCRIPTOR tmp;
            UsbBuildGetDescriptorRequest(urb, 
                sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                USB_STRING_DESCRIPTOR_TYPE, iString, 0, &tmp, NULL,
                sizeof(tmp), NULL);

            /* Find the size of the string */
            status = BT_CallUSBD(bt, NULL, urb);
            if (NT_SUCCESS(status)) {
                sd = MEM_Alloc(tmp.bLength + /* one more */ sizeof(WCHAR));
                if (sd) {
                    /* Zero last WCHAR to NULL terminate the string */
                    RtlZeroMemory(((BYTE*)sd)+tmp.bLength, sizeof(WCHAR));
                    UsbBuildGetDescriptorRequest(urb,
                        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                        USB_STRING_DESCRIPTOR_TYPE, iString, 0, sd, NULL,
                        tmp.bLength, NULL);

                    /* Call the device to get the actual string */
                    status = BT_CallUSBD(bt, NULL, urb);
                    if (NT_SUCCESS(status)) {
                        BT_TRACE2(" String #%u: %S",(UINT32)iString, sd->bString);
                    }
                    MEM_Free(sd);
                }
            }
            MEM_Free(urb);
        }
    }
    return sd;
}

/**
 * Collects pipe information from the specified interface
 */
STATIC void BT_CollectPipeInformation(BtDevice * bt, ULONG index)
{
    ULONG i;
    USBD_INTERFACE_INFORMATION * interfaceInfo = bt->interfaceInfo[index];

    BT_TRACE1("checking interface #%u",interfaceInfo->InterfaceNumber);
    BT_TRACE("================");
    BT_TRACE1("  NumberOfPipes: %lu", interfaceInfo->NumberOfPipes);
    BT_TRACE1("  Length: %hu", interfaceInfo->Length);
    BT_TRACE1("  Alt Setting: %hu", interfaceInfo->AlternateSetting);
    BT_TRACE1("  Interface: %u", interfaceInfo->InterfaceNumber);
    BT_TRACE3("  Class, subclass, protocol: %02X,%02X,%02X",
        interfaceInfo->Class,
        interfaceInfo->SubClass,
        interfaceInfo->Protocol);

    for (i=0; i<interfaceInfo->NumberOfPipes; i++) {
        USBD_PIPE_INFORMATION * pipe = interfaceInfo->Pipes+i;
        BT_TRACE("---------------");
        BT_TRACE1("  PipeType: %d",pipe->PipeType);
        BT_TRACE1("  EndpointAddress: %02X", pipe->EndpointAddress);
        BT_TRACE1("  MaxPacketSize: %d", pipe->MaximumPacketSize);
        BT_TRACE1("  Interval: %u", (unsigned int)pipe->Interval);
        BT_TRACE1("  Handle: %08X", pipe->PipeHandle);
        BT_TRACE1("  MaxTxSize: %08lX", pipe->MaximumTransferSize);
        switch (pipe->EndpointAddress) {
        case BT_HCI_EVENT_ENDPOINT:
            ASSERT(pipe->PipeType == UsbdPipeTypeInterrupt);
            if (pipe->PipeType == UsbdPipeTypeInterrupt) {
                BT_TRACE("*** Event pipe ***");
                ASSERT(!bt->eventPipe);
                bt->eventPipe = pipe;
            }
            break;

        case BT_HCI_ACL_OUT_ENDPOINT:
            ASSERT(pipe->PipeType == UsbdPipeTypeBulk);
            if (pipe->PipeType == UsbdPipeTypeBulk) {
                BT_TRACE("*** ACL pipe (out) ***");
                ASSERT(!bt->aclPipeOut);
                bt->aclPipeOut = pipe;
            }
            break;

        case BT_HCI_ACL_IN_ENDPOINT:
            ASSERT(pipe->PipeType == UsbdPipeTypeBulk);
            if (pipe->PipeType == UsbdPipeTypeBulk) {
                BT_TRACE("*** ACL pipe (in) ***");
                ASSERT(!bt->aclPipeIn);
                bt->aclPipeIn = pipe;
            }
            break;

        case BT_HCI_SCO_OUT_ENDPOINT:
            ASSERT(pipe->PipeType == UsbdPipeTypeIsochronous);
            if (pipe->PipeType == UsbdPipeTypeIsochronous) {
                BT_TRACE("*** SCO pipe (out) ***");
                ASSERT(!bt->scoPipeOut);
                bt->scoPipeOut = pipe;
            }
            break;

        case BT_HCI_SCO_IN_ENDPOINT:
            ASSERT(pipe->PipeType == UsbdPipeTypeIsochronous);
            if (pipe->PipeType == UsbdPipeTypeIsochronous) {
                BT_TRACE("*** SCO pipe (in) ***");
                ASSERT(!bt->scoPipeIn);
                bt->scoPipeIn = pipe;
            }
            break;

        default:
            break;
        }
    }
    BT_TRACE("================");
}

/**
 * Selects the configuration
 */
STATIC NTSTATUS BT_SelectConfiguration(BtDevice * bt, 
    USB_CONFIGURATION_DESCRIPTOR * config)
{
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    PURB urb;
    ULONG interfaceIndex;
    USBD_INTERFACE_LIST_ENTRY* interfaceList = MEM_Alloc(
        sizeof(USBD_INTERFACE_LIST_ENTRY) * (config->bNumInterfaces + 1));

    if (!interfaceList) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* initialize the interface list */
    for (interfaceIndex = 0;
         interfaceIndex < config->bNumInterfaces;
         interfaceIndex++) {

        USB_INTERFACE_DESCRIPTOR* idesc = USBD_ParseConfigurationDescriptorEx(
            config, config, interfaceIndex, 0, -1, -1, -1);
        ASSERT(idesc);
        if (idesc) {
            USBD_INTERFACE_LIST_ENTRY* entry = interfaceList + interfaceIndex;
            ASSERT(idesc->bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE);

            entry->InterfaceDescriptor = idesc;
            entry->Interface = NULL;
        } else {
            BT_TRACE1("missing interface #%d",interfaceIndex);
            return STATUS_DEVICE_CONFIGURATION_ERROR;
        }
    }

    interfaceList[config->bNumInterfaces].InterfaceDescriptor = NULL;
    interfaceList[config->bNumInterfaces].Interface = NULL;
    urb = USBD_CreateConfigurationRequestEx(config, interfaceList);
    ASSERT(urb);
    if (urb) {
        for (interfaceIndex = 0; 
             interfaceIndex < config->bNumInterfaces;
             interfaceIndex++) {
            ULONG i;
            USBD_INTERFACE_INFORMATION* interfaceInfo = 
                interfaceList[interfaceIndex].Interface;
            for (i=0; i<interfaceInfo->NumberOfPipes; i++) {
                interfaceInfo->Pipes[i].MaximumTransferSize = 
                    USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
            }
        }

        status = BT_CallUSBD(bt, NULL, urb);
        if (NT_SUCCESS(status)) {

            /* store configuration handle */
            ASSERT(urb->UrbSelectConfiguration.ConfigurationHandle);
            bt->configurationHandle = 
                urb->UrbSelectConfiguration.ConfigurationHandle;

            /* collect pipe information */
            status = STATUS_SUCCESS;
            for (interfaceIndex = 0; 
                 interfaceIndex < BT_DEVICE_INTERFACE_COUNT && 
                     NT_SUCCESS(status);
                 interfaceIndex++) {
                USBD_INTERFACE_INFORMATION* interfaceInfo = 
                    interfaceList[interfaceIndex].Interface;

                bt->interfaceInfo[interfaceIndex] = MEM_Alloc(interfaceInfo->Length);
                if (bt->interfaceInfo[interfaceIndex]) {
                    RtlCopyMemory(bt->interfaceInfo[interfaceIndex],
                        interfaceInfo, interfaceInfo->Length);
                    BT_CollectPipeInformation(bt, interfaceIndex);
                } else {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                }
            }

            if (NT_SUCCESS(status) && bt->eventPipe && 
                bt->aclPipeIn && bt->aclPipeOut 
                && bt->scoPipeIn && bt->scoPipeOut) {
                BT_TRACE("configuration looks good");
            } else {
                BT_TRACE("configuration is not recognized");
                status = STATUS_DEVICE_CONFIGURATION_ERROR;
            }
        }
        ExFreePool(urb);
    }

    MEM_Free(interfaceList);
    return status;
}

/**
 * Configuring the USB device, step 2.
 */
STATIC NTSTATUS BT_ConfigureDevice2(BtDevice * bt)
{
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    USB_CONFIGURATION_DESCRIPTOR * config = NULL;

    /*
     * Read the first configuration descriptor
     * This requires two steps:
     * 1. Read the fixed sized configuration desciptor (CD)
     * 2. Read the CD with all embedded interface and endpoint descriptors
     */
    URB * urb = MEM_Alloc(sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
    if (urb) {
        USB_CONFIGURATION_DESCRIPTOR tmpDescriptor;
        UsbBuildGetDescriptorRequest(urb, 
            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
            USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, 0, 
            &tmpDescriptor, NULL, sizeof(tmpDescriptor), NULL);

        status = BT_CallUSBD(bt, NULL, urb);
        if (NT_SUCCESS(status)) {
            ULONG size = tmpDescriptor.wTotalLength;
            config = MEM_Alloc(size);
            if (config) {
                UsbBuildGetDescriptorRequest(urb, 
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, 0, 
                    config, NULL, size, NULL);

                status = BT_CallUSBD(bt, NULL, urb);
                if (NT_SUCCESS(status)) {
                    BT_TRACE1("%u interfaces",config->bNumInterfaces);
                } else {
                    MEM_Free(config);
                    config = NULL;
                }
            } else {
                status = STATUS_INSUFFICIENT_RESOURCES;
            }
        }
        MEM_Free(urb);
    }

    if (NT_SUCCESS(status)) {
        /* Must have 2 interfaces */
        if (config->bNumInterfaces >= BT_DEVICE_INTERFACE_COUNT) {
            ASSERT(config);
            ASSERT(config->bDescriptorType == USB_CONFIGURATION_DESCRIPTOR_TYPE);
            bt->UsbConfigurationDescriptor = config;
            status = BT_SelectConfiguration(bt, config);
        } else {
            status = STATUS_DEVICE_CONFIGURATION_ERROR;
        }
    }
    return status;
}

/**
 * Configures the USB device.
 */
STATIC NTSTATUS BT_ConfigureDevice(BtDevice * bt)
{
    /* Read the device descriptor */
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    struct _URB_CONTROL_DESCRIPTOR_REQUEST request;
    URB * urb = (URB*)&request;
    USB_DEVICE_DESCRIPTOR * dd = &bt->DeviceDescriptor;
    UsbBuildGetDescriptorRequest(urb, 
            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
            USB_DEVICE_DESCRIPTOR_TYPE,  0, 0, dd, NULL, sizeof(*dd), NULL);

    status = BT_CallUSBD(bt, NULL, urb);
    if (NT_SUCCESS(status)) {

        BT_TRACE("Device descriptor:");
        BT_TRACE1("  VendorId: %04X", dd->idVendor);
        BT_TRACE1("  ProductId: %04X", dd->idProduct);
        BT_TRACE1("  Class: %02X", dd->bDeviceClass);
        BT_TRACE1("  SubClass: %02X", dd->bDeviceSubClass);
        BT_TRACE1("  Protocol: %02X", dd->bDeviceProtocol);

        /* detect Bluetooth controllers based on device descriptor */
        if (dd->bDeviceClass    == BT_DEVICE_CLASS_WIRELESS_CONTROLLER &&
            dd->bDeviceSubClass == BT_DEVICE_SUBCLASS_RF_CONTROLLER &&
            dd->bDeviceProtocol == BT_DEVICE_PROTOCOL_BLUETOOTH) {

            /* extract strings referenced by the device descriptors */
            bt->strManufacturer = BT_GetUsbString(bt, dd->iManufacturer);
            bt->strProduct      = BT_GetUsbString(bt, dd->iProduct);
            bt->strSerialNumber = BT_GetUsbString(bt, dd->iSerialNumber);

            /* set up the device */
            status = BT_ConfigureDevice2(bt);

        /* and ignore anything else */
        } else {
            BT_TRACE("unsupported device?");
            status = STATUS_DEVICE_CONFIGURATION_ERROR;
        }
    }
    return status;
}

/**
 * This is the dispatch routine for IRP_MN_START_DEVICE
 */
STATIC NTSTATUS BT_HandleStartDevice(BtDevice * bt, IRP * irp)
{
    Event event;
    NTSTATUS status;

    BT_TRACE("HandleStartDevice");

    // We cannot touch the device (send it any non pnp irps) until a
    // start device has been passed down to the lower drivers.
    // first pass the Irp down
    EVENT_Init(&event);
    IoCopyCurrentIrpStackLocationToNext(irp);
    IoSetCompletionRoutine(irp,BT_CompletionEvent,&event,TRUE,TRUE,TRUE);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    if (status == STATUS_PENDING) {
        EVENT_Wait(&event);
        status = irp->IoStatus.Status;
    }
    EVENT_Destroy(&event);
    ASSERT(NT_SUCCESS(status));
    if (!NT_SUCCESS(status)) {
        return status;
    }

    /*
     * Read the device descriptor, configuration descriptor 
     * and select the interface descriptors
     */
    status = BT_ConfigureDevice(bt);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    /*
     * enable the symbolic links for system components to open
     * handles to the device
     */
    status = IoSetDeviceInterfaceState(&bt->InterfaceName, TRUE);
    ASSERT(NT_SUCCESS(status));
    if (!NT_SUCCESS(status)) {
        return status;
    }

    MUTEX_Lock(&bt->common.mutex);
    bt->prevDevState = bt->deviceState;
    bt->deviceState = DeviceWorking;
    MUTEX_Unlock(&bt->common.mutex);

    return status;
}

/**
 * This is the dispatch routine for IRP_MN_START_DEVICE
 */
STATIC NTSTATUS BT_HandleQueryCapabilities(BtDevice * bt, IRP * irp)
{
    Event event;
    NTSTATUS status;
    PIO_STACK_LOCATION   irpStack = IoGetCurrentIrpStackLocation(irp);
    PDEVICE_CAPABILITIES pdc = irpStack->Parameters.DeviceCapabilities.Capabilities;

    BT_TRACE("HandleQueryCapabilities");
    ASSERT(pdc->Version >= 1 && pdc->Size >= sizeof(DEVICE_CAPABILITIES));
    if (pdc->Version < 1 || pdc->Size < sizeof(DEVICE_CAPABILITIES)) {
        return STATUS_UNSUCCESSFUL;
    }

    /* Add in the SurpriseRemovalOK bit before passing it down. */
    pdc->SurpriseRemovalOK = TRUE;
        
    EVENT_Init(&event);
    irp->IoStatus.Status = STATUS_SUCCESS;
    IoCopyCurrentIrpStackLocationToNext(irp);
    IoSetCompletionRoutine(irp,BT_CompletionEvent,&event,TRUE,TRUE,TRUE);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    if (status == STATUS_PENDING) {
        EVENT_Wait(&event);
        status = irp->IoStatus.Status;
    }
    EVENT_Destroy(&event);

    /* initialize PowerDownLevel to disabled */
    bt->PowerDownLevel = PowerDeviceUnspecified;
    if (NT_SUCCESS(status)) {
        ULONG i;
        bt->DeviceCapabilities = *pdc;       
        for (i = PowerSystemSleeping1; i <= PowerSystemSleeping3; i++) {
            if (bt->DeviceCapabilities.DeviceState[i] < PowerDeviceD3) {
                bt->PowerDownLevel = bt->DeviceCapabilities.DeviceState[i];
            }
        }

        /* 
         * since its safe to surprise-remove this device, we shall
         * set the SurpriseRemoveOK flag to supress any dialog to user.
         */
        pdc->SurpriseRemovalOK = TRUE;
    }

    if (bt->PowerDownLevel == PowerDeviceUnspecified ||
        bt->PowerDownLevel <= PowerDeviceD0) {    
        bt->PowerDownLevel = PowerDeviceD2;
    }
    return status;
}

/**
 * This routine is invoked when the device is removed or stopped.
 */
STATIC NTSTATUS BT_DeconfigureDevice(BtDevice * bt)
{
    ULONG size = sizeof(struct _URB_SELECT_CONFIGURATION);
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    URB * urb = MEM_Alloc(status);
    if (urb) {
        UsbBuildSelectConfigurationRequest(urb, (USHORT)size, NULL);
        status = BT_CallUSBD(bt, NULL, urb);
        MEM_Free(urb);
    }
    return status;
}

/**
 * This routine returns all the memory allocations acquired during
 * device startup. 
 */
STATIC VOID BT_ReleaseMemory(BtDevice * bt)
{
    int i;

    /* cancel pending IRPs */
    BT_CancelAll(bt);

    MEM_Free(bt->eventTx);
    MEM_Free(bt->aclTx);
    MEM_Free(bt->scoTx);

    MEM_Free(bt->strManufacturer);
    MEM_Free(bt->strProduct);
    MEM_Free(bt->strSerialNumber);

    if (bt->UsbConfigurationDescriptor) {
        MEM_Free(bt->UsbConfigurationDescriptor);
        bt->UsbConfigurationDescriptor = NULL;
    }

    for (i=0; i<COUNT(bt->interfaceInfo); i++) {
        if (bt->interfaceInfo[i]) {        
            MEM_Free(bt->interfaceInfo[i]);
            bt->interfaceInfo[i] = NULL;
        }
    }

    if (bt->InterfaceName.Buffer) {
        RtlFreeUnicodeString(&bt->InterfaceName);
        RtlZeroMemory(&bt->InterfaceName,sizeof(bt->InterfaceName));
    }
    if (bt->common.symLinkName) {
        IoDeleteSymbolicLink(bt->common.symLinkName);
        NT_FreeUnicodeString(bt->common.symLinkName);
        bt->common.symLinkName = NULL;
    }
    
    BUFFER_Destroy(&bt->inputQueue);
    BUFFER_Destroy(&bt->readBuffer);
    BUFFER_Destroy(&bt->writeBuffer);
    MUTEX_Destroy(&bt->common.mutex);

    MEM_DumpStat();
    ASSERT(QUEUE_IsEmpty(&bt->common.fileObjects));
}

/**
 * This is the dispatch routine for IRP_MN_STOP_DEVICE
 */
STATIC NTSTATUS BT_HandleStopDevice(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    BT_TRACE("HandleStopDevice");

    MUTEX_Lock(&bt->common.mutex);
    bt->prevDevState = bt->deviceState;
    bt->deviceState = DeviceStopped;
    MUTEX_Unlock(&bt->common.mutex);

    /* This is the right place to actually give up all the resources */
    BT_ReleaseMemory(bt);
    irp->IoStatus.Status =  BT_DeconfigureDevice(bt);
    irp->IoStatus.Information = 0;
    IoSkipCurrentIrpStackLocation(irp);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    ASSERT(NT_SUCCESS(status));
    return status;
}

/**
 * Sends an abort pipe request for open pipes.
 */
STATIC NTSTATUS BT_AbortPipes(BtDevice * bt)
{
    int i;
    NTSTATUS status = STATUS_SUCCESS;
    struct _URB_PIPE_REQUEST pipeRequest;
    URB* urb = (URB*)&pipeRequest;
    for (i=0; i<COUNT(bt->interfaceInfo) && NT_SUCCESS(status); i++) {
        USBD_INTERFACE_INFORMATION * interfaceInfo = bt->interfaceInfo[i];
        if (interfaceInfo) {
            ULONG j;
            for (j=0; j<interfaceInfo->NumberOfPipes && NT_SUCCESS(status); j++) {
                USBD_PIPE_INFORMATION * pipe = interfaceInfo->Pipes + j;
                BT_TRACE1("aborting pipe %02X",pipe->EndpointAddress);
                urb->UrbHeader.Length = sizeof(struct _URB_PIPE_REQUEST);
                urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
                urb->UrbPipeRequest.PipeHandle = pipe->PipeHandle;
                status = BT_CallUSBD(bt, NULL, urb);
            }
        }
    }
    return status;
}

/**
 * This is the dispatch routine for IRP_MN_STOP_DEVICE
 */
STATIC NTSTATUS BT_HandleRemoveDevice(BtDevice * bt, IRP * irp)
{
    ULONG requestCount;
    NTSTATUS status;

    /*
     * The Plug & Play system has dictated the removal of this device.
     * We have no choice but to detach and delete the device object.
     * (If we wanted to express an interest in preventing this removal,
     * we should have failed the query remove IRP).
     */
    BT_TRACE("HandleRemoveDevice");
    if (bt->deviceState != DeviceSurpriseRemoved) {
        status = IoSetDeviceInterfaceState(&bt->InterfaceName, FALSE);
        ASSERT(NT_SUCCESS(status));
        BT_AbortPipes(bt);
    }

    MUTEX_Lock(&bt->common.mutex);
    bt->prevDevState = bt->deviceState;
    bt->deviceState = DeviceRemoved;    
    MUTEX_Unlock(&bt->common.mutex);
    
    /* need 2 decrements */
    requestCount = BT_IoDecrement(bt);
    ASSERT(requestCount > 0);
    requestCount = BT_IoDecrement(bt);
    KeWaitForSingleObject(&bt->RemoveEvent,Executive, KernelMode,FALSE,NULL);

    BT_ReleaseMemory(bt);

    /*
     * We need to send the remove down the stack before we detach,
     * but we don't need to wait for the completion of this operation
     * (and to register a completion routine).
     */
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoSkipCurrentIrpStackLocation(irp);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);

    /* detach the FDO from the device stack */
    IoDetachDevice(bt->TopOfStackDeviceObject);
    IoDeleteDevice(bt->common.deviceObject);
    return status;
}

/**
 * This is the dispatch routine for IRP_MN_SURPRISE_REMOVAL
 */
STATIC NTSTATUS BT_HandleSurpriseRemove(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    BT_TRACE("HandleSurpriseRemove");

    /*
     * 1. fail pending requests
     * 2. return device and memory resources
     * 3. disable interfaces
     */
    MUTEX_Lock(&bt->common.mutex);
    bt->prevDevState = bt->deviceState;
    bt->deviceState = DeviceSurpriseRemoved;
    MUTEX_Unlock(&bt->common.mutex);

    status = IoSetDeviceInterfaceState(&bt->InterfaceName, FALSE);
    ASSERT(NT_SUCCESS(status));
    BT_CancelAll(bt);

    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(irp);
    status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
    return status;
}

/**
 * This routine produces the human readable name for a minor function for
 * the PnP Irps
 */
#if DEBUG
#define PNP_FUNCTION(foo) case foo: return #foo
STATIC Str BT_PnPFunction(UCHAR function)
{
    static char unknown[32];
    switch (function) {
    PNP_FUNCTION(IRP_MN_START_DEVICE);
    PNP_FUNCTION(IRP_MN_QUERY_REMOVE_DEVICE);
    PNP_FUNCTION(IRP_MN_REMOVE_DEVICE);
    PNP_FUNCTION(IRP_MN_CANCEL_REMOVE_DEVICE);
    PNP_FUNCTION(IRP_MN_STOP_DEVICE);
    PNP_FUNCTION(IRP_MN_QUERY_STOP_DEVICE);
    PNP_FUNCTION(IRP_MN_CANCEL_STOP_DEVICE);
    PNP_FUNCTION(IRP_MN_QUERY_DEVICE_RELATIONS);
    PNP_FUNCTION(IRP_MN_QUERY_INTERFACE);
    PNP_FUNCTION(IRP_MN_QUERY_CAPABILITIES);
    PNP_FUNCTION(IRP_MN_QUERY_RESOURCES);
    PNP_FUNCTION(IRP_MN_QUERY_RESOURCE_REQUIREMENTS);
    PNP_FUNCTION(IRP_MN_QUERY_DEVICE_TEXT);
    PNP_FUNCTION(IRP_MN_FILTER_RESOURCE_REQUIREMENTS);
    PNP_FUNCTION(IRP_MN_READ_CONFIG);
    PNP_FUNCTION(IRP_MN_WRITE_CONFIG);
    PNP_FUNCTION(IRP_MN_EJECT);
    PNP_FUNCTION(IRP_MN_SET_LOCK);
    PNP_FUNCTION(IRP_MN_QUERY_ID);
    PNP_FUNCTION(IRP_MN_QUERY_PNP_DEVICE_STATE);
    PNP_FUNCTION(IRP_MN_QUERY_BUS_INFORMATION);
    PNP_FUNCTION(IRP_MN_DEVICE_USAGE_NOTIFICATION);
    PNP_FUNCTION(IRP_MN_SURPRISE_REMOVAL);
    default:
        Sprintf(unknown,"UNKNOWN[%02X]",(unsigned int)function);
        return unknown;
    }
}
#endif /* DEBUG */

/**
 * Dispatch routine for IRP_MJ_PNP
 */
NTSTATUS BT_ComDispatchPnP(BtDeviceBase* dev, IRP* irp)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    BtDevice* bt = CAST(dev, BtDevice, common);

    BT_TRACE1("DispatchPnP(%s)",BT_PnPFunction(irpStack->MinorFunction));

    /* since the device is removed, fail the Irp. */
    if (bt->deviceState == DeviceRemoved) {
        status = STATUS_DELETE_PENDING;
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }

    BT_IoIncrement(bt);

    switch(irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:
        status = BT_HandleStartDevice(bt, irp);
        break;

    case IRP_MN_QUERY_CAPABILITIES:
        status = BT_HandleQueryCapabilities(bt, irp);
        break;

    case IRP_MN_STOP_DEVICE:
        status = BT_HandleStopDevice(bt, irp);
        BT_IoDecrement(bt);
        return status;

    case IRP_MN_SURPRISE_REMOVAL:
        status = BT_HandleSurpriseRemove(bt, irp);
        BT_IoDecrement(bt);
        return status;

    case IRP_MN_REMOVE_DEVICE:
        status = BT_HandleRemoveDevice(bt, irp);
        return status;

    default:
        IoSkipCurrentIrpStackLocation(irp);
        status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
        BT_IoDecrement(bt);
        return status;
    }

    // complete request 
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    // decrement count
    BT_IoDecrement(bt);
    return status;
}
