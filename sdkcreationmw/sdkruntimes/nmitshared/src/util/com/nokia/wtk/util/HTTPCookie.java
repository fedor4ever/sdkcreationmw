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



package com.nokia.wtk.util;

/* java.util */
import java.util.Iterator;

/**
 * Definition for an HTTP cookie.
 * See RFC 2965 "HTTP State Management Mechanism"
 *
 */
public interface HTTPCookie {

    // Names of cookie attributes
    static final String COMMENT     = "Comment";
    static final String COMMENT_URL = "CommentURL";
    static final String DISCARD     = "Discard";
    static final String DOMAIN      = "Domain";
    static final String MAX_AGE     = "Max-Age";
    static final String PATH        = "Path";
    static final String PORT        = "Port";
    static final String SECURE      = "Secure";
    static final String VERSION     = "Version";

    /**
     * Returns the URL of the response containing the cookie.
     * May return <code>null</code> if such information is unavailable
     * or not applicable.
     *
     * @returns the URL of the response containing the cookie.
     */
    public String getURL();

    /**
     * Returns the cookie name.
     *
     * @returns the cookie name
     */
    public String getName();

    /**
     * Returns the cookie value
     *
     * @returns the cookie value
     */
    public String getValue();

    /**
     * Returns number of cookie attributes.
     *
     * @returns number of cookie attributes
     */
    public int getAttributeCount();

    /**
     * Returns the cookie attributes iterator.
     * The iterator returned by this method does not have to (and probably
     * should not) support remove operation.
     *
     * @returns the cookie attributes iterator
     */
    public Iterator getAttributes();

    /**
     * Returns all cookie attributes as an array.
     *
     * @returns all cookie attributes as an array.
     */
    public Attribute [] getAllAttributes();

    /**
     * Returns the cookie attribute with the specified name.
     * If there's no such attribute, returns <code>null</code>
     *
     * @param name  the name of the attribute to get
     * @return      the attribute with the specified name, <code>null</code>
     *              if cookie has no such attribute
     */
    public Attribute getAttribute(String name);

    /**
     * A cookie attribute.
     */
    public interface Attribute {

        /**
         * Returns the name of the attribute.
         *
         * @returns the name of the attribute
         */
        public String getName();

        /**
         * Returns the value of the attribute. May return <code>null</code>
         * if the attribute does not have any value. Examples of such
         * attributes are "Secure" and "Discard".
         *
         * @returns the value of the attribute.
         */
        public String getValue();
    }
}
