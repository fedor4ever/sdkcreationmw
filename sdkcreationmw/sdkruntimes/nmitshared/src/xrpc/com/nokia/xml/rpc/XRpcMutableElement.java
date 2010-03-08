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
 * $Id: XRpcMutableElement.java,v 1.2 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcMutableElement.java
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
 * Represents an abstract mutable XRPC element. Basically, it has an editable
 * attribute list (but not value).
 *
 */
public interface XRpcMutableElement extends XRpcElement {

    /**
     * Removes the attribute.
     * @return <code>true</code> if attribute has been removed,
     *         <code>false</code> if it wasn't on the attribute list
     */
    public boolean removeAttribute(XRpcAttr attr);

    /**
     * Sets attribute of the element. If such attribute already exists,
     * it gets replaced. Otherwise, a new attribute is created and attribute
     * count changes.
     */
    public void setAttribute(XRpcAttr attr, String value);

    /**
     * Removes all attributes.
     */
    public void removeAllAttributes();
}

