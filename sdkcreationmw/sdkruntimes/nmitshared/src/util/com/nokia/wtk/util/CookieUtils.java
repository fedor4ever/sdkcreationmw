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
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

/**
 * Cookie related utilities.
 *
 */
public abstract class CookieUtils {

    /**
     * Instantiation is disabled
     */
    private CookieUtils() {
        throw new InternalError("CookieUtils is a set of static methods");
    }

    /**
     * Converts {@link HTTPCookie} into a string array in the format
     * understood by the toolkit. Toolkit 3.1 used two different
     * representations of a cookie in different places (with or without URL).
     * This method returns a string array that does NOT include the URL.
     *
     * @param cookie the cookie to convert
     * @returns cookie in the format understood by Toolkit 3.1
     */
    public static String [] convert(HTTPCookie cookie) {
        return convert(cookie, false);
    }

    /**
     * Converts {@link HTTPCookie} into a string array in the format
     * understood by the toolkit. Toolkit uses two different representations
     * of a cookie in different places (with or without URL).
     *
     * @param cookie the cookie to convert
     * @param includeUrl <code>true</code> to include URL as the first element
     *                   of the array.
     *
     * @returns cookie in the format understood by Toolkit 3.1
     */
    public static String [] convert(HTTPCookie cookie, boolean includeUrl) {
        int na = cookie.getAttributeCount();
        int n = (includeUrl ? (3+2*na) : (2+2*na));
        int k = 0;      // current index
        String [] toolkitCookie = new String[n];
        if (includeUrl) toolkitCookie[k++] = cookie.getURL();
        toolkitCookie[k++] = cookie.getName();
        toolkitCookie[k++] = cookie.getValue();
        if (na > 0) {
            HTTPCookie.Attribute [] attrs = cookie.getAllAttributes();
            for (int i=0; i<na; i++) {
                HTTPCookie.Attribute a = attrs[i];
                toolkitCookie[k++] = a.getName();
                toolkitCookie[k++] = a.getValue();
            }
        }
        return toolkitCookie;
    }

    /**
     * Converts a set of {@link HTTPCookie}s into a collection of string
     * arrays in the format understood by the toolkit. Toolkit uses two
     * different representations of a cookie in different places (with
     * or without URL). This method returns an iteration of string arrays
     * that include URL as the first element
     *
     * @param cookies the set of cookies to convert
     * @returns interator that returns cookies in the format understood
     *          by Toolkit 3.1
     */
    public static Iterator convert(HTTPCookie [] cookies) {
        if (cookies != null && cookies.length > 0) {
            List list = new ArrayList();
            for (int i=0; i<cookies.length; i++) {
                list.add(convert(cookies[i], true));
            }
            return list.iterator();
        }
        return EmptyIterator.getInstance();
    }
}
