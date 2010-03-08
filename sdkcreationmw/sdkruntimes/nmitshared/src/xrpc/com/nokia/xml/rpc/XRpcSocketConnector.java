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
 * $Id: XRpcSocketConnector.java,v 1.6 2006/02/03 07:08:32 slava Exp $
 */

// ========================================================================
//  Name        : XRpcSocketConnector.java
//  Part of     : XRPC library
//  Description :
//  Version     :
//
//  Copyright (c) 2000 - 2006 Nokia Corporation.
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

/* java.net*/
import java.net.Socket;
import java.net.InetAddress;

/**
 * XRPC connection factory that connects via TCP socket
 *
 */
public class XRpcSocketConnector implements XRpcConnector {

    private int port;
    private String host;
    private InetAddress addr;

    /**
     * Creates the connector that connects to the specified TCP port on
     * the local machine
     *
     * @param port the TCP port to connect to
     */
    public XRpcSocketConnector(int port) {
        this(null, port);
    }

    /**
     * Creates the connector that connects to the specified TCP endpoint
     *
     * @param host the host to connect to, <code>null</code> for local host
     * @param port the TCP port to connect to
     */
    public XRpcSocketConnector(String host, int port) {
        this.host = host;
        this.port = port;
    }

    /**
     * Connects to the address specified in the constructor.
     * @throws IOException if connection fails
     * @return a fresh instance of XRpcConnection
     */
    public XRpcConnection connect() throws IOException {
        if (addr == null) {
            // resolve the address
            addr = ((host == null) ?
                InetAddress.getByAddress("loopback", new byte[] {127,0,0,1}):
                InetAddress.getByName(host));
        }

        // connect to the address
        Socket s = new Socket(addr, port);
        XRpcConnection c = new XRpcConnection(s.getInputStream(),
                                              s.getOutputStream());

        // don't forget to close the socket
        c.addConnectionListener(new CloseSocket(s));
        return c;
    }

    /**
     * This object is notified when connection to the XRPC Registry has
     * been closed. Closes the TCP socket.
     */
    private static class CloseSocket implements XRpcConnectionListener {

        private Socket socket;

        /**
         * Creates the <code>CloseSocket</code> object
         * @param s the socket to close
         */
        CloseSocket(Socket s) {
            socket = s;
        }

        /**
         * Invoked on the connection thread after connection has been
         * terminated. This is a good place to perform cleanup, for example
         * close the socket.
         *
         * @param connection the source of the event
         */
        public void connectionClosed(XRpcConnection connection) {
            try {
                socket.close();
            } catch (IOException x) {
                // ignore
            }
        }
    }
}
