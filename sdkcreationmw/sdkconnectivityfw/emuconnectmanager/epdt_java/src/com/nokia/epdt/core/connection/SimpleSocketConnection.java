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
* Description: 
 *
*/

package com.nokia.epdt.core.connection;

import java.net.InetAddress;
import java.net.Socket;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.nokia.epdt.core.Constants;
import com.nokia.epdt.core.CoreHandler;
import com.nokia.epdt.core.CoreModel;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageFormatException;
import com.nokia.epdt.core.messaging.MessageQueue;

import org.apache.log4j.Logger;
import com.nokia.epdt.core.CoreController;

/**
 * SimpleSocketConnection
 */
public class SimpleSocketConnection implements Connection {
    private static final Logger log = Logger.getLogger(SimpleSocketConnection.class);
    private final String host;
    private final int port;
    private CoreController controller;
    private CoreModel coreModel;
    private CoreHandler handler;
    private Socket socket = null;
    private Thread input;
    private Thread output;

    /**
     * Creates a new instance of SimpleSocketConnection
     * @param host the host name.
     * @param port the port number.
     */
    public SimpleSocketConnection(String host, int port, CoreController controller) {
        this.controller = controller;
        this.coreModel = controller.getModel();
        this.handler = controller.getMessageHandler();
        this.host = host;
        this.port = port;
    }

    /**
     * Closes a simple socket connection
     */
    public void close() {
        coreModel.setConnectionStatus(ConnectionStatus.DISCONNECTING);
        try {
            socket.close();
            coreModel.setConnectionStatus(ConnectionStatus.IDLE);
        } catch ( IOException ioe ) {
            log.error(ioe);
        } finally {
            if ( socket != null ) {
                try {
                    socket.close();
                    coreModel.setConnectionStatus(ConnectionStatus.IDLE);
                } catch ( IOException ioe ) {
                    log.error(ioe);
                }
            }
        }
    }

    /**
     * Opens a simple socket connection
     */
    public void open() {
        log.debug("Open SimpleSocketConnection");
        coreModel.setConnectionStatus(ConnectionStatus.IDLE);
        coreModel.setConnectionStatus(ConnectionStatus.CONNECTING);
        try {
            if (log.isDebugEnabled()) log.debug("Resolving host " + host);
            InetAddress address = InetAddress.getByName(host);
            this.socket = new Socket(address, port);
            coreModel.setConnectionStatus(ConnectionStatus.ESTABLISHED);
            log.debug("Create connection input and output");
            input = new Input(this, socket.getInputStream(), handler.getMessageInputQueue(), controller);
            output = new Output(socket.getOutputStream(), handler.getMessageOutputQueue());
            log.debug("Starting connection input and output threads");
            input.start();
            output.start();
        } catch ( IOException ioe ) {
            log.error(ioe);
            coreModel.setConnectionStatus(ConnectionStatus.FAILED);
            // Exit the ECMT manager if connection fails
            log.debug("SimpleSocketConnection.open(): CONNECTION FAILED, EXITING:");
            controller.exitProgramCall();
        }
    }

    private static class Input extends Thread implements Constants {
        private Connection connection;
        private InputStream input;
        private MessageQueue inputQueue;
        private CoreModel model = null;
        private CoreController controller = null;

        /**
         * Creates a new instance of Input
         * @param c the associated connection
         * @param in socket input stream
         * @param q the queue where to put the messages
         */
        Input(Connection c, InputStream in, MessageQueue q, CoreController cc) {
            super("SimpleSocketConnection.Input");
            connection = c;
            input = in;
            inputQueue = q;
            model = cc.getModel();
            controller = cc;
        }

        public void run() {
            InputStream input = null;
            try {
                input = new BufferedInputStream(this.input);
                byte[] msg_len = new byte[MESSAGE_LENGTH_HEX_NUMERAL_LENGTH];

                while (true) {

                    // Get message length. Read first 10 bytes
                    int received = 0;
                    int to_read = msg_len.length;
                    do {
                        int n = input.read(msg_len, received, to_read);
                        if (n < 0) throw new IOException();
                        received += n;
                        to_read -= n;
                    } while (to_read > 0);

                    try {
                        int len = Message.parseInt(msg_len);
                        byte[] uid_body = new byte[len - MESSAGE_LENGTH_HEX_NUMERAL_LENGTH];

                        // Read the rest of the message
                        received = 0;
                        to_read = uid_body.length;
                        while (to_read > 0) {
                            int n = input.read(uid_body, received, to_read);
                            if (n < 0) throw new IOException();
                            received += n;
                            to_read -= n;
                        }

                        // Create a new message and put int into the message
                        // input queue
                        inputQueue.put(Message.parseMessage(uid_body));

                    } catch (MessageFormatException nfe) {
                        log.error(nfe);
                        throw new IOException(nfe.getMessage());
                    }
                }

            } catch (IOException ioe) {
                log.error(ioe);
                model.setConnectionStatus(ConnectionStatus.FAILED);
            } finally {
                if (input != null) {
                    try {
                        input.close();
                    } catch (IOException ioe) {
                        log.error(ioe);
                    }
                }

                if (connection != null) {
                    log.debug("SimpleSocketConnection.Input.run()\nClose simple socket connection");
                    connection.close();
                }

                // Exit the ECMT manager app when connection is terminated
                if(controller.getModel().isDiagnosticsUIVisible())
                {
                	controller.getModel().hideUtilitiesUI();
                	controller.getModel().hidePreferencesUI();
                }
                else
                {
                	controller.exitProgramCall();
                }
            }
        }
    }

    private static class Output extends Thread {
        private OutputStream output;
        private MessageQueue outputQueue;

        /**
         * Creates a new instance of Output
         * @param out the output stream
         * @param queue the queue from which to take the messages
         */
        Output(OutputStream out, MessageQueue queue) {
            super("SimpleSocketConnection.Output");
            output = out;
            outputQueue = queue;
        }

        public void run() {
            BufferedOutputStream buf = new BufferedOutputStream(output);
            try {
                while (true) {
                    // Take a message from output queue and write it
                    buf.write(outputQueue.getMessage().getMessage());
                    buf.flush();
                }
            } catch ( Exception ioe ) {
                log.error(ioe);
            } finally {
                try {
                    buf.close();
                } catch ( IOException ioe ) {
                    log.error(ioe);
                }
            }
        }
    }
}
