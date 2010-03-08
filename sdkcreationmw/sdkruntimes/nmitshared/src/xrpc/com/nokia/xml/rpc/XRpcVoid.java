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
 * $Id: XRpcVoid.java,v 1.3 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcVoid.java
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
 * Represents an XRPC <code>void</code> element
 *
 */
public final class XRpcVoid extends XRpcEmptyDataElement {

    /** Shared instance of <code>XRpcVoid</code> */
    private static XRpcVoid instance = null;

    /**
     * Returns the shared instance of <code>XRpcVoid</code>.
     * @return the shared instance of <code>XRpcVoid</code>.
     */
    public static XRpcVoid getInstance() {
        if (instance == null) instance = new XRpcVoid();
        return instance;
    }

    /**
     * Returns the data type of this element
     *
     * @return {@link XRpcDataType#VOID}
     */
    public XRpcDataType getDataType() {
        return XRpcDataType.VOID;
    }
}
