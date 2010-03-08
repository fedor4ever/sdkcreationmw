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
 * $Id: XRpcCore.java,v 1.5 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcCore.java
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
 * Interface to "xrpc.core" protocol. Each XRPC server is supposed to
 * implement this protocol.
 *
 * @see XRpcConnection
 *
 */
public interface XRpcCore {

    /**
     * This call does nothing. It exists for the single purpose of checking
     * if connection is still alive. Works even if "xrpc.core" protocol isn't
     * registered on the server.
     *
     * @throws  IOException if connection is broken
     */
    public void ping() throws IOException;

    /**
     * Returns the list of protocols registered on the server side of the
     * connection.
     *
     * @return  the list of protocols, empty array if no protocol is registered
     *          The latter should never happen because "xrpc.core" is one of
     *          the protocols. Never returns <code>null</code>.
     * @throws  IOException if connection is broken
     */
    public String [] listProtocols() throws IOException;

    /**
     * Returns information about the specified protocol
     *
     * @param   name    the protocol name
     * @return  information about the specified protocol, <code>null</code>
     *          if protocol is not registered.
     * @throws  IOException if connection is broken
     */
    public ProtocolInfo getProtocolInfo(String name) throws IOException;

    /**
     * XRPC protocol information.
     */
    public interface ProtocolInfo {
        public String getName();
        public String getDescription();
        public String getVendor();
        public String getVendorURL();
        public String getProtocolURL();
        public int getMajorVersion();
        public int getMinorVersion();
    }
}
