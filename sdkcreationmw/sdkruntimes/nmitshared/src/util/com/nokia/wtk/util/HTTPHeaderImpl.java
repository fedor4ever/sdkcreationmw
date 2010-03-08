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
 * Default implementation of {@link HTTPHeader} interface.
 *
 */
public class HTTPHeaderImpl implements HTTPHeader {

    /** header name */
    private String name;

    /** header value */
    private String value;

    /**
     * Creates a copy of another HTTP header
     * @param header another header
     */
    public HTTPHeaderImpl(HTTPHeader header) {
        this(header.getName(), header.getValue());
    }

    /**
     * Creates HTTP header object
     * @param name header name
     * @param value header value
     */
    public HTTPHeaderImpl(String name, String value) {
        this.name = name;
        this.value = value;
    }

    /**
     * Returns the header name
     * @return the header name
     */
    public String getName() {
        return name;
    }

    /**
     * Sets the header name
     * @param name the header name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Returns the header value
     * @return the header value
     */
    public String getValue() {
        return value;
    }

    /**
     * Sets the header value
     * @param value the header value
     */
    public void getValue(String value) {
        this.value = value;
    }

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
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        } else if (obj instanceof HTTPHeader) {
            HTTPHeader that = (HTTPHeader)obj;
            return Utils.equals(this.getName(),that.getName()) &&
                   Utils.equals(this.getValue(),that.getValue());
        } else {
            return false;
        }
    }

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
    public int hashCode() {
        return (Utils.hashCode(getName()) ^ Utils.hashCode(getValue()));
    }

    /**
     * Returns a string representation of this object. This method
     * is intended to be used only for debugging purposes, and the
     * content and format of the returned string may vary between
     * implementations. The returned string may be empty but may not
     * be <code>null</code>.
     *
     * @return  a string representation of this object.
     */
    public String toString() {
        return getName() + ": " + getValue();
    }
}
