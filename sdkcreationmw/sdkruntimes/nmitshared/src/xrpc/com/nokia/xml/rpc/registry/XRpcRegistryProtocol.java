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
 * $Id: XRpcRegistryProtocol.java,v 1.2 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcRegistryProtocol.java
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

/**
 * "xrpc.registry" protocol definitions.
 *
 */
interface XRpcRegistryProtocol {

    /* *********************************************************************
    //
    //  Describes the "xrpc.reg.Registry" protocol.
    //
    //  The client MUST maintain control connection to the XRPC Registry all
    //  the time. Once the connection is terminated, the registry assumes that
    //  all services registered over this connection are gone and notifies the
    //  listeners
    //
    // *********************************************************************/

    static final String XREG_REGISTRY_PROTOCOL = "xrpc.reg.Registry";
    static final int    XREG_REGISTRY_PROTOCOL_VER_MAJOR = 1;
    static final int    XREG_REGISTRY_PROTOCOL_VER_MINOR = 0;

    /* *********************************************************************
    //
    //  string registerService(string name, string host, int port, prop [])
    //
    //  Registers service with the registry. Parameters:
    //
    //  id    - existing service id (optional)
    //  name  - service (protocol name) being registered.
    //  host  - host name where the service is located (optional)
    //  port  - port number the service is listening on
    //  prop  - the service properties (array of xrpc.reg.Property structures)
    //
    //  If host is not specified, local host "127.0.0.1" is assumed.
    //  Returns the service ID assigned by the registry.
    //
    // *********************************************************************/

    static final String XREG_METHOD_REGISTER_SERVICE = "registerService";
    static final String XREG_PARAM_REGISTER_SERVICE_ID = "id";
    static final String XREG_PARAM_REGISTER_SERVICE_NAME = "name";
    static final String XREG_PARAM_REGISTER_SERVICE_HOST = "host";
    static final String XREG_PARAM_REGISTER_SERVICE_PORT = "port";
    static final String XREG_PARAM_REGISTER_SERVICE_PROP = "prop";

    /* *********************************************************************
    //
    //  void unregisterService(string id)
    //
    //  Unregisters service from the registry. Parameters:
    //
    //  id    - service id assigned by XRPC registry
    //
    // *********************************************************************/

    static final String XREG_METHOD_UNREGISTER_SERVICE = "unregisterService";
    static final String XREG_PARAM_UNREGISTER_SERVICE_ID = "id";

    /* *********************************************************************
    //
    //  xrpc.reg.ServiceInfo [] listServices(string name)
    //
    //  Lists registered services. Parameters:
    //
    //  name  - the name of the service (optional)
    //
    //  If the service name is not specified, then it returns the list of all
    //  registered services.
    //
    // *********************************************************************/

    static final String XREG_METHOD_LIST_SERVICES = "listServices";
    static final String XREG_PARAM_LIST_SERVICES_NAME = "name";

    /* *********************************************************************
    //
    //  xrpc.reg.ServiceInfo queryService(string id)
    //
    //  Returns information about the service identified by its id.
    //  Parameters:
    //
    //  id  - id of the service to query information about
    //
    // *********************************************************************/

    static final String XREG_METHOD_QUERY_SERVICE = "queryService";
    static final String XREG_PARAM_QUERY_SERVICE_ID = "id";

    /* *********************************************************************
    //
    //  boolean setProperty(string id, string name, string value)
    //
    //  Sets property for the specified service
    //
    //  id  - id of the service to modify
    //  name - property name
    //  value - property value
    //
    //  Returns true if the property has been created or updated, false
    //  if the client is not allowed to set this property
    //
    // *********************************************************************/

    static final String XREG_METHOD_SET_PROPERTY = "setProperty";
    static final String XREG_PARAM_SET_PROPERTY_ID = "id";
    static final String XREG_PARAM_SET_PROPERTY_NAME = "name";
    static final String XREG_PARAM_SET_PROPERTY_VALUE = "value";

    /* *********************************************************************
    //
    //  boolean clearProperty(string id, string name)
    //
    //  Clears (removes) a property for the specified service
    //
    //  id  - id of the service to modify
    //  name - name of the property to remove
    //
    //  Returns true if the property has been removed, false if the property
    //  did not exist or the client is not allowed to set this property
    //
    // *********************************************************************/

    static final String XREG_METHOD_CLEAR_PROPERTY = "clearProperty";
    static final String XREG_PARAM_CLEAR_PROPERTY_ID = "id";
    static final String XREG_PARAM_CLEAR_PROPERTY_NAME = "name";

