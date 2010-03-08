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
* Description:  Constants for MIDP on-device debug protocol.
*
*/


package com.nokia.j2me.sei.xrpc;

/**
 * Interface that defines constants for MIDP on-device debug protocol.
 */
interface EcmtGatewayProtocol {

    static final String GW_PROTOCOL  = "com.nokia.EcmtGw";
    static final String SEI_PROTOCOL  = "com.nokia.SEI";

    static final String SEI_OPEN_METHOD = "open";
    static final String SEI_OPEN_SID_PARAM = "sid";

    static final String SEI_CLOSE_METHOD = "close";
    static final String SEI_CLOSE_SID_PARAM = "sid";

    static final String SEI_SEND_METHOD = "send";
    static final String SEI_SEND_SID_PARAM = "sid";
    static final String SEI_SEND_CID_PARAM = "cid";
    static final String SEI_SEND_DATA_PARAM = "data";

    static final String SEI_CONNECT_METHOD = "connect";
    static final String SEI_CONNECT_SID_PARAM = "sid";
    static final String SEI_CONNECT_CID_PARAM = "cid";
    static final String SEI_CONNECT_PORT_PARAM = "port";

    static final String SEI_RESET_METHOD = "reset";
    static final String SEI_RESET_SID_PARAM = "sid";
    static final String SEI_RESET_CID_PARAM = "cid";
}
