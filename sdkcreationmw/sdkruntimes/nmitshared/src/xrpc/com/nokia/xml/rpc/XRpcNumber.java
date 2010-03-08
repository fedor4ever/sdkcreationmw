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
 * $Id: XRpcNumber.java,v 1.5 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcNumber.java
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
 * A base class for XRPC primitive data objects
 *
 */
public abstract class XRpcNumber extends XRpcBasicData {

    private Number value;
    private String text = null;

    protected XRpcNumber() {
        value = null;
    }

    protected XRpcNumber(Number x) {
        value = x;
    }

    protected final void setValue(Object object) throws ClassCastException {
        if (object == null) {
            value = null;
        } else {
            value = convert((Number)object);
        }
        text = null;
    }

    public final String getAsText() {
        if (text == null && value != null) text = value.toString();
        return text;
    }

    public final Object getValue() {
        return value;
    }

    public final Number getNumber() {
        return value;
    }

    abstract protected Number convert(Number x);
    abstract public void setAsText(String s) throws NumberFormatException;
}
