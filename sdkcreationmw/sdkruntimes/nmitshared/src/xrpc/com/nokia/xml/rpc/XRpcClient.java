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
 * $Id: XRpcClient.java,v 1.22 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcClient.java
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

/* java.util */
import java.util.Map;
import java.util.List;
import java.util.HashMap;
import java.util.ArrayList;

/* java.io */
import java.io.IOException;
import java.io.PrintStream;

/**
 * XRPC Client. You get an instance of this class from {@link XRpcConnection}.
 *
 * @see XRpcConnection
 */
public class XRpcClient {

    private Object lock = new Object();
    private long timeout = 0;

    private XRpcServer server;
    private XRpcConnection session;
    private XRpcResponseHandler responseHandler = null;
    private XRpcCore core;

    private PrintStream debugStream = null;
    private static int nextInputThreadIndex = 1;

    /**
     * maps call tag into call context object
     */
    private Map callContextMap = new HashMap(10);

    /**
     * this field is used for keeping track of current tag.
     * the tags are connection specific, i.e. different connections
     * may use same tags.
     */
    private char nextTagChar = 'A';

    /**
     * this field is used for keeping track of current tag.
     * the tags are connection specific, i.e. different connections
     * may use same tags.
     */
    private int nextTagNumber = 0;

    /**
     * Creates <code>XRpcClient</code> connected to the specified port
     * on the local host
     *
     * @param port the port to connect to
     *
     * @return the created XRpcClient object connected to the specified port
     *     on the local host
     *
     * @throws IOException if the connection fails
     */
    public static XRpcClient connect(int port) throws IOException {
        return connect(null, port);
    }

    /**
     * Creates <code>XRpcClient</code> connected to the specified port
     * at the specified host
     *
     * @param host the host to connect to, <code>null</code> if localhost
     * @param port the port to connect to
     *
     * @return the created XRpcClient object connected to the specified port
     *     at the specified host
     *
     * @throws IOException if the connection fails
     */
    public static XRpcClient connect(String host, int port)
    throws IOException {
        return XRpcConnection.connect(host, port).getClient();
    }

    /**
     * Creates <code>XRpcClient</code> for the specified session
     *
     * @param session the session to use by this client.
     */
    XRpcClient(XRpcConnection session) {
        this.session = session;
        server = session.getServer();

        responseHandler = new ResponseHandler();
        session.setResponseHandler(responseHandler);

        // start receiving thread
        new InputThread().start();
    }

    /**
     * Sets stream for debug output. Setting stream to <code>null</code>
     * disables debug output
     *
     * @param stream the stream that receives the debug output for this
     *   client, <code>null</code> to disable debug output.
     */
    public void setDebugStream(PrintStream stream) {
        synchronized (lock) {
            debugStream = stream;
            session.setDebugStream(stream);
        }
    }

    /**
     * Sets timeout for blocking operations. By default there's no timeout.
     * @param ms the timeout in milliseconds. Zero or negative number means
     * infinite timeout
     */
    public void setTimeout(long ms) {
        timeout = ms;
    }

    /**
     * Returns timeout for blocking operations.
     * @return the timeout in milliseconds. Zero or negative number means
     * infinite timeout
     */
    public long getTimeout() {
        return timeout;
    }

    /**
     * Invokes the remote side asynchronously. This call returns
     * immediately. The results of the call may be obtained later
     * by calling {@link #waitReply}
     *
     * @param protocol the target protocol
     * @param method the method to call
     * @param param the parameters of the call
     *
     * @return the unique id assigned to the the call.
     * @throws IOException if the call fails because of the I/O problem
     */
    public String invoke(String protocol,
                         String method,
                         XRpcDataElement [] param) throws IOException {
        XRpcCall call = createCall(protocol,method, param);
        call.write(getConnection().getWriter());
        return call.getCallID();
    }

    /**
     * Invokes the remote side. The remote side will never reply
     * to this call.
     *
     * @param protocol the target protocol
     * @param method the method to call
     * @param par the parameters of the call
     *
     * @throws IOException if XRPC call has failed due to I/O problems
     */
    public void invokeNoReply(String protocol,
                              String method,
                              XRpcDataElement [] par) throws IOException {
        XRpcCall call = new XRpcCall(null, protocol, method, par);
        call.write(getConnection().getWriter());
    }

