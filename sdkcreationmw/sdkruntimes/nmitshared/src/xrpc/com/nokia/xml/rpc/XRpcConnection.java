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
 * $Id: XRpcConnection.java,v 1.31 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcConnection.java
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
import java.util.List;
import java.util.Stack;
import java.util.ArrayList;

/* java.io */
import java.io.Reader;
import java.io.Writer;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;

/* java.io */
import java.net.SocketException;

/* org.xml.sax */
import org.xml.sax.Parser;
import org.xml.sax.InputSource;
import org.xml.sax.HandlerBase;
import org.xml.sax.SAXException;
import org.xml.sax.AttributeList;
import org.xml.sax.SAXParseException;

/* javax.xml.parsers */
import javax.xml.parsers.SAXParserFactory;

/**
 * XRPC connection
 *
 */
public class XRpcConnection implements XRpcConstants {

    private static SAXParserFactory parserFactory = null;
    private static final boolean verbose = "true".equals(
        System.getProperty("nokia.xrpc.debug"));

    private Object lock = new Object();
    private XRpcWriter writer;
    private Reader reader;
    private XRpcServer server;
    private XRpcClient client;
    private XRpcResponseHandler responseHandler;
    private XRpcThreadPool threadPool;
    private XRpcConnectionListener [] listeners;

    private Thread runThread;
    private boolean closed = false;
    private boolean sentRootTag = false;
    private boolean synchronizeCalls = true;

    /**
     * Stream for debug output. <code>null</code> disables debug output
     */
    private PrintWriter debugWriter = null;

    /**
     * Creates the XRPC connection to the specified port on the local host
     *
     * @param port the port to connect to
     *
     * @return the XRPC connection to the specified port on the local host
     * @throws IOException if the connection fails
     */
    public static XRpcConnection connect(int port) throws IOException {
        return connect(null, port);
    }

    /**
     * Creates the XRPC connection connected to the specified port
     * at the specified host
     *
     * @param host the host to connect to, <code>null</code> if localhost
     * @param port the port to connect to
     *
     * @return the XRPC connection connected to the specified port
     *     at the specified host
     * @throws IOException if the connection fails
     */
    public static XRpcConnection connect(String host, int port)
    throws IOException {
        return new XRpcSocketConnector(host, port).connect();
    }

    /**
     * Creates <code>XRpcConnection</code>
     */
    public XRpcConnection(InputStream in, OutputStream out)
    throws UnsupportedEncodingException {
        this(null, in, out);
    }

    /**
     * Creates <code>XRpcConnection</code>
     */
    public XRpcConnection(XRpcServer srv, InputStream in, OutputStream out)
    throws UnsupportedEncodingException {
        server = srv;
        reader = new InputStreamReader(in, XRPC_ENCODING);
        writer = new XRpcPrintWriter(out);
    }

    /**
     * Creates parser factory if it does not exist
     */
    private static synchronized void createParserFactory() {
        if (parserFactory == null) {
            parserFactory = SAXParserFactory.newInstance();
        }
    }

    /**
     * Returns the shared instance of parser factory
     */
    private static SAXParserFactory getParserFactory() {
        if (parserFactory == null) createParserFactory();
        return parserFactory;
    }

    /**
     * Parses the input stream until end of file or an error
     */
    void run() {

        try {

            // prevent recursion and simultaneous execution
            synchronized (lock) {
                if (runThread != null) {
                    if (runThread == Thread.currentThread()) {
                        throw new IllegalStateException("recursive runInput()");
                    } else if (runThread.isAlive()) {
                        throw new IllegalStateException("multiple runInput()s");
                    }
                }
                runThread = Thread.currentThread();
            }

            // create a handler
            HandlerBase handler = new Handler();

            // run parser
            Parser parser = getParserFactory().newSAXParser().getParser();
            parser.setDocumentHandler(handler);
            parser.setErrorHandler(handler);
            parser.parse(new InputSource(reader));

        } catch (SocketException x) {
            // the other side has dropped connection
        } catch (ConnectionClosedException x) {
            // ignore this one
        } catch (SAXException x) {
            Exception y = x.getException();
            if (y != null) {
                System.err.println(x.getMessage());
                printStackTrace(y);
            } else {
                printStackTrace(x);
            }
        } catch (Throwable x) {
            printStackTrace(x);
        } finally {
            fireConnectionClosed();
            synchronized (lock) {
                if (runThread == Thread.currentThread()) {
                    runThread = null;
                }
            }
        }
    }

