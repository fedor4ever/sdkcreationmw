/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements MIDP debugging session via Ecmt Gateway
*
*/


package com.nokia.j2me.sei;

/* java.net */
import java.net.InetAddress;
import java.net.UnknownHostException;

/* java.io */
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/* com.nokia.j2me.sei.xrpc */
import com.nokia.j2me.sei.xrpc.EcmtGatewaySession;

/* com.symbian.utils */
import com.symbian.utils.Debug;

/* com.symbian.tools.j2me.sei.preferences */
import com.symbian.tools.j2me.sei.preferences.UserPreferences;
import com.symbian.tools.j2me.sei.emulator.session.RemoteSession;

/**
 * Session object for on-device debugging via Ecmt Gateway.
 */
public class GatewaySession extends RemoteSession {

    private static class EcmtGatewayTransport implements SessionTransport {
        private EcmtGatewaySession session;
        EcmtGatewayTransport() throws IOException {
            session = new EcmtGatewaySession();
        }
        EcmtGatewaySession getSession() {
            return session;
        }
        int mapVmPort(int kvmPort) {
            return session.mapVmPort(kvmPort);
        }
        public InputStream getInputStream() throws IOException {
            return session.getInputStream();
        }
        public OutputStream getOutputStream() throws IOException {
            return session.getOutputStream();
        }
        public void close() throws IOException {
            session.close();
        }
    }

    /**
     * Our connection to EcmtGateway
     */
    private EcmtGatewayTransport ecmtTransport;

    /**
     * Initialize the session.
     * The intialization code shouldnt be in the constructor to allow shutdown
     * during initialization, coming from the user.
     *
     * @param prefs user preferences used for initializing the connection
     *
     * @throws Exception if some error occured
     */
    public void initialize(UserPreferences prefs) throws Exception {
        super.initialize(prefs);
        connectToSlave(ecmtTransport = new EcmtGatewayTransport());
        doHandshake(prefs.getSecurityPreferences().getChallenge());
    }

    /**
     * Maps KVM (possibly, handset) port number to local port.
     * @param kvmPort the port to map.
     * @return possibly different port number than can be used within this VM
     */
/* Dima temp
    public int mapVmPort(int kvmPort) {
        int port = ecmtTransport.mapVmPort(kvmPort);
        Debug.println(this, "GatewaySession: mapKVMPort "+kvmPort+" -> "+port);
        return port;
    }
*/
    /**
     * Get a free port on the device. Actually returns a local TCP port.
     * Connection to this port will be forwarded to the device.
     *
     * @return int free port
     * @throws Exception if some error occured
     */
    /* Dima temp
    
    public int getFreePort() throws Exception {
        int port = super.getFreePort();
        Debug.println(this, "GatewaySession: getFreePort -> " + port);
        ecmtTransport.getSession().listenAndConnect(port);
        return port;
    }
*/
    /**
     * Get SEI-Slave TCP Inet address. This session always returns localhost.
     * @return SEI-Slave TCP Inet address
     * @exception UnknownHostException if failed to resolve the host name
     */
    public InetAddress getSlaveInetAddress() throws UnknownHostException {
        return InetAddress.getLocalHost();
    }
}
