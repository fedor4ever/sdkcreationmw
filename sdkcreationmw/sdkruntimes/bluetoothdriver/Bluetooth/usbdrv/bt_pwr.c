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
* Description:  Power management
 *
*/


#include "bt_usb.h"

/**
 * The completion routine for device power DOWN irp.
 */
STATIC NTSTATUS BT_FinishDevPoDnIrp(DEVICE_OBJECT * dev, IRP * irp, PVOID ctx)
{
    BtDevice * bt = dev->DeviceExtension;
    NTSTATUS status = irp->IoStatus.Status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    POWER_STATE newState = irpStack->Parameters.Power.State;

    BT_TRACE("FinishDevPoDnIrp");
    ASSERT(bt == ctx);

    if (NT_SUCCESS(status) && irpStack->MinorFunction == IRP_MN_SET_POWER) {
        // update the cache;
        bt->Power.DeviceState = newState.DeviceState;
        PoSetPowerState(dev, DevicePowerState, newState);
    }

    PoStartNextPowerIrp(irp);
    BT_IoDecrement(bt);
    return STATUS_SUCCESS;
}

/**
 * This routine waits for the I/O in progress to finish and then
 * sends the device power irp (query/set) down the stack.
 */
STATIC VOID BT_HoldIoRequestCB(DEVICE_OBJECT * dev, VOID * Context)
{
    NTSTATUS status;
    BtDevice * bt = dev->DeviceExtension;
    BtWorkerThreadContext * context = Context;
    IRP * irp = context->irp;

    BT_TRACE("HoldIoRequestCB");

    // wait for I/O in progress to finish.
    // the stop event is signalled when the counter drops to 1.
    // invoke BT_IoDecrement twice: once each for the S-Irp and D-Irp.
    BT_IoDecrement(bt);
    BT_IoDecrement(bt);

    KeWaitForSingleObject(&bt->StopEvent, Executive, KernelMode, FALSE, NULL);

    // Increment twice to restore the count
    BT_IoIncrement(bt);
    BT_IoIncrement(bt);

    // now send the Irp down
    IoCopyCurrentIrpStackLocationToNext(irp);
    IoSetCompletionRoutine(irp, BT_FinishDevPoDnIrp, bt, TRUE, TRUE, TRUE);
    status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
    ASSERT(NT_SUCCESS(status));
    IoFreeWorkItem(context->workItem);
    MEM_Free(context);
}

/**
 * This routine is called on query or set power DOWN irp for the device.
 * This routine queues a workitem.
 */
STATIC NTSTATUS BT_HoldIoRequests(BtDevice * bt, IRP * irp)
{
    NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
    BtWorkerThreadContext * context = MEM_New(BtWorkerThreadContext);    
    BT_TRACE("HoldIoRequests");
    if(context) {
        PIO_WORKITEM item = IoAllocateWorkItem(bt->common.deviceObject);
        if(item) {
            context->irp = irp;
            context->bt = bt;
            context->workItem = item;
            IoMarkIrpPending(irp);            
            IoQueueWorkItem(item,BT_HoldIoRequestCB,DelayedWorkQueue,context);
            status = STATUS_PENDING;
        } else {
            MEM_Free(context);
        }
    }
    return status;
}

/**
 * This is the PoRequest - completion routine for the device power irp.
 * This routine is responsible for completing the system power irp, 
 * received as a context.
 */
STATIC
VOID
BT_DevPoCompletionRoutine(
    PDEVICE_OBJECT DeviceObject, 
    UCHAR MinorFunction,
    POWER_STATE PowerState,
    PVOID Context,
    PIO_STATUS_BLOCK IoStatus)
{
    BtCompletionContext * powerContext = Context;
    BtDevice * bt = powerContext->bt;
    PIRP irp = powerContext->irp;    
    BT_TRACE("DevPoCompletionRoutine");
    ASSERT(bt->PhysicalDeviceObject == DeviceObject);

    irp->IoStatus.Status = IoStatus->Status;
    PoStartNextPowerIrp(irp);

    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    BT_IoDecrement(bt);
    MEM_Free(powerContext);
}

/**
 * This routine is invoked from the completion routine of the system power
 * irp. This routine will PoRequest a device power irp. The system irp is 
 * passed as a context to the the device power irp.
 */
