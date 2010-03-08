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
 * $Id: XRpcMethod.java,v 1.5 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcMethod.java
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
 * A method handler. {@link XRpcProtocol} is the factory of such objects.
 *
 */
public interface XRpcMethod {

    /**
     * Executes the method. This method is invoked as a result of processing
     * &lt;call&gt; or &lt;notify&gt; XRPC packet.
     *
     * @param params the parameters of the call. If no parameters were sent
     *               by the caller, this is an empty array
     */
    public XRpcDataElement invoke(XRpcDataElement[] args)
        throws Exception;
}
