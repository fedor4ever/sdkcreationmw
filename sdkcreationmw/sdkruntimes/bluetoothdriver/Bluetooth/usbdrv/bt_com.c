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
* Description:  Serial port emulation
 *
*/


#include "bt_usb.h"

/**
 * This routine produces the human readable name for some SERIAL ioctl codes
 */
#if DEBUG
#define SERIAL_IOCTL_NAME(foo) case foo: return #foo " (unsupported)"
STATIC Str BT_SerialIoctl(ULONG code)
{
    static char unknown[32];
    switch (code) {
    SERIAL_IOCTL_NAME(IOCTL_SERIAL_RESET_DEVICE);
    SERIAL_IOCTL_NAME(IOCTL_SERIAL_SET_XOFF);
    SERIAL_IOCTL_NAME(IOCTL_SERIAL_SET_XON);
    SERIAL_IOCTL_NAME(IOCTL_SERIAL_SET_BREAK_ON);
    SERIAL_IOCTL_NAME(IOCTL_SERIAL_SET_BREAK_OFF);
    default:
        Sprintf(unknown,"UNKNOWN[%08X]",code);
        return unknown;
    }
}
#endif /* DEBUG */

/**
 * Cancel routine for IOCTL_SERIAL_WAIT_ON_MASK requests
 */
STATIC VOID BT_SerialCancelWait(DEVICE_OBJECT* dev, IRP* irp)
{
    BtDevice * bt = dev->DeviceExtension;
    BT_TRACE1("wait Irp %08X cancelled",irp);
    MUTEX_Lock(&bt->common.mutex);
    if (bt->waitIrp == irp) {
        bt->waitIrp = NULL;
    }
    MUTEX_Unlock(&bt->common.mutex);
    IoReleaseCancelSpinLock(irp->CancelIrql);
    irp->IoStatus.Status = STATUS_CANCELLED;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_SERIAL_INCREMENT);
}

/**
 * This function updates the event mask, the wait mask or both and
 * completes the wait IRP if necessary. Event bits are ORed with the
 * current event mask. Returns TRUE if the current wait IRP has been
 * completed, FALSE otherwise.
 */
BOOLEAN BT_UpdateEventMask(BtDevice* bt, ULONG event)
{
    IRP* waitIrpDone = NULL;
    ULONG events;

    MUTEX_Lock(&bt->common.mutex);
    bt->SerialEventMask |= event;
    events = (bt->SerialWaitMask & bt->SerialEventMask);
    if (events && bt->waitIrp) {
        /* remove reported events from the event masks */
        bt->SerialEventMask &= ~events;
        waitIrpDone = bt->waitIrp;
        bt->waitIrp = NULL;
    }
    MUTEX_Unlock(&bt->common.mutex);

    if (waitIrpDone) {
        ULONG* result = waitIrpDone->AssociatedIrp.SystemBuffer;
        *result = events;
        BT_TRACE2("wait IRP %08X done, event %04X",waitIrpDone,events);
        waitIrpDone->IoStatus.Status = STATUS_SUCCESS;
        waitIrpDone->IoStatus.Information = sizeof(ULONG);
        IoCompleteRequest(waitIrpDone, IO_SERIAL_INCREMENT);
        return TRUE;
    }

    return FALSE;
}

/**
 * Fills the input queue with received data. Symbian assumes that the input
 * queue is not larger than bt->SerialQueueSize.InSize, otherwise buffer
 * overrun will occur. Returns TRUE if the queue has been updated. Note that
 * even if returns FALSE, it doesn't mean that the queue is empty. It only
 * means that it hasn't been updated. The caller must hold the mutex.
 *
 * NOTE: this function updates SerialEventMask with SERIAL_EV_RXCHAR flag
 * if it moves any data to the input queue.
 */
