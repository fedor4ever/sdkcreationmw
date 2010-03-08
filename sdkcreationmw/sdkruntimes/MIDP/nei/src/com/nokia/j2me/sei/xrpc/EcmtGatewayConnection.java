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
* Description: 
*
*/


package com.nokia.j2me.sei.xrpc;

/* java.io */
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/* com.nokia.xml.rpc */
import com.nokia.xml.rpc.XRpcInt;
import com.nokia.xml.rpc.XRpcBinaryData;
import com.nokia.xml.rpc.XRpcDataElement;

/**
 * Represents a single tunneled connection.
 */
class EcmtGatewayConnection implements EcmtGatewayProtocol {

    private int connId;
    private int kvmPort;
    private int localPort;
    private EcmtGatewaySession session;
    private Input input;
    private OutputStream output;

    /**
     * Creates EcmtGatewayConnection.
     *
     * @param s the session containing this connection.
     * @param id connection id, zero for control connection.
     * @param kp port number of KVM side.
     * @param lp local port.
     */
    EcmtGatewayConnection(EcmtGatewaySession s, int id, int kp, int lp) {
        session = s;
        connId = id;
        kvmPort = kp;
        localPort = lp;
        output = new Output();
        input = new Input(output);
    }

    void close() throws IOException {
        input.close();
        output.close();
    }

    int getConnId() {
        return connId;
    }

    int getKVMPort() {
        return kvmPort;
    }

    int getLocalPort() {
        return localPort;
    }

    InputStream getInputStream() {
        return input;
    }

    OutputStream getOutputStream() {
        return output;
    }

    /**
     * Adds bytes to the input queue. This call unblocks the thread reading
     * the input stream.
     *
     * @param b the data to add to the input queue.
     */
    void addBytes(byte b[]) {
        input.addBytes(b, 0, b.length);
    }

    /**
     * Adds bytes to the input queue. This call unblocks the thread reading
     * the input stream.
     *
     * @param b the byte array containing the data
     * @param off offset where data begins
     * @param len number of bytes to add to the input queue
     */
    void addBytes(byte b[], int off, int len) {
        input.addBytes(b, off, len);
    }

    /**
     * Input stream reading the data arriving from Ecmt Gateway via XRPC
     */
    private class Input extends InputStream {
        private byte[] buf = new byte[128/* use small buffer for testing */];
        private int bufpos;
        private int bufsize;
        private boolean closed = false;
        private OutputStream output;

        Input(OutputStream out) {
            output = out;
        }

        public synchronized void close() {
            if (!closed) {
                closed = true;
                notifyAll();
            }
        }

