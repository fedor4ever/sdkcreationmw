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
 * A listener for HTTP content events.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceHTTPContentListener {

    /**
     * Called when any content has been requested.
     * @param request   the content that was requested.
     */
    public void contentRequested(DeviceContentRequest request);

    /**
     * Called when any content has been received as a response to a request.
     * This method should be called for all request responses, including
     * error responses and cached responses.
     * @param content   the content that was received.
     * @param cacheHit  if <code>true</code> then content was retrieved from the cache.
     */
    public void contentReceived(DeviceContentReceived content, boolean cacheHit);

    /**
     * Called when any content has been pushed onto the device.
     * @param content   the content that was pushed.
     */
    public void contentPushed(DeviceContentReceived content);

}   // End of DeviceHTTPContentListener
