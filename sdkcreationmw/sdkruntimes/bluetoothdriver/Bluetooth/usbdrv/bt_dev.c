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

// {FF820BCD-8A8B-4fe7-BD0A-D1F17A6BCD77}
static const GUID S60BT_GUID = 
    {0xff820bcd,0x8a8b,0x4fe7,{0xbd,0xa,0xd1,0xf1,0x7a,0x6b,0xcd,0x77}};
//DEFINE_GUID(S60BT_GUID, 
//0xff820bcd,0x8a8b,0x4fe7,0xbd,0xa,0xd1,0xf1,0x7a,0x6b,0xcd,0x77);

static PWSTR BtControlDeviceName = _W("\\Device\\HciUsbControl");
static PWSTR BtControlSymLinkName = _W("\\DosDevices\\HciUsbControl");
static PWSTR BtComDeviceNameFormat = _W("\\Device\\SerialHciUsb%d");
static PWSTR BtComSymLinkNameFormat = _W("\\DosDevices\\COM%d");

#define DosPrefixLength 12
#define DosPrefixSize (DosPrefixLength*sizeof(WCHAR))

/**
 * Completes the request with STATUS_INVALID_DEVICE_REQUEST.
 * Always returns STATUS_INVALID_DEVICE_REQUEST.
 */
NTSTATUS BT_CompleteInvalidRequest(BtDevice * bt, IRP * irp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    BT_IoDecrement(bt);
    return status;
}

/**
 * Dummy BtFileDispatch callback
 */
STATIC NTSTATUS BT_DummyFileDispatch(BtFile* file, IRP * irp)
{
    BT_TRACE("Invalid BtFileDispatch call");
    return STATUS_INVALID_DEVICE_REQUEST;
}

/**
 * Dummy BtDeviceDispatch callback
 */
STATIC NTSTATUS BT_DummyDeviceDispatch(BtDeviceBase* dev, IRP * irp)
{
    BT_TRACE("Invalid BtDeviceDispatch call");
    return STATUS_INVALID_DEVICE_REQUEST;
}

/**
 * Dummy BtDeviceDispatch callback that completes the request
 */
