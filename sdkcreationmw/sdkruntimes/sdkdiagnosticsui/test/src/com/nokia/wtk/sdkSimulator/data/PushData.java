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

/* com.nokia.wtk.device */
import com.nokia.wtk.device.DeviceContentReceived;


/**
* class PushData
*
* Represents Pushdata content This class extends the corresponding PushData class
* in Diagnostics, and it is identical to it.
*/
public class PushData extends com.nokia.wtk.diagnostics.browser.PushData {


  /**
   * constructor
   * @param type
   * @param encoding
   * @param headers as a String
   * @param location
   * @param data
   */
  public PushData( String type, String encoding, String headers, String location, byte [] data ) {
    super (type, encoding, headers, location, data);
  }

  /**
   * constructor
   * @param type
   * @param encoding
   * @param headers as a String
   * @param data
   */
  public PushData( String type, String encoding, String headers, byte [] data ) {
    super(type, encoding, headers, data);
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
    super(content.getType(),
          content.getEncoding(),
          content.getHeaders(),
          content.getData());
  }
}
