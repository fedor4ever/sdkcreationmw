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
 * $Id: XRpcAttrList.java,v 1.2 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcAttrList.java
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
 * An attribute list
 *
 */
public interface XRpcAttrList {

    /**
     * Returns number of attributes
     */
    public int getAttributeCount();

    /**
     * Returns ith attribute.
     * @exception {@link IndexOutOfBoundsException} if index is invalid
     */
    public XRpcAttr getAttribute(int i);

    /**
     * Returns the value of the ith attribute.
     * @exception {@link IndexOutOfBoundsException} if index is invalid
     */
    public String getAttributeValue(int i);

    /**
     * Returns the value of the attribute. If this element does not have
     * such attribute, returns <code>null</code>
     */
    public String getAttributeValue(XRpcAttr attr);
}

