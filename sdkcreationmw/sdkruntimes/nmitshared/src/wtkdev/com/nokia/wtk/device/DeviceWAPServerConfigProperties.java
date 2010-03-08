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




package com.nokia.wtk.device;


/**
 * A set of property names that are used to define WAP Server connection configurations.
 *
 */
public interface DeviceWAPServerConfigProperties {

    /**
     * The WAP server IP address that a device browser should use (Type = String)
     */
    public static final String ServerAddress = "device.wapserver.address";

    /**
     * The WAP server connection type that a device browser should use (Type = Integer)
     * This may be one of the following values:
     * <ul>
     * <li><code>CT_UNSECURE_CONNECTION_LESS</code></li>
     * <li><code>CT_UNSECURE_CONNECTION_ORIENTED</code></li>
     * <li><code>CT_SECURE_CONNECTION_LESS</code></li>
     * <li><code>CT_SECURE_CONNECTION_ORIENTED</code></li>
     * </ul>
     */
    public static final String ServerConnectionType = "device.wapserver.connection-type";

    /**
     * An unsecure "connection-less" connection type property value. (default value)
     */
    public static final Integer CT_UNSECURE_CONNECTION_LESS = new Integer(0);

    /**
     * An unsecure "connection-oriented" connection type property value.
     */
    public static final Integer CT_UNSECURE_CONNECTION_ORIENTED = new Integer(1);

    /**
     * A secure "connection-less" connection type property value.
     */
    public static final Integer CT_SECURE_CONNECTION_LESS = new Integer(2);

    /**
     * A secure "connection-oriented" connection type property value.
     */
    public static final Integer CT_SECURE_CONNECTION_ORIENTED = new Integer(3);

}   // End of DeviceWAPServerConfigProperties

