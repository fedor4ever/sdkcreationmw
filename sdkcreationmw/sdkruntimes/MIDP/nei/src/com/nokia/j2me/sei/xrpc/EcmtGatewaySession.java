/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XRPC connection between the gateway and UEI executable.
*
*/


package com.nokia.j2me.sei.xrpc;

/* java.util */
import java.util.List;
import java.util.Vector;
import java.util.Properties;

/* java.io */
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/* java.net */
import java.net.Socket;
import java.net.ServerSocket;

/* com.nokia.xml.rpc */
import com.nokia.xml.rpc.XRpcInt;
import com.nokia.xml.rpc.XRpcUtils;
import com.nokia.xml.rpc.XRpcShort;
import com.nokia.xml.rpc.XRpcClient;
import com.nokia.xml.rpc.XRpcServer;
import com.nokia.xml.rpc.XRpcMethod;
import com.nokia.xml.rpc.XRpcProtocol;
import com.nokia.xml.rpc.XRpcBinaryData;
import com.nokia.xml.rpc.XRpcConnection;
import com.nokia.xml.rpc.XRpcDataElement;

/* com.nokia.xml.rpc.registry */
import com.nokia.xml.rpc.registry.XRpcRegistry;
import com.nokia.xml.rpc.registry.XRpcRegistryClient;

/**
 * XRPC connection between the gateway and UEI executable.
 */
public class EcmtGatewaySession implements EcmtGatewayProtocol {

    private static final int SESSION_ID = 0;
    private static final String XRPC_LOGON_ID_PROP = "logon-id";
    private static final String SYSTEM_LOGON_ID_PROP = "xrpc.logon-id";
    private static final String SYSTEM_GW_HOST_PROP = "ecmt.gateway.host";

    private XRpcConnection xrpc;
    private XRpcServer server;
    private EcmtGatewayConnection controlConnection;
    private List connList = new Vector();
    private int nextConnId = 1;

    /**
     * Creates EcmtGatewaySession. That involves finding EcmtGateway using
     * XRPC registry, connecting to it and creating default control
     * connection.
     *
     * @throws IOException in case I/O troubles or if EcmtGateway is not
     * running.
     */
    public EcmtGatewaySession() throws IOException {
        // Connect to XRPC registry. Note that we are looking for EcmtGw
        // protocol and assuming that it understands SEI protocol as well.
        // I'm not sure if we need to register SEI protocol with XRPC
        // registry. At the time of this writing, we don't.
        String host = System.getProperty(SYSTEM_GW_HOST_PROP);
        XRpcRegistry reg = new XRpcRegistryClient(host,XRpcRegistry.XREG_DEFAULT_PORT);
        XRpcRegistry.ServiceInfo[] list = reg.listServices(GW_PROTOCOL);

        // Select appropriate gateway service based on the logon id
        XRpcRegistry.ServiceInfo gw = null;
        if (list != null && list.length > 0) {
            String logonId = System.getProperty(SYSTEM_LOGON_ID_PROP);
            // Don't check logon id when connecting to a remote gateway
            if (logonId != null && host == null) {
                int n = list.length;
                for (int i=0; i<n; i++) {
                    Properties props = list[i].getProperties();
                    String value = props.getProperty(XRPC_LOGON_ID_PROP);
                    if (value == null) {
                        // no logon id, weak candidate
                        if (gw != null) gw = list[i];
                    } else if (value.equals(logonId)) {
                        // perfect match
                        gw = list[i];
                        break;
                    }
                }
            } else {
                // no logon id, pick the first one
                gw = list[0];
            }
        }

        // Connect to the selected service
        if (gw != null) {
            server = new XRpcServer();
            server.addProtocol(new Protocol());
            xrpc = XRpcConnection.connect(host, gw.getPort());
            xrpc.setServer(server);
            controlConnection = new EcmtGatewayConnection(this,0,0,0);
            connList.add(controlConnection);

            // Tell the gateway that the session has been created
            xrpc.getClient().invokeAndWait(SEI_PROTOCOL, SEI_OPEN_METHOD,
                new XRpcDataElement[] {
                    new XRpcInt(SESSION_ID,SEI_OPEN_SID_PARAM),
                });
        } else {
            throw new IOException("Ecmt Gateway is not running");
        }
    }

    /**
     * Returns {@link XRpcClient} object that can be used to make XRPC calls
     * over the XRPC connection associated with this session.
     * @return XRPC client interface
     */
    XRpcClient getClient() {
        return xrpc.getClient();
    }