STATIC VOID BT_SendDeviceIrp(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    POWER_STATE powState;
    BtCompletionContext * powerContext;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    SYSTEM_POWER_STATE sysState = irpStack->Parameters.Power.State.SystemState;
    DEVICE_POWER_STATE devState = bt->DeviceCapabilities.DeviceState[sysState];

    BT_TRACE("SendDeviceIrp");

    // Read out the D-IRP out of the S->D mapping array captured in QueryCap's.
    // we can choose deeper sleep states than our mapping but never choose
    // lighter ones.
    powState.DeviceState = devState;
    powerContext = MEM_New(BtCompletionContext);
    if (!powerContext) {
        status = STATUS_INSUFFICIENT_RESOURCES;
    } else {
        powerContext->bt = bt;
        powerContext->irp = irp;
        status = PoRequestPowerIrp(
                            bt->PhysicalDeviceObject, 
                            irpStack->MinorFunction,
                            powState,
                            BT_DevPoCompletionRoutine,
                            powerContext, 
                            NULL);
    }

    if (!NT_SUCCESS(status)) {
        if(powerContext) {
            MEM_Free(powerContext);
        }
        PoStartNextPowerIrp(irp);
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        BT_IoDecrement(bt);
    }
}

/**
 * This is the completion routine for the system power irps of minor
 * function types IRP_MN_QUERY_POWER and IRP_MN_SET_POWER.
 * This completion routine sends the corresponding device power irp and
 * returns STATUS_MORE_PROCESSING_REQUIRED. The system irp is passed as a
 * context to the device power irp completion routine and is completed in
 * the device power irp completion routine.
 */
