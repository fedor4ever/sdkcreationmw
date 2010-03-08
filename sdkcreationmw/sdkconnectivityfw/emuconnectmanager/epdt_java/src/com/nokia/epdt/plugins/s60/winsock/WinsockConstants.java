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
* Description: 
*
*/


package com.nokia.epdt.plugins.s60.winsock;

/**
 * Defaines Winsock-related constants
 */
public interface WinsockConstants {

    // UIDs
    static final int WINSOCK_PREFS_UID = 0x1020D6CF;
    static final int WINSOCK_TARGET_UID = 0x1020D6D0;

    // message types
    static final int MESSAGE_TYPE_CONFIG = 0;
    static final int MESSAGE_TYPE_CONFIG_REQUEST = 1;
    static final int MESSAGE_TYPE_PPOXY_CONFIG_REQUEST = 2;

    // Winsock specific properties
    static final String USE_WINSOCK_PROP = "useWinsock";
    static final String WINSOCK_USE_PROXY_PROP = "winsockUseProxy";
    static final String WINSOCK_PROXY_SERVER_PROP = "winsockProxyServerName";
    static final String WINSOCK_PROXY_PORT_PROP = "winsockProxyServerPort";
    static final String WINSOCK_UI_ENABLED_PROP = "winsockUiEnabled"; 
}
