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


#ifndef _S60BT_IOCTL_H_
#define _S60BT_IOCTL_H_ 1

/* Required system headers */
#ifndef __KERNEL__
#  ifdef _WIN32
#    include <windows.h>
#    include <winioctl.h>
#  endif /* Unix */
#endif /* __KERNEL__ */

typedef int BtInt32;
typedef short BtInt16;
typedef unsigned int BtUint32;
typedef unsigned short BtUint16;
typedef WCHAR BtWchar;

#define FILE_DEVICE_S60BT       33333
#define IOCTL_S60BT(x,a)        CTL_CODE(FILE_DEVICE_S60BT,(0x70+x),METHOD_BUFFERED,a)
#define IOCTL_S60BT_R(x,type)   IOCTL_S60BT(x,FILE_READ_ACCESS)
#define IOCTL_S60BT_W(x,type)   IOCTL_S60BT(x,FILE_WRITE_ACCESS)
#define IOCTL_S60BT_WR(x,type)  IOCTL_S60BT(x,FILE_ANY_ACCESS)

/* IOCTL definitions */

/* 
 * IOCTL_S60BT_VERSION
 *
 * Returns version information
 */
typedef struct _BtVersion {
    BtUint32 major;
    BtUint32 minor;
    BtUint32 micro;
    BtUint32 nano;
} BtVersion;

#define IOCTL_S60BT_VERSION         IOCTL_S60BT_R(0,BtVersion)

/* 
 * IOCTL_S60BT_GET_PORT_LIST
 *
 * Returns COM port information. Minimum buffer size is the size of 
 * BtPortList. If there's enough room in the buffer, it's followed
 * by an array of BtPortInfo structures.
 */
typedef struct _BtPortList {
    BtUint32 totalCount;        /* total number of ports */
    BtUint32 availCount;        /* number of available ports */
    BtUint32 infoCount;         /* number of BtPortInfo structures followed */
} BtPortList;

typedef struct _BtPortInfo {
    BtUint32 portNum;           /* COM port number, e.g. 1 for COM1, etc. */
    BtUint32 openCount;         /* open count for this port, 0 if available */
    BtUint16 vid;               /* USB vendor ID */
    BtUint16 pid;               /* USB product ID */
    BtWchar manufacturer[128];  /* manufacturer name, if available */
    BtWchar productName[128];   /* product name, if available */
    BtWchar serialNumber[128];  /* serial number, if available */
} BtPortInfo;

#define IOCTL_S60BT_GET_PORT_LIST   IOCTL_S60BT_R(1,BtPortList)

#endif /* _S60BT_IOCTL_H_ */