    /**
     * Invokes the remote side with RPC call that takes single parameter.
     * The remote side will never reply to this call.
     *
     * @param protocol the target protocol
     * @param method the method to call
     * @param par the parameter of the call
     *
     * @throws IOException if XRPC call has failed due to I/O problems
     */
    public void invokeNoReply(String protocol,
                              String method,
                              XRpcDataElement par) throws IOException {
        XRpcCall call = new XRpcCall(null, protocol, method, par);
        call.write(getConnection().getWriter());
    }

    /**
     * Invokes the remote side with RPC call that takes no arguments.
     * The remote side will never reply to this call.
     *
     * @param protocol the target protocol
     * @param method the method to call
     *
     * @throws IOException if XRPC call has failed due to I/O problems
     */
    public void invokeNoReply(String protocol, String method)
    throws IOException {
        XRpcCall call = new XRpcCall(null, protocol, method);
        call.write(getConnection().getWriter());
    }

    /**
     * Generates a unique id and creates a call context.
     * Does not actually submit the call.
     *
     * @param protocol the target protocol
     * @param method the method to invoke
     * @param param the method parameters
     *
     * @return the call object
     */
    private XRpcCall createCall(String protocol,
                                String method,
                                XRpcDataElement [] param) {
        synchronized (lock) {
            String id;
            do {
                if (nextTagNumber == Integer.MAX_VALUE) {
                    nextTagNumber = 0;
                    if (nextTagChar == 'Z') {
                        nextTagChar = 'A';
                    } else {
                        nextTagChar++;
                    }
                } else {
                    nextTagNumber++;
                }
                StringBuffer sb = new StringBuffer();
                sb.append(nextTagChar).append(nextTagNumber);
                id = sb.toString();
            } while (callContextMap.get(id) != null);

            // create reply and put it into the map
            XRpcCall call = new XRpcCall(id, protocol, method, param);
            callContextMap.put(id, new CallContext(call));
            return call;
        }
    }

    /**
     * Invokes the remote side and waits for reply
     *
     * @param call the call context
     *
     * @return the result of the call
     * @throws IOException if XRPC call has failed due to I/O problems
     */
    private XRpcDataElement invokeAndWait(XRpcCall call) throws IOException {
        CallContext context;
        synchronized (lock) {
            context = (CallContext)callContextMap.get(call.getCallID());
        }
        if (context == null) {
            throw new IllegalArgumentException("invalid call");
        }

        call.write(getConnection().getWriter());
        return waitFor(context);
    }

    /**
     * Invokes the remote method and waits for completion.
     *
     * @param protocol the target protocol
     * @param method the method to invoke
     * @param param the method parameters
     *
     * @return the result of the call
     * @throws IOException if XRPC call has failed due to I/O problems
     */
    public XRpcDataElement invokeAndWait(String protocol,
                                         String method,
                                         XRpcDataElement [] param)
                                         throws IOException {
        return invokeAndWait(createCall(protocol, method, param));
    }

    /**
     * Invokes the remote side and waits for reply
     *
     * @param context the call context
     *
     * @return the result of the call
     * @throws IOException if XRPC call has failed due to I/O problems
     */
    private XRpcDataElement waitFor(CallContext context) throws IOException {
        long now = System.currentTimeMillis();
        long deadline = now + timeout;
        try {
            synchronized (context) {

                // wait with timeout
                if (deadline > now) {
                    while (!context.done) {
                        if (deadline <= now) {
                            throw new IOException("XRPC call timed out");
                        }
                        try { context.wait(deadline - now); }
                        catch (Throwable x) {
                            x.printStackTrace();
                        }
                        now = System.currentTimeMillis();
                    }

                // infinite wait
                } else {
                    while (!context.done) {
                        try { context.wait(); }
                        catch (Throwable x) {
                            x.printStackTrace();
                        }
                    }
                }
            }
        } finally {
            synchronized (lock) {
                callContextMap.remove(context.call.getCallID());
            }
        }
        if (context.error) throw new IOException("XRPC call failed");
        return context.result;
    }

