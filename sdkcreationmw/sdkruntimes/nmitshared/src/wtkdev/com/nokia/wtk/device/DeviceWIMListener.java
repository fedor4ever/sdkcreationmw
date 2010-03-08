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
 * A listener for WIM service events.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceWIMListener {

    /**
     * Called upon receiving a WIM event.
     * @param event     the type of event. Possible events are:
     *                  <ul>
     *                    <li> {@link #EVENT_SERVICE_STARTED}
     *                    <li> {@link #EVENT_SERVICE_STOPPED}
     *                    <li> {@link #EVENT_WIM_ENABLED}
     *                    <li> {@link #EVENT_WIM_DISABLED}
     *                    <li> {@link #EVENT_CERTIFICATE_ADDED}
     *                    <li> {@link #EVENT_CERTIFICATE_REMOVED}
     *                    <li> {@link #EVENT_KEY_ADDED}
     *                    <li> {@link #EVENT_KEY_REMOVED}
     *                    <li> {@link #EVENT_PIN_ENABLED}
     *                    <li> {@link #EVENT_PIN_DISABLED}
     *                    <li> {@link #EVENT_PIN_BLOCKED}
     *                    <li> {@link #EVENT_PIN_UNBLOCKED}
     *                  </ul>
     *
     * @param rdr       the WIM reader ID. This param value can be <code>-1</code>
     *                  for following events:
     *                  <ul>
     *                    <li> {@link #EVENT_SERVICE_STARTED}
     *                    <li> {@link #EVENT_SERVICE_STOPPED}
     *                  </ul>
     *
     * @param id        ID of the Certificate, Key or PIN. This param value can
     *                  be <code>-1</code> for following events:
     *                  <ul>
     *                    <li> {@link #EVENT_SERVICE_STARTED}
     *                    <li> {@link #EVENT_SERVICE_STOPPED}
     *                    <li> {@link #EVENT_WIM_ENABLED}
     *                    <li> {@link #EVENT_WIM_DISABLED}
     *                  </ul>
     *
     */
    public void wimEvent(int event, int rdr, int id);

    /**
     * Called upon receiving a detailed WIM status message.
     * This can be used to construct a WIM activity log.
     * @param rdr       the WIM reader ID
     * @param message   the WIM log message
     */
    public void statusMessage(int rdr, String message);

    //
    // Event types:
    //

    /** WIM service started event.
     * Starting WIM service up may be an asynchronous operation that checks and
     * and opens the WIM applications on all the smartcard readers. WIM service
     * is usually started upon the initilization of WTLS, SignText(), WPKI or
     * WIM management funtionality.
     */
    public static final int EVENT_SERVICE_STARTED       = 1;

    /** WIM service stopped event.
     * Stopping the WIM service is the process of closing any logical channels
     * that are open on smart cards.  The WIM service is usually stopped when
     * WIM's are no longer needed, e.g. when a browser is closed.
     */
    public static final int EVENT_SERVICE_STOPPED       = 2;

    /** WIM enabled event */
    public static final int EVENT_WIM_ENABLED           = 3;

    /** WIM disabled event */
    public static final int EVENT_WIM_DISABLED          = 4;

    /** New Certificate added to WIM event */
    public static final int EVENT_CERTIFICATE_ADDED     = 5;

    /** Certificate removed from WIM event */
    public static final int EVENT_CERTIFICATE_REMOVED   = 6;

    /** New Key added to WIM event */
    public static final int EVENT_KEY_ADDED             = 7;

    /** Key removed from WIM event */
    public static final int EVENT_KEY_REMOVED           = 8;

    /** PIN enabled event */
    public static final int EVENT_PIN_ENABLED           = 9;

    /** PIN disabled event */
    public static final int EVENT_PIN_DISABLED          = 10;

    /** PIN blocked event */
    public static final int EVENT_PIN_BLOCKED           = 11;

    /** PIN unblocked event */
    public static final int EVENT_PIN_UNBLOCKED         = 12;

}   // End of DeviceWIMListener