STATIC NTSTATUS BT_DummyDeviceDispatch2(BtDeviceBase* dev, IRP * irp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    BT_TRACE("Invalid BtDeviceDispatch call");
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

/**
 * Bumps up the I/O count. When OutStandingIO bumps from 1 to 2, 
 * clears the StopEvent
 */
LONG BT_IoIncrement(BtDevice * bt)
{
    LONG  result;
    MUTEX_Lock(&bt->common.mutex);
    result = InterlockedIncrement(&bt->OutStandingIO);
    if (result == 2) {
        KeClearEvent(&bt->StopEvent);
    }
    MUTEX_Unlock(&bt->common.mutex);
    return result;
}

/**
 * Decrements the outstanding I/O count. When OutStandingIO drops from 2 to 1,
 * sets the StopEvent. When it drops from 1 to 0, sets the RemoveEvent
 */
LONG BT_IoDecrement(BtDevice * bt)
{
    LONG  result;
    MUTEX_Lock(&bt->common.mutex);
    result = InterlockedDecrement(&bt->OutStandingIO);
    ASSERT(result >= 0);
    switch (result) {
    case 1:
        KeSetEvent(&bt->StopEvent, IO_NO_INCREMENT, FALSE);
        break;
    case 0:
        ASSERT(bt->deviceState == DeviceRemoved);
        KeSetEvent(&bt->RemoveEvent, IO_NO_INCREMENT, FALSE);
        break;
    default:
        break;
    }
    MUTEX_Unlock(&bt->common.mutex);
    return result;
}

/**
 * Builds a IRP_MJ_INTERNAL_DEVICE_CONTROL IRP. Assumes that the method
 * of passing buffer to the driver is METHOD_NEITHER, which is the simplest
 * case (everything is up to the driver).
 */
IRP * BT_CreateUsbSubmitRequest(PDEVICE_OBJECT dev, URB * urb)
{
    IRP * irp = IoAllocateIrp(dev->StackSize, FALSE);
    ASSERT(irp);
    if (irp) {
        IO_STACK_LOCATION* irpSp = IoGetNextIrpStackLocation(irp);
        irpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        irpSp->Parameters.Others.Argument1 = urb;
        irpSp->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
        ASSERT((IOCTL_INTERNAL_USB_SUBMIT_URB & 3) == METHOD_NEITHER);
    }
    return irp;
}

/**
 * This routine is a completion routine for IRPs submitted by 
 * BT_TimeCallUSBD. The context parameter points to Event
 */
STATIC NTSTATUS BT_IOComplete(PDEVICE_OBJECT dev, IRP * irp, PVOID ctx)
{
    EVENT_Set(ctx);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * Synchronously submits a URB down the stack. This function is being 
 * used for short transactions overs the control pipe, so it has a
 * relatively short timeout (200 ms at the time of this writing)
 */
NTSTATUS BT_CallUSBD(BtDevice * bt, BtFile * f, URB * urb)
{
    return BT_TimeCallUSBD(bt, f, urb, BT_USB_TIMEOUT);
}

/**
 * Submits a URB down the stack with a timeout (in milliseconds)
 * NOTE that BtFile is NULL if the call is not made internally 
 * by the driver (as opposed to acting on behalf of the user level
 * code)
 */
NTSTATUS BT_TimeCallUSBD(BtDevice * bt, BtFile * f, URB * urb, int ms)
{
    IRP * irp = BT_CreateUsbSubmitRequest(bt->TopOfStackDeviceObject, urb);
    if (irp) {
        NTSTATUS status;
        Event event;
        EVENT_Init(&event);

        BT_IoIncrement(bt);
        IoSetCompletionRoutine(irp,BT_IOComplete,&event,TRUE,TRUE,TRUE);

        /* submit the request */
        status = IoCallDriver(bt->TopOfStackDeviceObject, irp);
        if (status == STATUS_PENDING) {
            if (EVENT_TimeWait(&event, ms) == WAIT_STATE_TIMEOUT) {
                BT_TRACE("USBD request timed out");
                IoCancelIrp(irp);
                EVENT_Wait(&event);
                status = STATUS_IO_TIMEOUT;
            } else {
                status = irp->IoStatus.Status;
            }
        }

        IoFreeIrp(irp);

        if (!NT_SUCCESS(status)) {
            BT_TRACE2("USBD request failed, err %08X (%08X)",status,
                urb->UrbHeader.Status);
        }

        BT_IoDecrement(bt);
        EVENT_Destroy(&event);
        return status;
    } else {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

/**
 * Common dispatch routine for IRP_MJ_CREATE.
 */
STATIC NTSTATUS BT_BaseDispatchCreate(BtDeviceBase* bt, IRP* irp)
{
    BtFile * f = MEM_New(BtFile);
    if (f) {
        /* initialize the context */
        IO_STACK_LOCATION * irpSp = IoGetCurrentIrpStackLocation(irp);
        RtlZeroMemory(f, sizeof(*f));
        f->dev = bt;
        f->fileObject = irpSp->FileObject;
        irpSp->FileObject->FsContext = f;

        MUTEX_Lock(&bt->mutex);
        QUEUE_InsertTail(&bt->fileObjects, &f->entry);
        MUTEX_Unlock(&bt->mutex);
        return STATUS_SUCCESS;

    } else {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

/**
 * Dispatch routine for IRP_MJ_CREATE on COM device.
 */
STATIC NTSTATUS BT_ComDispatchCreate(BtDeviceBase* btdev, IRP* irp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_STATE;
    BtDevice * bt = CAST(btdev,BtDevice,common);
    if (bt->deviceState == DeviceWorking) {
        status = BT_BaseDispatchCreate(btdev, irp);
        if (NT_SUCCESS(status)) {
            if (!bt->eventIrp) BT_SubmitEventRequest(bt);
            if (!bt->aclIrp) BT_SubmitAclRequest(bt);
            if (!bt->scoIrp) BT_SubmitScoRequest(bt);
        }
    }
    return status;
}

/**
 * Dispatch routine for IRP_MJ_CREATE.
 */
STATIC NTSTATUS BT_DispatchCreate(DEVICE_OBJECT* dev, IRP* irp)
{
    NTSTATUS status;
    BtDeviceBase* btdev = dev->DeviceExtension;
    BT_TRACE("DispatchCreate");
    status = btdev->dispatchCreate(btdev, irp);
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

/**
 * Common dispatch routine for IRP_MJ_CLOSE.
 */
STATIC NTSTATUS BT_BaseDispatchClose(BtFile* file, IRP* irp)
{
    BtDeviceBase* dev = file->dev;

    ASSERT(!QUEUE_IsEmpty(&dev->fileObjects));
    ASSERT(QUEUE_Contains(&dev->fileObjects, &file->entry));

    MUTEX_Lock(&dev->mutex);
    QUEUE_RemoveEntry(&file->entry);
    MUTEX_Unlock(&dev->mutex);
 
    /* deallocate the context */
    MEM_Free(file);
    return STATUS_SUCCESS;
}

/**
 * Dispatch routine for IRP_MJ_CLOSE on COM device.
 */
STATIC NTSTATUS BT_ComDispatchClose(BtFile* file, IRP* irp)
{
    BtDevice * bt = CAST(file->dev,BtDevice,common);
    BT_BaseDispatchClose(file, irp);
    MUTEX_Lock(&bt->common.mutex);
    BUFFER_Clear(&bt->inputQueue);
    BUFFER_Clear(&bt->readBuffer);
    BUFFER_Clear(&bt->writeBuffer);
    MUTEX_Unlock(&bt->common.mutex);
    return STATUS_SUCCESS;
}

/**
 * Dispatch routine for IRP_MJ_CLOSE.
 */
STATIC NTSTATUS BT_DispatchClose(DEVICE_OBJECT* dev, IRP* irp)
{
    NTSTATUS status;
    IO_STACK_LOCATION * irpSp = IoGetCurrentIrpStackLocation(irp);
    BtDeviceBase* btdev = dev->DeviceExtension;
    BtFile* file = irpSp->FileObject->FsContext;
    BT_TRACE("DispatchClose");
    ASSERT(file->dev == btdev);
    status = btdev->dispatchClose(file, irp);
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

/**
 * Cancels all pending IRPs for all file objects attached to the
 * specified device.
 */
void BT_CancelAll(BtDevice * bt)
{
    IRP* eventIrp;
    IRP* aclIrp;
    IRP* scoIrp;
    IRP* waitIrp;
    IRP* readIrp;

    MUTEX_Lock(&bt->common.mutex);
    eventIrp = bt->eventIrp;
    aclIrp = bt->aclIrp;
    scoIrp = bt->scoIrp;
    waitIrp = bt->waitIrp;
    readIrp = bt->readIrp;
    bt->eventIrp = NULL;
    bt->aclIrp = NULL;
    bt->scoIrp = NULL;
    bt->waitIrp = NULL;
    bt->readIrp = NULL;
    MUTEX_Unlock(&bt->common.mutex);
    
    if (eventIrp) IoCancelIrp(eventIrp);
    if (aclIrp) IoCancelIrp(aclIrp);
    if (scoIrp) IoCancelIrp(scoIrp);
    if (waitIrp) IoCancelIrp(waitIrp);
    if (readIrp) IoCancelIrp(readIrp);
}

/**
 * Dummy dispatch routine for IRP_MJ_CLEANUP
 */
STATIC NTSTATUS BT_BaseDispatchCleanup(BtFile* file, IRP* irp)
{
    return STATUS_SUCCESS;
}

/**
 * Dispatch routine for IRP_MJ_CLEANUP on COM device
 */
STATIC NTSTATUS BT_ComDispatchCleanup(BtFile* file, IRP* irp)
{
    IRP* waitIrp;
    IRP* readIrp;
    BtDevice* bt = CAST(file->dev,BtDevice,common);

    BT_IoIncrement(bt);
    
    /* cancel all Irp's that belong to input Irp's fileobject */
    MUTEX_Lock(&bt->common.mutex);
    waitIrp = bt->waitIrp;
    readIrp = bt->readIrp;
    bt->waitIrp = NULL;
    bt->readIrp = NULL;
    BUFFER_Clear(&bt->inputQueue);
    BUFFER_Clear(&bt->readBuffer);
    BUFFER_Clear(&bt->writeBuffer);
    MUTEX_Unlock(&bt->common.mutex);
    if (waitIrp) IoCancelIrp(waitIrp);
    if (readIrp) IoCancelIrp(readIrp);

    BT_IoDecrement(bt);
    return STATUS_SUCCESS;
}

/**
 * Dispatch routine for IRP_MJ_CLEANUP
 */
STATIC NTSTATUS BT_DispatchCleanup(DEVICE_OBJECT * dev, IRP * irp)
{
    NTSTATUS status;
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    BtDeviceBase* btdev = dev->DeviceExtension;
    BtFile* file = irpSp->FileObject->FsContext;
    BT_TRACE("DispatchCleanup");
    status = btdev->dispatchCleanup(file, irp);
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

/**
 * Cancel routine for IRP_MJ_READ
 */
STATIC VOID BT_CancelRead(DEVICE_OBJECT* dev, IRP* irp)
{
    BtDevice * bt = dev->DeviceExtension;
    BT_TRACE1("read Irp %08X cancelled",irp);
    if (bt->readIrp == irp) {
        MUTEX_Lock(&bt->common.mutex);
        if (bt->readIrp == irp) bt->readIrp = NULL;
        MUTEX_Unlock(&bt->common.mutex);
    }
    irp->IoStatus.Information = 0;
    irp->IoStatus.Status = STATUS_CANCELLED;
    IoReleaseCancelSpinLock(irp->CancelIrql);
    IoCompleteRequest(irp, IO_NO_INCREMENT);
}

/**
 * Dispatch routine for IRP_MJ_READ on COM device.
 */
STATIC NTSTATUS BT_ComDispatchRead(BtFile* file, IRP * irp)
{
    IRP* oldIrp;
    NTSTATUS status;
    BtDevice* bt = CAST(file->dev,BtDevice,common);
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    ULONG length = irpSp->Parameters.Read.Length;
    void* buffer;

    BT_TRACE2("DispatchRead [%08X] %d bytes",irp,length);

    if (irp->MdlAddress) {
        buffer = MmGetSystemAddressForMdl(irp->MdlAddress);
    } else {
        buffer = irp->AssociatedIrp.SystemBuffer;
    }
    ASSERT(buffer);

    MUTEX_Lock(&bt->common.mutex);
    oldIrp = bt->readIrp;
    bt->readIrp = NULL;
    if (length) {
        ULONG bytesRead = BT_LockedRead(bt, buffer, length);
        if (bytesRead > 0) {
            status = STATUS_SUCCESS;
            irp->IoStatus.Information = bytesRead;
            BT_FillInputQueue(bt);
            BT_TRACE2("read IRP %08X complete, %d bytes",irp,bytesRead);
            BT_DUMP(buffer, bytesRead);
        } else {
            bt->readIrp = irp;
            irp->IoStatus.Status = status = STATUS_PENDING;
            irp->IoStatus.Information = 0;
            BT_TRACE1("read IRP %08X pending",irp);

            /* Mark this IRP as pending and cancelable */
            IoMarkIrpPending(irp);
            IoSetCancelRoutine(irp, BT_CancelRead);
        }
    } else {
        /* complete zero length read immediately */
        status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        BT_TRACE1("read IRP %08X done (empty buffer)",irp);
    }
    MUTEX_Unlock(&bt->common.mutex);

    if (oldIrp) IoCancelIrp(oldIrp);
    BT_UpdateEventMask(bt, 0);

    /* IRP is completed by BT_DispatchRead */
    return status;
}

/**
 * Dispatch routine for IRP_MJ_READ.
 */
STATIC NTSTATUS BT_DispatchRead(DEVICE_OBJECT * dev, IRP* irp)
{
    NTSTATUS status;
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    BtDeviceBase* btdev = dev->DeviceExtension;
    BtFile* file = irpSp->FileObject->FsContext;
    status = btdev->dispatchRead(file, irp);
    if (status != STATUS_PENDING) {
        irp->IoStatus.Status = status;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
    }
    return status;
}

/**
 * Dispatch routine for IRP_MJ_WRITE on COM device.
 */
STATIC NTSTATUS BT_ComDispatchWrite(BtFile* file, IRP * irp)
{
    NTSTATUS status;
    BtDevice* bt = CAST(file->dev,BtDevice,common);
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    ULONG length = irpSp->Parameters.Write.Length;
    BT_TRACE1("DispatchWrite (%d bytes)",length);
    BT_IoIncrement(bt);

    /* Zero length write is a noop */
    if (length) {
        const void* data;
        if (irp->MdlAddress) {
            data = MmGetSystemAddressForMdl(irp->MdlAddress);
        } else {
            data = irp->AssociatedIrp.SystemBuffer;
        }
        ASSERT(data);
        status = BT_Write(bt, data, length);
    } else {
        status = STATUS_SUCCESS;
    }

    irp->IoStatus.Information = NT_SUCCESS(status) ? length : 0;
    BT_IoDecrement(bt);
    return status;
}

/**
 * Dispatch routine for IRP_MJ_WRITE.
 */
STATIC NTSTATUS BT_DispatchWrite(DEVICE_OBJECT* dev, IRP* irp)
{
    NTSTATUS status;
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    BtDeviceBase* btdev = dev->DeviceExtension;
    BtFile* file = irpSp->FileObject->FsContext;
    status = btdev->dispatchWrite(file, irp);
    if (status != STATUS_PENDING) {
        irp->IoStatus.Status = status;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
    }
    return status;
}

/**
 * Dispatch routine for IRP_MJ_POWER.
 * Device-specific handler completes the IRP.
 */
STATIC NTSTATUS BT_DispatchPower(DEVICE_OBJECT* dev, IRP* irp)
{
    //BtDeviceBase* btdev = dev->DeviceExtension;
    //return btdev->dispatchCreate(btdev, irp);

    BtDeviceBase* btdev = dev->DeviceExtension;
    BtDevice * bt = CAST(btdev,BtDevice,common);

    if( DeviceRemoved == bt->deviceState || 
        DeviceSurpriseRemoved == bt->deviceState )
    {
        PoStartNextPowerIrp(irp);
        irp->IoStatus.Status =  STATUS_NO_SUCH_DEVICE ;
        IoCompleteRequest(irp, IO_NO_INCREMENT );
        return STATUS_NO_SUCH_DEVICE ;
    }
    
    PoStartNextPowerIrp(irp);
    IoSkipCurrentIrpStackLocation(irp);
    return PoCallDriver(bt->TopOfStackDeviceObject, irp);
}

/**
 * Dispatch routine for IRP_MJ_PNP.
 * Device-specific handler completes the IRP.
 */
STATIC NTSTATUS BT_DispatchPnP(DEVICE_OBJECT* dev, IRP* irp)
{
    BtDeviceBase* btdev = dev->DeviceExtension;
    return btdev->dispatchPnp(btdev, irp);
}

/**
 * Dispatch routine for IRP_MJ_DEVICE_CONTROL. Device-specific callback is
 * responsible for completing the IRP
 */
STATIC NTSTATUS BT_DispatchDeviceControl(DEVICE_OBJECT * dev, IRP* irp)
{
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    BtDeviceBase* btdev = dev->DeviceExtension;
    BtFile* file = irpSp->FileObject->FsContext;
    return btdev->dispatchDeviceControl(file, irp);
}

/**
 * Creates a COM port device object. Initializes deviceObject, symLinkName
 * and  win32Name fields of the BtDeviceBase structure as well as device
 * specific callbacks. All other fields are initialized to zero.
 */
STATIC BtDevice* BT_CreateComDevice(DRIVER_OBJECT* driver, int devNum)
{
    int i;
    DEVICE_OBJECT * device = NULL;
    UNICODE_STRING devName;
    WCHAR deviceName[64];
    NTSTATUS status;

    swprintf(deviceName, BtComDeviceNameFormat, devNum);
    RtlInitUnicodeString(&devName, deviceName);

    // Create the device used for communications with user mode code 
    status = IoCreateDevice(driver, sizeof(BtDevice), &devName, 
                            FILE_DEVICE_SERIAL_PORT, 0, TRUE, &device);

    if (NT_SUCCESS(status)) {
        BT_TRACE2("Created device %08x %wZ", device, &devName);

        // Create a symbolic link that the Win32 apps can specify to 
        // gain access to this driver/device.
        for (i = devNum+1; i<devNum+256; i++) {
            WCHAR symLink[64];
            UNICODE_STRING symLinkName;
            swprintf(symLink,BtComSymLinkNameFormat,i);
            RtlInitUnicodeString(&symLinkName, symLink);
            status = IoCreateSymbolicLink(&symLinkName, &devName);
            if (NT_SUCCESS(status)) {
                BtDevice* bt = device->DeviceExtension;
                BtDeviceBase* base = &bt->common;
                BT_TRACE1("Created symbolic link %wZ", &symLinkName);
                bt->portNum = i;
                base->deviceObject = device;
                base->symLinkName = NT_DupUnicodeString(&symLinkName,0);
                if (base->symLinkName) {
                    base->type = BtComDeviceType;
                    base->win32Name = *base->symLinkName;
                    base->win32Name.Buffer += DosPrefixLength;
                    base->win32Name.Length -= DosPrefixSize;
                    base->win32Name.MaximumLength -= DosPrefixSize;
                    base->dispatchCreate = BT_ComDispatchCreate;
                    base->dispatchCleanup = BT_ComDispatchCleanup;
                    base->dispatchClose = BT_ComDispatchClose;
                    base->dispatchRead = BT_ComDispatchRead;
                    base->dispatchWrite = BT_ComDispatchWrite;
                    base->dispatchDeviceControl = BT_ComDispatchDeviceControl;
                    base->dispatchPower = BT_ComDispatchPower;
                    base->dispatchPnp = BT_ComDispatchPnP;
                    return bt;
                } else {
                    status = IoDeleteSymbolicLink(&symLinkName);
                    ASSERT(NT_SUCCESS(status));
                    IoDeleteDevice(device);
                    return NULL;
                }
            } else if (status == STATUS_OBJECT_NAME_COLLISION) {
                // stepped on existing COM port, keep trying 
                BT_TRACE1("symlink %wZ already exists",&symLinkName);
            } else {
                BT_TRACE2("IoCreateSymbolicLink(%wZ) failed, status %08X",
                    &symLinkName,status);
                break;
            }
        }

        BT_TRACE("giving up...");
        IoDeleteDevice(device);
    } else {
        BT_TRACE2("IoCreateDevice(%wZ) failed, status %08X",&devName,status);
    }

    return NULL;
}

/**
 * Creates the control device, not associated with COM port.
 */
STATIC NTSTATUS BT_CreateControlDevice(DRIVER_OBJECT* driver)
{
    NTSTATUS status;
    DEVICE_OBJECT * device = NULL;
    UNICODE_STRING devName;

    /* Create the device used for communications with user mode code */
    RtlInitUnicodeString(&devName, BtControlDeviceName);
    status = IoCreateDevice(driver, sizeof(BtDeviceBase), &devName, 
                            FILE_DEVICE_S60BT, 0, TRUE, &device);

    if (NT_SUCCESS(status)) {
        UNICODE_STRING symLinkName;
        BT_TRACE2("Created device %08x %wZ", device, &devName);
        RtlInitUnicodeString(&symLinkName, BtControlSymLinkName);
        status = IoCreateSymbolicLink(&symLinkName, &devName);
        if (NT_SUCCESS(status)) {
            BtDeviceBase* bt = device->DeviceExtension;
            BT_TRACE1("Created symbolic link %wZ", &symLinkName);
            bt->deviceObject = device;
            bt->symLinkName = NT_DupUnicodeString(&symLinkName,0);
            if (bt->symLinkName) {
                QUEUE_Init(&bt->fileObjects);
                MUTEX_Init(&bt->mutex);
                bt->type = BtBaseDeviceType;
                bt->win32Name = *bt->symLinkName;
                bt->win32Name.Buffer += DosPrefixLength;
                bt->win32Name.Length -= DosPrefixSize;
                bt->win32Name.MaximumLength -= DosPrefixSize;
                bt->dispatchCreate = BT_BaseDispatchCreate;
                bt->dispatchCleanup = BT_BaseDispatchCleanup;
                bt->dispatchClose = BT_BaseDispatchClose;
                bt->dispatchRead = BT_DummyFileDispatch;
                bt->dispatchWrite = BT_DummyFileDispatch;
                bt->dispatchDeviceControl = BT_BaseDispatchDeviceControl;
                bt->dispatchPower = BT_DummyDeviceDispatch;
                bt->dispatchPnp = BT_DummyDeviceDispatch2;

                /* Clear the DO_DEVICE_INITIALIZING flag. */
                device->Flags |= DO_POWER_PAGABLE;
                device->Flags &= ~DO_DEVICE_INITIALIZING;
                return STATUS_SUCCESS;
            } else {
                IoDeleteSymbolicLink(&symLinkName);
                status = STATUS_INSUFFICIENT_RESOURCES;
            }
        } else if (status == STATUS_OBJECT_NAME_COLLISION) {
            /* stepped on existing COM port, keep trying */
            BT_TRACE1("symlink %wZ already exists",&symLinkName);
        } else {
            BT_TRACE2("IoCreateSymbolicLink(%wZ) failed, status %08X",
                &symLinkName,status);
        }
        IoDeleteDevice(device);
    } else {
        BT_TRACE2("IoCreateDevice(%wZ) failed, status %08X",&devName, status);
    }

    return status;
}

/**
 * AddDevice is responsible for creating functional device objects (FDO) or
 * filter device objects (filter DO) for the physical device objects (PDO)
 * enumerated by the Plug and Play (PnP) Manager.
 */
NTSTATUS BT_AddDevice(PDRIVER_OBJECT driver, PDEVICE_OBJECT pdo)
{
    NTSTATUS status;
    DEVICE_OBJECT * dev = NULL;
    BtDevice * bt = NULL;
    int i;

    BT_TRACE("AddDevice");

    for (i=0; i<256 && !bt; i++) {
        bt = BT_CreateComDevice(driver, i);
    }

    if (!bt) {
        // returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        return STATUS_UNSUCCESSFUL;
    }

    // Initialize the device extension
    ASSERT(bt->common.deviceObject);
    ASSERT(bt == bt->common.deviceObject->DeviceExtension);
    dev = bt->common.deviceObject;
    dev->Flags |= DO_DIRECT_IO;
    bt->PhysicalDeviceObject = pdo;
    bt->deviceState =  DeviceNotStarted;
    bt->prevDevState = DeviceNotStarted;
    QUEUE_Init(&bt->common.fileObjects);
    MUTEX_Init(&bt->common.mutex);

    /* assume H4 framing */
    bt->codec = &BtCodecH4;  

    // Default serial port parameters
    bt->SerialQueueSize.InSize = 1500;
    bt->SerialQueueSize.OutSize = 1500;
    bt->SerialBaudRate.BaudRate = 9600;
    bt->SerialLineControl.StopBits = STOP_BIT_1;
    bt->SerialLineControl.Parity = NO_PARITY;
    bt->SerialLineControl.WordLength = 8;
    bt->SerialHandflow.ControlHandShake = 
        SERIAL_RTS_HANDSHAKE |
        SERIAL_DTR_HANDSHAKE;
    bt->SerialEventMask = 
        SERIAL_EV_CTS | 
        SERIAL_EV_DSR | 
        SERIAL_EV_TXEMPTY;
    bt->SerialState = 
        SERIAL_DTR_STATE | SERIAL_DSR_STATE |
        SERIAL_RTS_STATE | SERIAL_CTS_STATE;

    // This event is signaled when the OutstandingIO becomes 0
    KeInitializeEvent(&bt->RemoveEvent, NotificationEvent, FALSE);

    // This event is signaled when the OutstandingIO becomes 1
    KeInitializeEvent(&bt->StopEvent, NotificationEvent, TRUE);

    // OutstandingIo count biased to 1.
    // Transition to 0 during remove device means IO is finished.
    // Transition to 1 means the device can be stopped
    bt->OutStandingIO = 1;

    // set the flags as underlying PDO
    if (pdo->Flags & DO_POWER_PAGABLE) {
        dev->Flags |= DO_POWER_PAGABLE;
    }

    // Set the initial power state of the device
    bt->Power.DeviceState = PowerDeviceD0;
    bt->Power.SystemState = PowerSystemWorking;
    PoSetPowerState(dev, DevicePowerState, bt->Power);

    // attach our driver to device stack
    // The return value of IoAttachDeviceToDeviceStack is the top of the
    // attachment chain. This is where all the IRPs should be routed.
    bt->TopOfStackDeviceObject = IoAttachDeviceToDeviceStack(dev, pdo);
    ASSERT(bt->TopOfStackDeviceObject);
    if (!bt->TopOfStackDeviceObject) {
        IoDeleteDevice(dev);
        return STATUS_NO_SUCH_DEVICE;
    }
        
    // Register device interfaces
    status = IoRegisterDeviceInterface(bt->PhysicalDeviceObject, 
                                       &S60BT_GUID, NULL, 
                                       &bt->InterfaceName);
    if (!NT_SUCCESS(status)) {
        BT_TRACE1("IoRegisterDeviceInterface failed, err %08X",status);
        IoDetachDevice(bt->TopOfStackDeviceObject);
        IoDeleteDevice(dev);
        return status;
    }

    /* Allocate buffers */
    bt->eventTx = MEM_New(BtTransfer);
    if (bt->eventTx) {
        RtlZeroMemory(bt->eventTx, OFFSET(BtTransfer,data));
        bt->eventTx->bt = bt;
        bt->aclTx = MEM_New(BtTransfer);
        if (bt->aclTx) {
            RtlZeroMemory(bt->aclTx, OFFSET(BtTransfer,data));
            bt->aclTx->bt = bt;
            bt->scoTx = MEM_New(BtTransfer);
            if (bt->scoTx) {
                RtlZeroMemory(bt->scoTx, OFFSET(BtTransfer,data));
                bt->scoTx->bt = bt;
                BUFFER_Init(&bt->inputQueue);
                BUFFER_Init(&bt->readBuffer);
                BUFFER_Init(&bt->writeBuffer);

                /* Clear the DO_DEVICE_INITIALIZING flag. */
                BT_TRACE1("registered interface %wZ",&bt->InterfaceName);
                dev->Flags &= ~DO_DEVICE_INITIALIZING;
                return STATUS_SUCCESS;
            }
            MEM_Free(bt->aclTx);
        }
        MEM_Free(bt->eventTx);
    }
    IoDeleteSymbolicLink(bt->common.symLinkName);
    NT_FreeUnicodeString(bt->common.symLinkName);
    IoDetachDevice(bt->TopOfStackDeviceObject);
    IoDeleteDevice(dev);
    return STATUS_INSUFFICIENT_RESOURCES;
}

STATIC NTSTATUS BT_DispatchSysCtrl( PDEVICE_OBJECT pdo, PIRP irp )
{
    BtDevice                *bt;
    NTSTATUS                status;

    BT_TRACE( "received WMI IRP" );

    bt = (BtDevice *)pdo->DeviceExtension;

    if( DeviceRemoved == bt->deviceState || 
        DeviceSurpriseRemoved == bt->deviceState )
    {
        status = STATUS_NO_SUCH_DEVICE;

        irp->IoStatus.Status = status;

        IoCompleteRequest (irp, IO_NO_INCREMENT);

        return status;
    }

    //
    // Set up the I/O stack location for the next lower driver (the target device
    // object for the IoCallDriver call). Call IoSkipCurrentIrpStackLocation
    // because the function driver does not change any of the IRP's values in the
    // current I/O stack location. This allows the system to reuse I/O stack
    // locations.
    //
    IoSkipCurrentIrpStackLocation (irp);

    //
    // Pass the WMI (system control) IRP down the device stack to be processed by the
    // bus driver.
    //
    status = IoCallDriver( bt->TopOfStackDeviceObject,irp );

    return status;
}

/**
 * Called when driver is being unloaded.
 */
STATIC VOID BT_DriverUnload(DRIVER_OBJECT * driver)
{
    BT_TRACE("DriverUnload");
    if (driver->DeviceObject) {
        BtDeviceBase* dev = driver->DeviceObject->DeviceExtension;
        ASSERT(dev->type == BtBaseDeviceType);
        ASSERT(QUEUE_IsEmpty(&dev->fileObjects));
        MUTEX_Destroy(&dev->mutex);
        if (dev->symLinkName) {
            IoDeleteSymbolicLink(dev->symLinkName);
            NT_FreeUnicodeString(dev->symLinkName);
            dev->symLinkName = NULL;
        }
        BT_TRACE("deleting control device");
        IoDeleteDevice(dev->deviceObject);
    }
    MEM_Shutdown();
}

#ifdef DUMP_MEM_ALLOC

static struct _MemContext {
    const MemHook * next;
} mc = {0};

/**
 * Memory hooks
 */
static Bool BT_MemInit(MemContext * ctx, const MemHook * next)
{
    ctx->next = next;
    return True;
}

static void * BT_MemAlloc(MemContext * ctx, size_t size)
{
    void * ptr = MEM_Alloc2(ctx->next, size);
    BT_TRACE2(" ++++ %08X - %u bytes\n",ptr,size);
    return ptr;
}

static void BT_MemFree(MemContext * ctx, void * ptr)
{
    BT_TRACE1(" ---- %08X\n",ptr);
    MEM_Free2(ctx->next, ptr);
}

#endif /* DUMP_MEM_ALLOC */

#if DEBUG
/**
 * Debug trace
 */
int BT_DbgPrint(const char* format, ...)
{
    int n;
    StrBuf256 buf;
    va_list va;
    va_start(va, format);
    STRBUF_InitBufXXX(&buf);
    STRBUF_AppendFormatVa(&buf.sb, format, va);
    n = DbgPrint("%s",STRBUF_Text(&buf.sb));
    STRBUF_Destroy(&buf.sb);
    va_end(va);
    return n;
}
#endif /* DEBUG */

/**
 * Installable driver initialization entry point.
 */
NTSTATUS DriverEntry(DRIVER_OBJECT * driver, UNICODE_STRING * regpath)
{
#ifdef DUMP_MEM_ALLOC
    MemProc mp;
#endif /* DUMP_MEM_ALLOC */

    MEM_InitModule();
    
#ifdef DUMP_MEM_ALLOC
    memset(&mp, 0, sizeof(mp));
    mp.memInit = BT_MemInit;
    mp.memAlloc = BT_MemAlloc;
    mp.memFree = BT_MemFree;
    MEM_Hook(&mp, &mc);
#endif /* DUMP_MEM_ALLOC */
 
    BT_TRACE("DriverEntry");
    BT_TRACE("version "PRODUCT_VERSION" built on "__DATE__" at "__TIME__"");
    driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BT_DispatchDeviceControl;
    driver->MajorFunction[IRP_MJ_POWER] = BT_DispatchPower;
    driver->MajorFunction[IRP_MJ_PNP] = BT_DispatchPnP;
    driver->MajorFunction[IRP_MJ_CREATE] = BT_DispatchCreate;
    driver->MajorFunction[IRP_MJ_CLOSE] = BT_DispatchClose;
    driver->MajorFunction[IRP_MJ_CLEANUP] = BT_DispatchCleanup;
    driver->MajorFunction[IRP_MJ_READ] = BT_DispatchRead;
    driver->MajorFunction[IRP_MJ_WRITE] = BT_DispatchWrite;
    driver->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = BT_DispatchSysCtrl;
    driver->DriverUnload = BT_DriverUnload;
    driver->DriverExtension->AddDevice = BT_AddDevice;

    /* Finally, create the control device */
    return BT_CreateControlDevice(driver);    
}
