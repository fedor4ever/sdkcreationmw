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
 * $Id: XRpcElement.java,v 1.3 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcElement.java
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
 * Represents an abstract XRPC element. Basically, it represents a tag
 * and all its attributes.
 *
 */
public interface XRpcElement extends XRpcAttrList {

    /**
     * Returns the tag that represents this element in the XML stream
     */
    public XRpcTag getTag();

    /**
     * Write this element and all its children elements into an XML stream.
     */
    public void write(XRpcWriter writer);
}

