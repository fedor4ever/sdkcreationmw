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
 * $Id: XRpcRegistryClient.java,v 1.6 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcRegistryClient.java
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
import java.util.Map;
import java.util.Iterator;
import java.util.Hashtable;
import java.util.Properties;

/* java.io */
import java.io.IOException;

/* com.nokia.xml.rpc */
import com.nokia.xml.rpc.XRpcInt;
import com.nokia.xml.rpc.XRpcString;
import com.nokia.xml.rpc.XRpcClient;
import com.nokia.xml.rpc.XRpcBoolean;
import com.nokia.xml.rpc.XRpcDataArray;
import com.nokia.xml.rpc.XRpcDataStruct;
import com.nokia.xml.rpc.XRpcConnection;
import com.nokia.xml.rpc.XRpcDataElement;
import com.nokia.xml.rpc.XRpcSocketConnector;
import com.nokia.xml.rpc.XRpcConnectionListener;

/**
 * XRPC Registry client.
 *
 */
public class XRpcRegistryClient implements XRpcRegistry, XRpcRegistryProtocol {

    /**
     * Current connection to the XRPC registry
     */
    private XRpcConnection xrpc;

    /**
     * Object that reconnects to the registry
     */
    private XRpcSocketConnector connector;

    /**
     * Maps service ids into ServiceInfo
     */
    private Map clientMap = new Hashtable();

    /**
     * Creates the XRPC Registry client connected to local XRPC Registry.
     *
     * @throws IOException  if XRPC Registry is not running
     */
    public XRpcRegistryClient() throws IOException {
        this(null, 0);
    }

    /**
     * Creates the XRPC Registry client connected to the XRPC Registry
     * running on specified machine and listening on the specified TCP port.
     *
     * @param host  the machine to connect to. If this parameter
     *              is  <code>null</code>, the client will connect
     *              to the local machine.
     * @param port  the TCP port to connect to. If this parameter is zero,
     *              the client will connect to the default XRPC Registry
     *              port (3651)
     *
     * @throws IOException  if XRPC Registry is not running
     */
    public XRpcRegistryClient(String host, int port) throws IOException {
        if (port == 0) port = XREG_DEFAULT_PORT;
        connector = new XRpcSocketConnector(host, port);
        xrpc = connector.connect();
        xrpc.addConnectionListener(new ConnectionListener());
    }

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
    public boolean lock(String name) throws IOException {
        XRpcDataElement [] args = null;
        if (name != null && name.length() > 0) {
            args = new XRpcDataElement [] {
                new XRpcString(name, XREG_PARAM_LOCK_NAME)
            };
        }
        XRpcDataElement result = call(XREG_METHOD_LOCK, args);
        return ((result instanceof XRpcBoolean) &&
            ((XRpcBoolean)result).booleanValue());
    }

    /**
     * Releases the lock held by the client.
     *
     * @throws          IOException if connection is broken
     */
    public void unlock() throws IOException {
        call(XREG_METHOD_UNLOCK, null);
    }

    /**
     * Close the connection to the XRPC Registry. Be sure to call this
     * method after you no longer need this object. It closes the TCP
     * connection to the XRPC Registry and terminates the listening
     * thread. After this method has been called, the XRPC Registry
     * client object is unusable.
     */
    public void close() {
        xrpc.close();
    }

