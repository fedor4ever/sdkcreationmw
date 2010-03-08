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


import java.util.Iterator;


/**
 * An interface to the device HTTP loader.
 * This interface is implemented by a device simulator component.
 * These method are optional and may not be implemented by all device implementations.
 *
 */
public interface DeviceHTTPLoader {

    /**
     * Injects push content into the device loader,
     * with the same effect as if content had been pushed onto the device.
     * @param content   the content to be pushed.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void pushContent(DeviceContentReceived content);

    /**
     * Gets the current set of cached content URLs.
     * @return          the set of cached content URLs (as Strings)
     *                  or <code>null</code> if non-existent.
     */
    public Iterator getCacheURLs();

    /**
     * Gets content from the cache.
     * @param url       the url of the cached content.
     * @return          the cached content or <code>null</code> if non-existent.
     */
    public DeviceContentReceived getCachedContent(String url);

    /**
     * Clears the content cache.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void clearCache();

    /**
     * Gets the current set of cookies.
     * Each cookie is represented by an array of String values,
     * arranged as a source URL in element 0, followed by a set
     * of sequential name/value pairs.  Each pair is a component
     * of the cookie value as received in an actual HTTP
     * <code>Set-Cookie</code> or <code>Set-Cookie2</code>
     * response header.  The set of possible pairs is defined
     * in RFC 2965.
     * @return          the set of cookies, each represented as
     *                  a <code>String[]</code> or <code>null</code> if non-existent.
     */
    public Iterator getCookies();

    /**
     * Clears all saved cookies.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void clearCookies();

}   // End of DeviceHTTPLoader