    /* *********************************************************************
    //
    //  void registerListener(string protocol)
    //
    //  registers named listener.
    //
    //  protocol  - the name of the listener protocol to register. Currently,
    //              only "xrpc.reg.RegistryListener" protocol is supported
    //
    // *********************************************************************/

    static final String XREG_METHOD_REGISTER_LISTENER = "registerListener";
    static final String XREG_PARAM_REGISTER_LISTENER_PROTO = "protocol";

    /* *********************************************************************
    //
    //  void unregisterListener(string protocol)
    //
    //  Unregisters named listener.
    //
    //  protocol  - the name of the listener protocol to unregister.
    //              Currently, only "xrpc.reg.RegistryListener" protocol
    //              is supported
    //
    // *********************************************************************/

    static final String XREG_METHOD_UNREGISTER_LISTENER = "unregisterListener";
    static final String XREG_PARAM_UNREGISTER_LISTENER_PROTO = "protocol";

    /* *********************************************************************
    //
    //  boolean lock(string name)
    //
    //  Acquires a named lock.
    //
    //  name  - the name of the lock. If this parameter is missing or the name
    //          is empty, a global unnamed lock is acquired. It's guaranteed
    //          that while unnamed lock is owned by a client, no modifications
    //          will be made to the service database by any other client.
    //          One client session is not allowed to hold more than one lock.
    //
    // *********************************************************************/

    static final String XREG_METHOD_LOCK = "lock";
    static final String XREG_PARAM_LOCK_NAME = "name";

    /* *********************************************************************
    //
    //  void unlock()
    //
    //  Releases a lock held by the client.
    //
    // *********************************************************************/
    static final String XREG_METHOD_UNLOCK = "unlock";

    /*
     * Definition of xrpc.reg.Property structure. After XRPC-encoding
     * it looks like this:
     *
     *    <struct type="xrpc.reg.Property">
     *      <string name="name">foo</string>
     *      <string name="value">bar</string>
     *    </struct>
     */

    static final String XREG_STRUCT_PROPERTY = "xrpc.reg.Property";
    static final String XREG_STRUCT_PROPERTY_NAME = "name";
    static final String XREG_STRUCT_PROPERTY_VALUE = "value";

    /*
     * Definition of xrpc.reg.ServiceInfo structure. After XRPC-encoding
     * it looks like this:
     *
     *   <struct name="xrpc.reg.ServiceInfo">
     *      <string name="id">SADfdasd</string>
     *      <string name="name">Foo</string>
     *      <string name="host">bar.foo.com</string>
     *      <int name="port">12345</int>
     *      <array name="prop">
     *        <struct type="xrpc.reg.Property">
     *          <string name="name">foo</string>
     *          <string name="value">bar</string>
     *        </struct>
     *        <!-- more properties ... -->
     *      </array>
     *    </struct>
     */

    static final String XREG_STRUCT_SERVICE_INFO = "xrpc.reg.ServiceInfo";
    static final String XREG_STRUCT_SERVICE_INFO_ID = "id";
    static final String XREG_STRUCT_SERVICE_INFO_NAME = "name";
    static final String XREG_STRUCT_SERVICE_INFO_HOST = "host";
    static final String XREG_STRUCT_SERVICE_INFO_PORT = "port";
    static final String XREG_STRUCT_SERVICE_INFO_PROP = "prop";

    /* *********************************************************************
    //
    //  "xrpc.reg.RegistryListener" protocol.
    //
    //  This listener receives notification about services being registered
    //  or unregistered.
    //
    // *********************************************************************/

    static final String XREG_LISTENER_PROTOCOL = "xrpc.reg.RegistryListener";
    static final int    XREG_LISTENER_PROTOCOL_VER_MAJOR = 1;
    static final int    XREG_LISTENER_PROTOCOL_VER_MINOR = 0;

    static final String XREG_LISTENER_PARAM_SERVICE = "service";

    /* *********************************************************************
    //
    //  void serviceRegistered(xrpc.reg.ServiceInfo service)
    //
    //  Invoked when a new service is registered
    //
    // *********************************************************************/

    static final String XREG_LISTENER_METHOD_SERVICE_REG  = "serviceRegistered";
    static final String XREG_LISTENER_PARAM_SERVICE_REG = XREG_LISTENER_PARAM_SERVICE;

    /* *********************************************************************
    //
    //  void serviceUnregistered(xrpc.reg.ServiceInfo service)
    //
    //  Invoked when existing service gets unregistered
    //
    // *********************************************************************/

    static final String XREG_LISTENER_METHOD_SERVICE_UNREG = "serviceUnregistered";
    static final String XREG_LISTENER_PARAM_SERVICE_UNREG = XREG_LISTENER_PARAM_SERVICE;
}