STATIC 
NTSTATUS BT_SysPoCompletionRoutine(PDEVICE_OBJECT dev, PIRP irp, PVOID ctx)
{
    NTSTATUS status = irp->IoStatus.Status;
 	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    BtDevice * bt = ctx;
    ASSERT(ctx == dev->DeviceExtension);

    BT_TRACE1("SysPoCompletionRoutine, status = %08lX",status);
    if (!NT_SUCCESS(status)) {
        PoStartNextPowerIrp(irp);
        BT_IoDecrement(bt);
        return STATUS_SUCCESS;
    }

    // ..otherwise update the cached system power state (IRP_MN_SET_POWER)
    if (irpStack->MinorFunction == IRP_MN_SET_POWER) {
        bt->Power.SystemState = irpStack->Parameters.Power.State.SystemState;
    }

    // queue device irp and return STATUS_MORE_PROCESSING_REQUIRED
    BT_SendDeviceIrp(bt, irp);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * This routine services irps of minor type IRP_MN_SET_POWER for the 
 * system power state
 */
STATIC NTSTATUS BT_SystemSetPower(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack =  IoGetCurrentIrpStackLocation(irp);
    SYSTEM_POWER_STATE sysState = irpStack->Parameters.Power.State.SystemState;
    
    BT_TRACE("SystemSetPower");
    BT_TRACE2("Set request for system power state S%X"
           "Current system power state S%X\n", 
           sysState - 1, bt->Power.SystemState - 1);

    IoCopyCurrentIrpStackLocationToNext(irp);
    IoSetCompletionRoutine(irp,BT_SysPoCompletionRoutine,bt,TRUE,TRUE,TRUE);
    status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
    return STATUS_PENDING;
}

/**
 * This routine services irps of minor type IRP_MN_QUERY_POWER
 * for the device power state
 */
STATIC NTSTATUS BtDeviceQueryPower(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    DEVICE_POWER_STATE devState = irpStack->Parameters.Power.State.DeviceState;

    BT_TRACE("DeviceQueryPower");
    BT_TRACE2("Query for device power state D%X"
           "Current device power state D%X\n",
           devState - 1, bt->Power.DeviceState - 1);

    if (devState < bt->Power.DeviceState) {
        status = STATUS_SUCCESS;
    } else {
        status = BT_HoldIoRequests(bt, irp);
        if (status == STATUS_PENDING) {
            return status;
        }
    }

    // on error complete the Irp.
    // on success pass it to the lower layers
    PoStartNextPowerIrp(irp);
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    if (!NT_SUCCESS(status)) {
        IoCompleteRequest(irp, IO_NO_INCREMENT);
    } else {
        IoSkipCurrentIrpStackLocation(irp);
        status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
    }
    BT_IoDecrement(bt);
    return status;
}

/**
 * This routine processes queue of pending irps.
 */
STATIC NTSTATUS BT_SetDeviceFunctional(BtDevice * bt, IRP * irp)
{
    NTSTATUS status = irp->IoStatus.Status;
    PIO_STACK_LOCATION irpStack =  IoGetCurrentIrpStackLocation(irp);
    POWER_STATE newState = irpStack->Parameters.Power.State;
    DEVICE_POWER_STATE newDevState = newState.DeviceState;
    DEVICE_POWER_STATE oldDevState = bt->Power.DeviceState;

    BT_TRACE("SetDeviceFunctional");

    // update the cached state
    bt->Power.DeviceState = newDevState;

    // restore appropriate amount of state to our h/w
    // this driver does not implement partial context
    // save/restore.
    PoSetPowerState(bt->common.deviceObject, DevicePowerState, newState);
    PoStartNextPowerIrp(irp);
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    BT_IoDecrement(bt);
    return STATUS_SUCCESS;
}

/**
 * Completion routine for the device power UP irp with minor function
 * IRP_MN_SET_POWER.
 */
STATIC NTSTATUS BT_FinishDevPoUpIrp(DEVICE_OBJECT * dev, IRP* irp, VOID* ctx)
{
    NTSTATUS status = irp->IoStatus.Status;
    BtDevice * bt = dev->DeviceExtension;
    BT_TRACE("FinishDevPoUpIrp");
    ASSERT(bt == ctx);

    if (irp->PendingReturned) {
        IoMarkIrpPending(irp);
    }
    if (!NT_SUCCESS(status)) {
        PoStartNextPowerIrp(irp);
        BT_IoDecrement(bt);
        return STATUS_SUCCESS;
    }

    BT_SetDeviceFunctional(bt, irp);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * Handles IRP_MN_SET_POWER requests for the device power state
 */
STATIC NTSTATUS BtDeviceSetPower(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    POWER_STATE newState = irpStack->Parameters.Power.State;    
    DEVICE_POWER_STATE oldDevState = bt->Power.DeviceState;
    DEVICE_POWER_STATE newDevState = newState.DeviceState;

    BT_TRACE("DeviceSetPower");
    BT_TRACE2("Set request for device power state D%X"
            "Current device power state D%X\n", newDevState - 1,
            bt->Power.DeviceState - 1);

    if (newDevState < oldDevState) {
        BT_TRACE("Adding power to the device");
        // send the power IRP to the next driver in the stack
        IoCopyCurrentIrpStackLocationToNext(irp);
        IoSetCompletionRoutine(irp,BT_FinishDevPoUpIrp,bt,TRUE,TRUE,TRUE);
        status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
	} else {

        // newDevState >= oldDevState 
        //
        // hold I/O if transition from D0 -> DX (X = 1, 2, 3)
        // if transition from D1 or D2 to deeper sleep states, 
        // I/O queue is already on hold.

        if (oldDevState == PowerDeviceD0 && newDevState > oldDevState) {
            // D0 -> DX transition
            BT_TRACE("Removing power from the device");
            status = BT_HoldIoRequests(bt, irp);
            if (!NT_SUCCESS(status)) {
                PoStartNextPowerIrp(irp);
                irp->IoStatus.Status = status;
                irp->IoStatus.Information = 0;
                IoCompleteRequest(irp, IO_NO_INCREMENT);
                BT_IoDecrement(bt);
                return status;
            } else {
                return STATUS_PENDING;
            }

        } else if (oldDevState == PowerDeviceD0 &&  
                   newDevState == PowerDeviceD0) {
            //
            // D0 -> D0
            // unblock the queue which may have been blocked processing
            // query irp
            BT_TRACE("SetD0 request");
        }   

        IoCopyCurrentIrpStackLocationToNext(irp);
        IoSetCompletionRoutine(irp,BT_FinishDevPoDnIrp,bt,TRUE,TRUE,TRUE);
        status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
        ASSERT(NT_SUCCESS(status));
    }

    return STATUS_PENDING;
}

/**
 * Handles IRP_MN_QUERY_POWER for the system power states.
 */
STATIC NTSTATUS BT_SystemQueryPower(BtDevice * bt, IRP * irp)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    SYSTEM_POWER_STATE sysState = irpStack->Parameters.Power.State.SystemState;
    
    BT_TRACE("SystemQueryPower");
    BT_TRACE2("Query for system power state S%X"
            "Current system power state S%X\n", sysState - 1,
            bt->Power.SystemState - 1);

    IoCopyCurrentIrpStackLocationToNext(irp);
    IoSetCompletionRoutine(irp,BT_SysPoCompletionRoutine,bt,TRUE,TRUE,TRUE);
    status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
    return STATUS_PENDING;
}

/**
 * The power dispatch routine.
 */
NTSTATUS BT_ComDispatchPower(BtDeviceBase* dev, IRP* irp)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    BtDevice* bt = CAST(dev, BtDevice, common);

    BT_TRACE("DispatchPower");

    if (bt->deviceState == DeviceRemoved) {
        // Even if a driver fails the IRP, it must nevertheless call
        // PoStartNextPowerIrp to inform the Power Manager that it
        // is ready to handle another power IRP.
        PoStartNextPowerIrp(irp);
        irp->IoStatus.Status = status = STATUS_DELETE_PENDING;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }

    // if the device is not started yet, pass it down
    if (bt->deviceState == DeviceNotStarted) {
        PoStartNextPowerIrp(irp);
        IoSkipCurrentIrpStackLocation(irp);
        return PoCallDriver(bt->TopOfStackDeviceObject, irp);
    }

    BT_IoIncrement(bt);
    switch(irpStack->MinorFunction) {    
    case IRP_MN_SET_POWER:

        //
        // The Power Manager sends this IRP for one of the
        // following reasons:
        // 1) To notify drivers of a change to the system power state.
        // 2) To change the power state of a device for which
        //    the Power Manager is performing idle detection.
        // A driver sends IRP_MN_SET_POWER to change the power
        // state of its device if it's a power policy owner for the
        // device.
        //

        BT_TRACE("IRP_MN_SET_POWER");
        IoMarkIrpPending(irp);
        switch(irpStack->Parameters.Power.Type) {
        case SystemPowerState:
            BT_SystemSetPower(bt, irp);
            status = STATUS_PENDING;
            break;

        case DevicePowerState:
            BtDeviceSetPower(bt, irp);
            status = STATUS_PENDING;
            break;

        default:
            status = BT_CompleteInvalidRequest(bt, irp);
            break;
        }

        break;

    case IRP_MN_QUERY_POWER:

        //
        // The Power Manager sends a power IRP with the minor
        // IRP code IRP_MN_QUERY_POWER to determine whether it
        // can safely change to the specified system power state
        // (S1-S5) and to allow drivers to prepare for such a change.
        // If a driver can put its device in the requested state,
        // it sets status to STATUS_SUCCESS and passes the IRP down.
        //

        BT_TRACE("IRP_MN_QUERY_POWER");
        IoMarkIrpPending(irp);
        switch(irpStack->Parameters.Power.Type) {
        case SystemPowerState:
            BT_SystemQueryPower(bt, irp);
            status = STATUS_PENDING;
            break;

        case DevicePowerState:
            BtDeviceQueryPower(bt, irp);
            status = STATUS_PENDING;
            break;

        default:
            status = BT_CompleteInvalidRequest(bt, irp);
            break;
        }
        break;

    case IRP_MN_WAIT_WAKE:

        //
        // The minor power IRP code IRP_MN_WAIT_WAKE provides
        // for waking a device or waking the system. Drivers
        // of devices that can wake themselves or the system
        // send IRP_MN_WAIT_WAKE. The system sends IRP_MN_WAIT_WAKE
        // only to devices that always wake the system, such as
        // the power-on switch.
        //

    case IRP_MN_POWER_SEQUENCE:

        // A driver sends this IRP as an optimization to determine
        // whether its device actually entered a specific power state.
        // This IRP is optional. Power Manager cannot send this IRP.

    default:
        PoStartNextPowerIrp(irp);
        IoSkipCurrentIrpStackLocation(irp);
        status = PoCallDriver(bt->TopOfStackDeviceObject, irp);
        if (!NT_SUCCESS(status)) {
            ASSMSG1("lower drivers failed Irp, err %08lX",status);
        }        
        BT_IoDecrement(bt);
        break;
    }

    return status;
}