    /**
     * Returns the client to server connection.
     *
     * @return the active connection
     * @throws IOException if connection is already closed
     */
    public XRpcConnection getConnection() throws IOException {
        if (session.isClosed()) throw new IOException("connection closed");
        return session;
    }

    /**
     * Close the connection with the server
     */
    public void close() {
        session.close();
    }

    private static synchronized int getInputThreadIndex() {
        return nextInputThreadIndex++;
    }

    //=======================================================================
    //          C O D E    P R O T O C O L
    //=======================================================================

    /**
     * Returns the interface to "xrpc.core" protocol
     * @return the interface to "xrpc.core" protocol
     */
    public XRpcCore getCore() {
        if (core == null) {
            core = new Core();
        }
        return core;
    }

    /**
     * Client implementation of "xrpc.core" protocol
     */
    private class Core implements XRpcCore, XRpcConstants {

        /**
         * This call does nothing. It exists for the single purpose of
         * checking if connection is still alive. Works even if "xrpc.core"
         * protocol isn't registered on the server.
         *
         * @throws IOException if connection is broken
         */
        public void ping() throws IOException {
            invokeAndWait(XCORE_PROTOCOL,XCORE_METHOD_PING,null);
        }

        /**
         * Returns the list of protocols registered on the server side of the
         * connection.
         *
         * @return  the list of protocols, empty array if no protocol is
         *          registered. The latter should never happen because
         *          "xrpc.core" is one of the protocols. Never returns
         *          <code>null</code>.
         * @throws  IOException if connection is broken
         */
        public String [] listProtocols() throws IOException {
            String [] protocols = null;
            XRpcDataElement e;
            e = invokeAndWait(XCORE_PROTOCOL,XCORE_METHOD_LIST_PROTOCOLS,null);
            if (e instanceof XRpcDataArray) {
                XRpcDataArray a = (XRpcDataArray)e;
                int n = a.getElementCount();
                if (n == 0) {
                    protocols = new String[0];
                } else {
                    List protList = new ArrayList(n);
                    for (int i=0; i<n; i++) {
                        XRpcDataElement info = a.getDataElement(i);
                        if (info instanceof XRpcString) {
                            protList.add(((XRpcString)info).getString());
                        }
                    }
                    n = protList.size();
                    protocols = (String[])protList.toArray(new String[n]);
                }
            }
            return protocols;
        }

        /**
         * Returns information about the specified protocol.
         *
         * @param   name    the protocol name
         * @return  information about the specified protocol,
         *          <code>null</code> if protocol is not registered.
         * @throws  IOException if connection is broken
         */
        public ProtocolInfo getProtocolInfo(String name) throws IOException {
            XRpcDataStruct data = getProtocolData(name);
            if (data != null) {
                return new Protocol(data);
            }
            return null;
        }

        /**
         * Returns information about the specified protocol.
         *
         * @param   name    the protocol name
         * @return  information about the specified protocol in XRPC format,
         *          <code>null</code> if protocol is not registered.
         * @throws  IOException if connection is broken
         */
        XRpcDataStruct getProtocolData(String name) throws IOException {
            XRpcDataElement e;
            XRpcDataElement [] args = null;
            if (name != null) {
                args = new XRpcDataElement [] {
                    new XRpcString(name, XCORE_PARAM_PROTO_NAME)
                };
            }
            e = invokeAndWait(XCORE_PROTOCOL,XCORE_METHOD_GET_PROTOCOL_INFO,args);
            if (e instanceof XRpcDataStruct) {
                XRpcDataStruct struct = (XRpcDataStruct)e;
                String type = struct.getStructType();
                if (XCORE_STRUCT_PROTO_INFO.equals(type)) {
                    return struct;
                }
            }
            return null;
        }

        /**
         * Protocol information
         */
        private class Protocol implements XRpcCore.ProtocolInfo {

            private String name;
            private XRpcDataStruct protData;
            private XRpcDataStruct protVersion;

