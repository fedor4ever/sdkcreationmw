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
import java.util.Enumeration;

import com.nokia.wtk.util.CharacterEncoding;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.MimeType;


/**
 * URLrequest is a request packet to URLloader.
 *
 */
public class URLrequest extends URLmessage implements
      //DeviceContentRequest,
      MessageWriters {

    /** HTTP operations */
    public final static String GET = "GET";
    public final static String POST = "POST";

    // HTTP operation.Default value is GET
    private String m_httpOperation = GET;

    // Post data to send
    private String m_postData = null;

    // Reload attribute will force reload
    private boolean m_reload = false;

    // Previous attribute will skip Freshness calculations
    // =true if <prev/> is forced
    private boolean m_previous = false;

    // User-Agent header ID:
    //private static final String USER_AGENT_ID = "Nokia-WAP-Toolkit";
    private static final String USER_AGENT_ID = "Nokia-MIT-Browser";

    // default value for Accept-Charset header:
    private static String defaultAcceptCharset;

    // acceptable Gateway types
    private final String GW_CONTENT_TYPES =
            MimeType.WML_BINARY + "," +
            MimeType.WML_SCRIPT_BINARY + "," +
            MimeType.VCARD_SOURCE + "," +
            MimeType.VcARD_SOURCE + "," +
            MimeType.VCAL_SOURCE + "," +
            MimeType.VCALENDAR_SOURCE + "," +
            MimeType.VcALENDAR_SOURCE + "," +
            MimeType.IMAGE_WBMP + "," +
            MimeType.WBXML_BINARY + "," +
            MimeType.MULTIPART_MIXED_WSP + "," +
            MimeType.MULTIPART_MIXED + "," +
            MimeType.IMAGE_GIF + "," +
            MimeType.HASHED_CERTIFICATE + "," +
            MimeType.SIGNED_CERTIFICATE + "," +
            MimeType.USER_CERTIFICATE + "," +
            MimeType.IMAGE_JPG;

    // acceptable HTTP types
    private final String HTTP_CONTENT_TYPES =
            MimeType.WML_SOURCE + "," +
            MimeType.WML_SCRIPT_SOURCE + "," +
            GW_CONTENT_TYPES;

    // Cache Control Header:
    public static final String CACHE_CONTROL_HEADER = "Cache-control";



    /**
     * Creates a URL request.
     * Default operation is GET.
     * @param u - URL to get
     */
    public URLrequest(URL u) {
        createRequestHeaders();
        m_url = u;
    }

    private static String getDefaultAcceptCharsetHeaderValue() {
        if (defaultAcceptCharset == null) {
            defaultAcceptCharset = CharacterEncoding.findEncoding(CharacterEncoding.Enum_ASCII).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Latin1).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Latin2).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Latin3).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Latin4).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Cyrillic).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Arabic).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Greek).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Hebrew).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Latin5).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_SJIS).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_EUCJP).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_ISO2022JP).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_GB2312).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Big5).getMIMEName()
            +  ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_UTF7).getMIMEName()
            +  ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_UTF8).getMIMEName()
            +  ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_UTF16).getMIMEName()
            + ", "
            + CharacterEncoding.findEncoding(CharacterEncoding.Enum_Unicode).getMIMEName();
        }
        return defaultAcceptCharset;
    }

    /**
     * Add all default request headers.
     */
    protected void createRequestHeaders() {
        //
        // Create an Accept-Charset header with all of the supported
        // character encodings.
        //
        appendHeader(new Header(MimeType.ACCEPT_CHARSET_HEADER,
                                getDefaultAcceptCharsetHeaderValue()));
    }

    /**
     * setOperation sets an operation type e.g. GET or POST
     * Default value is GET ie. URLrequest.get
     * @param operation - operation name
     */
    public void setOperation(String operation) {
        if (operation.equalsIgnoreCase(GET)) {
            m_httpOperation = GET;
        }
        else if(operation.equalsIgnoreCase(POST)) {
            m_httpOperation = POST;
        }
        // Todo throw exception
    }

    /**
     * Returns the current operation
     */
    public String getOperation() {
        return m_httpOperation;
    }

    /**
     * Sets post data to send
     * @param data - data to send
     * @param ct - content for post data
     */
    public void setPostData(String data, String ct) {
        if ((data != null) && (ct != null)) {
            // Don't encode data, because it is not safe to do that here
            m_postData = data;
            // Add content type
            replaceHeader(new Header(MimeType.CONTENT_TYPE_HEADER, ct));

            // Add content length
            byte a[] = m_postData.getBytes();
            replaceHeader(new Header(MimeType.CONTENT_LENGTH_HEADER, a.length));
            return;
        } else {
            // null data given, reset headers
            m_postData = null;
            removeHeader(MimeType.CONTENT_TYPE_HEADER);
            removeHeader(MimeType.CONTENT_LENGTH_HEADER);
        }
    }

    /**
     * Set referer see RFC 2068
     * @param url to referrer
     */
    public void setReferer(URL url) {
        String bu = UrlLibrary.getBase(url);
        setReferer(bu);
    }

    /**
     * Set referer see RFC 2068
     * @param url to referrer
     */
    public void setReferer(String bu) {
        Header h = new Header(MimeType.REFERER_HEADER, bu);
        replaceHeader(h);
    }


    /**
     * Set Accept-Charset, see RFC 2068
     * @param url to referrer
     */
    public void setAcceptCharset(String charset) {
        Header h = new Header(MimeType.ACCEPT_CHARSET_HEADER, charset);
        replaceHeader(h);
    }

    /**
     * Set CacheControl header
     * @param cc the CacheControl
     */
    public void setCacheControl(String cc) {
        Header h = new Header(CACHE_CONTROL_HEADER, cc);
        replaceHeader(h);
    }

    /**
     * Set a particular Location Header
     * @param name the name of location header
     * @param value the value of location header
     */
     public void setLocationHeader(String name, String value) {
        Header h = new Header(name, value);
        replaceHeader(h);
    }

    /**
     * Get normalized server name (used as a key by auth cache)
     * @returns host or null
     */
    public String getServerKey() {
        if (m_url == null)
            return null;
            String srvk = m_url.getHost().toLowerCase();
        if (m_url.getPort() != 80) {
            srvk = srvk + ":" + m_url.getPort();
        }
        return srvk;
    }

    /**
     * Returns true if this request is a reload
     */
    public boolean isReload() {
        return m_reload;
    }

    /**
     * Returns true if this request is a <prev/>
     */
    public boolean isPrevious() {
        return m_previous;
    }

    /**
     * Returns true if this request is a Condional Get
     */
    public boolean isConditionalGet() {
        if (getOperation().equalsIgnoreCase(GET)) {
            HeaderList cc_h = getHeaders();
            if (cc_h != null) {
                if (cc_h.hasHeader("If-Modified-Since")
                 || cc_h.hasHeader("If-Unmodified-Since")
                 || cc_h.hasHeader("If-Match")
                 || cc_h.hasHeader("If-None-Match")
                 || cc_h.hasHeader("If-Range")) {
                    return true;
                 }
            }
        }
        return false;
    }

    /**
     * Sets value for reload attribute
     * @param reload if true then reload is forced
     */
    public void setReload(boolean reload) {
        m_reload = reload;
    }

    /**
     * Sets value for Previous attribute
     * @param previous if true then <prev/> is forced
     */
    public void setPrevious(boolean previous) {
        m_previous = previous;
    }

    /**
     * Gets the requested content location.
     * @return the URI string representing the content location.
     */
    public String getLocation() {
        return getURL().toExternalForm();
    }

    /**
     * Gets the method used to access the content.
     * For HTTP requests, this may be "GET" or "POST".
     * @return the method used to access the content.
     */
    public String getMethod() {
        return getOperation();
    }

    /**
     * Gets the POST data to be used when using the POST method.
     * @return the POST data encoded as a text string.
     */
    public String getPostData() {
        return m_postData;
    }


    /**
     * Gets the POST data to be used when using the POST method.
     * @return the POST data encoded as a text string.
     */
    public byte [] getData() {
        if (m_postData == null) {
            return null;
        } else {
            // Encoding?
            return m_postData.getBytes();
        }
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
     * Shows everything in a human readable form.
     */
    public String toString() {
        StringBuffer sb = new StringBuffer();

        sb.append("URL Request:");

        // Headers
        sb.append("\n[HeaderList: ");
        if(m_headerList != null) {
            sb.append(m_headerList.toString());
        } else {
            sb.append("null");
        }
        sb.append("]");

        // URL
        sb.append("\n[HTTPOperation: ");
        if(m_httpOperation != null) {
            sb.append(m_httpOperation);
        } else {
            sb.append("null");
        }
        sb.append("]");

        // Post Data
        sb.append("\n[PostData: ");
        if(m_postData != null) {
            sb.append(m_postData.toString());
        } else {
            sb.append("null");
        }
        sb.append("]");

        sb.append(super.toString());

        return sb.toString();
    }
}
