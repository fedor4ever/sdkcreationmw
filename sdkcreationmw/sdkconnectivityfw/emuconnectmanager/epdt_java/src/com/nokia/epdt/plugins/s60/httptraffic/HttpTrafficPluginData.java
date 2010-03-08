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


package com.nokia.epdt.plugins.s60.httptraffic;

/** com.nokia.wtk.util */
import com.nokia.wtk.util.ArrayUtils;

/* HttpTrafficPluginData represent HTTP data that passed to constructor
 * as a set of bytes in the form:
 * Type           - 1 byte, reflects THTTPEvent
 * Transaction ID - 4 bytes, HTTP transaction ID
 * Proc ID        - 4 bytes, ID of the process that uses HTTP.
 * Status         - 4 bytes, HTTP status, 0 for requests
 * Headers length - 4 bytes
 * Headers        - <Headers length> bytes, HTTP headers
 * Method length  - 4 bytes, length of the method name
 * Method         - <Method Length> bytes, method name
 * URL length     - 4 bytes, length of the request URL
 * URL            - <URL length> bytes, request URL
 * Body length    - 4 bytes
 * Body           - <Body length> bytes, HTTP body.
 */
class HttpTrafficPluginData
{
  public static final int DATA_LEN_SIZE = 4;
  public static final int DATA_TYPE_SIZE = 2;

  public static final short EVENT_DATA_TYPE = 1;
  public static final short TRANS_ID_DATA_TYPE = 2;
  public static final short PROC_ID_DATA_TYPE = 3;
  public static final short STATUS_DATA_TYPE = 4;
  public static final short METHOD_DATA_TYPE = 5;
  public static final short URL_DATA_TYPE = 6;
  public static final short HEADERS_DATA_TYPE = 7;
  public static final short BODY_DATA_TYPE = 8;

  public HttpTrafficPluginData (byte[] data)
  {
    init (data);
  }

  public byte[] getHTTPBody ()
  {
    return _body;
  }

  public String getHTTPHeaders ()
  {
    return _headers;
  }

  public int getHTTPStatus ()
  {
    return _status;
  }

  public String getReqURL ()
  {
    return _reqURL;
  }

  public String getMethod ()
  {
    return _method;
  }

  public byte getEventType ()
  {
    return _type;
  }

  public int getTransId ()
  {
    return _transID;
  }

  public int getProcId ()
  {
    return _procID;
  }


  private void init (byte[] rawData)
  {
    int     offset = 0;
    short   dataType = -1;
    int     dataLen = -1;

    while (offset <= rawData.length) {
      dataType = readShort (rawData, offset);
      dataLen = readInt (rawData, offset + DATA_TYPE_SIZE);
      offset += (DATA_TYPE_SIZE + DATA_LEN_SIZE);

      if (dataType > 0 && dataLen > 0) {
        switch (dataType) {
          case EVENT_DATA_TYPE:
            _type = rawData[offset];
            break;
          case TRANS_ID_DATA_TYPE:
            _transID = readInt (rawData, offset);
            break;
          case PROC_ID_DATA_TYPE:
            _procID = readInt (rawData, offset);
            break;
          case STATUS_DATA_TYPE:
            _status = readInt (rawData, offset);
            break;
          case METHOD_DATA_TYPE:
            _method = new String (rawData, offset, dataLen);
            break;
          case URL_DATA_TYPE:
            _reqURL = new String (rawData, offset, dataLen);
            break;
          case HEADERS_DATA_TYPE:
            _headers = new String (rawData, offset, dataLen);
            break;
          case BODY_DATA_TYPE:
            _body = ArrayUtils.subArray(rawData, offset, offset+dataLen);
            break;
        }
        offset += dataLen;
      }
    }
  }


  private int readInt (byte[] data, int offset)
  {
    int res = Integer.MIN_VALUE;
    if (offset < data.length - 3) {
      res = (((int) data[offset] << 24) |
             (((int) data[offset + 1] & 0xff) << 16) |
             (((int) data[offset + 2] & 0xff) << 8) |
             ((int) data[offset + 3] & 0xff));
    }
    return res;
  }

  private short readShort (byte[] data, int offset)
  {
    short res = Short.MIN_VALUE;
    if (offset < data.length - 1) {
      res = (short) ((((short) data[offset]) << 8) |
                    (((short) data[offset + 1]) & 0xff));
    }
    return res;
  }


  private byte[]    _body;
  private String    _headers;
  private int       _status = -1;
  private String    _method;
  private String    _reqURL;
  private int       _transID = -1;
  private byte      _type = -1;
  private int       _procID = -1;

}
