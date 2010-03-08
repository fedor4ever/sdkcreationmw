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



package com.nokia.wtk.http;

import java.net.URL;
import java.util.Vector;
import java.util.Enumeration;
import java.io.ByteArrayInputStream;
import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.text.ParseException;

import com.nokia.wtk.util.CharacterEncoding;
import com.nokia.wtk.util.MimeType;


/**
 * URLresponse is returned from URLloader.
 *
 */
public class URLresponse extends URLmessage {

    // Data from web server
    private byte m_data[] = null;

    // HTTP status from web server
    private int m_status = 0;

    // HTTP message from web server
    private String m_message = null;

    // MIME type
    private String m_mimeType = null;

    // Character Set
    private String m_charSet = null;

    // Cache hit, tells if the response comes from the cache
    private int m_cacheHitCounter = 0;

    // Tells if the response was pushed rather than pulled
    private boolean m_pushed = false;

    // Original request
    private URLrequest request = null;

    // Altered content
    private URLresponse originalContent = null;

    /**
     * Constructs URLresponse
     */
    public URLresponse() {}

    /**
     * Constructs a copy of a URLresponse
     */
    private URLresponse(URLresponse other) {
        this(other.getData());
        setURL(other.getURL());
        setTID(other.getTID());
        if (other.getHeaders() != null) {
            setHeaders((HeaderList)other.getHeaders().clone());
        }
        setStatus(other.getStatus());
        setMessage(other.getMessage());
        setMimeType(other.getMimeType());
        setCharSet(other.getCharSet());
        setCacheHits(other.getCacheHits());
        setRequest((URLrequest)other.getRequest());
    }

    /**
     * Constructs URLresponse
     * @param bytes is response bytes
     * @param hl is header list
     */
    public URLresponse(byte[] bytes, HeaderList hl) {
        this(bytes);
        setHeaders(hl);
    }

    /**
     * Constructs URLresponse
     * @param bytes is response bytes
     */
    public URLresponse(byte[] bytes) {
        setData(bytes);
    }

    /**
     * Sets content in the byte format
     */
    public void setData(byte [] bytes) {
        m_data = bytes;
    }

    /**
     * Sets status
     */
    public void setStatus(int s) {
        m_status = s;
    }

    /**
     * Sets status as a String.
     */
    public void setStatus(String s) {
        if (s == null || s.length() == 0)
            return;
        try {
            m_status = Integer.parseInt(s);
        } catch (Exception ex) {}
    }

    /**
     * Sets message.  Trim quotes and spaces.
     */
    public void setMessage(String m) {
        if (m != null) {
            m = m.trim();
            if (m.startsWith("\"")) {
                m = m.substring(1);
                int i = m.lastIndexOf("\"");
                if (i > 0) {
                    m = m.substring(0, i);
                }
            }
        }
        m_message = m;
    }

    /**
     * Returns the length of data
     * @return the length of data or zero if non-existent.
     */
    public int getDataLength() {
        return (m_data != null) ? m_data.length : 0;
    }

    /**
     * Returns data in String format
     * @return a text string representing the response data
     */
    public String getDataString() {
        // Convert response data to text using the received character encoding:
        CharacterEncoding enc = CharacterEncoding.findEncoding(getCharSet());
        if (enc == null) {
            enc = CharacterEncoding.getDefault();
        }
        return enc.decodeString(getData(), 0, getData().length);
    }

    /**
     * Parse status line (first line of HTTP response stream):
     *
     *    HTTP-Version SP Status-Code SP Reason
     *
     * @see RFC 2068
     */
    public void parseStatusLine(String sline) {
        if (sline == null) {
             return;
        }
        int statIndex = sline.indexOf(' ');
        if (statIndex > 0) {
            int msgIndex = sline.indexOf(' ', statIndex + 1);
            if (msgIndex > 0) {
                String stat = sline.substring(statIndex + 1, msgIndex);
                String msg = sline.substring(msgIndex + 1);
                setMessage(msg);
                setStatus(stat);
            }
        }
    }

