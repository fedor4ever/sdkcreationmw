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
public class PushData extends BrowserData
                      implements DeviceContentReceived
{


  /**
   * constructor
   * @param type
   * @param encoding
   * @param headers as a String
   * @param location
   * @param data
   */
  public PushData( String type, String encoding, String headers, String location, byte [] data ) {
    super (type, encoding, headers, data);
    this.location = location;
    this.transactionId = DeviceContentReceived.INVALID_TRANS_ID;
  }

  /**
   * constructor
   * @param type
   * @param encoding
   * @param headers as a String
   * @param data
   */
  public PushData( String type, String encoding, String headers, byte [] data ) {
    this(type, encoding, headers, null, data);
  }

  /**
   * constructor
   * @param contentType
   * @param encoding
   * @param headers as a double array of Strings
   * @param data
   */
  public PushData (String type,
                   String encoding,
                   String[][] headers,
                   byte [] data) {
    super (type, encoding, headers, data);
  }

  /**
   * constructor
   * @param content a DeviceContentReceived implementation
   */
  public PushData( DeviceContentReceived content ) {
    this(content.getType(),
         content.getEncoding(),
         content.getHeaders(),
         content.getData());
    this.location = content.getLocation();
    this.transactionId = DeviceContentReceived.INVALID_TRANS_ID;
  }

  public String getRequestLocation() {
    return null;
  }

  public int getRequestStatus( StringBuffer msgbuf ) {
    return STATUS_NOT_APPLICABLE;
  }

  public String getLocation() {
    return location;
  }

  // not used in this implementation
  public long getTransactionId() {
    return transactionId;
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

  private static final int STATUS_NOT_APPLICABLE = 0;

  private String type;
  private String encoding;
  private String location;
  private long transactionId;
}
