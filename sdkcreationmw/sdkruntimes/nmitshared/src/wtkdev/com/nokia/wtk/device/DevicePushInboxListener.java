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
 * A listener for push message "inbox" state change events.
 * This interface will be implemented by the hosting application.
 *
 * @see DevicePushInbox
 */
public interface DevicePushInboxListener {

    /**
     * Called when a push message has been added to the inbox.
     *
     * @param mid       a unique identifier for this message which can
     *                  be used later across interfaces to access a message.
     * @param msg       the message that was added, because it may not
     *                  last long enough to retrieve later.
     */
    public void messageAdded(int mid, DeviceContentReceived msg);

    /**
     * Called when a push message has been removed from the inbox.
     *
     * @param mid       a unique message identifier for the message.
     * @param reason    the reason for message deletion.
     *                  Possible values are:
     *                  <ul>
     *                    <li> {@link #REMOVED_USER}
     *                    <li> {@link #REMOVED_EXPIRED}
     *                    <li> {@link #REMOVED_ACTION}
     *                    <li> {@link #REMOVED_REPLACED}
     *                  </ul>
     */
    public void messageRemoved(int mid, int reason);

    /**
     * Called when a push message has been activated (i.e., the action that
     * the content indicates has been performed: content downloaded,
     * cache entries invalidated, etc).
     *
     * @param mid       unique message identifier for the message.
     */
    public void messageActivated(int mid);


    /** A push message has been removed by the user */
    public static final int REMOVED_USER        = 0;

    /** A push message has been removed due to its expiration */
    public static final int REMOVED_EXPIRED     = 1;

    /** A push message has been removed as a result of another message's activation */
    public static final int REMOVED_ACTION      = 2;

    /** A push message has been replaced by another (more up-to-date) message */
    public static final int REMOVED_REPLACED    = 3;

    /** A push message has been removed because there is no action to take.
     *  (i.e., a signal-none action or an empty URI field)
     */
    public static final int REMOVED_NO_ACTION   = 4;

    /** A push message has been removed because a there is no more room in the inbox. */
    public static final int REMOVED_CAPACITY    = 5;

}   // End of DevicePushInboxListener