    /**
     * Return a stream into a data
     * @return the data stream
     */
    public ByteArrayInputStream getDataStream() {
        return new ByteArrayInputStream(getData());
    }

    /**
     * Returns HTTP status
     */
    public int getStatus() {
        return m_status;
    }

    /**
     * Gets the status code from the content request.
     * If a text buffer is supplied, then the corresponding message will also be returned.
     * @param msgbuf an optional buffer to return the status message as well
     * @return the status code that resulted from the content request
     */
    public int getRequestStatus(StringBuffer msgbuf) {
        if (getMessage() != null) {
            msgbuf.append(getMessage());
        }
        return getStatus();
    }

    /**
     * Returns true if HTTP status code is OK.
     */
    public boolean isHttpStatusOk() {
        return ((getStatus() < 300) || (getStatus() > 600));
    }

    /**
     * Returns reason message
     */
    public String getMessage() {
        return m_message;
    }

    /**
     * Returns the MIME type from headers.
     * @return String
     */
    public String getMimeType() {
        return m_mimeType;
    }

    /**
     * Sets the MIME type.
     * @param type defines mimetype
     */
    public void setMimeType(String type) {
        m_mimeType = type;
    }

    /**
     * @return the charset value from headers.
     */
    public String getCharSet() {
        return m_charSet;
    }

    /**
     * Sets the Character set.
     * @param set defines Character Set
     */
    public void setCharSet(String set) {
        m_charSet = set;
    }

    /**
     * Sets the "pushed" indicator for this content descriptor
     * @param pushed if <code>true</code> then flag this response as pushed rather than pulled.
     */
    public void setPushed(boolean pushed) {
        this.m_pushed = pushed;
    }

    /**
     * Returns <code>true</code> if this response was pushed rather than pulled.
     */
    public boolean isPushed() {
        return m_pushed;
    }

    /**
     * Returns <code>true</code> if this response came from the cache.
     */
    public boolean isCacheHit() {
        return (m_cacheHitCounter > 0);
    }

    /**
     * Returns the number of cache hits on this response object.
     */
    public int getCacheHits() {
        return m_cacheHitCounter;
    }

    /**
     * Sets the number of cache hits on this response object.
     * @param n the number to assign
     */
    private void setCacheHits(int n) {
        m_cacheHitCounter = n;
    }

    /**
     * Bump value of number of cache hits on this response.
     */
    public void addCacheHit() {
        m_cacheHitCounter++;
    }

