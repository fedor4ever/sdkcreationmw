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
* Description:  HCI utilities
 *
*/


#include "bt_usb.h"

/**
 * All these functions return the full length (including the header)
 * of the full HCI packet of the corresponding type. Special return
 * values are:
 *
 *   HCI_PACKET_INCOMPLETE - not enough data to figure out the packet length
 *   HCI_PACKET_CORRUPT - invalid packet structure
 *
 * The fact that the function returns the packet length doesn't mean that
 * the whole packet is contained in the buffer.
 */
int BT_HciCommandPacketLength(const UCHAR* pkt, int len)
{
    if (len >= 3) {
        return (pkt[2] + 3);
    } else {
        return HCI_PACKET_INCOMPLETE;
    }
}

int BT_HciEventPacketLength(const UCHAR* pkt, int len)
{
    if (len >= 2) {
        return (pkt[1] + 2);
    } else {
        return HCI_PACKET_INCOMPLETE;
    }
}

int BT_HciAclDataPacketLength(const UCHAR* pkt, int len)
{
    if (len >= 4) {
        return (pkt[2] + (pkt[3] << 8) + 4);
    } else {
        return HCI_PACKET_INCOMPLETE;
    }
}

int BT_HciScoDataPacketLength(const UCHAR* pkt, int len)
{
    if (len >= 3) {
        return (pkt[2] + 3);
    } else {
        return HCI_PACKET_INCOMPLETE;
    }
}
