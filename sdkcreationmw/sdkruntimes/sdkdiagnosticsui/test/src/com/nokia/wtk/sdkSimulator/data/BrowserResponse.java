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


package com.nokia.wtk.sdkSimulator.data;

/**
* class BrowserResponse
*
* Represents an HTTP response. This class extends the corresponding BrowserResponse class
* in Diagnostics, and it is identical to it.
*/
public class BrowserResponse extends com.nokia.wtk.diagnostics.browser.BrowserResponse {

/**
   * Constructs a fully loaded BrowserResponse object. Invokes the superclass
   * constructor.
   *
   * @param requestURL
   * @param status
   * @param responseURL
   * @param contentType
   * @param contentEncoding
   * @param responseHeaders
   * @param responseData
   * @param cacheHit
   * @param transactionId
   */
  public BrowserResponse( String requestURL,
                          int status,
                          String responseURL,
                          String contentType,
                          String contentEncoding,
                          String responseHeaders,
                          byte [] responseData,
                          boolean cacheHit,
                          long transactionId) {

    // we set both contentType and contentEncoding to null, because currently
    // we are not always getting the correct value. When the superclass
    // constructor gets null for these parameters, it extracts the correct values
    // from the content type header.

    super(requestURL,
          status,
          responseURL,
          null,
          null,
          responseHeaders,
          responseData,
          cacheHit,
          transactionId);
  }

}