            Protocol(String name) {
                this.name = name;
            }
            Protocol(XRpcDataStruct data) {
                name = data.getStringData(XCORE_STRUCT_PROTO_INFO_NAME);
                setData(data);
            }
            private void setData(XRpcDataStruct data) {
                protData = data;
                protVersion = data.getStructField(XCORE_STRUCT_PROTO_INFO_VER,
                                                  XCORE_STRUCT_PROTO_VER);
            }
            private XRpcDataStruct getData() {
                if (protData != null) {
                    try { setData(getProtocolData(name)); }
                    catch (IOException x) {
                        PrintStream out = debugStream;
                        if (out != null) x.printStackTrace(out);
                    }
                }
                return protData;
            }
            private int getVersion(String which) {
                if (protVersion != null) {
                    XRpcInt i = protVersion.getIntField(which);
                    if (i != null) return i.getNumber().intValue();
                }
                return -1;
            }
            private String getString(String field) {
                XRpcDataStruct data = getData();
                return ((data != null) ? data.getStringData(field) : null);
            }
            public String getName() {
                return name;
            }
            public String getDescription() {
                return getString(XCORE_STRUCT_PROTO_INFO_DESCR);
            }
            public String getVendor() {
                return getString(XCORE_STRUCT_PROTO_INFO_VENDOR);
            }
            public String getVendorURL() {
                return getString(XCORE_STRUCT_PROTO_INFO_VURL);
            }
            public String getProtocolURL() {
                return getString(XCORE_STRUCT_PROTO_INFO_PURL);
            }
            public int getMajorVersion() {
                return getVersion(XCORE_STRUCT_PROTO_VER_MAJOR);
            }
            public int getMinorVersion() {
                return getVersion(XCORE_STRUCT_PROTO_VER_MINOR);
            }
            private StringBuffer append(StringBuffer buf, String name) {
                String s = getString(name);
                if (s != null) {
                    buf.append(", ").append(name).append("=").append(s);
                }
                return buf;
            }
            public String toString() {
                StringBuffer buf = new StringBuffer("Protocol[");
                buf.append("name=").append(getName());
                append(buf, XCORE_STRUCT_PROTO_INFO_DESCR);
                append(buf, XCORE_STRUCT_PROTO_INFO_VENDOR);
                append(buf, XCORE_STRUCT_PROTO_INFO_VURL);
                append(buf, XCORE_STRUCT_PROTO_INFO_PURL);
                int v1 = getMajorVersion();
                int v2 = getMinorVersion();
                buf.append(", version=").append(v1).append(".").append(v2);
                buf.append("]");
                return buf.toString();
            }
        }
    }

    //=======================================================================
    //          R E S P O N S E    H A N D L E R
    //=======================================================================

    private static class CallContext {
        final XRpcCall call;
        boolean done = false;
        boolean error = false;
        XRpcDataElement result = null;
        CallContext(XRpcCall call) {
            this.call = call;
        }
    }

    private class InputThread extends Thread {
        InputThread() {
            super("XRPC-ClientInputThread-" + getInputThreadIndex());
            setDaemon(true);
        }

        public void run() {
            try {
                session.run();
            } finally {
                session.close();

                // are there any calls in progress
                CallContext [] calls = null;
                synchronized (lock) {
                    int n = callContextMap.size();
                    if (n > 0) {
                        calls = (CallContext[])
                        callContextMap.values().toArray(new CallContext[n]);
                        callContextMap.clear();
                    }
                }

                if (calls != null) {
                    // cancel calls in progress, unblock waiters
                    for (int i=0; i<calls.length; i++) {
                        CallContext context = calls[i];
                        synchronized (context) {
                            context.done = true;
                            context.error = true;
                            context.notifyAll();
                        }
                    }
                }
            }
        }
    }

    private class ResponseHandler implements XRpcResponseHandler {

        /**
         * Handles successful completion of the call
         *
         * @param id the call id
         * @param result the result returned by the remote side
         */
        public void handleResponse(String id, XRpcDataElement result) {
            CallContext context;
            synchronized (lock) {
                context = (CallContext)callContextMap.get(id);
            }
            if (context != null) {
                synchronized (context) {
                    context.done = true;
                    context.error = false;
                    context.result = result;
                    context.notifyAll();
                }
            }
        }
    }
}
