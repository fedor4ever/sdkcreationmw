/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HCI UART (otherwise known as H4) protocol codec
 *
*/


#include "bt_usb.h"

/**
 * See Bluetooth Specification part H:4
 * HCI UART Transport Layer
 */
#define HCI_UART_PACKET_TYPE_COMMAND        0x01
#define HCI_UART_PACKET_TYPE_ACL_DATA       0x02
#define HCI_UART_PACKET_TYPE_SCO_DATA       0x03
#define HCI_UART_PACKET_TYPE_EVENT          0x04

typedef struct _BtFrameTypeH4 {
    Str name;
    BtPacketType type;
    BtPacketLengthProc length;
} BtFrameTypeH4;

/**
 * H4 frame descriptors
 */
STATIC const BtFrameTypeH4 BtFrameH4[5] = {
    { 0 }, /* dummy entry */
    { "HCI Command", BtPacketHciCommand, BT_HciCommandPacketLength },
    { "ACL Data",    BtPacketAclData,    BT_HciAclDataPacketLength },
    { "SCO Data",    BtPacketScoData,    BT_HciScoDataPacketLength },
    { "HCI Event",   BtPacketHciEvent,   BT_HciEventPacketLength   }
};

/**
 * Extract raw HCI packet from H4 frame
 */
STATIC BtPacketType BT_DecodeH4(BtPacketInfo* pkt, const UCHAR* buf, int len)
{
    if (len > 1) {
        int hciLength;
        UCHAR type = buf[0];
        switch (type) {
        case HCI_UART_PACKET_TYPE_COMMAND:
        case HCI_UART_PACKET_TYPE_ACL_DATA:
        case HCI_UART_PACKET_TYPE_SCO_DATA:
        case HCI_UART_PACKET_TYPE_EVENT:
           
            hciLength = BtFrameH4[type].length(buf+1,len-1);
            switch (hciLength) {
            case HCI_PACKET_INCOMPLETE:
                return BtPacketUnknown;
            case HCI_PACKET_CORRUPT:
                return BtPacketInvalid;
            default:
                ASSERT(hciLength > 0);
                pkt->name = BtFrameH4[type].name;
                pkt->header = 1;
                pkt->length = hciLength; 
                pkt->trailer = 0;
                if ((hciLength+1) <= len) {
                    return BtFrameH4[type].type;
                } else {
                    return BtPacketIncomplete;
                }
            }

        default:
            return BtPacketInvalid;
        }
    } else {
        return BtPacketIncomplete;
    }
}

/**
 * Given a raw HCI packet, writes H4 frame into the buffer
 */
STATIC Bool BT_EncodeH4(Buffer* b, BtPacketType t, const UCHAR* pkt, int len)
{
    UCHAR indicator;
    switch (t) {
    case BtPacketHciCommand: indicator = HCI_UART_PACKET_TYPE_COMMAND;  break;
    case BtPacketAclData:    indicator = HCI_UART_PACKET_TYPE_ACL_DATA; break;
    case BtPacketScoData:    indicator = HCI_UART_PACKET_TYPE_SCO_DATA; break;
    case BtPacketHciEvent:   indicator = HCI_UART_PACKET_TYPE_EVENT;    break;
    default: 
        return False;
    }
    if (BUFFER_PutChar(b, indicator)) {
        if (BUFFER_Put(b, pkt, len, False)) {
            return True;
        }
        BUFFER_Unput(b, 1);
    }
    return False;
}

/* Codec descriptor */
const BtCodec BtCodecH4 = {
    "H4",
    BT_EncodeH4,
    BT_DecodeH4
};
