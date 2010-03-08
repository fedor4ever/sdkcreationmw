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

/**
 * Handles IOCTL_S60BT_GET_PORT_LIST ioctl.
 */
STATIC int BT_GetPortInfo(DRIVER_OBJECT* driver, BtPortList* list, int bufLen)
{
    /* I don't think I need to synchronize access to the device list */    
    int bytesWritten = sizeof(*list);
    DEVICE_OBJECT* devObj = driver->DeviceObject;
    BtPortInfo* info = (BtPortInfo*)(list+1);
    RtlZeroMemory(list, sizeof(*list));
    while (devObj) {
        BtDeviceBase* btdev = devObj->DeviceExtension;
        ASSERT(btdev->type==BtBaseDeviceType || btdev->type==BtComDeviceType);
        if (btdev->type == BtComDeviceType) {
            BtDevice* bt = CAST(btdev,BtDevice,common);

            const WCHAR* manufacturer = bt->strManufacturer ? 
                bt->strManufacturer->bString : L"";
            const WCHAR* productName = bt->strProduct ? 
                bt->strProduct->bString : L"";
            const WCHAR* serialNumber = bt->strSerialNumber ? 
                bt->strSerialNumber->bString : L"";
            BT_TRACE5("    COM%d: %d (%S,%S,%S)",bt->portNum,
                btdev->fileObjects.size, manufacturer, productName, 
                serialNumber);

            list->totalCount++;
            MUTEX_Lock(&btdev->mutex);
            if (QUEUE_IsEmpty(&btdev->fileObjects)) {
                list->availCount++;
            }
            /* Fill in next BtPortInfo if we still have room */
            if (bufLen >= (bytesWritten + (int)sizeof(*info))) {
                RtlZeroMemory(info, sizeof(*info));
                info->openCount = QUEUE_Size(&btdev->fileObjects);
                info->portNum = bt->portNum;
                info->vid = bt->DeviceDescriptor.idVendor;
                info->pid = bt->DeviceDescriptor.idProduct;
                wcsncpy(info->manufacturer, manufacturer, 
                  COUNT(info->manufacturer));
                wcsncpy(info->productName, productName, 
                  COUNT(info->productName));
                wcsncpy(info->serialNumber, serialNumber, 
                  COUNT(info->serialNumber));

                /* make sure strings are NULL terminated */
                info->manufacturer[COUNT(info->manufacturer)-1] =
                info->productName[COUNT(info->productName)-1] =
                info->serialNumber[COUNT(info->serialNumber)-1] = 0;

                list->infoCount++;
                bytesWritten += sizeof(*info);
                info++;
            }
            MUTEX_Unlock(&btdev->mutex);
        }
        devObj = devObj->NextDevice;
    }
    BT_TRACE1("  totalCount: %d",list->totalCount);
    BT_TRACE1("  availCount: %d",list->availCount);
    BT_TRACE1("  infoCount:  %d",list->infoCount);
    return bytesWritten;
}

/**
 * Dispatch routine for IRP_MJ_DEVICE_CONTROL on control device.
 */
NTSTATUS BT_BaseDispatchDeviceControl(BtFile* f, IRP* irp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);

    ULONG code = irpStack->Parameters.DeviceIoControl.IoControlCode;
    PVOID ioBuffer = irp->AssociatedIrp.SystemBuffer;
    ULONG inBufLen = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outBufLen = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    RELEASE_TRY {

        switch(code) {
        case IOCTL_S60BT_VERSION:
            BT_TRACE("IOCTL_S60BT_VERSION");
            if (inBufLen  == 0 && outBufLen >= sizeof(BtVersion)) {
                BtVersion* ver = ioBuffer;
                ver->major = PRODUCT_VERSION_MAJOR;
                ver->minor = PRODUCT_VERSION_MINOR;
                ver->micro = PRODUCT_VERSION_MICRO;
                ver->nano = PRODUCT_VERSION_NANO;
                irp->IoStatus.Information = sizeof(BtVersion);
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_INVALID_BUFFER_SIZE;
            }
            break;

        case IOCTL_S60BT_GET_PORT_LIST:
            BT_TRACE("IOCTL_S60BT_GET_PORT_LIST");
            if (inBufLen  == 0 && outBufLen >= sizeof(BtPortList)) {
                irp->IoStatus.Information = BT_GetPortInfo(
                    f->dev->deviceObject->DriverObject,
                    ioBuffer, outBufLen);
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_INVALID_BUFFER_SIZE;
            }
            break;

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

    } RELEASE_CATCH(status)

    irp->IoStatus.Status = status;
    IoCompleteRequest(irp, IO_SERIAL_INCREMENT);
    return status;
}

/**
 * Dispatch routine for IRP_MJ_DEVICE_CONTROL on COM device.
 */
NTSTATUS BT_ComDispatchDeviceControl(BtFile* f, IRP* irp)
{
    ULONG info = 0;
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    BtDevice* bt = CAST(f->dev,BtDevice,common);
    ULONG code = irpStack->Parameters.DeviceIoControl.IoControlCode;
    PVOID ioBuffer = irp->AssociatedIrp.SystemBuffer;
    ULONG inBufLen = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outBufLen = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    if (!bt->eventPipe || !bt->aclPipeIn || !bt->aclPipeOut ||
        !bt->scoPipeIn || !bt->scoPipeOut) {

        BT_TRACE2("DispatchDeviceControl [%08X] %08X, not ready",irp, code);
        status = STATUS_DEVICE_NOT_READY;
        irp->IoStatus.Status = status;
        IoCompleteRequest(irp, IO_SERIAL_INCREMENT);
        return status;
    }

     /* handle IOCTLs that require device to be working */
    __try {
        RELEASE_TRY {
            IO_STATUS_BLOCK serialStatus;
            if (BT_IsSerialDeviceControl(bt, irp, &serialStatus)) {
                status = serialStatus.Status;
                info = serialStatus.Information;
            } else {
                switch(code) {
                default :
                    BT_TRACE2("DispatchDeviceControl [%08X] %08X?",irp, code);
                    status = STATUS_INVALID_DEVICE_REQUEST;
                    break;
                }
            }

        } RELEASE_CATCH(status)

    } __finally {
        if (status != STATUS_PENDING) {
            irp->IoStatus.Status = status;
            irp->IoStatus.Information = info;
            IoCompleteRequest(irp, IO_SERIAL_INCREMENT);
        }
    }

    return status;
}
