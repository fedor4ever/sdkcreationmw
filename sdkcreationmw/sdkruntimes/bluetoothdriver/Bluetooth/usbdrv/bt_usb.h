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



#ifndef _S60BT_H_
#define _S60BT_H_ 1

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;

#if _MSC_VER < 1300
/* little trickery to compile XP DDK with VC6 compiler */
#   undef _MSC_VER
#   define _MSC_VER 1300
#   define DECLSPEC_ALIGN(foo)
#   define _W64
#endif /* _MSC_VER < 1300 */

#include "s_lib.h"
#include "usb.h"
#include "usbdlib.h"
#include "usbdrivr.h"
#include "ntddser.h"

#include "bt_ioctl.h"
#include "bt_ver.h"

/* debug trace */
#if DEBUG

#  ifndef BT_DEBUG_PREFIX
#    define BT_DEBUG_PREFIX "S60BT: "
#  endif /* BT_DEBUG_PREFIX */

extern int BT_DbgPrint(const char* format, ...);
#  define BT_DUMP(data,len) \
        PRINT_Dump(BT_DbgPrint,data,len,0)
#  define BT_TRACE(s) \
        BT_DbgPrint(BT_DEBUG_PREFIX##"%s\n",s)
#  define BT_TRACE0(s) \
        BT_DbgPrint(BT_DEBUG_PREFIX##"%s\n",s)
#  define BT_TRACE1(s,p1) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1)
#  define BT_TRACE2(s,p1,p2) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1,p2)
#  define BT_TRACE3(s,p1,p2,p3) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1,p2,p3)
#  define BT_TRACE4(s,p1,p2,p3,p4) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1,p2,p3,p4)
#  define BT_TRACE5(s,p1,p2,p3,p4,p5) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1,p2,p3,p4,p5)
#  define BT_TRACE6(s,p1,p2,p3,p4,p5,p6) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1,p2,p3,p4,p5,p6)
#  define BT_TRACE7(s,p1,p2,p3,p4,p5,p6,p7) \
        BT_DbgPrint(BT_DEBUG_PREFIX##s##"\n",p1,p2,p3,p4,p5,p6,p7)
#else  /* DEBUG */
#  define BT_DUMP(data,len)                 NOTHING
#  define BT_TRACE(s)                       NOTHING
#  define BT_TRACE0(s)                      NOTHING
#  define BT_TRACE1(s,p1)                   NOTHING
#  define BT_TRACE2(s,p1,p2)                NOTHING
#  define BT_TRACE3(s,p1,p2,p3)             NOTHING
#  define BT_TRACE4(s,p1,p2,p3,p4)          NOTHING
#  define BT_TRACE5(s,p1,p2,p3,p4,p5)       NOTHING
#  define BT_TRACE6(s,p1,p2,p3,p4,p5,p6)    NOTHING
#  define BT_TRACE7(s,p1,p2,p3,p4,p5,p6,p7) NOTHING
#endif /* DEBUG */

#if DEBUG
#  define RELEASE_TRY 
#  define RELEASE_CATCH(_status)
#else /* !DEBUG */
#  define RELEASE_TRY __try
#  define RELEASE_CATCH(_status) \
    __except(EXCEPTION_EXECUTE_HANDLER) { \
        _status = GetExceptionCode(); \
    }
#endif /* !DEBUG */

/*
 * Timeout for transaction over control pipe
 */
#define BT_USB_TIMEOUT 200  /* milliseconds */

/**
 * Endpoints
 */
#define BT_HCI_COMMAND_ENDPOINT             0x00    /* out */
#define BT_HCI_EVENT_ENDPOINT               0x81    /* in  */
#define BT_HCI_ACL_OUT_ENDPOINT             0x02    /* out */
#define BT_HCI_ACL_IN_ENDPOINT              0x82    /* in  */
#define BT_HCI_SCO_OUT_ENDPOINT             0x03    /* out */
#define BT_HCI_SCO_IN_ENDPOINT              0x83    /* in  */

/*
 * Device class codes for Bluetooth wireless controllers.
 * See http://www.usb.org/developers/defined_class
 */
#define BT_DEVICE_CLASS_WIRELESS_CONTROLLER 0xe0
#define BT_DEVICE_SUBCLASS_RF_CONTROLLER    0x01
#define BT_DEVICE_PROTOCOL_BLUETOOTH        0x01

#define BT_DEVICE_INTERFACE_COUNT           2
#define BT_HCI_COMMAND_INTERFACE            0
#define BT_ACL_DATA_INTERFACE               0
#define BT_SCO_DATA_INTERFACE               1

/**
 * See Bluetooth Specification part H:3
 * HCI RS232 Transport Layer
 */
#define HCI_RS232_PACKET_TYPE_COMMAND       0x01
#define HCI_RS232_PACKET_TYPE_ACL_DATA      0x02
#define HCI_RS232_PACKET_TYPE_SCO_DATA      0x03
#define HCI_RS232_PACKET_TYPE_EVENT         0x04
#define HCI_RS232_PACKET_TYPE_MESSAGE       0x05
#define HCI_RS232_PACKET_TYPE_NEGOTIATION   0x06

#define SERIAL_MSR_DCTS     0x01    // delta clear to send
#define SERIAL_MSR_DDSR     0x02    // delta data set ready
#define SERIAL_MSR_TERI     0x04    // trailing edge ring indicator
#define SERIAL_MSR_DDCD     0x08    // delta data carrier detect
#define SERIAL_MSR_CTS      0x10    // clear to send
#define SERIAL_MSR_DSR      0x20    // data set ready
#define SERIAL_MSR_RI       0x40    // ring indicator
#define SERIAL_MSR_DCD      0x80    // data carrier detect

#define SERIAL_MCR_DTR      0x01
#define SERIAL_MCR_RTS      0x02
#define SERIAL_MCR_OUT1     0x04
#define SERIAL_MCR_OUT2     0x08
#define SERIAL_MCR_LOOP     0x10

/**
 * We complete read requests only if both DTR and RTS flags are set (and of
 * course, the data is available). I don't quite understand why, but when
 * Symbian is not ready to receive data (for example, because it runs out
 * of space in the receive buffer), it drops RTS (Ready To Send) leaving 
 * DTR (Data Terminal Ready) up. So we require both flags up.
 */
#define SERIAL_READABLE_STATE (SERIAL_RTS_STATE | SERIAL_DTR_STATE)
#define BT_CanRead(bt) \
    (((bt)->SerialState & SERIAL_READABLE_STATE) == SERIAL_READABLE_STATE)

/**
 * According to HCI specification, the host must be able to accept
 * HCI Event Packets with up to 255 bytes of data excluding the HCI
 * Event Packet header.
 */

#define HCI_PACKET_BUFFER_SIZE              512

/**
 * Input queue longer than 256 bytes cause buffer overruns in S60 2.8 SDK.
 * Why that small? The com port logical device driver (ECOMM.LDD) has input
 * queue 1024 bytes. It will signal us that it's running out of buffer space
 * when it reaches 769 bytes watermark (75% + 1), that is, when 255 bytes are
 * left in its buffer. If at that point, what the physical device driver
 * (ECDRV.PDD) has already received plus what's sitting in the driver's
 * queue, exceeds 255 bytes AND there's no outstanding read, then buffer
 * overrun will occur in ECOMM.LDD and SocketServer will panic. Hence,
 * the maximum size of the driver's queue is 127 bytes at once. It's a
 * shame, because such a small queue slows things down quite a bit.
 */
#define SERIAL_MAX_INPUT_QUEUE_SIZE         127

typedef enum _BtDevState {
    DeviceNotStarted,			    /* not started */
    DeviceStopped,				    /* device stopped */
    DeviceWorking,				    /* started and working */
    DevicePendingStop,			    /* stop pending  */
    DevicePendingRemove,		    /* remove pending */
    DeviceSurpriseRemoved,		    /* removed by surprise */
    DeviceRemoved				    /* removed */
} BtDevState;

typedef enum _BtPacketType {
    BtPacketInvalid = -1,           /* packet format error */
    BtPacketUnknown,                /* not enough data to parse the packet */
    BtPacketIncomplete,             /* incomplete packet */
    BtPacketHciCommand,             /* HCI command packet */
    BtPacketAclData,                /* ACL data packet */
    BtPacketScoData,                /* SCO data packet */
    BtPacketHciEvent,               /* HCI event packet */
    BtPacketOther                   /* all other kinds of packets */
} BtPacketType;

typedef struct _BtPacketInfo {
    Str name;                       /* packet name, for debugging purposes */
    int header;                     /* size of non-HCI header */
    int length;                     /* size of the HCI part, 0 if unknown */
    int trailer;                    /* size of non-HCI trailer */
} BtPacketInfo;

/* packet codec definition */
typedef BtPacketType (*BtDecode)(BtPacketInfo* pkt, const UCHAR* buf, int len);
typedef Bool (*BtEncode)(Buffer* b, BtPacketType t, const UCHAR* pkt, int len);
typedef struct _BtCodec {
    Str name;
    BtEncode encode;
    BtDecode decode;
} BtCodec;

/* available codecs */
extern const BtCodec BtCodecH4;

/* asynchronous transfer context */
typedef struct _BtTransfer {
    URB* urb;
    struct _BtDevice* bt;
    UCHAR data[HCI_PACKET_BUFFER_SIZE];
} BtTransfer;

/**
 * Device specific callbacks
 */
typedef struct _BtFile BtFile;
typedef struct _BtDeviceBase BtDeviceBase;
typedef NTSTATUS (*BtDeviceDispatch)(BtDeviceBase* dev, IRP* irp);
typedef NTSTATUS (*BtFileDispatch)(BtFile* file, IRP* irp);

/**
 * Types of device objects created by the driver
 */
typedef enum _BtDeviceType {
    BtBaseDeviceType = 1,
    BtComDeviceType
} BtDeviceType;

/**
 * Common part for all device extensions
 */
struct _BtDeviceBase {

    /* Type of this device object */
    BtDeviceType type;

    /* Functional Device Object */
    PDEVICE_OBJECT deviceObject;

    /* Symbolic name for this device (\DosDevices\COM1, etc) */
    UNICODE_STRING* symLinkName;

    /* Win32 name for this device (COM1, etc) */
    UNICODE_STRING win32Name;

    /* Device mutex */
    Mutex mutex;

    /* List of file objects (BtFile) */
    Queue fileObjects;

    /* Device specific dispatch routines */
    BtDeviceDispatch dispatchCreate;
    BtFileDispatch dispatchCleanup;
    BtFileDispatch dispatchClose;
    BtFileDispatch dispatchRead;
    BtFileDispatch dispatchWrite;
    BtFileDispatch dispatchDeviceControl;
    BtDeviceDispatch dispatchPower;
    BtDeviceDispatch dispatchPnp;
};

/**
 * Device extension associated with a COM port device
 */
typedef struct _BtDevice {

    /* Common part for all types of device extensions */
    BtDeviceBase common;

    /* COM port number, e.g. 1 for COM1, etc. */
    int portNum;

    /* Device object we call when submitting Urbs */
    PDEVICE_OBJECT TopOfStackDeviceObject;

    /* The bus driver object */
    PDEVICE_OBJECT PhysicalDeviceObject;

    /*
     * Name buffer for our named Functional device object link
     * The name is generated based on the driver's class GUID
     */
    UNICODE_STRING InterfaceName;

    /*
     * Bus drivers set the appropriate values in this structure in response
     * to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
     * alter the capabilities set by the bus driver.
     */
    DEVICE_CAPABILITIES DeviceCapabilities;

    /* Device descriptor */
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;

    /* Configuration Descriptor */
    USB_CONFIGURATION_DESCRIPTOR * UsbConfigurationDescriptor;

    /* Actual strings referenced by the USB device descriptor */
    USB_STRING_DESCRIPTOR * strManufacturer;
    USB_STRING_DESCRIPTOR * strProduct;
    USB_STRING_DESCRIPTOR * strSerialNumber;

    /*
     * Interface Information structures
     * Interface 0: HCI commands, events and ACL data
     * Interface 1: SCO data
     */
    USBD_INTERFACE_INFORMATION * interfaceInfo[BT_DEVICE_INTERFACE_COUNT];
    USBD_CONFIGURATION_HANDLE  configurationHandle;
    ULONG currentInterface;

    /* USB pipes */
    USBD_PIPE_INFORMATION * eventPipe;
    USBD_PIPE_INFORMATION * aclPipeIn;
    USBD_PIPE_INFORMATION * aclPipeOut;
    USBD_PIPE_INFORMATION * scoPipeIn;
    USBD_PIPE_INFORMATION * scoPipeOut;

    /* Current state of device */
    BtDevState deviceState;

    /* State prior to removal query */
    BtDevState prevDevState;

    /* Current power state */
    POWER_STATE Power;

    /* Default power state to power down to on self-susped */
    ULONG PowerDownLevel;

    /* Various I/O stuff */
    KEVENT RemoveEvent;
    KEVENT StopEvent;
    ULONG OutStandingIO;
    IRP* eventIrp;
    IRP* aclIrp;
    IRP* scoIrp;
    IRP* waitIrp;
    IRP* readIrp;
    BtTransfer* eventTx;
    BtTransfer* aclTx;
    BtTransfer* scoTx;
    Buffer inputQueue;
    Buffer readBuffer;
    Buffer writeBuffer;
    ULONG leftover;
    const BtCodec* codec;

    /* COM port parameters */
    SERIAL_BAUD_RATE SerialBaudRate;
    SERIAL_LINE_CONTROL SerialLineControl;
    SERIAL_TIMEOUTS SerialTimeouts;
    SERIAL_CHARS SerialChars;
    SERIAL_HANDFLOW SerialHandflow;
    SERIALPERF_STATS SerialStats;
    SERIAL_QUEUE_SIZE SerialQueueSize;
    ULONG SerialWaitMask;
    ULONG SerialEventMask;
    ULONG SerialState;

} BtDevice;

struct _BtFile {
    QEntry entry;
    BtDeviceBase* dev;
    FILE_OBJECT* fileObject;
};

typedef struct _BtCompletionContext {
    BtDevice * bt;
    IRP * irp;
} BtCompletionContext;

typedef struct _BtWorkerThreadContext {
    BtDevice * bt;
    IRP * irp;
    PIO_WORKITEM workItem;
} BtWorkerThreadContext;

extern LONG BT_IoIncrement(BtDevice * v2u);
extern LONG BT_IoDecrement(BtDevice * v2u);
extern IRP * BT_CreateUsbSubmitRequest(PDEVICE_OBJECT dev, URB * urb);
extern VOID BT_CancelAll(BtDevice * v2u);
extern NTSTATUS BT_TimeCallUSBD(BtDevice * d, BtFile * f, URB * u, int t);
extern NTSTATUS BT_TimeCallUSBD(BtDevice * d, BtFile * f, URB * u, int t);
extern NTSTATUS BT_CallUSBD(BtDevice * v2u, BtFile * f, URB * urb);
extern NTSTATUS BT_IOComplete(PDEVICE_OBJECT dev, IRP * irp, PVOID ctx);
extern NTSTATUS BT_CompleteInvalidRequest(BtDevice * v2u, IRP * irp);
extern NTSTATUS BT_ComDispatchPnP(BtDeviceBase * dev, IRP * irp);
extern NTSTATUS BT_ComDispatchPower(BtDeviceBase * dev, IRP * irp);
extern NTSTATUS BT_ComDispatchDeviceControl(BtFile* file, IRP * irp);
extern NTSTATUS BT_BaseDispatchDeviceControl(BtFile* file, IRP * irp);
extern ULONG    BT_LockedRead(BtDevice* bt, UCHAR* dest, ULONG maxlen);
extern BOOLEAN  BT_FillInputQueue(BtDevice* bt);
extern NTSTATUS BT_Write(BtDevice* bt, const void* data, ULONG len);
extern BOOLEAN  BT_UpdateEventMask(BtDevice* bt, ULONG event);
extern BOOLEAN  BT_SubmitEventRequest(BtDevice * bt);
extern BOOLEAN  BT_SubmitAclRequest(BtDevice * bt);
extern BOOLEAN  BT_SubmitScoRequest(BtDevice * bt);
extern NTSTATUS BT_SelectInterface(BtDevice * bt, UCHAR interfaceNumber);
extern BOOLEAN  BT_IsSerialDeviceControl(BtDevice* bt, IRP * irp,
                                         IO_STATUS_BLOCK* ioStatus);

/* HCI utilities */
#define HCI_PACKET_INCOMPLETE   (0)
#define HCI_PACKET_CORRUPT      (-1)
extern int BT_HciCommandPacketLength(const UCHAR* pkt, int len);
extern int BT_HciEventPacketLength(const UCHAR* pkt, int len);
extern int BT_HciAclDataPacketLength(const UCHAR* pkt, int len);
extern int BT_HciScoDataPacketLength(const UCHAR* pkt, int len);
typedef Bool (*BtPacketLengthProc)(const UCHAR* pkt, int len);

#endif /* _S60BT_H_ */