    /**
     * Returns the writer. Also writes the XML header and the root tag
     * if it's not already done so
     */
    XRpcWriter getWriter() throws IOException {
        sendRootTag();
        return writer;
    }

    /**
     * Returns the server object associated with this connection.
     * May return <code>null</code>
     */
    public XRpcServer getServer() {
        return server;
    }

    /**
     * Returns the client, creates one if necessary
     */
    public XRpcClient getClient() {
        if (client == null) {
            synchronized (this) {
                if (client == null) {
                    client = new XRpcClient(this);
                }
            }
        }
        return client;
    }

    /**
     * Sets the server for this connection
     */
    public void setServer(XRpcServer srv) {
        if (srv == null) throw new NullPointerException("srv");
        synchronized (lock) {
            if (server != null) {
                throw new IllegalStateException("server already set");
            }
            server = srv;
        }
    }

    /**
     * Sets the response handler
     */
    public void setResponseHandler(XRpcResponseHandler handler) {
        if (handler == null) throw new NullPointerException("handler");
        synchronized (lock) {
            if (responseHandler != null) {
                throw new IllegalStateException("response handler already set");
            }
            responseHandler = handler;
        }
    }

    /**
     * Close the connection
     */
    public void close() {
        synchronized (writer.getLock()) {
            if (!closed) {
                closed = true;
                if (sentRootTag) {
                    writer.print("</");
                    writer.print(XRpcTag.ROOT.toString());
                    writer.print('>');
                    writer.println();
                    writer.flush();
                    writer.close();
                }
            }
        }
        Thread thread = runThread;
        if (thread != null && thread != Thread.currentThread()) {
            thread.interrupt();
            try {
                // this should be really quick
                // timeout is just in case
                thread.join(30000);
            } catch (Exception x) {
                printStackTrace(x);
            }
        }
    }

    /**
     * Checks if connection has been closed
     */
    public boolean isClosed() {
        return closed;
    }

    /**
     * Debug trace facility
     */
    private void printStackTrace(Throwable x) {
        PrintWriter out = debugWriter;
        if (out != null) x.printStackTrace(out);
        else x.printStackTrace();
    }

    /**
     * Debug trace facility
     */
    private void debugTrace(String s) {
        PrintWriter out = debugWriter;
        if (out != null) out.println(s);
    }

    /**
     * Tests if verbose debug trace is enabled
     */
    private boolean isVerbose() {
        return verbose;
    }


    /**
     * Verbose debug trace facility
     */
    private void verboseTrace(String s) {
        if (isVerbose()) {
            PrintWriter out = debugWriter;
            if (out != null) out.println(s);
        }
    }

    /**
     * Sets stream for debug output. Setting stream to <code>null</code>
     * disables debug output
     */
    public void setDebugStream(OutputStream stream) {
        if (stream == null) {
            debugWriter = null;
        } else {
            debugWriter = new PrintWriter(stream, true);
        }
    }

    /**
     * Sets a writer for debug output. Setting stream to <code>null</code>
     * disables debug output
     */
    public void setDebugWriter(Writer writer) {
        if (writer == null) {
            debugWriter = null;
        } else if (writer instanceof PrintWriter) {
            debugWriter = (PrintWriter)writer;
        } else {
            debugWriter = new PrintWriter(writer, true);
        }
    }