BOOLEAN BT_FillInputQueue(BtDevice* bt)
{
    if (BT_CanRead(bt)) {
        /* Keep no more than bt->SerialQueueSize.InSize bytes in 
         * the input queue */
        ULONG bytesInQueue = BUFFER_Size(&bt->inputQueue);
        if (bytesInQueue < bt->SerialQueueSize.InSize) {
            ULONG bytesToMove = BUFFER_Size(&bt->readBuffer);
            ULONG maxBytesToMove = bt->SerialQueueSize.InSize - bytesInQueue;
            if (bytesToMove > maxBytesToMove) bytesToMove = maxBytesToMove;
            if (bytesToMove > 0) {
                BT_TRACE1("moving %d bytes to the input queue", bytesToMove);
                BUFFER_Move(&bt->readBuffer, &bt->inputQueue, bytesToMove);
                bt->SerialEventMask |= SERIAL_EV_RXCHAR;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * Sets DTR and RTS flags. We complete read requests only if both DTR 
 * and RTS flags are set. I don't quite understand why, but when Symbian
 * runs out of space in the receive buffer, i.e. not read to receive, it
 * drops RTS (Ready To Send) leaving DTR (Data Terminal Ready) up.
 */
STATIC void BT_SetRTSDTR(BtDevice* bt, ULONG flags)
{
    IRP* waitIrpDone = NULL;
    IRP* readIrpDone = NULL;
    ULONG flagsChanged;
    ULONG eventsOccured;
    ULONG bytesRead;

    MUTEX_Lock(&bt->common.mutex);
    flagsChanged = flags & ~(bt->SerialState & flags);
    if (flagsChanged) {
        bt->SerialState |= flagsChanged;
        BT_FillInputQueue(bt);
    }

    eventsOccured = (bt->SerialWaitMask & bt->SerialEventMask);
    if (eventsOccured && bt->waitIrp) {
        /* remove reported events from the event mask */
        bt->SerialEventMask &= ~eventsOccured;
        waitIrpDone = bt->waitIrp;
        bt->waitIrp = NULL;
    }

    if (bt->readIrp) {
        IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(bt->readIrp);
        ULONG length = irpSp->Parameters.Read.Length;
        void* dest = bt->readIrp->AssociatedIrp.SystemBuffer;
        bytesRead = BT_LockedRead(bt, dest, length);
        if (bytesRead > 0) {
            readIrpDone = bt->readIrp;
            bt->readIrp = NULL;
            readIrpDone->IoStatus.Status = STATUS_SUCCESS;
            readIrpDone->IoStatus.Information = bytesRead;
        }
    }
    MUTEX_Unlock(&bt->common.mutex);

    if (readIrpDone) {
        BT_TRACE2("read IRP %08X done, %d bytes",readIrpDone,bytesRead);
        IoCompleteRequest(readIrpDone, IO_SERIAL_INCREMENT);
    }

    if (waitIrpDone) {
        ULONG* result = waitIrpDone->AssociatedIrp.SystemBuffer;
        *result = eventsOccured;
        BT_TRACE2("wait IRP %08X done, event %04X",waitIrpDone,eventsOccured);
        waitIrpDone->IoStatus.Status = STATUS_SUCCESS;
        waitIrpDone->IoStatus.Information = sizeof(ULONG);
        IoCompleteRequest(waitIrpDone, IO_SERIAL_INCREMENT);
    }
}

/**
 * Handles serial I/O controls. If it is, updates IoStatus and returns 
 * TRUE. Otherwise, returns FALSE.
 */
BOOLEAN BT_IsSerialDeviceControl(BtDevice* bt, IRP* irp,
                                 IO_STATUS_BLOCK* status)
{
    IO_STACK_LOCATION* irpSp = IoGetCurrentIrpStackLocation(irp);
    ULONG code = irpSp->Parameters.DeviceIoControl.IoControlCode;
    PVOID ioBuffer = irp->AssociatedIrp.SystemBuffer;
    ULONG inBufLen = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outBufLen = irpSp->Parameters.DeviceIoControl.OutputBufferLength;

    status->Status = STATUS_SUCCESS;
    status->Information = 0;

    switch (code) {
    case IOCTL_SERIAL_SET_BAUD_RATE:
        BT_TRACE("IOCTL_SERIAL_SET_BAUD_RATE");
        if (inBufLen < sizeof(SERIAL_BAUD_RATE)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            bt->SerialBaudRate = *((SERIAL_BAUD_RATE*)ioBuffer);
        }
        return TRUE;

    case IOCTL_SERIAL_GET_BAUD_RATE:
        BT_TRACE("IOCTL_SERIAL_GET_BAUD_RATE");
        if (outBufLen < sizeof(SERIAL_BAUD_RATE)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((SERIAL_BAUD_RATE*)ioBuffer) = bt->SerialBaudRate;
            status->Information = sizeof(SERIAL_BAUD_RATE);
        }
        return TRUE;

    case IOCTL_SERIAL_SET_MODEM_CONTROL:
        BT_TRACE("IOCTL_SERIAL_SET_MODEM_CONTROL");
        if (inBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        }
        return TRUE;

    case IOCTL_SERIAL_SET_FIFO_CONTROL:
        BT_TRACE("IOCTL_SERIAL_SET_FIFO_CONTROL");
        if (inBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        }
        return TRUE;

    case IOCTL_SERIAL_SET_LINE_CONTROL:
        BT_TRACE("IOCTL_SERIAL_SET_LINE_CONTROL");
        if (inBufLen < sizeof(SERIAL_LINE_CONTROL)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            bt->SerialLineControl = *((SERIAL_LINE_CONTROL*)ioBuffer);
        }
        return TRUE;

    case IOCTL_SERIAL_GET_LINE_CONTROL:
        BT_TRACE("IOCTL_SERIAL_GET_LINE_CONTROL");
        if (outBufLen < sizeof(SERIAL_LINE_CONTROL)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((SERIAL_LINE_CONTROL*)ioBuffer) = bt->SerialLineControl;
            status->Information = sizeof(SERIAL_LINE_CONTROL);
        }
        return TRUE;

    case IOCTL_SERIAL_SET_TIMEOUTS:
        BT_TRACE("IOCTL_SERIAL_SET_TIMEOUTS");
        if (inBufLen < sizeof(SERIAL_TIMEOUTS)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            SERIAL_TIMEOUTS* timeouts = ioBuffer;
            if ((timeouts->ReadIntervalTimeout == MAXULONG) &&
                (timeouts->ReadTotalTimeoutMultiplier == MAXULONG) &&
                (timeouts->ReadTotalTimeoutConstant == MAXULONG)) {
                status->Status = STATUS_INVALID_PARAMETER;
            } else {
                bt->SerialTimeouts = *timeouts;
            }
        }
        return TRUE;

    case IOCTL_SERIAL_GET_TIMEOUTS:
        BT_TRACE("IOCTL_SERIAL_GET_TIMEOUTS");
        if (outBufLen < sizeof(SERIAL_TIMEOUTS)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((SERIAL_TIMEOUTS*)ioBuffer) = bt->SerialTimeouts;
            status->Information = sizeof(SERIAL_TIMEOUTS);
        }
        return TRUE;

    case IOCTL_SERIAL_SET_CHARS:
        BT_TRACE("IOCTL_SERIAL_SET_CHARS");
        if (inBufLen < sizeof(SERIAL_CHARS)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            bt->SerialChars = *((SERIAL_CHARS*)ioBuffer);
        }
        return TRUE;

    case IOCTL_SERIAL_GET_CHARS:
        BT_TRACE("IOCTL_SERIAL_GET_CHARS");
        if (outBufLen < sizeof(SERIAL_CHARS)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((SERIAL_CHARS*)ioBuffer) = bt->SerialChars;
            status->Information = sizeof(SERIAL_CHARS);
        }
        return TRUE;

    case IOCTL_SERIAL_SET_RTS:
        BT_TRACE("IOCTL_SERIAL_SET_RTS");
        BT_SetRTSDTR(bt, SERIAL_RTS_STATE);
        return TRUE;

    case IOCTL_SERIAL_CLR_RTS:
        BT_TRACE("IOCTL_SERIAL_CLR_RTS");
        MUTEX_Lock(&bt->common.mutex);
        bt->SerialState &= ~SERIAL_RTS_STATE;
        MUTEX_Unlock(&bt->common.mutex);
        return TRUE;

    case IOCTL_SERIAL_SET_DTR:
        BT_TRACE("IOCTL_SERIAL_SET_DTR");
        BT_SetRTSDTR(bt, SERIAL_DTR_STATE);
        return TRUE;

    case IOCTL_SERIAL_CLR_DTR:
        BT_TRACE("IOCTL_SERIAL_CLR_DTR");
        MUTEX_Lock(&bt->common.mutex);
        bt->SerialState &= ~SERIAL_DTR_STATE;
        MUTEX_Unlock(&bt->common.mutex);
        return TRUE;

    case IOCTL_SERIAL_RESET_DEVICE:
    case IOCTL_SERIAL_SET_XOFF:
    case IOCTL_SERIAL_SET_XON:
    case IOCTL_SERIAL_SET_BREAK_ON:
    case IOCTL_SERIAL_SET_BREAK_OFF:
        BT_TRACE1("%s",BT_SerialIoctl(code));
        return TRUE;

    case IOCTL_SERIAL_SET_QUEUE_SIZE:
        BT_TRACE("IOCTL_SERIAL_SET_QUEUE_SIZE");
        if (inBufLen < sizeof(SERIAL_QUEUE_SIZE)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            bt->SerialQueueSize = *((SERIAL_QUEUE_SIZE*)ioBuffer);
            BT_TRACE2("Queue size: in %d bytes, out %d bytes",
                bt->SerialQueueSize.InSize, bt->SerialQueueSize.OutSize);
            if (bt->SerialQueueSize.InSize > SERIAL_MAX_INPUT_QUEUE_SIZE) {
                bt->SerialQueueSize.InSize = SERIAL_MAX_INPUT_QUEUE_SIZE;
                BT_TRACE1("sorry, max input queue size is %d bytes",
                    bt->SerialQueueSize.InSize);
            }
        }
        return TRUE;

    case IOCTL_SERIAL_GET_WAIT_MASK:
        BT_TRACE("IOCTL_SERIAL_GET_WAIT_MASK");
        if (outBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((ULONG*)ioBuffer) = bt->SerialWaitMask;
            status->Information = sizeof(ULONG);
        }
        return TRUE;

    case IOCTL_SERIAL_SET_WAIT_MASK:
        BT_TRACE("IOCTL_SERIAL_SET_WAIT_MASK");
        if (inBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            ULONG mask = *((ULONG*)ioBuffer);
            if (mask & ~(SERIAL_EV_RXCHAR   |
                         SERIAL_EV_RXFLAG   |
                         SERIAL_EV_TXEMPTY  |
                         SERIAL_EV_CTS      |
                         SERIAL_EV_DSR      |
                         SERIAL_EV_RLSD     |
                         SERIAL_EV_BREAK    |
                         SERIAL_EV_ERR      |
                         SERIAL_EV_RING     |
                         SERIAL_EV_PERR     |
                         SERIAL_EV_RX80FULL |
                         SERIAL_EV_EVENT1   |
                         SERIAL_EV_EVENT2)) {
                BT_TRACE1("invalid wait mask %04X", mask);
                status->Status = STATUS_INVALID_PARAMETER;
            } else {
                IRP* oldWaitIrp;
                BT_TRACE1("wait mask %04X", mask);

                MUTEX_Lock(&bt->common.mutex);
                bt->SerialWaitMask = mask;
                oldWaitIrp = bt->waitIrp;
                bt->waitIrp = NULL;
                MUTEX_Unlock(&bt->common.mutex);

                if (oldWaitIrp) {
                    ULONG* result = oldWaitIrp->AssociatedIrp.SystemBuffer;
                    *result = 0;
                    BT_TRACE1("wait IRP %08X done",oldWaitIrp);
                    oldWaitIrp->IoStatus.Status = STATUS_SUCCESS;
                    oldWaitIrp->IoStatus.Information = sizeof(ULONG);
                    IoCompleteRequest(oldWaitIrp, IO_SERIAL_INCREMENT);
                }
            }
        }
        return TRUE;

    case IOCTL_SERIAL_WAIT_ON_MASK:
        BT_TRACE("IOCTL_SERIAL_WAIT_ON_MASK");
        if (outBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            IRP* oldIrp;

            MUTEX_Lock(&bt->common.mutex);
            /* Mark this IRP as pending */
            IoMarkIrpPending(irp);
            IoSetCancelRoutine(irp, BT_SerialCancelWait);
            oldIrp = bt->waitIrp;
            bt->waitIrp = irp;
            MUTEX_Unlock(&bt->common.mutex);

            /* Cancel old IRP and reset the event mask */
            if (oldIrp) IoCancelIrp(oldIrp);
            if (!BT_UpdateEventMask(bt, 0)) {
                BT_TRACE1("wait IRP %08X pending",irp);
            }
            status->Status = STATUS_PENDING;
        }
        return TRUE;

    case IOCTL_SERIAL_IMMEDIATE_CHAR:
        BT_TRACE("IOCTL_SERIAL_IMMEDIATE_CHAR");
        if (inBufLen < sizeof(UCHAR)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            // NOT IMPLEMENTED YET!
            ASSMSG("IOCTL_SERIAL_IMMEDIATE_CHAR not implemented");
        }
        return TRUE;

    case IOCTL_SERIAL_PURGE:
        BT_TRACE("IOCTL_SERIAL_PURGE");
        if (inBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            ULONG mask = *((ULONG*)ioBuffer);
            if ((!mask) || (mask & (~(SERIAL_PURGE_TXABORT |
                                      SERIAL_PURGE_RXABORT |
                                      SERIAL_PURGE_TXCLEAR |
                                      SERIAL_PURGE_RXCLEAR)))) {
                status->Status = STATUS_INVALID_PARAMETER;
            } else {
                if (mask & SERIAL_PURGE_RXABORT) {
                    /* Purge all read requests. */
                    IRP* readIrp;
                    BT_TRACE("purging read requests");
                    MUTEX_Lock(&bt->common.mutex);
                    readIrp = bt->readIrp;
                    bt->readIrp = NULL;
                    MUTEX_Unlock(&bt->common.mutex);
                    if (readIrp) IoCancelIrp(readIrp);
                }
                if (mask & SERIAL_PURGE_RXCLEAR) {
                    /* Purge the receive buffer */
                    BT_TRACE("purging receive buffers");
                    MUTEX_Lock(&bt->common.mutex);
                    bt->leftover = 0;
                    BUFFER_Clear(&bt->inputQueue);
                    BUFFER_Clear(&bt->readBuffer);
                    MUTEX_Unlock(&bt->common.mutex);
                }
            }
        }
        return TRUE;

    case IOCTL_SERIAL_GET_HANDFLOW:
        BT_TRACE("IOCTL_SERIAL_GET_HANDFLOW");
        if (outBufLen < sizeof(SERIAL_HANDFLOW)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((SERIAL_HANDFLOW*)ioBuffer) = bt->SerialHandflow;
            status->Information = sizeof(SERIAL_HANDFLOW);
        }
        return TRUE;

    case IOCTL_SERIAL_SET_HANDFLOW:
        if (inBufLen < sizeof(SERIAL_HANDFLOW)) {
            BT_TRACE("IOCTL_SERIAL_SET_HANDFLOW - buffer too small");
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            SERIAL_HANDFLOW* handflow = ioBuffer;
            BT_TRACE2("IOCTL_SERIAL_SET_HANDFLOW (handshake=%02X,flow=%08X)",
                handflow->ControlHandShake, handflow->FlowReplace);
            bt->SerialHandflow = *handflow;
        }
        return TRUE;

    case IOCTL_SERIAL_GET_COMMSTATUS:
        if (outBufLen < sizeof(SERIAL_STATUS)) {
            BT_TRACE("IOCTL_SERIAL_GET_COMMSTATUS - buffer too small");
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            SERIAL_STATUS* comStat = ioBuffer;
            RtlZeroMemory(comStat, sizeof(SERIAL_STATUS));
            MUTEX_Lock(&bt->common.mutex);
            comStat->AmountInInQueue = BUFFER_Size(&bt->inputQueue);
            MUTEX_Unlock(&bt->common.mutex);
            status->Information = sizeof(SERIAL_STATUS);
            BT_TRACE1("IOCTL_SERIAL_GET_COMMSTATUS (InQueue=%d)",
                comStat->AmountInInQueue);
        }
        return TRUE;

    case IOCTL_SERIAL_GET_PROPERTIES:
        BT_TRACE("IOCTL_SERIAL_GET_PROPERTIES");
        if (outBufLen < sizeof(SERIAL_COMMPROP)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            SERIAL_COMMPROP* p = ioBuffer;
            status->Information = sizeof(SERIAL_COMMPROP);

            RtlZeroMemory(p, sizeof(SERIAL_COMMPROP));
            p->PacketLength       = sizeof(SERIAL_COMMPROP);
            p->PacketVersion      = 2;
            p->ServiceMask        = SERIAL_SP_SERIALCOMM;
            p->MaxBaud            = SERIAL_BAUD_USER;
            p->SettableBaud       = SERIAL_BAUD_9600    |
                                    SERIAL_BAUD_14400   |
                                    SERIAL_BAUD_19200   |
                                    SERIAL_BAUD_38400   |
                                    SERIAL_BAUD_56K     |
                                    SERIAL_BAUD_128K    |
                                    SERIAL_BAUD_115200  |
                                    SERIAL_BAUD_57600   |
                                    SERIAL_BAUD_USER;

            p->ProvSubType        = SERIAL_SP_RS232;
            p->ProvCapabilities   = SERIAL_PCF_DTRDSR |
                                    SERIAL_PCF_RTSCTS |
                                    SERIAL_PCF_CD     |
                                    SERIAL_PCF_PARITY_CHECK |
                                    SERIAL_PCF_TOTALTIMEOUTS |
                                    SERIAL_PCF_INTTIMEOUTS;
            p->SettableParams     = SERIAL_SP_PARITY |
                                    SERIAL_SP_BAUD |
                                    SERIAL_SP_DATABITS |
                                    SERIAL_SP_STOPBITS |
                                    SERIAL_SP_HANDSHAKING |
                                    SERIAL_SP_PARITY_CHECK |
                                    SERIAL_SP_CARRIER_DETECT;
            p->SettableData       = SERIAL_DATABITS_5 |
                                    SERIAL_DATABITS_6 |
                                    SERIAL_DATABITS_7 |
                                    SERIAL_DATABITS_8;
            p->SettableStopParity = SERIAL_STOPBITS_10 |
                                    SERIAL_STOPBITS_15 |
                                    SERIAL_STOPBITS_20 |
                                    SERIAL_PARITY_NONE |
                                    SERIAL_PARITY_ODD  |
                                    SERIAL_PARITY_EVEN |
                                    SERIAL_PARITY_MARK |
                                    SERIAL_PARITY_SPACE;
        }
        return TRUE;

    case IOCTL_SERIAL_XOFF_COUNTER:
        BT_TRACE("IOCTL_SERIAL_XOFF_COUNTER");
        if (inBufLen < sizeof(PSERIAL_XOFF_COUNTER)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        }
        return TRUE;

    case IOCTL_SERIAL_LSRMST_INSERT:
        BT_TRACE("IOCTL_SERIAL_LSRMST_INSERT");
        if (inBufLen < sizeof(UCHAR)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        }
        return TRUE;

    case IOCTL_SERIAL_GET_MODEMSTATUS:
        BT_TRACE("IOCTL_SERIAL_GET_MODEMSTATUS");
        if (outBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            ULONG msr = 0;
            MUTEX_Lock(&bt->common.mutex);
            if (bt->SerialState & SERIAL_DSR_STATE) msr |= SERIAL_MSR_DSR;
            if (bt->SerialState & SERIAL_CTS_STATE) msr |= SERIAL_MSR_CTS;
            MUTEX_Unlock(&bt->common.mutex);
            *((PULONG)ioBuffer) = msr;
            status->Information = sizeof(ULONG);
        }
        return TRUE;

    case IOCTL_SERIAL_GET_MODEM_CONTROL:
        BT_TRACE("IOCTL_SERIAL_GET_MODEM_CONTROL");
        if (outBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            ULONG mcr = 0;
            MUTEX_Lock(&bt->common.mutex);
            if (bt->SerialState & SERIAL_DTR_STATE) mcr |= SERIAL_MCR_DTR;
            if (bt->SerialState & SERIAL_RTS_STATE) mcr |= SERIAL_MCR_RTS;
            MUTEX_Unlock(&bt->common.mutex);
            *((PULONG)ioBuffer) = mcr;
            status->Information = sizeof(ULONG);
        }
        return TRUE;

    case IOCTL_SERIAL_CONFIG_SIZE:
        BT_TRACE("IOCTL_SERIAL_CONFIG_SIZE");
        if (outBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((PULONG)ioBuffer) = 0;
            status->Information = sizeof(ULONG);
        }

    case IOCTL_SERIAL_GET_DTRRTS:
        BT_TRACE("IOCTL_SERIAL_GET_DTRRTS");
        if (outBufLen < sizeof(ULONG)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            *((PULONG)ioBuffer) = bt->SerialState & 
                (SERIAL_DTR_STATE | SERIAL_RTS_STATE);
            status->Information = sizeof(ULONG);
        }
        return TRUE;

    case IOCTL_SERIAL_GET_STATS:
        BT_TRACE("IOCTL_SERIAL_GET_STATS");
        if (outBufLen < sizeof(SERIALPERF_STATS)) {
            status->Status = STATUS_BUFFER_TOO_SMALL;
        } else {
            // NOT IMPLEMENTED YET!
            ASSMSG("IOCTL_SERIAL_GET_STATS not implemented");
            *((SERIALPERF_STATS*)ioBuffer) = bt->SerialStats;
            status->Information = sizeof(ULONG);
        }
        return TRUE;

    case IOCTL_SERIAL_CLEAR_STATS:
        RtlZeroMemory(&bt->SerialStats, sizeof(bt->SerialStats));
        return TRUE;

    default:
        return FALSE;
    }
}
