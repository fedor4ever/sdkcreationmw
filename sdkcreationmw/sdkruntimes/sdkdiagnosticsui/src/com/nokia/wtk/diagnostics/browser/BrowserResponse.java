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


package com.nokia.wtk.diagnostics.browser;

/* java.io */
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/* com.nokia.wtk.device */
import com.nokia.wtk.device.DeviceContentReceived;

/**
 * Ready-to-use implementation of {@link DeviceContentReceived} interface.
 */
public class BrowserResponse extends BrowserData
                             implements DeviceContentReceived
{

  private static final String [] HTTP_Reason_Phrase_1xx = {
      /* 100 */ "Continue",
      /* 101 */ "Switching Protocols"
  };

    private static final String [] HTTP_Reason_Phrase_2xx = {
        /* 200 */ "OK",
        /* 201 */ "Created",
        /* 202 */ "Accepted",
        /* 203 */ "Non-Authoritative Information",
        /* 204 */ "No Content",
        /* 205 */ "Reset Content",
        /* 206 */ "Partial Content"
    };

    private static final String [] HTTP_Reason_Phrase_3xx = {
        /* 300 */ "Multiple Choices",
        /* 301 */ "Moved Permanently",
        /* 302 */ "Moved Temporarily",
        /* 303 */ "See Other",
        /* 304 */ "Not Modified",
        /* 305 */ "Use Proxy"
    };

    private static final String [] HTTP_Reason_Phrase_4xx = {
        /* 400 */ "Bad Request",
        /* 401 */ "Unauthorized",
        /* 402 */ "Payment Required",
        /* 403 */ "Forbidden",
        /* 404 */ "Not Found",
        /* 405 */ "Method Not Allowed",
        /* 406 */ "Not Acceptable",
        /* 407 */ "Proxy Authentication Required",
        /* 408 */ "Request Time-Out",
        /* 409 */ "Conflict",
        /* 410 */ "Gone",
        /* 411 */ "Length Required",
        /* 412 */ "Precondition Failed",
        /* 413 */ "Request Entity Too Large",
        /* 414 */ "Request-URL Too Large",
        /* 415 */ "Unsupported Media Type",
        /* 416 */ "Requested range not satisfiable",
        /* 417 */ "Expectation Failed"
    };

    private static final String [] HTTP_Reason_Phrase_5xx = {
        /* 500 */ "Server Error",
        /* 501 */ "Not Implemented",
        /* 502 */ "Bad Gateway",
        /* 503 */ "Out of Resources",
        /* 504 */ "Gateway Time-Out",
        /* 505 */ "HTTP Version Not Supported"
    };

    private static final String [][] HTTP_Reason_Phrase_Group = {
        new String[0],
        HTTP_Reason_Phrase_1xx,
        HTTP_Reason_Phrase_2xx,
        HTTP_Reason_Phrase_3xx,
        HTTP_Reason_Phrase_4xx,
        HTTP_Reason_Phrase_5xx
    };

    private static final String DEFAULT_REASON = "Unknown status code";


    private String requestURL;
    private int status;
    private String responseURL;
    private boolean cacheHit;
    private long transactionId;

    /**
     * Constructs a fully loaded BrowserResponse object.
     * @param requestURL
     * @param status
     * @param responseURL
     * @param contentType
     * @param enc
     * @param headers
     * @param data
     * @param cacheHit
     * @param transactionId
     */
    public BrowserResponse (String requestURL, int status, String responseURL,
        String contentType, String enc, String headers,
        byte [] data, boolean cacheHit, long transactionId) {

        // first invoke the BrowserData constructor
        super(contentType, enc, headers, data);

        this.requestURL = requestURL;
        this.status = status;

        if (responseURL != null) {
            this.responseURL = responseURL;
        } else {
            String contentLocation = getHeaderValue (CONTENT_LOCATION);
            this.responseURL = (contentLocation != null) ? contentLocation :
                requestURL;
        }
        this.cacheHit = cacheHit;
        this.transactionId = transactionId;
    }


    public String getRequestLocation() {
        return requestURL;
    }

    public int getRequestStatus(StringBuffer msgbuf) {
        int groupIndex = status/100;
        if (groupIndex >= 0 && groupIndex < HTTP_Reason_Phrase_Group.length) {
            String [] phraseGroup = HTTP_Reason_Phrase_Group[groupIndex];
            int phraseIndex = status%100;
            if (phraseIndex >= 0 && phraseIndex < phraseGroup.length) {
                msgbuf.append(phraseGroup[phraseIndex]);
                return status;
            }
        }
        msgbuf.append(DEFAULT_REASON);
        return status;
    }

    public String getLocation() {
        return responseURL;
    }

    public boolean isCacheHit() {
        return cacheHit;
    }

    public long getTransactionId() {
        return transactionId;
    }

    public void setTransactionId(long transactionId) {
        this.transactionId = transactionId;
    }

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.defaultWriteObject();
        serializeHeaders(out);
        serializeData(out);
    }

    private void readObject(ObjectInputStream in) throws IOException,
        ClassNotFoundException {
        in.defaultReadObject();
        deserializeHeaders(in);
        deserializeData(in);
    }

}
