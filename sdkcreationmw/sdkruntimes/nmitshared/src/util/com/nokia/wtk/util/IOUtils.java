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



package com.nokia.wtk.util;

/* java.io */
import java.lang.ref.SoftReference;

/* java.io */
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

/* java.net */
import java.net.URL;
import java.net.URLConnection;
import java.net.MalformedURLException;

/**
 * Set of static I/O utilities.
 *
 */
public abstract class IOUtils {

    /** Default size of an intermediate buffer */
    public static final int DEFAULT_BUFFER_SIZE = 2048;

    // URL prefixes
    public static final String FILE_PROTOCOL   = "file";
    public static final String JAR_PROTOCOL    = "jar";

    public static final String FILE_URL_PREFIX  = FILE_PROTOCOL + ':';
    public static final String JAR_URL_PREFIX   = JAR_PROTOCOL + ':';

    // see fixFileURL()
    public static final String FILE_URL_PREFIX_1   = FILE_URL_PREFIX + "/";
    public static final String FILE_URL_PREFIX_2   = FILE_URL_PREFIX + "//";
    public static final String FILE_URL_PREFIX_3   = FILE_URL_PREFIX + "//";

    /** Thread local pool of intermediate buffers of default size */
    private static BufferPool bufferPool = new BufferPool();

    /**
     * Fix a file URL to start with <code>"file://"</code>.
     *
     * @param url the URL to fix
     * @return the "fixed" URL
     */
    public static String fixFileURL(String url) {
        if (url.startsWith(FILE_URL_PREFIX)) {
            if (!url.startsWith(FILE_URL_PREFIX_3)) {
                String tail;
                if (url.startsWith(FILE_URL_PREFIX_2)) {
                    tail = url.substring(FILE_URL_PREFIX_2.length());
                } else if (url.startsWith(FILE_URL_PREFIX_1)) {
                    tail = url.substring(FILE_URL_PREFIX_1.length());
                } else {
                    tail = url.substring(FILE_URL_PREFIX.length());
                }
                return FILE_URL_PREFIX_3 + tail;
            }
        }
        return url;
    }

    /**
     * Return the canonical path of the file given its URL.
     *
     * @param url the file URL
     * @return the canonical path of the file, or <code>null</code> if
     *     an error occurs or if the parameter is not a file URL
     */
    public static String getCanonicalPath(String url) {
        String path = null;
        if (url != null) {
            if (url.startsWith(FILE_URL_PREFIX)) {
                int len = url.length();
                int i = FILE_URL_PREFIX.length();
                while (i<len && url.charAt(i) == '/') i++;
                if (i<len) {
                    int last = url.indexOf('#',i);
                    if (last < 0) last = url.length();
                    String fname = url.substring(i,last);
                    try {
                        File f = new File(fname);
                        if (f.exists()) {
                            path = f.getCanonicalPath();
                        } else {
                            f = new File('/' + fname);
                            if (f.exists()) {
                                path = f.getCanonicalPath();
                            }
                        }
                    } catch (IOException x) {
                        // ignore
                    }
                }
            }
        }
        return path;
    }

    /**
     * Converts file into a nicely formatted, escaped URL string.
     * If the file name contains non-ASCII characters, it will be
     * UTF-8 encoded prior to URL encoding.
     *
     * @param f the file to convert
     *
     * @return the file URL as a string
     */
    public static String encodeFileURL(File f) throws MalformedURLException {
        // NOTE: we rely on the assumption that the URL produced by the
        // File.toURL method is not URL-escaped
        String url = f.getAbsoluteFile().toURL().toExternalForm();
        int len = url.length();

        // on Windoze, the absolute path (almost) always contains a drive
        // letter followed by a colon which we will need to encode. So,
        // allocate at least two more characters
        StringBuffer buf = new StringBuffer(len + 2);

        // copy the schema
        int i = 0;
        for (;i<len; i++) {
            char c = url.charAt(i);
            buf.append(c);
            if (c == ':') break;
        }

        // encode the parts of the file name
        String lastPart = null;
        int lastSeparator = i;
        int nextSeparator;
        int slashCount = 0;   // counts slashes after file:
        while ((nextSeparator = url.indexOf('/',lastSeparator+1)) > 0) {
            if (nextSeparator > (lastSeparator+1)) {
                if (lastPart == null) {
                    while ((slashCount++) < 3) {
                        buf.append('/');
                    }
                }
                lastPart = url.substring(lastSeparator+1,nextSeparator);
                lastPart = StringUtils.urlEncode(lastPart);
                buf.append(lastPart);
            } else if (lastPart == null) {
                slashCount++;
            }
            buf.append('/');
            lastSeparator = nextSeparator;
        }

        // encode the last part
        if ((lastSeparator+1) < len) {
            lastPart = url.substring(lastSeparator+1,len);
            lastPart = StringUtils.urlEncode(lastPart);
            buf.append(lastPart);
        }

        return buf.toString();
    }

