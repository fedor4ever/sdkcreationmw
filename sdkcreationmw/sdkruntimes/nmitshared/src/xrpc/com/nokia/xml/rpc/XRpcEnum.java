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
 * $Id: XRpcEnum.java,v 1.2 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcEnum.java
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
 * Base class for a typesafe enumeration.
 *
 */
abstract class XRpcEnum {

    /** The string that identifies the enum value */
    private String name;

    /**
     * The hashcode. Since the hashcode never changes, there's no need
     * to calculate it every time. Assumption is here that normally strings
     * don't have zero hashcode. Note that the String object itself caches
     * its hashcode since JDK 1.3
     */
    private int hash = 0;

    // Disallow public instantiation
    private XRpcEnum() {}

    /**
     * Create a new enum value
     * @param id identifier for enum value
     */
    protected XRpcEnum(String id) {
        if (id == null) throw new NullPointerException("id");
        name = id;
    }

    /**
     * Return the ID string for this Enum value.
     */
    public final String getID() {
        return name;
    }

    /**
     * Compares this object to the specifed object.
     * The result is <code>true</code> if and only if the argument is not
     * <code>null</code> and is a <code>Enum</code> object with the same name.
     * @param     obj   the object to compare this tag with.
     * @return    <code>true</code> if the objects are equal;
     *            <code>false</code> otherwise.
     */
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        } else if (obj instanceof XRpcEnum) {
            XRpcEnum that = (XRpcEnum)obj;
            // since hash code is cached, checking it is significantly
            // faster than comparing strings
            if (that.hashCode() == hashCode()) {
                return name.equals(getID());
            }
        }
        return false;
    }

    /**
     * @return the hashcode for this object.
     */
    public final int hashCode() {
        if (hash == 0) {
            // calculate the hash code if we haven't done it yet. Note that
            // the String object itself caches its hashcode since JDK 1.3
            hash = name.hashCode();
        }
        return hash;
    }

    /**
     * Return the string representation of this Enum value.
     */
    public String toString() {
        return getID();
    }
}
