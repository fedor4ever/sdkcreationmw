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
 * $Id: XRpcRegistry.java,v 1.3 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcRegistry.java
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


package com.nokia.xml.rpc.registry;

/* java.util */
import java.util.Properties;

/* java.io */
import java.io.IOException;

/**
 * Interface to "xrpc.registry" protocol.
 *
 */
public interface XRpcRegistry {

    /**
     * Standard XRPC registry port. This port number has been assigned to
     * XRPC by IANA. The complete list of assigned port numbers can be found
     * at http://www.iana.org/assignments/port-numbers
     */
    public static final int XREG_DEFAULT_PORT = 3651;

    /**
     * Acquires a named lock.
     *
     * @param name      the name of the lock. If this parameter is
     *                  <code>null</code> or empty, a global unnamed
     *                  lock is acquired. It's guaranteed that while
     *                  unnamed lock is owned by a client, no modifications
     *                  will be made to the service database by any other
     *                  client.  One client session is not allowed to hold
     *                  more than one lock.
     *
     * @return          <code>true</code> if the lock has been acquired,
     *                  <code>false</code> if this lock is already by this
     *                  XRPC registry session
     *
     * @throws          IOException if connection is broken
     */
    public boolean lock(String name) throws IOException;

    /**
     * Releases the lock held by the client.
     *
     * @throws          IOException if connection is broken
     */
    public void unlock() throws IOException;

    /**
     * Registers an XRPC service with the registry.
     *
     * @param service   service protocol name
     * @param host      host name or (typically) <code>null</code>
     * @param port      listening port
     * @param props     service properties
     *
     * @return          the service id assigned by the registry
     * @throws          IOException if connection is broken
     */
    public String registerService(String service, String host,
        int port, Properties props) throws IOException;

    /**
     * Unregisters the XRPC service previously registered with the
     * <code>registerService</code> call.
     *
     * @param id        the if of the service to deregister. This is the
     *                  string returned by the <code>registerService</code>
     *                  call.
     *
     * @throws          IOException if connection is broken
     */
    public void unregisterService(String id) throws IOException;

    /**
     * Requests the list of registered services.
     *
     * @param protocol  the service protocol name. If this parameter is
     *                  <code>null</code>, then the full list of all
     *                  registered services is returned.
     *
     * @return          the information about the services that matched
     *                  the protocol name, all registered services if
     *                  no protocol name was provided. Returns empty
     *                  array if there's no service information to return.
     *
     * @throws          IOException if connection is broken
     */
    public ServiceInfo [] listServices(String protocol) throws IOException;

    /**
     * Queries the service information.
     *
     * @param id        the service id
     *
     * @return          the information about the requested service,
     *                  or <code>null</code> if the specified id is
     *                  invalid.
     *
     * @throws          IOException if connection is broken
     */
    public ServiceInfo queryService(String id) throws IOException;

    /**
     * Close the connection to the XRPC Registry. Be sure to call this
     * method after you no longer need this object. It closes the TCP
     * connection to the XRPC Registry and terminates the listening
     * thread. After this method has been called, the XRPC Registry
     * object is unusable.
     */
    public void close();

    /**
     * The service information returned by the <code>XRpcRegistry</code>
     * <code>registerService</code> call.
     */
    public interface ServiceInfo {
        public String getID();
        public String getProtocol();
        public String getHost();
        public int getPort();
        public Properties getProperties();
    }
}
