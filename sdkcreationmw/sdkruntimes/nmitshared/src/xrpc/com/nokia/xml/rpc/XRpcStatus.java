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
 * $Id: XRpcStatus.java,v 1.3 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcStatus.java
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
 * A typesafe enumeration of XRPC status codes
 *
 */
public final class XRpcStatus extends XRpcEnum {

    // individual XRPC status codes
    public static final XRpcStatus OK       = new XRpcStatus("ok");
    public static final XRpcStatus NOPROTO  = new XRpcStatus("notarget");
    public static final XRpcStatus NOMETHOD = new XRpcStatus("nomethod");
    public static final XRpcStatus SYSERR   = new XRpcStatus("syserr");

    /** all XRPC status codes */
    private static final XRpcStatus [] all = {
        OK, NOPROTO, NOMETHOD, SYSERR
    };

    /** number of status codes */
    private static int count = all.length;

    /**
     * Creates <code>XRpcDataType</code>.
     */
    private XRpcStatus(String s) {
        super(s);
    }

    /**
     * Returns number of status codes.
     */
    public static int getCount() {
        return count;
    }

    /**
     * Returns ith tag.
     */
    public static XRpcStatus getTag(int i) {
        return all[i];
    }

    /**
     * Returns <code>XRpcStatus</code> for the given status string, or
     * <code>null</code> if status string is invalid.
     */
    public static XRpcStatus getStatus(String s) {
        // we only have a few of those, just do plain linear search
        if (s != null && s.length() > 0) {
            for (int i=0; i<count; i++) {
                if (all[i].toString().equals(s)) {
                    return all[i];
                }
            }
        }
        return null;
    }
}
