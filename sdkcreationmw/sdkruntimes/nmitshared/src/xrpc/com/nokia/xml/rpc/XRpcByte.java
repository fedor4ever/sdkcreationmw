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
 * $Id: XRpcByte.java,v 1.5 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcByte.java
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
 * XRPC <code>byte</code> primitive data type
 *
 */
public final class XRpcByte extends XRpcNumber {

    public XRpcByte() {
    }

    public XRpcByte(byte x) {
        super(new Byte(x));
    }

    public XRpcByte(Number x) {
        setValue(x);
    }

    public XRpcByte(byte x, String name) {
        this(x);
        setAttribute(XRpcAttr.NAME, name);
    }

    public XRpcByte(Number x, String name) {
        this(x);
        setAttribute(XRpcAttr.NAME, name);
    }

    public XRpcByte(String s) throws NumberFormatException {
        setAsText(s);
    }

    public XRpcDataType getDataType() {
        return XRpcDataType.BYTE;
    }

    protected Number convert(Number x) {
        if (x instanceof Byte) return x;
        return (new Byte(x.byteValue()));
    }

    public void setAsText(String s) throws NumberFormatException {
        setValue(new Byte(s));
    }
}