    /**
     * Gets the session id. Since we only support one session per XRPC
     * connection, this method always returns zero.
     * @return the session id.
     */
    int getSessionId() {
        return SESSION_ID;
    }

    /**
     * Gets input stream for the control connection.
     * @return input stream for the control connection.
     */
    public InputStream getInputStream() {
        return controlConnection.getInputStream();
    }

    /**
     * Gets output stream for the control connection.
     * @return output stream for the control connection.
     */
    public OutputStream getOutputStream() {
        return controlConnection.getOutputStream();
    }

    /**
     * Maps KVM port number to local port.
     * @param kvmPort the port to map.
     * @return port number than can be used within this VM
     */
    public synchronized int mapVmPort(int kvmPort) {
        int n = connList.size();
        for (int i=0; i<n; i++) {
            EcmtGatewayConnection c = (EcmtGatewayConnection)connList.get(i);
            if (c.getKVMPort() == kvmPort) {
                return c.getLocalPort();
            }
        }
        return -1;
    }

    /**
     * Prepares for connecting to a piece of code running on the handset.
     * Starts listening on an arbitrary TCP port and returns the port number.
     * The connection is forwarded to the handset.
     *
     * @param kvmPort the handset port
     * @return local listening TCP port
     * @throws IOException in case if I/O trouble.
     */
    public synchronized int listenAndConnect(int kvmPort)
    throws IOException {
        int connId = nextConnId++;
        xrpc.getClient().invokeAndWait(SEI_PROTOCOL, SEI_CONNECT_METHOD,
            new XRpcDataElement[] {
                new XRpcInt(SESSION_ID,SEI_CONNECT_SID_PARAM),
                new XRpcInt(connId, SEI_CONNECT_CID_PARAM),
                new XRpcShort((short)kvmPort, SEI_CONNECT_PORT_PARAM)
            });

        ServerSocket socket = new ServerSocket();
        socket.bind(null);
        int localPort = socket.getLocalPort();
        EcmtGatewayConnection conn = new EcmtGatewayConnection(this,connId,kvmPort,localPort);
        new AcceptThread(socket, conn).start();
        connList.add(conn);
        return localPort;
    }

    private synchronized EcmtGatewayConnection getConnection(int cid) {
        int n = connList.size();
        for (int i=0; i<n; i++) {
            EcmtGatewayConnection c = (EcmtGatewayConnection)connList.get(i);
            if (c.getConnId() == cid) {
                return c;
            }
        }
        return null;
    }

    private synchronized boolean resetConnection(int cid) {
        int n = connList.size();
        for (int i=0; i<n; i++) {
            EcmtGatewayConnection c = (EcmtGatewayConnection)connList.get(i);
            if (c.getConnId() == cid) {
                try { c.close(); }
                catch (IOException x) {}
                connList.remove(i);
                return true;
            }
        }
        return false;
    }

    public synchronized void close() {
        int n = connList.size();
        for (int i=0; i<n; i++) {
            EcmtGatewayConnection c = (EcmtGatewayConnection)connList.get(i);
            try { c.close(); }
            catch (IOException x) {}
        }
        connList.clear();
    }

    //=======================================================================
    //          X R P C    M A G I C
    //=======================================================================

    /**
     * XRPC protocol handler
     */
    private class Protocol implements XRpcProtocol {
        public String getName() { return SEI_PROTOCOL; }
        public String getDescription() { return "MIDP Debug"; }
        public String getVendor() { return "Nokia"; }
        public String getVendorURL() { return "n/a"; }
        public String getProtocolURL() { return "n/a"; }
        public int getMajorVersion() { return 1; }
        public int getMinorVersion() { return 0; }

        /**
         * Creates a new handler for the specified method. Returns
         * <code>null</code> if there's no such method.
         *
         * @param name the method name
         * @return the method handler, <code>null</code> of none
         */
        public XRpcMethod getMethod(String name) {
            if (name.equals(SEI_SEND_METHOD)) {
                return new XSend();
            } else if (name.equals(SEI_RESET_METHOD)) {
                return new XReset();
            } else if (name.equals(SEI_CLOSE_METHOD)) {
                return new XClose();
            } else {
                return null;
            }
        }
    }

    /**
     * "send" method handler
     */
    private class XSend implements XRpcMethod {
        public XRpcDataElement invoke(XRpcDataElement[] args) {
            // Decode parameters
            XRpcInt sid, cid;
            XRpcBinaryData data;
            sid = XRpcUtils.findIntElementByName(args,SEI_SEND_SID_PARAM);
            cid = XRpcUtils.findIntElementByName(args,SEI_SEND_CID_PARAM);
            data = XRpcUtils.findBinaryElementByName(args,SEI_SEND_DATA_PARAM);
            if (sid != null && cid != null && data != null) {
                int sessionId = sid.getNumber().intValue();
                if (sessionId == SESSION_ID) {
                    int connId = cid.getNumber().intValue();
                    EcmtGatewayConnection c = getConnection(connId);
                    c.addBytes(data.toByteArray());
                }
            }
            return null;
        }
    }

