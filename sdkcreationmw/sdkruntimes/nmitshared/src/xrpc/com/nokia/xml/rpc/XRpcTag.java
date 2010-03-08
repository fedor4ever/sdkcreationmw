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
 * $Id: XRpcTag.java,v 1.8 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcTag.java
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
 * A typesafe enumeration of XRPC tags
 *
 */
public final class XRpcTag extends XRpcEnum {

    // root element
    public static final XRpcTag ROOT     = new XRpcTag("xrpc");

    // primitive data types
    public static final XRpcTag DOUBLE   = new XRpcTag("double");
    public static final XRpcTag LONG     = new XRpcTag("long");
    public static final XRpcTag INT      = new XRpcTag("int");
    public static final XRpcTag SHORT    = new XRpcTag("short");
    public static final XRpcTag BYTE     = new XRpcTag("byte");
    public static final XRpcTag BOOLEAN  = new XRpcTag("bool");
    public static final XRpcTag BASE64   = new XRpcTag("base64");
    public static final XRpcTag STRING   = new XRpcTag("string");

    // aggregates
    public static final XRpcTag STRUCT   = new XRpcTag("struct");
    public static final XRpcTag ARRAY    = new XRpcTag("array");

    // special values
    public static final XRpcTag NULL     = new XRpcTag("null");
    public static final XRpcTag VOID     = new XRpcTag("void");

    // call tags
    public static final XRpcTag CALL     = new XRpcTag("call");
    public static final XRpcTag PARAMS   = new XRpcTag("params");

    // response tags
    public static final XRpcTag RESPONSE = new XRpcTag("response");
    public static final XRpcTag RESULT   = new XRpcTag("result");

    /** all XRPC tags */
    private static final XRpcTag [] allTags = {
        ROOT,
        DOUBLE, LONG, INT, SHORT, BYTE, BOOLEAN, BASE64, STRING,
        STRUCT, ARRAY,
        NULL, VOID,
        CALL, PARAMS,
        RESPONSE, RESULT
    };

    /** Maps tag string into tags */
    private static Map tagMap = null;

    /**
     * Creates <code>XRpcTag</code>.
     */
    private XRpcTag(String tag) {
        super(tag);
    }

    /**
     * Returns number of tags.
     */
    public static int getCount() {
        return allTags.length;
    }

    /**
     * Returns ith tag.
     */
    public static XRpcTag getTag(int i) {
        return allTags[i];
    }

    /**
     * Returns <code>XRpcTag</code> for the given tag, or
     * <code>null</code> if there's no data type with such name
     */
    public static XRpcTag getTag(String s) {
        if (tagMap == null) initTagMap();
        return (XRpcTag) tagMap.get(s);
    }

    /**
     * Initializes the string to tag map.
     */
    private static synchronized void initTagMap() {
        if (tagMap == null) {
            tagMap = new HashMap(31);
            for (int i=0; i<allTags.length; i++) {
                tagMap.put(allTags[i].getID(), allTags[i]);
            }
        }
    }
}

