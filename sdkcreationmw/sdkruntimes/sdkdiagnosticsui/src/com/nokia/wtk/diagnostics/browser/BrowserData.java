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
import java.io.Serializable;

/* java.util */
import java.util.StringTokenizer;

/* com.nokia.wtk.device */
import com.nokia.wtk.device.DeviceContentData;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.HTTPHeaderNames;


/**
 * <p>Title: BrowserData</p>
 * <p>Description: BrowserData is an abstract, Serializable class that is the
 * common parent of the three browser content related data classes.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
abstract class BrowserData implements
                            DeviceContentData,
                            Serializable,
                            HTTPHeaderNames {

  /**
   * Returns the headers for this BrowserData instance as a two dimensional
   * array of String.
   * @return a two dimensional array of Strings. The second array is name and
   * value pairings.
   */
  public String [][] getHeaders() {
    return headers;
  }

  /**
   * Returns the headers for this BrowserData instance as a String of the format:
   * <code>name:value0xAname2:value20xA</code>
   * @return returns the headers as a String.
   */
  public String getHeadersAsString() {
    String [][] hdrs = getHeaders();
    StringBuffer buf = new StringBuffer();
    if ( hdrs != null ) {
      for ( int i = 0; i < hdrs.length; i++ ) {
        buf.append( hdrs[i][NAME_INDEX] );
        buf.append( (char)PAIR_DELIMITER );
        buf.append( hdrs[i][VALUE_INDEX] );
        buf.append( '\n' );
      }
    }

    return buf.toString();
  }

  /**
   * Returns the data for this BrowserData instance as a byte array.
   * @return the data for this BrowserData instance as a byte array.
   */
  public byte [] getData() {
    return data;
  }

  /**
   * Gets the content type
   * @return          the content MIME type.
   */
  public String getType() {
      return contentType;
  }

  /**
   * Gets the encoding of the content data
   * @return          the content data encoding.
   */
  public String getEncoding() {
      return encoding;
  }

  /**
   * Returns the number of bytes in the data for this BrowserData instance.
   * @return the number of bytes in the data for this BrowserData instance.
   */
  public int dataSize() {
    return data.length;
  }


  /**
   * Protected constructor
   * @param contentType
   * @param encoding
   * @param headers as a String
   * @param data
   */
  protected BrowserData (String type,
                         String encoding,
                         String headers,
                         byte [] data) {

    setHeaders(headers);
    setDataFields(type,encoding,data);
  }

  /**
   * Protected constructor
   * @param contentType
   * @param encoding
   * @param headers as a double array of Strings
   * @param data
   */
  protected BrowserData (String type,
                         String encoding,
                         String[][] headers,
                         byte [] data) {

    this.headers = headers;
    setDataFields(type,encoding,data);
  }


  protected void setData( byte [] data ) {
    this.data = data;
  }

  protected void setDataFields( String type, String encoding, byte [] data ) {
    this.data = data;
    if (type != null) {
      this.contentType = type;
    }
    else {
      setContentType();
    }
    if (encoding != null) {
      this.encoding = encoding;
    }
    else {
      setEncoding();
    }
  }

  protected void setHeaders( String headers ) {
    if ( headers != null && headers.length() > 0 ) {
      this.headers = parseHeaders( headers );
    } else {
      this.headers = null;
    }
  }

  /**
   * Internal structure is name:value0xAname2:value20xA
   * @param headers
   * @return
   */
  protected String [][] parseHeaders( String headers ) {
    StringTokenizer tokenizer = new StringTokenizer( headers, "\n" );
    String [][] parsedHeaders = new String[tokenizer.countTokens()][SECOND_DIMENSION_SIZE];
    String ele;
    int index, count = 0;
    while ( tokenizer.hasMoreElements() ) {
      ele = (String)tokenizer.nextElement();
      index = ele.indexOf( PAIR_DELIMITER );
      if (index > 0) {
        parsedHeaders[count][NAME_INDEX] = ele.substring(0, index).trim();
        parsedHeaders[count++][VALUE_INDEX] = ele.substring(index + 1).trim();
      }
    }

    return parsedHeaders;
  }


  protected String getHeaderValue (String headerName) {
      String [][] hdrs = getHeaders();
      if (hdrs != null) {
          for (int idx = 0; idx < hdrs.length; idx++) {
              if (hdrs[idx][0].equalsIgnoreCase (headerName)) {
                    return hdrs[idx][1];
              }
          }
      }
      return null;
  }

  /**
   * Sets the value of content-type by deriving it
   * from the content-type header if it exists, or setting
   * it to the default value otherwise.
   */
  protected void setContentType () {
    contentType = getHeaderValue(CONTENT_TYPE);
    if (contentType != null) {
      int separator = contentType.indexOf(';');
      if (separator >= 0) {
        contentType = contentType.substring(0,separator);
      }
      contentType = contentType.trim();
    }
    else {
      contentType = DEFAULT_CONTENT_TYPE;
    }
  }

  /**
  * Deduces charset from the content-type header. Example of what
  * we are parsing here:
  * Content-Type: text/html; charset=iso-8859-4
  */
  protected void setEncoding () {
    encoding = null;
    String hdr = getHeaderValue(CONTENT_TYPE);
    if (hdr != null) {
      String[] hdrFields = StringUtils.tokenize(hdr,";");
      for (int i=1; i<hdrFields.length; i++) {
        String[] paramFields = StringUtils.tokenize(hdrFields[i],"=");
        if (paramFields.length == 2 &&
            paramFields[0].trim().toLowerCase().equals("charset")) {
          encoding = paramFields[1].trim().toLowerCase();
          break;
        }
      }
    }
  }

  protected void serializeHeaders( ObjectOutputStream stream ) throws IOException {
    if ( headers != null ) {
      stream.writeInt( headers.length );
      for ( int i = 0; i < headers.length; i++ ) {
        stream.writeObject( headers[i][NAME_INDEX] );
        stream.writeObject( headers[i][VALUE_INDEX] );
      }
    } else {
      stream.writeInt( -1 );
    }
  }

  protected void serializeData( ObjectOutputStream stream ) throws IOException {
    if ( data != null ) {
      stream.writeInt( data.length );
      // Write and flush part of the byte stream. Found that writing an entire
      // large array failed, deserializing revealed 0's instead of data.
      int pos = 0;
      int count;
      while ( pos < data.length ) {
        count = ( data.length - pos ) > 500 ? 500 : ( data.length - pos ) % 500;
        stream.write( data, pos, count );
        stream.flush();
        pos += count;
      }
    } else {
      stream.writeInt( -1 );
    }
  }

  protected void deserializeHeaders( ObjectInputStream stream ) throws IOException {
    int size = stream.readInt();
    if ( size != -1 ) {
      headers = new String[size][SECOND_DIMENSION_SIZE];
      try {
        for ( int i = 0; i < size; i++ ) {
          headers[i][NAME_INDEX] = (String)stream.readObject();
          headers[i][VALUE_INDEX] = (String)stream.readObject();
        }
      } catch ( ClassNotFoundException e ) {
        // Do something?
      }
    } else {
      headers = null;
    }
  }

  protected void deserializeData( ObjectInputStream stream ) throws IOException {
    int size = stream.readInt();
    if ( size != -1 ) {
      data = new byte[size];
      stream.readFully( data );
    } else {
      data = null;
    }
  }

  private static final int NAME_INDEX = 0;
  private static final int VALUE_INDEX = 1;
  private static final int PAIR_DELIMITER = ':';
  private static final int SECOND_DIMENSION_SIZE = 2;


  /**
  * Default content type as defined in RFC 2616, Section 7.2.1
  */
  protected static final String DEFAULT_CONTENT_TYPE = "application/octet-stream";

  protected String contentType = null;
  protected String encoding = null;

  protected transient byte [] data = null;
  protected transient String [][] headers = null;
}
