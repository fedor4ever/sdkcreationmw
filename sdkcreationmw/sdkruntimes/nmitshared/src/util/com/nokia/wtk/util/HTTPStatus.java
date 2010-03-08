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
 * This interface defines HTTP status codes
 *
 */
public interface HTTPStatus {

    public static final int CONTINUE           = 100;
    public static final int SWITCHING_PROTOCOL = 101;

    public static final int OK                 = 200;
    public static final int CREATED            = 201;
    public static final int ACCEPTED           = 202;
    public static final int PARTIAL            = 203;
    public static final int NO_CONTENT         = 204;
    public static final int RESET_CONTENT      = 205;
    public static final int PARTIAL_CONTENT    = 206;

    public static final int AMBIGUOUS          = 300;
    public static final int MOVED              = 301;
    public static final int REDIRECT           = 302;
    public static final int REDIRECT_METHOD    = 303;
    public static final int NOT_MODIFIED       = 304;
    public static final int USE_PROXY          = 305;
    public static final int REDIRECT_KEEP_VERB = 307;

    public static final int BAD_REQUEST        = 400;
    public static final int DENIED             = 401;
    public static final int PAYMENT_REQ        = 402;
    public static final int FORBIDDEN          = 403;
    public static final int NOT_FOUND          = 404;
    public static final int BAD_METHOD         = 405;
    public static final int NONE_ACCEPTABLE    = 406;
    public static final int PROXY_AUTH_REQ     = 407;
    public static final int REQUEST_TIMEOUT    = 408;
    public static final int CONFLICT           = 409;
    public static final int GONE               = 410;
    public static final int LENGTH_REQUIRED    = 411;
    public static final int PRECOND_FAILED     = 412;
    public static final int REQUEST_TOO_LARGE  = 413;
    public static final int URI_TOO_LONG       = 414;
    public static final int UNSUPPORTED_MEDIA  = 415;
    public static final int BAD_RANGE          = 416;
    public static final int EXPECTATION_FAILED = 417;

    public static final int SERVER_ERROR       = 500;
    public static final int NOT_SUPPORTED      = 501;
    public static final int BAD_GATEWAY        = 502;
    public static final int SERVICE_UNAVAIL    = 503;
    public static final int GATEWAY_TIMEOUT    = 504;
    public static final int VERSION_NOT_SUP    = 505;
}