    /**
     * Registers an XRPC service with the registry.
     *
     * @param sid       existing service id (optional)
     * @param service   service protocol name
     * @param host      host name or (typically) <code>null</code>
     * @param port      listening port
     * @param props     service properties
     *
     * @return          the service id assigned by the registry
     * @throws          IOException if connection is broken
     */
    private synchronized String registerService(String sid,
        String service, String host, int port, Properties props)
        throws IOException {

        // calculate number of parameters. Two of them, service and port
        // are mandatory, others are optional
        int i = 0, n = 2;
        if (host != null) n++;
        if (props != null && !props.isEmpty()) n++;
        if (sid != null && sid.length() > 0) n++;
        XRpcDataElement [] args = new XRpcDataElement [n];
        args[i++] = new XRpcString(service, XREG_PARAM_REGISTER_SERVICE_NAME);
        args[i++] = new XRpcInt(port, XREG_PARAM_REGISTER_SERVICE_PORT);
        if (sid != null && sid.length() > 0) {
            args[i++] = new XRpcString(sid,XREG_PARAM_REGISTER_SERVICE_ID);
        }
        if (host != null) {
            args[i++] = new XRpcString(host,XREG_PARAM_REGISTER_SERVICE_HOST);
        }
        if (props != null && !props.isEmpty()) {
            int k = 0;
            XRpcDataElement [] p = new XRpcDataElement[props.size()];
            Iterator entries = props.entrySet().iterator();
            while (entries.hasNext()) {
                Map.Entry entry = (Map.Entry)entries.next();
                String name = (String)entry.getKey();
                String value = (String)entry.getValue();
                p[k++] = new XRpcDataStruct(new XRpcDataElement [] {
                    new XRpcString(name,XREG_STRUCT_PROPERTY_NAME),
                    new XRpcString(value,XREG_STRUCT_PROPERTY_VALUE)
                }, XREG_STRUCT_PROPERTY);
            }
            args[i++] = new XRpcDataArray(p,XREG_STRUCT_SERVICE_INFO_PROP);
        }

        // call the XRPC Registry
        XRpcDataElement id = call(XREG_METHOD_REGISTER_SERVICE, args);
        if (id instanceof XRpcString) {
            sid = ((XRpcString)id).getString();
            clientMap.put(sid, new ClientInfo(sid,service,host,port,props));
            return sid;
        }

        // throw an exception here? The registry returned something that
        // we didn't expect
        return null;
    }

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
    public String registerService(String service, String host, int port,
                                  Properties props) throws IOException {
        return registerService(null, service, host, port, props);
    }

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
    public synchronized void unregisterService(String id) throws IOException {
        if (id == null) throw new NullPointerException("id");
        clientMap.remove(id);
        call(XREG_METHOD_UNREGISTER_SERVICE, id,
             XREG_PARAM_UNREGISTER_SERVICE_ID);
    }

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
    public ServiceInfo [] listServices(String protocol) throws IOException {
        XRpcDataElement list = call(XREG_METHOD_LIST_SERVICES, protocol,
             XREG_PARAM_LIST_SERVICES_NAME);
        if (list instanceof XRpcDataArray) {
            XRpcDataArray array = (XRpcDataArray)list;
            int k = 0, n = array.getElementCount();
            ServiceInfo [] result = new ServiceInfo[n];
            for (int i=0; i<n; i++) {
                XRpcDataElement elem = array.getDataElement(i);
                if (elem instanceof XRpcDataStruct) {
                    XRpcDataStruct si = (XRpcDataStruct)elem;
                    if (XREG_STRUCT_SERVICE_INFO.equals(si.getStructType())) {
                        result[k++] = new XRpcServiceInfo(si);
                    }
                }
            }
            if (k < result.length) {
                ServiceInfo [] dest = new ServiceInfo[k];
                System.arraycopy(result, 0, dest, 0, k);
                result = dest;
            }
            return result;
        }
        return new ServiceInfo[0];
    }

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
    public ServiceInfo queryService(String id) throws IOException {
        if (id == null) throw new NullPointerException("id");
        XRpcDataElement info = call(XREG_METHOD_QUERY_SERVICE, id,
             XREG_PARAM_QUERY_SERVICE_ID);
        if (info instanceof XRpcDataStruct) {
            XRpcDataStruct struct = (XRpcDataStruct)info;
            if (XREG_STRUCT_SERVICE_INFO.equals(struct.getStructType())) {
                return new XRpcServiceInfo(struct);
            }
        }
        return null;
    }

    /**
     * Makes a blocking XRPC Registry call. Waits until the call is complete
     * and returns the results if any.
     *
     * @param method    the method to call
     * @param param     the method parameters
     *
     * @return          the method's return value
     *
     * @throws          IOException if connection is broken
     */
    private XRpcDataElement call(String method, XRpcDataElement [] param)
    throws IOException {
        XRpcClient client = xrpc.getClient();
        return client.invokeAndWait(XREG_REGISTRY_PROTOCOL, method, param);
    }

    /**
     * Makes a blocking XRPC Registry call that takes a single string
     * parameter. Waits until the call is complete and returns the results
     * if any.
     *
     * @param method    the method to call
     * @param param     the method parameter
     * @param name      the name of the parameter
     *
     * @return          the method's return value
     *
     * @throws          IOException if connection is broken
     */
    private XRpcDataElement call(String method, String param, String name)
    throws IOException {
        XRpcDataElement [] args = null;
        if (param != null) {
            args = new XRpcDataElement [] {new XRpcString(param, name)};
        }
        return call(method, args);
    }