    /**
     * Check if root tag has been sent, and send one if necessary.
     * This call is only made on the input thread and therefore it
     * does not need to be synchronized.
     *
     * @return <code>true</code> when this method is invoked for the
     *         first time, al subsequent calls return <code>false</code>
     */
    private boolean sendRootTag() throws IOException {
        if (!sentRootTag) {
            synchronized (writer.getLock()) {
                if (closed) throw new IOException("connection closed");
                if (!sentRootTag) {
                    debugTrace("sending root tag");
                    sentRootTag = true;
                    writer.print(XML_HEADER);
                    writer.println();
                    writer.print('<');
                    writer.print(XRpcTag.ROOT.toString());
                    writer.print('>');
                    writer.println();
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Returns <code>true</code> if all XRPC calls are invoked on the
     * same thread. Returns <code>false</code> if multiple calls may
     * execute in parallel. The default is <code>true</code> which
     * forces all methods to execute synchronously.
     */
    public boolean isSynchronized() {
        return synchronizeCalls;
    }

    /**
     * Sets the "synchronized" flag which determines whether all XRPC
     * calls are invoked on the same thread. The default is <code>true</code>
     * which forces all methods to execute synchronously.
     */
    public void setSynchronized(boolean on) {
        synchronizeCalls = on;
    }


    /**
     * Allows us to distinguish between the exceptions thrown by the
     * parser and the exception thrown by us when connection is closed
     */
    private static class ConnectionClosedException extends SAXException {
        ConnectionClosedException() {
            super("Connection closed");
        }
    }

    //======================================================================
    //      L I S T E N E R    L I S T
    //======================================================================

    /**
     * Adds a connection listener
     *
     * @param l the listener to add
     */
    public synchronized void addConnectionListener(XRpcConnectionListener l) {
        if (listeners == null) {
            listeners = new XRpcConnectionListener[] {l};
        } else {
            int n = listeners.length;
            XRpcConnectionListener [] a = new XRpcConnectionListener[n+1];
            System.arraycopy(listeners, 0, a, 0, n);
            a[n] = l;
            listeners = a;
        }
    }

    /**
     * Removes a connection listener
     *
     * @param l the listener to remove
     */
    public synchronized void removeConnectionListener(XRpcConnectionListener l) {
        if (listeners != null) {
            if (listeners.length == 1) {
                if (listeners[0] == l) {
                    listeners = null;
                }
            } else {
                int n = listeners.length;
                for (int i=0; i<n; i++) {
                    if (listeners[i] == l) {
                        XRpcConnectionListener [] a = new XRpcConnectionListener[n-1];
                        System.arraycopy(listeners,0,a,0,i);
                        if (i<(n-1)) System.arraycopy(listeners,i+1,a,i,n-i-1);
                        listeners = a;
                        break;
                    }
                }
            }
        }
    }

    /**
     * Notifies the listeners that the source (data) range has changed.
     *
     * @param prevRange the old range.
     * @param newRange the new range.
     */
    private void fireConnectionClosed() {
        XRpcConnectionListener [] connectionListeners = listeners;
        if (connectionListeners != null) {
            int n = connectionListeners.length;
            for (int i=0; i<n; i++) {
                try {
                    connectionListeners[i].connectionClosed(this);
                } catch (Exception x) {
                    printStackTrace(x);
                }
            }
        }
    }

    //=======================================================================
    //      S A X    H A N D L E R
    //=======================================================================

    private interface TagHandler {
        public boolean canSkip();
        public void characters(char[] buf, int offset, int len)
            throws SAXException;
        public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
            throws SAXException;
        public void endTag()
            throws SAXException;
    }

    /**
     * The stack of the document handler contains these objects
     */
    private static class StackEntry {
        private TagHandler handler;
        private XRpcTag tag;
        StackEntry(TagHandler handler, XRpcTag tag) {
            this.handler = handler;
            this.tag = tag;
        }
        TagHandler getHandler() { return handler; }
        XRpcTag getTag() { return tag; }
    }

    /**
     * Copies attributes to the ata element. Unknown attributes are ignored
     */
    private static
    void copyAttributes(XRpcBasicElement data, AttributeList attrs) {
        int n = attrs.getLength();
        for (int i=0; i<n; i++) {
            String name = attrs.getName(i);
            XRpcAttr attr = XRpcAttr.getAttribute(name);
            if (attr != null) {
                String value = attrs.getValue(i);
                if (value != null) {
                    data.setAttribute(attr, value);
                }
            } else {
                System.out.println("ignoring attribute " + name + '=' +
                                                           attrs.getValue(i));
            }
        }
    }

    /**
     * SAX handler
     */
    private class Handler extends HandlerBase {

        /**
         * The stack containing {@link StackEntry} objects
         */
        private Stack stack = new Stack();
        private int skip = 0;
        private TagHandler ignoreHandler = null;
        private TagHandler emptyHandler = null;

        /**
         * Creates the <code>Handler</code>
         */
        Handler() {
        }

        /**
         * Returns the shared instance of handler that ignores everything
         */
        TagHandler getIgnoreHandler() {
            if (ignoreHandler == null) ignoreHandler = new IgnoreHandler();
            return ignoreHandler;
        }

        /**
         * Returns the shared instance of handler for empty tag
         */
        TagHandler getEmptyHandler() {
            if (emptyHandler == null) emptyHandler = new EmptyHandler();
            return emptyHandler;
        }

        /**
         * Returns current tag from the top of the stack
         */
        private XRpcTag currentTag() {
            if (stack.empty()) return null;
            return ((StackEntry)stack.peek()).getTag();
        }

        /**
         * Returns current handler from the top of the stack
         */
        private TagHandler currentHanlder() {
            if (stack.empty()) return null;
            return ((StackEntry)stack.peek()).getHandler();
        }

        /**
         * Throws {@link SAXException} if connection has been closed
         */
        private void checkIfClosed() throws SAXException {
            if (closed) {
                throw new ConnectionClosedException();
            }
        }

        /**
         * Receives notification of the beginning of the document.
         */
        public void startDocument() throws SAXException {
            checkIfClosed();
            debugTrace("Handler: startDocument()");
        }

        /**
         * Receives notification of the end of the document.
         */
        public void endDocument() throws SAXException {
            checkIfClosed();
            debugTrace("Handler: endDocument()");
        }

        /**
         * Receive notification of the start of an element.
         */
        public void startElement(String name, AttributeList attrs)
        throws SAXException {
            checkIfClosed();
            if (isVerbose()) {
                verboseTrace("Handler: startElement(\"" + name + "\")");
            }
            if (skip > 0) {
                verboseTrace("Handler: skipping...");
                skip++;
                return;
            }

            TagHandler handler = currentHanlder();
            XRpcTag tag = XRpcTag.getTag(name);
            if (tag == null) {
                if (handler != null && handler.canSkip()) {
                    skip++;
                    return;
                }

                // cannot skip unknown tags in this state; throw an exception
                throw new SAXException("unknown tag \"" + name + "\"");
            }

            TagHandler newHandler;
            if (handler == null) {
                if (tag == XRpcTag.ROOT) {
                    newHandler = new RootHandler();
                } else {
                    // deal with missing root element
                    newHandler = new RootHandler().handleTag(tag, attrs);
                }
            } else {
                newHandler = handler.handleTag(tag, attrs);
            }

            stack.push(new StackEntry(newHandler, tag));
        }

        /**
         * Receive notification of the end of an element.
         */
        public void endElement(String name) throws SAXException {
            checkIfClosed();
            if (isVerbose()) {
                verboseTrace("Handler: endElement(\"" + name + "\")");
            }
            if (skip > 0) {
                skip--;
                return;
            }

            if (stack.empty()) {
                throw new SAXException("empty stack");
            }

            XRpcTag tag = XRpcTag.getTag(name);
            StackEntry last = (StackEntry)stack.pop();
            if (last.getTag() != tag) {
                throw new SAXException("tag mismatch");
            }

            last.getHandler().endTag();
        }

        public void characters(char[] buf, int off, int len)
        throws SAXException {
            checkIfClosed();
            if (isVerbose()) {
                verboseTrace("Handler: characters(" +
                            (new String(buf,off,len)) + ")");
            }
            if (stack.empty()) {
                throw new SAXException("empty stack");
            }
            StackEntry current = (StackEntry)stack.peek();
            current.getHandler().characters(buf,off,len);
        }

        public void ignorableWhitespace(char[] buf, int offset, int len)
        throws SAXException {
            checkIfClosed();
            if (isVerbose()) {
                verboseTrace("Handler: ignorableWhitespace(" +
                            (new String(buf, offset, len)) + ")");
            }
        }

        public void processingInstruction(String target, String data)
        throws SAXException {
            checkIfClosed();
            if (isVerbose()) {
                verboseTrace("Handler: processingInstruction(\"" + target +
                             "\", \"" + data + "\")");
            }
        }

        public void error (SAXParseException err) throws SAXParseException {
            throw err;
        }

        public void warning (SAXParseException err) {
            if (debugWriter != null) debugTrace("WARNING: " + err);
        }

        //===================================================================
        //      T A G    H A N D L E R S
        //===================================================================

        /**
         * Base class for many tag handlers
         */
        private abstract class BasicHandler implements TagHandler, XRpcConstants {
            public boolean canSkip() { return false; }
            public void characters(char[] buf, int offset, int len)
                throws SAXException { /* ignore */ }
            public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
                throws SAXException { unexpectedTag(tag); return null; }
            public void endTag() throws SAXException {}

            protected void unexpectedTag(XRpcTag tag) throws SAXException {
                throw new SAXException("unexpected tag <" + tag + '>');
            }
        }

        /**
         * Handler for elements that contain nothing
         */
        private class EmptyHandler extends BasicHandler {
            // BasicHandler will do the job
        }

        /**
         * Handler that ignores everything
         */
        private class IgnoreHandler extends BasicHandler {
            public boolean canSkip() { return true; }
            public TagHandler handleTag(XRpcTag tag, AttributeList attrs) {
                return this;
            }
        }

        /**
         * Handler that ignores everything within the tag and send simple
         * response when the tag ends
         */
        private class ErrorHandler extends BasicHandler {
            private XRpcResponse response = null;

            ErrorHandler(String id, XRpcStatus status) {

                // error response with no id are useless
                if (id != null) {
                    response = new XRpcResponse(id, status);
                }
            }

            public boolean canSkip() { return true; }
            public TagHandler handleTag(XRpcTag tag, AttributeList attrs) {
                return getIgnoreHandler();
            }
            public void endTag() throws SAXException {
                if (response != null) {
                    try {
                        response.write(getWriter());
                    } catch (IOException x) {
                        printStackTrace(x);
                        throw new SAXException(x);
                    }
                }
            }
        }

        /**
         * Handler for the root element
         */
        private class RootHandler extends BasicHandler {
            public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
            throws SAXException {

                // at the top level we only care about <call> and <response>
                if (tag == XRpcTag.CALL || tag == XRpcTag.RESPONSE) {

                    // get the call tag
                    String id = attrs.getValue(XRpcAttr.ID.toString());
                    if (tag == XRpcTag.CALL) {

                        // get the protocol name
                        String pname = attrs.getValue(XRpcAttr.PROTOCOL.toString());

                        // get protocol handler from the server object
                        XRpcServer srv = server;
                        XRpcProtocol prot = null;
                        if (srv != null) prot = srv.getProtocol(pname);
                        if (prot == null) {

                            // no such protocol
                            if (debugWriter != null) {
                                debugTrace("SERVER: unknown protocol: " + pname);
                            }

                            return new ErrorHandler(id,XRpcStatus.NOPROTO);

                        } else {

                            // get method handler from the protocol
                            String mname = attrs.getValue(XRpcAttr.METHOD.toString());
                            XRpcMethod method = prot.getMethod(mname);
                            if (method == null) {

                                // no such method
                                if (debugWriter != null) {
                                    debugTrace("SERVER: no such method: " +
                                                prot + '.' + mname);
                                }
                                return new ErrorHandler(id,XRpcStatus.NOMETHOD);
                            }

                            return new CallHandler(attrs,method, id);
                        }
                    } else {

                        // handle response
                        String s = attrs.getValue(XRpcAttr.STATUS.toString());
                        XRpcStatus status = XRpcStatus.getStatus(s);
                        if (status == null) {
                            return getIgnoreHandler();
                        } else {
                            return new ResponseHandler(id, status);
                        }
                    }
                }
                return super.handleTag(tag, attrs);
            }
        }

        /**
         * Handler for the call element
         */
        private class CallHandler extends BasicHandler implements Runnable {
            private String id;
            private XRpcMethod method;
            private ParamHandler paramHandler = null;
            private XRpcDataElement[] params = null;

            CallHandler(AttributeList atts, XRpcMethod method, String id) {
                this.method = method;
                this.id = id;
            }

            public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
            throws SAXException {

                if (tag == XRpcTag.PARAMS) {
                    // allow multiple <params> blocks?
                    if (paramHandler == null) {
                        paramHandler = new ParamHandler();
                    }
                    return paramHandler;
                }
                return super.handleTag(tag, attrs);
            }

            public void endTag() throws SAXException {

                // prepare the parameter block
                if (paramHandler == null || paramHandler.getCount() == 0) {
                    params = XRpcBasicDataContainer.NO_DATA_ELEMENTS;
                } else {
                    params = paramHandler.getData();
                }

                // determine how to invoke this method
                boolean useThisThread = synchronizeCalls;
                if (useThisThread && method instanceof XRpcAsyncMethod) {
                    useThisThread = !((XRpcAsyncMethod)method).isAsynchronous();
                }

                if (useThisThread) {
                    run();
                } else {
                    if (threadPool == null) {
                        threadPool = new XRpcThreadPool();
                    }
                    threadPool.run(this);
                }
            }

            /**
             * Actually invokes the handler. Implements {@link Runnable}
             * interface.
             */
            public void run() {

                // invoke the method, handle all exceptions
                // no response is sent if there's no call id
                XRpcResponse response = null;
                try {
                    XRpcDataElement results = method.invoke(params);
                    if (id != null) {
                        response = new XRpcResponse(id, results);
                    }
                } catch (Throwable x) {

                    // unhandled exception
                    printStackTrace(x);
                    if (id != null) {
                        response = new XRpcResponse(id, XRpcStatus.SYSERR);
                    }
                }

                // no response is sent if there's no call id
                if (response != null) {
                    try {
                        response.write(getWriter());
                    } catch (IOException x) {
                        printStackTrace(x);
                        try { reader.close(); }
                        catch (IOException y) { /* ignore */ }
                    }
                }
            }
        }

        /**
         * Handler for the response element
         */
        private class ResponseHandler extends BasicHandler {
            private String callTag;
            private XRpcStatus status;
            private ParamHandler paramHandler = null;

            ResponseHandler(String callTag, XRpcStatus status) {
                this.callTag = callTag;
                this.status = status;
            }

            public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
            throws SAXException {
                if (tag == XRpcTag.RESULT) {
                    // allow multiple <result> or <error> blocks?
                    if (paramHandler == null) {
                        paramHandler = new ParamHandler();
                    }
                    return paramHandler;
                } else {
                    return super.handleTag(tag, attrs);
                }
            }

            public void endTag() {

                XRpcResponseHandler handler = responseHandler;
                if (handler != null) {
                    // prepare the parameter block
                    XRpcDataElement data = null;
                    if (paramHandler != null && paramHandler.getCount() > 0) {
                        data = paramHandler.getDataAt(0);
                    }

                    // invoke the handler, handle all exceptions
                    try {
                        handler.handleResponse(callTag, data);
                    } catch (Throwable x) {
                        printStackTrace(x);
                    }
                }
            }
        }

        /**
         * Handler for call parameter block.
         */
        private class ParamHandler extends DataSetHandler {
            private boolean isVoid = false;

            public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
            throws SAXException {
                if (tag == XRpcTag.VOID) {
                    if (isVoid) {
                        throw new SAXException("multiple <" + tag + "> tags");
                    } else if (getCount() > 0) {
                        unexpectedTag(tag);
                        //NOT REACHED
                    }

                    isVoid = true;
                    return getEmptyHandler();
                } else {
                    if (isVoid) {
                        unexpectedTag(tag);
                        //NOT REACHED
                    }
                }
                return super.handleTag(tag, attrs);
            }
        }

        /**
         * Handler for a block that contains data.
         */
        private class DataSetHandler extends BasicHandler {
            private List list = null;
            private XRpcDataElement[] array = null;

            /**
             * Returns number of data entries parsed so far
             */
            int getCount() {
                return ((list == null) ? 0 : list.size());
            }

            /**
             * Lazily allocates {@link ArrayList} that holds the parameters
             */
            private List getList() {
                if (list == null) list = new ArrayList();
                return list;
            }

            /**
             * Adds data element to the list of parameters
             */
            private void add(XRpcBasicDataElement data) {
                getList().add(data);
                array = null;
            }

            /**
             * Adds data element to the list of parameters
             */
            private void add(XRpcBasicDataContainer data) {
                getList().add(data);
                array = null;
            }

            /**
             * Returns the data parsed so far
             */
            XRpcDataElement[] getData() {
                if (array == null) {
                    if (list == null) {
                        array = new XRpcDataElement[0];
                    } else {
                        array = new XRpcDataElement[list.size()];
                        array = (XRpcDataElement[])list.toArray(array);
                    }
                }
                return array;
            }

            /**
             * Returns the data at specified index
             */
            XRpcDataElement getDataAt(int index) {
                if (list != null) {
                    return (XRpcDataElement)list.get(index);
                }
                return null;
            }

            /**
             * Handles the tag
             */
            public TagHandler handleTag(XRpcTag tag, AttributeList attrs)
            throws SAXException {
                if (tag == XRpcTag.BYTE)    { return new ByteHandler(attrs);  }
                if (tag == XRpcTag.SHORT)   { return new ShortHandler(attrs); }
                if (tag == XRpcTag.INT)     { return new IntHandler(attrs);   }
                if (tag == XRpcTag.LONG)    { return new LongHandler(attrs);  }
                if (tag == XRpcTag.STRING)  { return new StringHandler(attrs);}
                if (tag == XRpcTag.BOOLEAN) { return new BoolHandler(attrs);  }
                if (tag == XRpcTag.BASE64)  { return new Base64Handler(attrs);}
                if (tag == XRpcTag.STRUCT)  { return new StructHandler(attrs);}
                if (tag == XRpcTag.ARRAY)   { return new ArrayHandler(attrs); }
                if (tag == XRpcTag.NULL) {
                    XRpcNull nullTag = new XRpcNull();
                    copyAttributes(nullTag, attrs);
                    add(nullTag);
                    return new EmptyHandler();
                }

                // super will throw
                return super.handleTag(tag, attrs);
            }

            /**
             * Handler for a primitive data element
             */
            abstract class DataHandler extends BasicHandler {
                private StringBuffer sbuf = null;
                private XRpcBasicData data;

                protected DataHandler(XRpcBasicData data, AttributeList attrs) {
                    this.data = data;
                    copyAttributes(data, attrs);
                }
                protected void finish() throws Exception {
                    data.setAsText(getString());
                    add(data);
                }
                public String getString() {
                    return ((sbuf == null) ? "" : sbuf.toString());
                }
                public void characters(char[] buf, int offset, int len) {
                    if (len > 0) {
                        if (sbuf == null) sbuf = new StringBuffer(len);
                        sbuf.append(buf, offset, len);
                    }
                }
                public void endTag() throws SAXException {
                    try { finish(); }
                    catch (Exception x) {
                        if (x instanceof SAXException) {
                            throw (SAXException)x;
                        } else {
                            throw new SAXException(x);
                        }
                    }
                }
            }

            /**
             * Handler for a double number
             */
            private class DoubleHandler extends DataHandler {
                DoubleHandler(AttributeList attrs) {
                    super(new XRpcDouble(), attrs);
                }
            }

            /**
             * Handler for a byte number
             */
            private class ByteHandler extends DataHandler {
                ByteHandler(AttributeList attrs) {
                    super(new XRpcByte(), attrs);
                }
            }

            /**
             * Handler for a short number
             */
            private class ShortHandler extends DataHandler {
                ShortHandler(AttributeList attrs) {
                    super(new XRpcShort(), attrs);
                }
            }

            /**
             * Handler for an integer number
             */
            private class IntHandler extends DataHandler {
                IntHandler(AttributeList attrs) {
                    super(new XRpcInt(), attrs);
                }
            }

            /**
             * Handler for a long number
             */
            private class LongHandler extends DataHandler {
                LongHandler(AttributeList attrs) {
                    super(new XRpcLong(), attrs);
                }
            }

            /**
             * Handler for a string
             */
            private class StringHandler extends DataHandler {
                StringHandler(AttributeList attrs) {
                    super(new XRpcString(), attrs);
                }
            }

            /**
             * Handler for a boolean
             */
            private class BoolHandler extends DataHandler {
                BoolHandler(AttributeList attrs) {
                    super(new XRpcBoolean(), attrs);
                }
            }

            /**
             * Handler for a BASE64 encoded binary data block
             */
            private class Base64Handler extends DataHandler {
                Base64Handler(AttributeList attrs) {
                    super(new XRpcBinaryData(), attrs);
                }
            }

            /**
             * Handler for a structure
             */
            private class StructHandler extends DataSetHandler {
                private XRpcDataStruct struct = new XRpcDataStruct();
                StructHandler(AttributeList attrs) {
                    copyAttributes(struct, attrs);
                }
                public void endTag() {
                    struct.addElements(this.getData());
                    DataSetHandler.this.add(struct);
                }
            }

            /**
             * Handler for an array
             */
            private class ArrayHandler extends DataSetHandler {
                private XRpcDataArray array = new XRpcDataArray();
                ArrayHandler(AttributeList attrs) {
                    copyAttributes(array, attrs);
                }
                public void endTag() {
                    array.addElements(this.getData());
                    DataSetHandler.this.add(array);
                }
            }
        }
    }
}
