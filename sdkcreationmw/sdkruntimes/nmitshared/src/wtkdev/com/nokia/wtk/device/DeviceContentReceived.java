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
 * A parameterization interface for received content.
 * This interface is implemented by a device simulator component as required
 * so support various event listeners.
 *
 */
public interface DeviceContentReceived extends DeviceContentData {

    /**
     * Gets the original content request location, if any.
     * If, for example, this represents pushed content, then
     * there will be no location information.
     * @return          the URL representing the original content request
     *                  or <code>null</code> if not applicable.
     */
    public String getRequestLocation();

    /**
     * Gets the status of the content request, if applicable.
     * If a text buffer is supplied, then the corresponding message
     * will also be returned.
     * @param msgbuf    an optional buffer to return the status message as well.
     * @return          the HTTP status code (see RFC2068) that resulted from
     *                  the content request operation or zero if not applicable.
     */
    public int getRequestStatus(StringBuffer msgbuf);

    /**
     * Gets the content location.
     * If, for example, this represents pushed content, then
     * there will be no location information.
     * @return          the URL representing the actual content location
     *                  or <code>null</code> if not applicable.
     */
    public String getLocation();

    public long getTransactionId();


  /**
   * Constant value for Transaction Id Not Applicable.
   */

   public static final long INVALID_TRANS_ID = -1;


}   // End of DeviceContentReceived