    /**
     * Attaches HTTP header list into this URL response
     * This will parse out value for characterset
     */
    public void setHeaders(HeaderList hl) {
        super.setHeaders(hl);

        if (hl == null) {
            return;
        }

        // Set Content type and Character Set
        Header contentTypeHeader = hl.getHeader(MimeType.CONTENT_TYPE_HEADER);
//MessageWriters.DebugMsg.println("URLresponse.setHeaders: contentTypeHeader=" + contentTypeHeader);
/*/
        if (contentTypeHeader == null) {
            // Let's try CContent-type, because decoder has a bug
            contentTypeHeader = hl.getHeader("C" + MimeType.CONTENT_TYPE_HEADER);
        }
//*/
        if (contentTypeHeader != null) {
            // Gets header values for Content type
            Vector v = contentTypeHeader.getHeaderValues();
            if ((v != null) && (v.size() > 0)) {
                //
                // The first value is charset
                //
                HeaderValue first = (HeaderValue)v.elementAt(0);
                if (first != null) {
                    setMimeType(first.getType());
                    //
                    // Get all parameters for Content Type
                    //
                    Vector params = first.getParams();
                    if (params != null) {
                        //
                        // Find charset
                        //
                        for (Enumeration e2 = params.elements() ; e2.hasMoreElements() ;) {
                            NameValuePair vp = (NameValuePair)e2.nextElement();
                            //DEBUG.println("param name[" + vp.getName() + "]:[" + vp.getValue() + "]");
                            if (vp != null && vp.getName().equalsIgnoreCase("charset")) {
                                // Character set found.
                                // Set value to uppercase and modify it:
                                setCharSet(vp.getValue().toUpperCase().replace('_', '-'));
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * Sets the original content request.
     * @param request the original content request.
     */
    public void setRequest(URLrequest request) {
        this.request = request;
    }

    /**
     * Gets the original content request.
     * @return the original content request.
     */
//    public DeviceContentRequest getRequest() {
    public URLrequest getRequest() {
         return this.request;
    }

    /**
     * Gets the original content request location, if any.
     * @return the URL string representing the original content request
     *         or <code>null</code> if not applicable.
     */
    public String getRequestLocation() {
       return (this.request != null) ? this.request.getLocation() : null;
    }

    /**
     * Saves the original content if a content handler is about to modify this response data.
     * @see #getOriginalContent
     */
    public void saveOriginalContent() {
        this.originalContent = new URLresponse(this);
    }

    /**
     * Gets the original content if a content handler has modified this response data.
     * @return the original content or <code>null</code> if no modification was done.
     */
//    public DeviceContentReceived getOriginalContent() {
//        return originalContent;
//    }

    /**
     * Gets the content location.
     * @return the URL string representing the content location.
     */
    public String getLocation() {
        return getURL().toExternalForm();
    }

    /**
     * Gets the content type.
     * @return the content MIME type.
     */
    public String getType() {
        //TODO: remove possible extension if it's a recognized encoding name

        if (getMimeType() == null) {
            setHeaders(getHeaders());	// if no MIME type is set, try reprocessing headers
        }
        String type = getMimeType();
        return type;
    }

    /**
     * Gets the content data.
     * @return the content data.
     */
    public byte[] getData() {
        return m_data;
    }

    /**
     * Gets the encoding of the content data.
     * @return the content data encoding.
     */
    public String getEncoding() {
        String encoding = null;
        // Encoding is only relevant for "text/*" MIME types:
        if ((getType() != null) && getType().startsWith("text/")) {
            // Apply intelligence to get encoding from charset, mimetype extension or schema default
            encoding = getCharSet();
            if ((encoding == null) || (encoding.length() == 0)) {
                // If encoding is not defined, use default encoding per schema:
                String url = getLocation();
                if (url != null) {
                    if (url.startsWith(MimeType.URL_HTTP_SCHEME + ":")) {
                        encoding = CharacterEncoding.getHTTPDefault().getMIMEName();
                    } else if (url.startsWith(MimeType.URL_HTTPS_SCHEME + ":")) {
                        encoding = CharacterEncoding.getHTTPDefault().getMIMEName();
                    } else if (url.startsWith(MimeType.URL_FILE_SCHEME + ":")) {
                        encoding = CharacterEncoding.getFileEncoding().getMIMEName();
                    }
                }
            }
            if ((encoding == null) || (encoding.length() == 0)) {
                // If encoding is still not defined, use default encoding (UTF8)
                encoding = CharacterEncoding.getDefault().getMIMEName();
            }
        }
        return encoding;
    }

    /**
     * Gets the set of request header names.
     * @return the set of request header names.
     */
    public String[] getHeaderNames() {
        HeaderList headers = getHeaders();
        String[] names = new String[(headers != null) ? headers.size() : 0];
        if (names.length > 0) {
            Enumeration e = headers.elements();
            for (int i = 0; e.hasMoreElements(); i++) {
                Header h = (Header)e.nextElement();
                names[i] = h.getName();
            }
        }
        return names;
    }

    /**
     * Gets the value of a request header.
     * @param name the name of a request header.
     * @return the value of a request header or <code>null</code> if non-existent.
     */
    public String getHeaderValue(String name) {
        String value = null;
        HeaderList headers = getHeaders();
        if ((headers != null) && (name != null)) {
            Header h = headers.getHeader(name);
            if (h != null) {
                value = h.getValue();
            }
        }
        return value;
    }

    /**
     * A convenience function to get the virtual URL from the headers of a response.
     * @see Push Message spec and zhttp spec on Content-Location.
     */
    public URL getURLFromHeaders() {
            String urlVal = this.getHeaderValue("X-Wap-Content-URI");
            if (urlVal == null)
                    urlVal = this.getHeaderValue("Content-Location");
            if (urlVal == null) {
                    return null;
            }
            URL u = null;
            try {
                    u = new URL(urlVal);
            } catch (Exception e) {
                    return null;
            }
            return u;
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a byte.
     * @param b the byte
     * @return the string representation of a byte.
     */
    private static String toString(byte b) {
        int hi = ((b & 0xF0) >>> 4);    // upper nybble
        int lo = (b & 0x0F);            // Lower nybble
        return "" + ((hi >= 10) ? (char)('A' + (hi - 10)) : (char)('0' + hi))
                  + ((lo >= 10) ? (char)('A' + (lo - 10)) : (char)('0' + lo));
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a byte array.
     * @param bytes the byte array
     * @return the string representation of a byte array.
     */
    private static String toString(byte[] bytes) {
        StringBuffer sbuf = new StringBuffer();
        if (bytes != null) {
            sbuf.append('[');
            for (int i = 0; i < bytes.length; i++) {
                if (i > 0) sbuf.append(',');
                sbuf.append(toString(bytes[i]));
            }
            sbuf.append(']');
        } else {
            sbuf.append("null");
        }
        return sbuf.toString();
    }

    /**
     * Returns URLresponse in a human readable form.
     * @return String
     */
    public String toString() {
        StringBuffer sb = new StringBuffer();

        sb.append("URL Response:");

        // Status
        sb.append("[HTTP Status: ");
        sb.append(getStatus());
        sb.append("]");
        // Mime Type
        sb.append("[Mime Type: ");
        sb.append(getMimeType());
        sb.append("]");
        // Status
        sb.append("[CharSet: ");
        sb.append(getCharSet());
        sb.append("]");

        // Message
        sb.append("[HTTP Message: ");
        if (m_message != null) {
            sb.append(getMessage());
            sb.append("]");
        } else {
            sb.append("null]");
        }
        // Cache hit
        sb.append("[Cache Hit: ");
        sb.append(getCacheHits());
        sb.append("]");

        // Data length
        sb.append("[Data Length: ");
        sb.append(getDataLength());
        sb.append("]");

        // Data
        // BB Note - Very expensive to dump the data itself!
        // The "Session" view provides a means to expose this level of detail on demand.
        sb.append("[Data: ");
        sb.append(toString(getData()));
        sb.append("]");

        sb.append(super.toString());

        return sb.toString();
    }

    /**
     * Read in a file as an HTTP response.
     *
     * @param path path to file
     * @return URLresponse object suitable for processing
     */
    static public URLresponse readFile(String path) throws IOException {
        return readFile(path, null);
    }

    /**
     * Read a file formatted as an HTTP response into the URLresponse object provided.
     *
     * @param path path to file
     * @param resp URLresponse object to read into.  If null, we allocate one.
     * @return URLresponse object suitable for processing
     * @see RFC 2068
     */
    static public URLresponse readFile(String path, URLresponse resp) throws IOException {
        if (path == null)
                throw new IOException("No filename to read.");
        //
        // Open file.  We need two streams because the utilities can't be
        // used consecutively.
        //

        FileInputStream fin = null;
        try {
            fin = new FileInputStream(path);
        } catch (IOException ex) {
                throw new IOException("Can't open file " + path);
        }
        return readFile(fin, resp);
    }

    static public URLresponse readFile(InputStream is, URLresponse resp) throws IOException {
        if (is == null)
                throw new IOException("No stream to read.");
        BufferedInputStream bin = new BufferedInputStream(is);
        String statusLine = null;
        String topHeaders = null;
        byte[] content = null;
        bin.mark(0xFFFF);
        try {
                statusLine = readStatusLine(bin);
                bin.reset();
                topHeaders = MimeMessageParser.getHeaderString(bin);
                bin.reset();
                content = MimeMessageParser.getContentBytes(bin);
        } catch (Exception ex) {
                throw new IOException("Can't parse message headers");
        }
        HeaderList top_hl = null;
        try {
                top_hl = new HeaderList( topHeaders );
        } catch (ParseException p_ex) {
                throw new IOException("Can't parse message headers");
        }
        if (resp == null)
                resp = new URLresponse();
        resp.parseStatusLine(statusLine);
        resp.setHeaders(top_hl.decode());
        resp.setData(content);
        return resp;
    }

    /**
     * Write this response to a HTTP_RESPONSE file.
     * The format is the standard HTTP 1.0 response:
     *
     *    Status-Line CRLF
     *    Header CRLF Header CRLF ...
     *    CRLF
     *    Body
     *
     * @param path path to file
     * @return URLresponse object suitable for processing
     * @see RFC 2068
     */
    public void writeFile(String path) throws IOException {
        if (path == null)
                throw new IOException("No filename to write.");

        HeaderList hdrs = this.getHeaders();
        if (hdrs == null)
                throw new IOException("No HTTP Headers to write.");

        // We assume this message is an OK response
        String statusLine = "HTTP/1.0 200 OK\r\n";
        String hStr = hdrs.encodeQP().toString();
        FileOutputStream fos = null;
        try {
                fos = new FileOutputStream( path );
                fos.write( statusLine.getBytes(), 0, statusLine.length());
                fos.write( hStr.getBytes(), 0, hStr.length());
                fos.write( (int)'\r' );
                fos.write( (int)'\n' );
                byte[] data = this.getData();
                if (data != null)
                        fos.write(data, 0, data.length);
                fos.close();
        } catch (IOException ex) {
                throw new IOException("Couldn't write to " + path + ": " +
                        ex.getMessage());
        }
    }

    /**
     * Read the first line of an HTTP_RESPONSE file
     * which should be the status line.  Set the BufferedInput to be at
     * the beginning of the headers.
     * @param InputStream of HTTP_RESPONSE being parsed.
     * @return the status line String or null if not found.
     * @see RFC 2068
     */
    public static String readStatusLine(BufferedInputStream input) {
        StringBuffer sb = new StringBuffer();
	int ch;
        try {
    	  while ((ch = input.read()) != -1) {
	    if (ch == '\n') {
                break;
	    }
            sb.append((char)ch);
          }
        } catch (IOException ex) {
            return null;
    	}

        String statusLine = sb.toString();
        //
        // Does this look like a status line?
        //
        if (statusLine.startsWith("HTTP/1.")) {
            input.mark(0xFFFF);
            return statusLine;
        }

        // Not a status line.  Do nothing and return null.
        return null;
    }

    /**
     * UNIT TEST of I/O functions
     */
    public static void main(String[] args) {
        String testHeaders = new String(
           "Content-Type:text/foo\r\n" +
           "Cache-Control: no-cache\r\n"
        );
        String myData = "Here's my data";
        String filename = "resp.out";
        URLresponse r1 = null;
        if (args.length > 0) {
            filename = args[0];
            try {
                r1 = URLresponse.readFile(filename);
            } catch (IOException ex) {
                System.out.println("Error reading " + filename + ":" + ex.getMessage());
                System.out.println("Usage: URLresponse <http-response-file>");
                System.exit(1);
            }
            if (r1 != null) {
                System.out.println("Read file " + filename + ":\n" + r1);
            }
            System.exit(0);
        }

        // No args
        URLresponse r2 = null;
        try {
                HeaderList hl = new HeaderList(testHeaders);
                URLresponse resp = new URLresponse(myData.getBytes(), hl);
                resp.writeFile(filename);
                r2 = URLresponse.readFile(filename);
        } catch (Exception ex) {
                System.out.println("Can't write file: " + ex);
        }
        System.out.println("Wrote to file " + filename);
        if (r2 != null)
                System.out.println("Read file:\n" + r2);
    }
}
