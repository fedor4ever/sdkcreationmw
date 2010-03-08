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

/**
 * Names of various HTTP headers. This interfaces defines no methods.
 */
public interface HTTPHeaderNames {

    // General headers(may appear in both request and response messages)
    static final String CACHE_CONTROL        = "Cache-Control";
    static final String DATE                 = "Date";
    static final String PRAGMA               = "Pragma";
    static final String TRAILER              = "Trailer";
    static final String TRANSFER_ENCODING    = "Transfer-Encoding";
    static final String UPGRADE              = "Upgrade";
    static final String VIA                  = "Via";
    static final String WARNING              = "Warning";

    // HTTP request headers
    static final String ACCEPT               = "Accept";
    static final String ACCEPT_CHARSET       = "Accept-Charset";
    static final String ACCEPT_ENCODING      = "Accept-Encoding";
    static final String ACCEPT_LANGUAGE      = "Accept-Language";
    static final String ACCEPT_RANGES        = "Accept-Ranges";
    static final String AUTORIZATION         = "Authorization";
    static final String FROM                 = "From";
    static final String HOST                 = "Host";
    static final String IF_MATCH             = "If-Match";
    static final String IF_MODIFIED_SINCE    = "If-Modified-Since";
    static final String IF_NONE_MATCH        = "If-None-Match";
    static final String IF_RANGE             = "If-Range";
    static final String IF_UNMODIFIED_SINCE  = "If-Unmodified-Since";
    static final String PROXY_AUTORIZATION   = "Proxy-Authorization";
    static final String RANGE                = "Range";
    static final String REFERER              = "Referer";
    static final String USER_AGENT           = "User-Agent";

    // HTTP response headers
    static final String AGE                  = "Age";
    static final String CONTENT_DISPOSITION  = "Content-Disposition";
    static final String CONTENT_ENCODING     = "Content-Encoding";
    static final String CONTENT_LANGUAGE     = "Content-Language";
    static final String CONTENT_LENGTH       = "Content-Length";
    static final String CONTENT_LOCATION     = "Content-Location";
    static final String CONTENT_MD5          = "Content-MD5";
    static final String CONTENT_RANGE        = "Content-Range";
    static final String CONTENT_TYPE         = "Content-Type";
    static final String ETAG                 = "ETag";
    static final String EXPIRES              = "Expires";
    static final String LAST_MOFIFIED        = "Last-Modified";
    static final String LOCATION             = "Location";
    static final String PROXY_AUTHNTICATE    = "Proxy-Authenticate";
    static final String SERVER               = "Server";
    static final String VARY                 = "Vary";
    static final String WWW_AUTHNTICATE      = "WWW-Authenticate";

    // WAP specific headers
    static final String X_WAP_APPLICATION_ID = "X-Wap-Application-Id";
    static final String X_WAP_CONTENT_URI    = "X-Wap-Content-URI";
    static final String X_WAP_INITIATOR_URI  = "X-Wap-Initiator-URI";
    static final String X_WAP_TOD            = "X-WAP_TOD"; // Time Of Day

    // UAProf headers
    static final String PROFILE              = "x-wap-profile";
    static final String PROFILE_DIFF         = "x-wap-profile-diff";
    static final String PROFILE_WARNING      = "x-wap-profile-warning";

    // MSISDN (Mobile Station Integrated Services Directory Number) headers
    static final String MSISDN_BEARER        = "X-Nokia-BEARER";
    static final String MSISDN_CMODE         = "X-Nokia-CONNECTION_MODE";
    static final String MSISDN               = "X-Nokia-MSISDN";
    static final String MSISDN_IP_ADDRESS    = "X-Nokia-IP_ADDRESS";
    static final String MSISDN_AUTHORIZATION = "X-Nokia-AUTHORIZATION";
    static final String MSISDN_IMSI          = "X-Nokia-IMSI";
    static final String MSISDN_SGSN          = "X-Nokia-SGSN";
    static final String MSISDN_GGSN          = "X-Nokia-GGSN";
    static final String MSISDN_CHARGING_ID   = "X-Nokia-CHARGING_ID";
    static final String MSISDN_PREPAID_IND   = "X-Nokia-PREPAID_IND";
    static final String MSISDN_WTLS          = "X-Nokia-WTLS";

    // Location Info headers
    static final String LOC_METHOD           = "X-NOKIA-WAP-LOC-LOCATIONINFO-METHOD";
    static final String LOC_LATITUDE         = "X-NOKIA-WAP-LOC-LAT";
    static final String LOC_LONGITUDE        = "X-NOKIA-WAP-LOC-LONG";
    static final String LOC_GSM_INFORMATION  = "X-NOKIA-WAP-LOC-GSM-INFORMATION";
    static final String LOC_LL_FORMAT        = "X-NOKIA-WAP-LOC-LL-FORMAT";
    static final String LOC_PN_FORMAT        = "X-NOKIA-WAP-LOC-PN-FORMAT";
    static final String LOC_ACTUAL_LL        = "X-NOKIA-WAP-LOC-ACTUAL-LL";
    static final String LOC_ACCURACY         = "X-NOKIA-WAP-LOC-ACCURACY";
    static final String LOC_ERROR            = "X-NOKIA-WAP-LOC-ERROR";

    // Deprecated aliases
    static final String CONTENT_Length       = CONTENT_LENGTH;
    static final String CONTENT_Location     = CONTENT_LOCATION;
}
