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
 * A parameterization interface for content data. Should be extended by
 * specific interfaces for different types of content transactions
 * (e.g. request, response, push, etc.)
 * This interface is implemented by a device simulator component as required
 * so support various event listeners.
 *
 */
public interface DeviceContentData {

    /**
     * Gets the content type
     * @return          the content MIME type.
     */
    public String getType();

    /**
     * Gets the encoding of the content data
     * @return          the content data encoding.
     */
    public String getEncoding();

    /**
     * Gets the content data
     * @return          the content data.
     */
    public byte[] getData();

    /**
     * Gets the set of content headers.
     * Each header is organized as a two-element array,
     * where the first element is the header name and
     * the second element is the header value.
     * @return          the set of request headers.
     */
    public String[][] getHeaders();


}   // End of DeviceContentData


