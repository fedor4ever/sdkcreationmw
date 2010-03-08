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
 * $Id: XRpcBoolean.java,v 1.6 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcBoolean.java
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

/* java.text */
import java.text.ParseException;

/**
 * XRPC string data
 *
 */
public final class XRpcBoolean extends XRpcBasicData implements XRpcConstants {

    private Boolean value;

    public XRpcBoolean() {
        value = null;
    }

    public XRpcBoolean(boolean b) {
        value = (b ? Boolean.TRUE : Boolean.FALSE);
    }

    public XRpcBoolean(Boolean b) {
        value = b;
    }

    public XRpcBoolean(boolean b, String name) {
        this(b);
        setAttribute(XRpcAttr.NAME, name);
    }

    public XRpcBoolean(Boolean b, String name) {
        this(b);
        setAttribute(XRpcAttr.NAME, name);
    }

    public XRpcBoolean(String s) throws ParseException {
        setAsText(s);
    }

    public XRpcDataType getDataType() {
        return XRpcDataType.BOOLEAN;
    }

    public Object getValue() {
        return value;
    }

    public String getAsText() {
        if (value == null) {
            return null;
        } else {
            return (value.booleanValue() ? XRpcConstants.TRUE :
                                           XRpcConstants.FALSE);
        }
    }

    public void setAsText(String s) throws ParseException {
        if (TRUE.equals(s)) {
            value = Boolean.TRUE;
        } else if (FALSE.equals(s)) {
            value = Boolean.FALSE;
        } else {
            throw new ParseException(s,0);
        }
    }

    public Boolean getBoolean() {
        return value;
    }

    public boolean booleanValue() {
        return value.booleanValue();
    }
}