    /**
     * "reset" method handler
     */
    private class XReset implements XRpcMethod {
        public XRpcDataElement invoke(XRpcDataElement[] args) {
            // Decode parameters
            XRpcInt sid, cid;
            sid = XRpcUtils.findIntElementByName(args,SEI_RESET_SID_PARAM);
            cid = XRpcUtils.findIntElementByName(args,SEI_RESET_CID_PARAM);
            if (sid != null && cid != null) {
                int sessionId = sid.getNumber().intValue();
                if (sessionId == SESSION_ID) {
                    int connId = cid.getNumber().intValue();
                    resetConnection(connId);
                }
            }
            return null;
        }
    }

    /**
     * "close" method handler
     */
    private class XClose implements XRpcMethod {
        public XRpcDataElement invoke(XRpcDataElement[] args) {
            // Decode parameters
            XRpcInt sid;
            sid = XRpcUtils.findIntElementByName(args,SEI_RESET_SID_PARAM);
            if (sid != null) {
                int sessionId = sid.getNumber().intValue();
                if (sessionId == SESSION_ID) {
                    close();
                }
            }
            return null;
        }
    }

    //=======================================================================
    //          F O R W A R D I N G
    //=======================================================================

    /**
     * Generic interface that can be used to close something I/O related.
     * Such as a socket for example.
     */
    interface Close {
        public void close() throws IOException;
    }

    /**
     * A thread that waits for one incoming connection, then spawns two
     * forwarding threads, closes the listening socket and exits.
     */
    private class AcceptThread extends Thread {
        private ServerSocket socket;
        private EcmtGatewayConnection conn;
        AcceptThread(ServerSocket s, EcmtGatewayConnection c) {
            super("EcmtAccept-" + s.getLocalPort());
            socket = s;
            conn = c;
        }
        public void run() {
            try {
                Socket s = socket.accept();
                new ConnectionThread("EcmtToSocket-" + socket.getLocalPort(),
                    new CloseSocket(s), conn.getInputStream(),
                    s.getOutputStream()).start();
                new ConnectionThread("SocketToEcmt-" + socket.getLocalPort(),
                    new CloseConnection(conn), s.getInputStream(),
                    conn.getOutputStream()).start();
            } catch (Exception x) {
            } finally {
                try { socket.close(); }
                catch (IOException x) {}
            }
        }
    }

    /**
     * Implementation of Close interface that closes EcmtGatewayConnection
     */
    private static class CloseConnection implements Close {
        private EcmtGatewayConnection connection;
        CloseConnection(EcmtGatewayConnection c) {
            connection = c;
        }
        public void close() throws IOException {
            connection.close();
        }
    }

    /**
     * Implementation of Close interface that closes a socket.
     */
    private static class CloseSocket implements Close {
        private Socket socket;
        CloseSocket(Socket s) {
            socket = s;
        }
        public void close() throws IOException {
            socket.close();
        }
    }

    /**
     * Generic thread that reads data from an input stream, writes to the
     * output stream and in the end closes something.
     */
    private class ConnectionThread extends Thread {
        private InputStream in;
        private OutputStream out;
        private Close close;
        ConnectionThread(String name, Close close,
                         InputStream in, OutputStream out) {
            super(name);
            this.in = in;
            this.out = out;
            this.close = close;
        }
        public void run() {
            try {
                int b;
                byte [] buf = new byte[2048];
                // Wait until at least one byte arrives
                while ((b = in.read()) >= 0) {
                    // Try to read as much as we can without blocking
                    int avail = in.available();
                    if (avail > 0) {
                        int n = 0;
                        buf[n++] = (byte)b;
                        while (avail > 0) {
                            int max = buf.length - n;
                            int len = Math.min(max,avail);
                            int nread = in.read(buf, n, len);
                            if (nread <= 0) break;
                            avail -= nread;
                            n += nread;
                            out.write(buf, 0, n);
                            n = 0;
                        }
                    } else {
                        out.write(b);
                    }
                    out.flush();
                }
            } catch (IOException x) {
                // ignore
            } catch (Exception x) {
                x.printStackTrace();
            } finally {
                try { close.close(); }
                catch (IOException x) {}
            }
        }
    }
}
