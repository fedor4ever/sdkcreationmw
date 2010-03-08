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
 * $Id: XRpcDataType.java,v 1.4 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcDataType.java
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
import java.util.Map;
import java.util.HashMap;

/**
 * A typesafe enumeration of XRPC data types
 *
 */
public final class XRpcDataType extends XRpcEnum {

    // individual XRPC data types
    public static final XRpcDataType DOUBLE  = new XRpcDataType(XRpcTag.DOUBLE);
    public static final XRpcDataType LONG    = new XRpcDataType(XRpcTag.LONG);
    public static final XRpcDataType INT     = new XRpcDataType(XRpcTag.INT);
    public static final XRpcDataType SHORT   = new XRpcDataType(XRpcTag.SHORT);
    public static final XRpcDataType BYTE    = new XRpcDataType(XRpcTag.BYTE);
    public static final XRpcDataType BOOLEAN = new XRpcDataType(XRpcTag.BOOLEAN);
    public static final XRpcDataType STRING  = new XRpcDataType(XRpcTag.STRING);
    public static final XRpcDataType BINARY  = new XRpcDataType(XRpcTag.BASE64);

    public static final XRpcDataType STRUCT  = new XRpcDataType(XRpcTag.STRUCT);
    public static final XRpcDataType ARRAY   = new XRpcDataType(XRpcTag.ARRAY);

    public static final XRpcDataType NULL    = new XRpcDataType(XRpcTag.NULL);
    public static final XRpcDataType VOID    = new XRpcDataType(XRpcTag.VOID);

    /** all XRPC data types */
    private static final XRpcDataType [] allTypes = {
        DOUBLE, LONG, INT, SHORT, BYTE, BOOLEAN, STRING, BINARY,
        STRUCT, ARRAY,
        NULL, VOID
    };

    /** Maps tag string into a data type */
    private static Map typeMap = null;

    /** The XML tag that represents this data type */
    private XRpcTag tag;

    /**
     * Creates <code>XRpcDataType</code>.
     */
    private XRpcDataType(XRpcTag tag) {
        super(tag.getID());
        this.tag = tag;
    }

    /**
     * Returns the XML tag that represents this data type.
     */
    public XRpcTag getTag() {
        return tag;
    }

    /**
     * Returns number of data types.
     */
    public static int getCount() {
        return allTypes.length;
    }

    /**
     * Returns ith data type.
     */
    public static XRpcDataType getType(int i) {
        return allTypes[i];
    }

    /**
     * Returns <code>XRpcDataType</code> for the given tag, or
     * <code>null</code> if there's no data type with such name
     */
    public static XRpcDataType getType(String s) {
        if (typeMap == null) initTypeMap();
        return (XRpcDataType) typeMap.get(s);
    }

    /**
     * Returns <code>XRpcDataType</code> for the given tag, or
     * <code>null</code> if tag does not represent a data type.
     */
    public static XRpcDataType getType(XRpcTag tag) {
        return getType(tag.getID());
    }

    /**
     * Initializes the string to type map.
     */
    private static synchronized void initTypeMap() {
        if (typeMap == null) {
            typeMap = new HashMap(17);
            for (int i=0; i<allTypes.length; i++) {
                typeMap.put(allTypes[i].getID(), allTypes[i]);
            }
        }
    }
}

