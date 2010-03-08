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
public interface DeviceWindowProperties {

    //
    // Properties that MUST be implemented by the device:
    //

    /**
     * The title of the device GUI window. (Type = String)
     * If the value is not defined by the device, then one will be supplied by the host.
     */
    public static final String DeviceWindowTitle = "device.window.title";

    /**
     * The location of the upper left corner of the device GUI window. (Type = Point)
     * The host will supply an initial value, but the device should change this value
     * in response to end-user window manipulation.
     */
    public static final String DeviceWindowLocation = "device.window.location";

    /**
     * The visibility state of the device GUI window. (Type = Boolean)
     *
     * The device should only set this value to <code>true</code> when the device
     * window is shown as a side-effect of a call to <code>Device.openDevice()</code>.
     *
     * The device should only set this value to <code>false</code> in response to
     * an end-user request to close the device window.  This is to allow a host
     * application to detect window closure as a signal that the device has been
     * shut down.
     */
    public static final String DeviceWindowVisible = "device.window.visible";

    /**
     * The SDK phone number. (Type = String)
     *
     */
    public static final String DevicePhonenumber = "device.window.phonenumber";

}   // End of DeviceWindowProperties

