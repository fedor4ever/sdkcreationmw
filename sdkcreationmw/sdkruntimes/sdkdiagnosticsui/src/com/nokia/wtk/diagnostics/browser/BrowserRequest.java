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
import com.nokia.wtk.device.DeviceContentRequest;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */

public class BrowserRequest extends BrowserData implements DeviceContentRequest {

  public BrowserRequest( String url, String method, String headers, byte [] data ) {

    // first invoke the BrowserData constructor
    // pass null for contentType and encoding, they will be
    // extracted from the headers if any data exists.
    super (null, null, headers, data);
    this.url = url;
    this.method = method;
  }

  public String getLocation() {
    return url;
  }

  public String getMethod() {
    return method;
  }


  private void writeObject( ObjectOutputStream out ) throws IOException {
    out.defaultWriteObject();
    serializeHeaders( out );
    serializeData( out );
  }

  private void readObject( ObjectInputStream in ) throws IOException,
    ClassNotFoundException {
    in.defaultReadObject();
    deserializeHeaders( in );
    deserializeData( in );
  }

  private String url;
  private String method;
}
