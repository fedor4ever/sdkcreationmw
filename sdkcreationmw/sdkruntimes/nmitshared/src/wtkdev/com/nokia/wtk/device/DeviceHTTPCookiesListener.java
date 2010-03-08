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
 * A listener for HTTP cookie events.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceHTTPCookiesListener {

    /**
     * Called when a new HTTP cookie is added.
     * Cookies are represented as an array of String values,
     * arranged as a set of sequential name/value pairs, as
     * defined in RFC 2965 for an HTTP <code>Set-Cookie</code>
     * or <code>Set-Cookie2</code> response header.
     * @param url       source URL of the cookie.
     * @param cookie    the HTTP cookie header value.
     */
    public void cookieAdded(String url, String[] cookie);

    /**
     * Called when an existing HTTP cookie has been changed.
     * Cookies are represented as an array of String values,
     * arranged as a set of sequential name/value pairs, as
     * defined in RFC 2965 for an HTTP <code>Set-Cookie</code>
     * or <code>Set-Cookie2</code> response header.
     * @param url       source URL of the cookie.
     * @param cookie    the HTTP cookie header value.
     */
    public void cookieChanged(String url, String[] cookie);

    /**
     * Called when an HTTP cookie is removed.
     * Cookies are represented as an array of String values,
     * arranged as a set of sequential name/value pairs, as
     * defined in RFC 2965 for an HTTP <code>Set-Cookie</code>
     * or <code>Set-Cookie2</code> response header.
     * @param cookie    the HTTP cookie header value.
     */
    public void cookieRemoved(String[] cookie);

    /**
     * Called when all HTTP cookies have been cleared.
     */
    public void cookiesCleared();

}   // End of DeviceHTTPCookiesListener
