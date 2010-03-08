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
 * $Id: XRpcServer.java,v 1.3 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcServer.java
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
import java.util.HashMap;

/* java.io */
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/* java.net */
import java.net.Socket;
import java.net.ServerSocket;

/**
 * Handles server side of XRPC
 *
 */
public class XRpcServer {

    private Map protoMap = new HashMap(7);
    private ListenThread listenThread;

    public XRpcServer() {
    }

    public void addProtocol(XRpcProtocol protocol) {
        synchronized (protoMap) {
            protoMap.put(protocol.getName(), protocol);
        }
    }

    public void removeProtocol(XRpcProtocol protocol) {
        synchronized (protoMap) {
            if (protoMap.get(protocol.getName()) == protocol) {
                protoMap.remove(protocol.getName());
            }
        }
    }

    public void removeProtocol(String name) {
        synchronized (protoMap) {
            protoMap.remove(name);
        }
    }

    public XRpcProtocol getProtocol(String name) {
        synchronized (protoMap) {
            return (XRpcProtocol)protoMap.get(name);
        }
    }

    public synchronized void stop() {
        if (listenThread != null) {
            listenThread.interrupt();
        }
    }

    public synchronized void start() throws IOException {
        if (listenThread != null && listenThread.isAlive()) {
            throw new IllegalStateException("server is already running");
        }
        listenThread = new ListenThread();
        listenThread.start();
    }

    public synchronized int getServerPort() {
        if (listenThread != null && listenThread.isAlive()) {
            return listenThread.getPort();
        } else {
            return -1;
        }
    }

    private class ConnectionThread extends Thread {
        private XRpcConnection connection;
        private Socket socket;
        ConnectionThread(XRpcConnection c, Socket s) {
            super("XRpcConnection");
            connection = c;
            socket = s;
        }
        public void run() {
            try {
                connection.run();
            } finally {
                try { socket.close(); }
                catch (IOException x) {}
            }
        }
    }

    private class ListenThread extends Thread {
        private ServerSocket listenSocket;
        ListenThread() throws IOException {
            super("XRpcServer");
            listenSocket = new ServerSocket(0);
        }
        int getPort() {
            return listenSocket.getLocalPort();
        }
        public void run() {
            XRpcServer srv = XRpcServer.this;
            try {
                while (!isInterrupted()) {
                    try {
                        Socket s = listenSocket.accept();
                        InputStream in = s.getInputStream();
                        OutputStream out = s.getOutputStream();
                        XRpcConnection c = new XRpcConnection(srv,in,out);
                        new ConnectionThread(c, s).start();
                    } catch (Exception x) {
                        x.printStackTrace();
                    }
                }
            } finally {
                synchronized (srv) {
                    if (listenThread == Thread.currentThread()) {
                        listenThread = null;
                    }
                }
            }
        }
    }
}
