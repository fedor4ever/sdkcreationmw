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
 * $Id: XRpcAttrValue.java,v 1.2 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcAttrValue.java
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

/**
 * An immutable attribute-value pair.
 *
 */
public final class XRpcAttrValue {

    private XRpcAttr attr;
    private String value;

    /**
     * Creates attribute-value pair.
     */
    public XRpcAttrValue(XRpcAttr attr, String value) {
        this.attr = attr;
        this.value = value;
        if (attr == null) throw new NullPointerException("attr");
    }

    /**
     * Returns the attribute.
     */
    public XRpcAttr getAttr() {
        return attr;
    }

    /**
     * Returns the value
     */
    public String getValue() {
        return value;
    }

    /**
     * Checks if this object is "equal to" another object.
     */
    public boolean equals(Object obj) {
        if (obj == this) return true;
        if (obj instanceof XRpcAttrValue) {
            XRpcAttrValue that = (XRpcAttrValue)obj;
            return (that.getAttr() == attr &&
                    XRpcUtils.equals(that.getValue(), value));
        }
        return false;
    }

    /**
     * Returns hash code for this object
     */
    public int hashCode() {
        int h = attr.hashCode();
        if (value != null) h += value.hashCode();
        return h;
    }
}
