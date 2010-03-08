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
 * $Id: XRpcAsyncMethod.java,v 1.2 2005/08/26 22:45:24 slava Exp $
 */

// ========================================================================
//  Name        : XRpcAsyncMethod.java
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
 * An extension of {@link XRpcMethod} which allows the handler to request
 * asynchronous execution. That is, if <code>isAsynchronous()</code> returns
 * <code>true</code>, this method will be invoked on its own thread even
 * if the connection is configured to invoke all handlers on the same
 * thread. Typically, this would be used by the methods that may need
 * to block for a long time without blocking other incoming XRPC calls.
 *
 */
public interface XRpcAsyncMethod extends XRpcMethod {

    /**
     * If this method returns <code>true</code>, then the handler's
     * <code>invoke()</code> method will be invoked on its own thread.
     */
    public boolean isAsynchronous();
}
