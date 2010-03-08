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
import java.util.Date;
import java.text.ParseException;

/**
 * URLmessage is a base class for URLresponse and URLrequest
 *
 * @version 1.0
 */

public class URLmessage {
    // URL
    protected URL m_url = null;
    // Message Headers
    protected HeaderList m_headerList = null;
    // new line
    protected static final String ln = System.getProperty("line.separator");
    // Transaction ID
    private int m_tid = 0;

    /**
     * setHeaders will define header that are sent to a web server.
     * @param hl - headers to send
     */
    public void setHeaders(HeaderList hl) {
        m_headerList = hl;
    }

    /**
     * Returns the current headers
     */
    public HeaderList getHeaders() {
        return m_headerList;
    }

    /**
     * Append header value to existing
     * @param h - header to append
     */
    public void appendHeader(Header h) {
        if(m_headerList == null) {
            m_headerList = new HeaderList();
        }
        m_headerList.append(h);
    }

    /**
     * Append header value to existing
     * @param h - header to append
     */
    public void replaceHeader(Header h) {
        if(m_headerList == null) {
            m_headerList = new HeaderList();
        }
        m_headerList.replace(h);
    }

    /**
     * Append header value to existing
     * @param h - header to append
     */
    public void removeHeader(String h) {
        if(m_headerList == null) {
            return;
        }
        m_headerList.remove(h);
    }

    /**
     * Get a header value
     * @param key - get a HTTP header value
     */
    public String getHeaderValue(String key) {
        if(m_headerList == null) {
            return null;
        }
        return m_headerList.getHeaderValue(key);
    }

    /**
     * Get a header value as an int.
     * @param key A HTTP header name (e.g., "Age", "Max-Age")
     * @return The integer value of the header or -1 on any failure
     */
    public int getHeaderIntValue(String key) {
	Header hdr;
	int rtn_val = -1;	// default value on failure

        if (m_headerList == null) {
            return -1;
        }
        hdr = m_headerList.getHeader(key);
	if (hdr == null)
		return -1;
	return hdr.getIntValue();
    }

    /**
     * Get the header value of the 'name' header
     * and return it as number of seconds since 1970.
     * @param key A HTTP header name (e.g., "Date", "Expires")
     * @return The integer value of the header or -1 on any failure
     */
    public long getHeaderDateInSeconds(String key) {
	if (m_headerList == null)
		return -1;
	Header date_h = m_headerList.getHeader(key);
	if (date_h == null)
		return -1;
	long date_val = -1;	// default failure value
	Date date = null;
	try {
		date = date_h.getDateValue();
		date_val = date.getTime(); // Returns value in MilliSeconds
	} catch (ParseException e) { 
	}
	return date_val/1000 ;
    }

    /**
     * Set URL
     * @param u - URL to set
     */
    public void setURL(URL u) {
        m_url = u;
    }

    /**
     * Return the current URL
     */
    public URL getURL() {
        return m_url;
    }

    /**
     * Return the current URL (as a string), minus any fragment.
     */
    public String getLoadableURL() {
	return UrlLibrary.getBase( m_url );
    }

    /**
     * Set transaction ID
     */
    public void setTID(int id) {
	m_tid = id;
    }

    /**
     * Return transaction ID of this message
     */
    public int getTID() {
	return m_tid;
    }

    public String toString() {
        String ln = System.getProperty("line.separator");
        StringBuffer sb = new StringBuffer();

        // URL
        sb.append("[URL: ");
        if(m_url != null) {
            sb.append(m_url);
        } else {
            sb.append("null");
        }
        sb.append("]");

        // HeaderFields
        sb.append("[HeaderList: ");
        if(m_headerList != null) {
            sb.append(m_headerList.toString());
        } else {
            sb.append("null");
        }
        sb.append("]");


        return sb.toString();
    }
}