    /**
     * Detects broken connection and re-registers services with the XRPC
     * registry
     */
    private class ConnectionListener implements XRpcConnectionListener {
        public void connectionClosed(XRpcConnection connection) {
            synchronized (XRpcRegistryClient.this) {
                if (xrpc == connection) {
                    ServiceInfo [] services;
                    services = (ServiceInfo[])clientMap.values().toArray(
                        new ServiceInfo[clientMap.size()]);

                    int delay = 100;
                    while (!Thread.interrupted()) {
                        try {
                            clientMap.clear();
                            xrpc = connector.connect();
                            for (int i=0; i<services.length; i++) {
                                ServiceInfo svc = services[i];
                                registerService(svc.getID(),svc.getProtocol(),
                                                svc.getHost(), svc.getPort(),
                                                svc.getProperties());
                            }
                            xrpc.addConnectionListener(this);
                            break;
                        } catch (IOException x) {
                            // continue to try
                            try {
                                Thread.sleep(delay);
                            } catch (InterruptedException y) {
                                break;
                            } catch (Exception z) {
                                z.printStackTrace();
                            }
                            if (delay < 10000) delay += 100;
                        }
                    }
                }
            }
        }
    }

    /**
     * Information about the registered service.
     */
    private static class ClientInfo implements ServiceInfo {
        private String id;
        private String service;
        private String host;
        private int port;
        private Properties props;

        ClientInfo(String id, String service, String host, int port,
                   Properties props) {
            this.id = id;
            this.service = service;
            this.host = host;
            this.port = port;
            if (props != null) {
                this.props = (Properties)props.clone();
            }
        }
        public String getID() { return id; }
        public String getProtocol() { return service; }
        public String getHost() { return host; }
        public int getPort() { return port; }
        public Properties getProperties() { return props; }
    }

    /**
     * The service information returned by the <code>XRpcRegistry</code>
     * <code>registerService</code> call.
     */
    private static class XRpcServiceInfo implements ServiceInfo {
        private XRpcDataStruct info;
        private Properties props;

        XRpcServiceInfo(XRpcDataStruct xrpcInfo) {
            info = xrpcInfo;
        }
        private String getString(String field) {
            XRpcString s = info.getStringField(field);
            return (s == null) ? null : s.getString();
        }
        public String getID() {
            return getString(XREG_STRUCT_SERVICE_INFO_ID);
        }
        public String getProtocol() {
            return getString(XREG_STRUCT_SERVICE_INFO_NAME);
        }
        public String getHost() {
            return getString(XREG_STRUCT_SERVICE_INFO_HOST);
        }
        public int getPort() {
            XRpcInt i = info.getIntField(XREG_STRUCT_SERVICE_INFO_PORT);
            return (i == null) ? 0 : i.getNumber().intValue();
        }
        public Properties getProperties() {
            if (props == null) {
                props = new Properties();
                XRpcDataArray xrpcProps = info.getArrayField(
                    XREG_STRUCT_SERVICE_INFO_PROP);

                if (xrpcProps != null) {
                    int n = xrpcProps.getElementCount();
                    for (int i=0; i<n; i++) {
                        XRpcDataElement elem = xrpcProps.getDataElement(i);
                        if (elem instanceof XRpcDataStruct) {
                            XRpcDataStruct p = (XRpcDataStruct)elem;
                            String type = p.getStructType();
                            if (XREG_STRUCT_PROPERTY.equals(type)) {

                                // name
                                XRpcString nameElem = p.getStringField(
                                    XREG_STRUCT_PROPERTY_NAME);
                                String name = null;
                                if (nameElem != null) {
                                    name = nameElem.getString();
                                }

                                // value
                                XRpcString valueElem = p.getStringField(
                                    XREG_STRUCT_PROPERTY_VALUE);
                                String value = null;
                                if (valueElem != null) {
                                    value = valueElem.getString();
                                }

                                if (name != null && value != null) {
                                    props.setProperty(name, value);
                                }
                            }
                        }
                    }
                }
            }
            return props;
        }

        /**
         * For debug trace purposes ONLY. Subject to change.
         * @return information about this object, useful for debug output
         */
        public String toString() {
            StringBuffer sb = new StringBuffer();
            sb.append("ServiceInfo[id=").append(getID());
            sb.append(",protocol=").append(getProtocol());
            sb.append(",host=").append(getHost());
            sb.append(",port=").append(getPort());
            sb.append(",props={");
            int i = 0;
            Properties p = getProperties();
            Iterator entries = p.entrySet().iterator();
            while (entries.hasNext()) {
                Map.Entry entry = (Map.Entry)entries.next();
                String name = (String)entry.getKey();
                String value = (String)entry.getValue();
                if (i++ > 0) sb.append(",");
                sb.append(name).append("=").append(value);
            }
            sb.append("}]");
            return sb.toString();
        }
    }

    /**
     * Test program
     * @param args ignored
     */
    public static void main(String [] args) {
        try {
            XRpcRegistry client = new XRpcRegistryClient();
            ServiceInfo [] info = client.listServices(null);
            for (int i=0; i<info.length; i++) {
                System.out.println(info[i]);
            }
            client.close();
        } catch (Exception x) {
            x.printStackTrace();
        }
        System.exit(0);
    }
}
