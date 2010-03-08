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
 * $Id: XRpcBinaryData.java,v 1.7 2005/10/26 02:22:32 slava Exp $
 */

// ========================================================================
//  Name        : XRpcBinaryData.java
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

/* java.io */
import java.io.IOException;

/**
 * XRPC binary data element
 */
public final class XRpcBinaryData extends XRpcBasicData {

    private byte [] value;

    public XRpcBinaryData() {
        value = null;
    }

    public XRpcBinaryData(byte [] a, int off, int len) {
        if (a != null) {
            value = new byte[len];
            System.arraycopy(a,off,value,0,len);
        }
    }

    public XRpcBinaryData(byte [] a, int off, int len, String name) {
        this(a, off, len);
        setAttribute(XRpcAttr.NAME, name);
    }

    public XRpcBinaryData(byte [] a) {
        value = dup(a);
    }

    public XRpcBinaryData(byte [] a, String name) {
        this(a);
        setAttribute(XRpcAttr.NAME, name);
    }

    public XRpcDataType getDataType() {
        return XRpcDataType.BINARY;
    }

    public Object getValue() {
        return dup(value);
    }

    public byte [] toByteArray() {
        return dup(value);
    }

    public String getAsText() {
        return XRpcUtils.encodeBASE64(value);
    }

    public void setAsText(String s) {
        try {
            value = XRpcUtils.decodeBASE64(s);
        } catch (IOException x) {
            x.printStackTrace();
            value = new byte[0];
        }
    }

    private static byte [] dup(byte [] a) {
        if (a == null) {
            return a;
        } else {
            int n = a.length;
            if (n == 0) {
                return a;
            } else {
                byte [] b = new byte[n];
                System.arraycopy(a,0,b,0,n);
                return b;
            }
        }
    }
}
