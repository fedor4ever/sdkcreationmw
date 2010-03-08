/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * $Id: XRpcBasicAttrList.java,v 1.2 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcBasicAttrList.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.xml.rpc;

/* java.util */
import java.util.List;
import java.util.ArrayList;

/**
 * Implementation of an attribute list
 *
 */
public class XRpcBasicAttrList implements XRpcAttrList {

    /**
     * The list of attributes
     */
    private List attributes = null;

    /**
     * Returns number of attributes
     */
    public final int getAttributeCount() {
        return ((attributes == null) ? 0 : attributes.size());
    }

    /**
     * Returns ith attribute.
     * @exception {@link IndexOutOfBoundsException} if index is invalid
     */
    public final XRpcAttr getAttribute(int i) {
        if (i >= getAttributeCount()) {
            throw new IndexOutOfBoundsException(Integer.toString(i));
        }
        return ((XRpcAttrValue)attributes.get(i)).getAttr();
    }

    /**
     * Returns the value of the ith attribute.
     * @exception {@link IndexOutOfBoundsException} if index is invalid
     */
    public final String getAttributeValue(int i) {
        if (i >= getAttributeCount()) {
            throw new IndexOutOfBoundsException(Integer.toString(i));
        }
        return ((XRpcAttrValue)attributes.get(i)).getValue();
    }

    /**
     * Checks if object has the specified attribute
     */
    public final boolean hasAttribute(XRpcAttr attr) {
        int n = getAttributeCount();
        for (int i=0; i<n; i++) {
            XRpcAttrValue a = (XRpcAttrValue)attributes.get(i);
            if (a.getAttr() == attr) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the value of the attribute. If this element does not have
     * such attribute, returns <code>null</code>
     */
    public final String getAttributeValue(XRpcAttr attr) {
        int n = getAttributeCount();
        for (int i=0; i<n; i++) {
            XRpcAttrValue a = (XRpcAttrValue)attributes.get(i);
            if (a.getAttr() == attr) {
                return a.getValue();
            }
        }
        return null;
    }

    /**
     * Sets attribute of the element. If such attribute already exists,
     * it gets replaced. Otherwise, a new attribute is created and attribute
     * count changes.
     */
    public void setAttribute(XRpcAttr attr, String value) {
        int n = getAttributeCount();
        for (int i=0; i<n; i++) {
            XRpcAttrValue a = (XRpcAttrValue)attributes.get(i);
            if (a.getAttr() == attr) {
                if (!XRpcUtils.equals(a.getValue(), value)) {
                    attributes.set(i, new XRpcAttrValue(attr, value));
                }
                return;
            }
        }

        if (attributes == null) attributes = new ArrayList(4);
        attributes.add(new XRpcAttrValue(attr, value));
    }

    /**
     * Removes all attributes.
     */
    public void removeAllAttributes() {
        if (attributes != null) attributes.clear();
    }

    /**
     * Removes the attribute.
     * @return <code>true</code> if attribute has been removed,
     *         <code>false</code> if it wasn't on the attribute list
     */
    public boolean removeAttribute(XRpcAttr attr) {
        int n = getAttributeCount();
        for (int i=0; i<n; i++) {
            XRpcAttrValue a = (XRpcAttrValue)attributes.get(i);
            if (a.getAttr() == attr) {
                attributes.remove(i);
                return true;
            }
        }
        return false;
    }
}