    //======================================================================
    //          S E R I A L I Z A T I O N
    //======================================================================

    /**
     * Serializes a serializable object into a byte array.
     *
     * @param obj the object to serialize
     * @throws IOException if an I/O error occurs.
     */
    public static byte [] serialize(Object obj) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        serialize(out, obj);
        return out.toByteArray();
    }

    /**
     * Serializes a serializable object
     *
     * @param out the stream to write seralized data to
     * @param obj the object to serialize
     * @throws IOException if an I/O error occurs.
     */
    public static void serialize(OutputStream out, Object obj)
    throws IOException {
        ObjectOutputStream serializer = new ObjectOutputStream(out);
        serializer.writeObject(obj);
    }

    /**
     * Deserializes ana object from the stream.
     *
     * @param data the byte array containing the serialized object
     * @return the deserialized object, or <code>null</code> if
     * deserialization fails
     */
    public static Object deserialize(byte [] data) {
        try {
            ByteArrayInputStream in = new ByteArrayInputStream(data);
            return new ObjectInputStream(in).readObject();
        } catch (Throwable x) {
            Trace.printStackTrace(x);
            return null;
        }
    }

    /**
     * Deserializes ana object from the stream.
     *
     * @param in the stream to read.
     * @return the deserialized object, or <code>null</code> if
     * deserialization fails
     */
    public static Object deserialize(InputStream in) {
        try {
            return new ObjectInputStream(in).readObject();
        } catch (Throwable x) {
            Trace.printStackTrace(x);
            return null;
        }
    }

    //======================================================================
    //          R E A D
    //======================================================================

    /**
     * Reads file into a byte array.
     *
     * @param file the file to read
     * @throws IOException if an I/O error occurs.
     */
    public static byte [] readFile(File file) throws IOException {
        return readFile(file.getPath());
    }

    /**
     * Reads file into a byte array.
     *
     * @param fileName the name of the file to read
     * @throws IOException if an I/O error occurs.
     */
    public static byte [] readFile(String fileName) throws IOException {
        InputStream in = new FileInputStream(fileName);
        try { return readStream(in); }
        finally { in.close(); }
    }

    /**
     * Reads data from the location specified by the URL. Typically,
     * <code>url</code> specifies an HTTP URL, although it can be
     * anything else, for example:
     * <blockquote>
     * <pre>
     *      http://slava.monich.com:5000/foo.jar
     *      file://d:/users/slava/classes/foo.jar
     *      jar:file://d:/users/slava/classes/foo.jar!/bar.txt
     * </pre>
     * </blockquote>
     *
     * @param url the URL pointing to the file in question
     * @return byte array containing the file data
     * @throws IOException if an I/O error occurs.
     */
    public static byte [] readURL(String url) throws IOException {
        return readURL(new URL(url));
    }

    /**
     * Reads data from the location specified by the URL. Typically,
     * <code>url</code> specifies an HTTP URL, although it can be
     * anything else, for example:
     * <blockquote>
     * <pre>
     *      http://slava.monich.com:5000/foo.jar
     *      file://d:/users/slava/classes/foo.jar
     *      jar:file://d:/users/slava/classes/foo.jar!/bar.txt
     * </pre>
     * </blockquote>
     *
     * @param url the URL pointing to the file in question
     * @return byte array containing the file data
     * @throws IOException if an I/O error occurs.
     */
    public static byte [] readURL(URL url) throws IOException {
        URLConnection conn = url.openConnection();
        InputStream in = conn.getInputStream();
        try { return readStream(in); }
        finally { in.close(); }
    }

    /**
     * Reads data from stream into a byte array. The stream remains open after
     * this method returns.
     *
     * @param in the input stream to read.
     * @return byte array containing the data read from the stream
     * @throws IOException if an I/O error occurs.
     */
    public static byte [] readStream(InputStream in) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        readStream(in, out);
        return out.toByteArray();
    }

    /**
     * Reads data from input stream and writes to output stream until the
     * end of file. Uses default buffer size.
     *
     * @param in the input stream to read.
     * @param out the input stream to read.
     * @return number of bytes copied from one stream to another
     * @throws IOException if an I/O error occurs.
     */
    public static int readStream(InputStream in, OutputStream out)
    throws IOException {
        return readStream(in, out, DEFAULT_BUFFER_SIZE);
    }

    /**
     * Reads data from input stream and writes to output stream until the
     * end of file. Does not close the streams.
     *
     * @param in the input stream to read.
     * @param out the input stream to read.
     * @param bufsiz the intermediate buffer size.
     * @return number of bytes copied from one stream to another
     * @throws IOException if an I/O error occurs.
     */
    public static int readStream(InputStream in, OutputStream out, int bufsiz)
    throws IOException {
        int n, total = 0;
        byte [] buf = bufferPool.getBuffer(bufsiz);
        while ((n = in.read(buf)) > 0) {
            out.write(buf,0,n);
            total += n;
        }
        return total;
    }

    //======================================================================
    //          W R I T E
    //======================================================================

    /**
     * Writes data into the file
     *
     * @throws IOException if an I/O error occurs.
     */
    public static void writeFile(byte[] data, File file)
    throws IOException {
        writeStream(data, new FileOutputStream(file));
    }

    /**
     * Writes data into the file
     *
     * @throws IOException if an I/O error occurs.
     */
    public static void writeFile(byte[] data, String file)
    throws IOException {
        writeStream(data, new FileOutputStream(file));
    }

    /**
     * Appends data to the file
     *
     * @throws IOException if an I/O error occurs.
     */
    public static void appendFile(byte[] data, String file)
    throws IOException  {
        writeStream(data, new FileOutputStream(file, true));
    }

    /**
     * Writes data into the stream and closes the output stream
     *
     * @throws IOException if an I/O error occurs.
     */
    public static void writeStream(byte[] data, OutputStream out)
    throws IOException {
        try {
            out.write(data);
            out.flush();
        } finally {
            out.close();
        }
    }

    /**
     * Copies data from one file to another
     *
     * @throws IOException if an I/O error occurs.
     */
    public static void copyFile(File src, File dest) throws IOException {
        InputStream in = new FileInputStream(src);
        try {
            OutputStream out = new FileOutputStream(dest);
            try { readStream(in, out); }
            finally { out.close(); }
        } finally {
            in.close();
        }
    }

    //======================================================================
    //          C O M P A R E
    //======================================================================

    /**
     * Reads both streams until the end of one or both streams and
     * compares the data. Returns <code>true</code> if both streams
     * emit the same data. Returns <code>false</code> if data are
     * different or one stream ends before the other.
     *
     * @throws IOException if an I/O error occurs.
     */
    public static boolean compareData(InputStream in1, InputStream in2)
    throws IOException {

        // NOTE: using single buffer to read both streams
        byte [] buf = bufferPool.getBuffer();
        int len = buf.length/2;
        int n1=0, n2=0;
        long offset = 0;    // this is for debugging only
        do {
            // try to read the same amount of data from both streams
            n1 = in1.read(buf, 0, len);
            n2 = in2.read(buf, len, len);

            // check for premature end of file
            if ((n1<0 && n2>=0) || (n2<0 && n1>=0)) {
                return false;
            }

            // if both streams report end of file, that's it
            if (n1 < 0 && n2 < 0) {
                break;
            }

            // compare the data read from both streams
            int n = Math.min(n1, n2);
            for (int i=0; i<n; i++) {
                if (buf[i] != buf[len+i]) {
                    return false;
                }
            }

            // now deal with situation when two streams produced
            // different amount of data
            if (n1 > n2) {

                // first stream produced more data then the second one.
                // read remaining bytes from the second stream
                n = (n1 - n2);
                for (int i=0; i<n; i++) {
                    int c = in2.read();
                    if (c < 0 || ((byte)c) != buf[n2+i]) {
                        return false;
                    }
                }

            } else if (n2 > n1) {

                // second stream produced more data then the first one.
                // read remaining bytes from the first stream
                n = (n2 - n1);
                for (int i=0; i<n; i++) {
                    int c = in1.read();
                    if (c < 0 || ((byte)c) != buf[len+n1+i]) {
                        return false;
                    }
                }

            }

            // keep track of curent offset from the beginning of the stream.
            // this is mostly for debugging - to know where exactly we have
            // found the difference
            offset += Math.max(n1,n2);

        } while (n1 > 0);

        return true;
    }

    /**
     * Compare two blocks of data referred to by URLs.
     *
     * @throws IOException if an I/O error occurs.
     */
    public static boolean compareData(URL url1, URL url2) throws IOException {
        InputStream in1 = url1.openConnection().getInputStream();
        InputStream in2 = url2.openConnection().getInputStream();
        try { return compareData(in1, in2); }
        finally {
            in1.close();
            in2.close();
        }
    }

    /**
     * Compare contents of the two files.
     *
     * @return <code>true</code> if both files exist and contain the same
     *         data. Otherwise returns <code>false</code>.
     * @throws IOException if an I/O error occurs.
     */
    public static boolean compareFiles(File f1, File f2) throws IOException {

        // check the obvious signs of files being different, such as file size
        if (!f1.exists() || !f2.exists() || f1.length() != f2.length()) {
            return false;
        }

        // both files exist and have the same size - need to actually
        // read them and compare the data
        InputStream in1 = new FileInputStream(f1);
        InputStream in2 = new FileInputStream(f2);
        try { return compareData(in1, in2); }
        finally {
            in1.close();
            in2.close();
        }
    }

    //======================================================================
    //          B L A C K    H O L E
    //======================================================================

    /**
     * This output stream discards all the data that have been written
     * to it. Since this object has no state or instance variables,
     * in most cases you can reuse a shared instance of it retured by
     * <code>getBlackHole()</code> method of {@link IOUtils} class.
     *
     */
    public static class NullOutputStream extends OutputStream {
        public void write(int b) {}
        public void write(byte b[]) {}
        public void write(byte b[], int off, int len) {}
    }

    /**
     * Shared instance of {@link NullOutputStream} which discards
     * all the data written to it.
     */
    private static OutputStream blackHole = null; // created on demand

    /**
     * Returns shared instance of {@link NullOutputStream} which discards
     * all the data written to it.
     */
    public static OutputStream blackHole() {
        if (blackHole == null) {
            synchronized (IOUtils.class) {
                if (blackHole == null) {
                    blackHole = new NullOutputStream();
                }
            }
        }
        return blackHole;
    }

    //======================================================================
    //          B U F F E R    P O O L
    //======================================================================

    /**
     * Pool of thread-local buffers.
     * NOTE: since buffers allocated by this class are being reused,
     * the buffer it returns may not be zero-filled.
     */
    public static class BufferPool {

        private ThreadLocal cache = new ThreadLocal();

        /**
         * Returns buffer of default size to the caller. Each thread gets its
         * own copy of the buffer, meaning that there's no need to synchronize
         * access to these buffers.
         */
        public byte [] getBuffer() {
            return getBuffer(DEFAULT_BUFFER_SIZE);
        }

        /**
         * Returns buffer of at least the specified size to the caller.
         * Each thread gets its own copy of the buffer, meaning that
         * there's no need to synchronize access to these buffers.
         */
        public byte [] getBuffer(int size) {
            byte[] buffer = null;
            SoftReference ref = (SoftReference)cache.get();
            if (ref != null) {
                buffer = (byte[])ref.get();
            }
            if (buffer == null || buffer.length < size) {
                buffer = new byte[Math.max(DEFAULT_BUFFER_SIZE,size)];
                cache.set(new SoftReference(buffer));
            }
            return buffer;
        }
    }
}

/*
 * Local Variables:
 * c-basic-offset:4
 * End:
 */