        /**
         * This method is invoked whenever "send" call is received via XRPC.
         * @param b the data buffer
         * @param off offset where the data begin
         * @param len number of bytes arrived
         */
        synchronized void addBytes(byte b[], int off, int len) {
            if (len > 0 && !closed) {
                int newsize = bufsize + len;
                if (newsize <= buf.length) {
                    // Data fit in the buffer
                    if ((bufpos+newsize) <= buf.length) {
                        // as a single block of memory
                        System.arraycopy(b, off, buf, bufpos+bufsize, len);
                    } else {
                        // split data in two pieces
                        int n1 = buf.length-bufpos;
                        int n2 = len-n1;
                        System.arraycopy(b, off, buf, bufpos, n1);
                        System.arraycopy(b, off+n1, buf, 0, n2);
                    }
                } else {
                    // We have to increase our buffer size
                    byte[] newbuf = new byte[newsize];
                    if ((bufpos+bufsize) <= buf.length) {
                        System.arraycopy(buf, bufpos, newbuf, 0, bufsize);
                        System.arraycopy(b, off, newbuf, bufsize, len);
                    } else {
                        int n1 = buf.length-bufpos;
                        int n2 = bufsize-n1;
                        System.arraycopy(buf, bufpos, newbuf, 0, n1);
                        System.arraycopy(buf, 0, newbuf, n1, n2);
                        System.arraycopy(b, off, newbuf, bufsize, len);
                    }
                    buf = newbuf;
                    bufpos = 0;
                }
                bufsize = newsize;
                notifyAll();
            }
        }
        public int available() {
            return bufsize;
        }
        public synchronized int read() {
            while (bufsize == 0) {
                if (closed) {
                    return -1;
                }
                try {
                    // We are about to block. Make sure that there's nothing
                    // sitting in the output buffer. The caller may be waiting
                    // for a response to the data written to the output
                    // stream. Not all KDP commands flush the output stream
                    // after writing a packet.
                    output.flush();
                    wait();
                } catch (IOException x) {
                    break;
                } catch (InterruptedException x) {
                }
            }
            bufsize--;
            int b = (buf[bufpos++] & 0xff);
            if (bufsize == 0 || bufpos == buf.length) bufpos = 0;
            return b;
        }
        public synchronized int read(byte b[], int off, int len) {
            if (b==null) {
                throw new NullPointerException();
            } else if (off < 0 || off > b.length || len < 0 ||
                       (off+len)>b.length || (off+len) < 0) {
                throw new IndexOutOfBoundsException();
            }

            int bytesToRead = len;
            while (bytesToRead > 0) {
                while (bufsize == 0) {
                    if (closed) {
                        return -1;
                    }
                    try {
                        // Flush the output stream. See comments in read()
                        // method above. They apply here as well.
                        output.flush();
                        wait();
                    } catch (IOException x) {
                        break;
                    } catch (InterruptedException x) {
                    }
                }

                int blockSize = bytesToRead;
                if (blockSize > bufsize) blockSize = bufsize;
                if ((bufpos+blockSize) <= buf.length) {
                    System.arraycopy(buf, bufpos, b, off, blockSize);
                    bufpos += blockSize;
                } else {
                    int n1 = buf.length-bufpos;
                    int n2 = blockSize-n1;
                    System.arraycopy(buf, bufpos, b, off, n1);
                    System.arraycopy(buf, 0, b, off+n1, n2);
                    bufpos = n2;
                }
                off += blockSize;
                bytesToRead -= blockSize;
                bufsize -= blockSize;
                if (bufsize == 0 || bufpos == buf.length) bufpos = 0;
            }
            return len;
        }
    }

    /**
     * Output stream forwarding the data via XRPC to the dataway.
     * Buffers the data before sending therm, in effort to increase
     * amount of data being sent in a single XRPC packet and therefore,
     * decrease overhead.
     */
    private class Output extends OutputStream {
        private byte[] buf = new byte[2048];
        private int nbytes;
        private boolean closed = false;

        public synchronized void close() throws IOException {
            if (!closed) {
                try { flush(); }
                finally { closed = true; }
            }
        }
        public synchronized void write(int b) throws IOException {
            if (closed) throw new IOException("connection closed");
            if (nbytes == buf.length) flush();
            buf[nbytes++] = (byte)b;
        }
        public synchronized void write(byte b[], int off, int len)
        throws IOException {
        if (closed) throw new IOException("connection closed");
            if ((nbytes + len) > buf.length) {
                flush();
                if (len >= buf.length) {
                    sendBytes(b, off, len);
                } else {
                    System.arraycopy(b, off, buf, nbytes, len);
                    nbytes += len;
                }
            } else {
                System.arraycopy(b, off, buf, nbytes, len);
                nbytes += len;
            }
        }
        public synchronized void flush() throws IOException {
            if (closed) throw new IOException("connection closed");
            if (nbytes > 0) {
                sendBytes(buf, 0, nbytes);
                nbytes = 0;
            }
        }

        /**
         * Flushes the internal buffer by makes XRPC "send" call. It's
         * important to not call this method too often because then XRPC
         * would add significant overhead. At the same time we need to avoid
         * the situation when data is sitting in the buffer for too long.
         *
         * @param b the data buffer
         * @param off offset where the data begin
         * @param len number of bytes to send
         * @throws IOException in case of I/O trouble
         */
        private void sendBytes(byte b[],int off,int len) throws IOException {
            if (len > 0) {
                session.getClient().invoke(SEI_PROTOCOL,SEI_SEND_METHOD,
                    new XRpcDataElement[] {
                        new XRpcInt(session.getSessionId(),SEI_SEND_SID_PARAM),
                        new XRpcInt(connId, SEI_SEND_CID_PARAM),
                        new XRpcBinaryData(b, off, len, SEI_SEND_DATA_PARAM)
                    });
            }
        }
    }
}
