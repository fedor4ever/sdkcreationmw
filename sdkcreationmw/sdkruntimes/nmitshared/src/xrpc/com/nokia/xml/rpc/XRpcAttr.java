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
 * $Id: XRpcAttr.java,v 1.4 2005/08/26 22:45:24 slava Exp $
 */

// ========================================================================
//  Name        : XRpcAttr.java
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
 * A typesafe enumeration of XRPC attributes
 *
 */
public final class XRpcAttr extends XRpcEnum {

    public static final XRpcAttr ID       = new XRpcAttr("id");
    public static final XRpcAttr NAME     = new XRpcAttr("name");
    public static final XRpcAttr TYPE     = new XRpcAttr("type");
    public static final XRpcAttr STATUS   = new XRpcAttr("status");
    public static final XRpcAttr METHOD   = new XRpcAttr("method");
    public static final XRpcAttr PROTOCOL = new XRpcAttr("target");
    public static final XRpcAttr SOURCE   = new XRpcAttr("source");

    /** all XRPC attributes */
    private static final XRpcAttr [] allAttributes = {
        ID, NAME, TYPE, STATUS, METHOD, PROTOCOL, SOURCE
    };

    /** Maps attribute strings into attributes */
    private static Map attributeMap = null;

    /**
     * Creates <code>XRpcAttr</code>.
     */
    private XRpcAttr(String tag) {
        super(tag);
    }

    /**
     * Returns number of tags.
     */
    public static int getCount() {
        return allAttributes.length;
    }

    /**
     * Returns ith tag.
     */
    public static XRpcAttr getAttribute(int i) {
        return allAttributes[i];
    }

    /**
     * Returns <code>XRpcAttr</code> for the given tag, or
     * <code>null</code> if there's no data type with such name
     */
    public static XRpcAttr getAttribute(String s) {
        if (attributeMap == null) initAttributeMap();
        return (XRpcAttr) attributeMap.get(s);
    }

    /**
     * Initializes the string to tag map.
     */
    private static synchronized void initAttributeMap() {
        if (attributeMap == null) {
            attributeMap = new HashMap(31);
            for (int i=0; i<allAttributes.length; i++) {
                attributeMap.put(allAttributes[i].getID(), allAttributes[i]);
            }
        }
    }
}

