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
 * An "inbox" for pushed messages.
 *
 * @see DevicePushInboxListener
 */
public interface DevicePushInbox {

    /**
     * Gets the set of IDs for push messages currently in this inbox.
     * Intended to be called at initialization time to gather
     * messages from a pre-populated inbox.
     *
     * @return          a set of unique message identifiers to be used later
     *                  by {@link DevicePushInbox#getMessage} to get
     *                  the corresponding push message.
     */
    public int[] getMessageIDs();

    /**
     * Gets a push message from this inbox.
     *
     * @param mid       a unique message identifier for a push message.
     * @return          the push message content associated with the ID
     *                  or <code>null</code> if non-existent.
     */
    public DeviceContentReceived getMessage(int mid);

    /**
     * Removes all messages from this inbox.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void clearMessages();

}   // End of DevicePushInbox
