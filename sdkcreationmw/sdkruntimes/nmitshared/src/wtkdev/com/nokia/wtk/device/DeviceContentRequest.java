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
 * A parameterization interface for a content request.
 * This interface is implemented by a device simulator component as required
 * so support various event listeners.
 *
 */
public interface DeviceContentRequest extends DeviceContentData {

    /**
     * Gets the requested content location.
     * @return          the URL string representing the content location.
     */
    public String getLocation();

    /**
     * Gets the client method used to access the content.
     * For HTTP requests, this may be "GET", "POST" or any other
     * method defined by RFC2068.  For FILE requests, this may
     * be presumed to be "GET".
     * @return          the client request method used to access the content.
     */
    public String getMethod();

}   // End of DeviceContentRequest


