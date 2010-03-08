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

/**
 * HTTP header. See RFC2616 for definitions of various HTTP headers
 *
 * @see HTTPHeaderNames
 */
public interface HTTPHeader extends HTTPHeaderNames {

    /**
     * Returns the header name
     * @return the header name
     */
    public String getName();

    /**
     * Returns the header value
     * @return the header value
     */
    public String getValue();

    /**
     * Compares the specified object with this header for equality.
     * Returns <code>true</code> if the given object also implements
     * <code>HTTPHeader</code> interface and the two objects represent
     * the same header.  More formally, two headers <code>h1</code> and
     * <code>h2</code> represent the same header iff<pre>
     *     (h1.getName()==null ?
     *      h2.getName()==null : h1.getName().equals(h2.getName())) &&
     *     (h1.getValue()==null ?
     *      h2.getValue()==null : h1.getValue().equals(h2.getValue()))
     * </pre>
     * This ensures that the <code>equals</code> method works properly
     * across different implementations of the <code>HTTPHeader</code>
     * interface.
     *
     * @param obj   object to be compared for equality with this header.
     * @return      <code>true</code> if the specified object is equal
     *              to this header.
     */
    public boolean equals(Object obj);

    /**
     * Returns the hash code value for this header.  The hash code
     * of an HTTP header <code>h</code> is defined to be: <pre>
     *     (h.getName()==null ? 0 : h.getName().hashCode()) ^
     *     (h.getValue()==null ? 0 : h.getValue().hashCode())
     * </pre>
     * This ensures that <code>h1.equals(h2)</code> implies that
     * <code>h1.hashCode()==h2.hashCode()</code> for any two headers
     * <code>h1</code> and <code>h2</code>, as required by the general
     * contract of <code>Object.hashCode</code>.
     *
     * @return the hash code value for this header.
     * @see Object#hashCode()
     * @see Object#equals(Object)
     * @see #equals(Object)
     */
    public int hashCode();
}
