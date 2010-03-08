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


/*
 * Created on Mar 27, 2003
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package com.nokia.wtk.sdkSimulator.data;

//java.io
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

//java.util
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.HashMap;
import java.util.Map;
import java.util.Date;
import java.text.DateFormat;

import com.nokia.wtk.device.DeviceMessageFragment;

/**
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class SMSMessageFragment implements Serializable, DeviceMessageFragment {

  public SMSMessageFragment() {
  }

  public SMSMessageFragment( String smsc, byte [] pdu ) {
    this.pdu = pdu;
    setMessageCenterAddress( smsc );
  }

  public SMSMessageFragment( DeviceMessageFragment frag ) {
    this.pdu = frag.getPDU();
    this.data = frag.getData();
    this.sourceAddress = frag.getSourceAddress();
    this.smsc = frag.getMessageCenterAddress();
    Map map = frag.getHeaders();
    Iterator iterator = map.keySet().iterator();
    while ( iterator.hasNext() ) {
      Object key = iterator.next();
      headers.put( key, map.get( key ) );
    }
    this.fragmentIndex = frag.getIndexInMessage();
    this.messageLength = frag.getMessageLength();
  }

  /**
   * Add a header to this fragment.
   * @param headerID
   * @param value
   */
  public void addHeader( int headerID, String value ) {
    if ( headerID == INDEX_ID_SHORT || headerID == INDEX_ID_LONG) {
      int spIndex = value.indexOf( ' ' );
      int openIndex = value.indexOf( '(' );
      int closeIndex = value.indexOf( ')' );
      if ( spIndex != -1 && openIndex != -1 && closeIndex != -1 ) {
        fragmentIndex = Integer.parseInt( value.substring( 0, spIndex ) );
        messageLength = Integer.parseInt( value.substring( openIndex + 1,
          closeIndex ) );
      }
    }
    // if this is the timestamp header, store it in the timestamp
    // field as a Date object, and as a formatted string in the header
    if (headerID == SMS_TP_SCTS) {
      try {
        long longTime = Long.parseLong(value);
        timestamp = new Date (longTime);
      } catch (NumberFormatException e) {
        System.err.println ("invalid timestamp received, defaulting to current time");
        timestamp = new Date();
      }
      DateFormat df = DateFormat.getInstance();
//      df.setTimeZone(TimeZone.getTimeZone("GMT-05"));
      value = df.format(timestamp);
    }

    headers.put( String.valueOf( headerID ), value );
    if (headerID == SMS_TP_OA) {
      setSourceAddress (value);
    }
  }

  public void setSourceAddress( String address ) {
    sourceAddress = address;
  }

  public void setMessageCenterAddress( String smsc ) {
    this.smsc = smsc;
  }

  /**
   * Sets the data for a binary fragment.
   * @param data
   */
  public void setData( byte [] data ) {
    //temp
    //System.err.println ("in setData (byte[])");
    this.data = data;
  }

  /**
   * Sets the data for a text-based fragment.
   * @param data
   */
  public void setData( String data ) {
    //temp
    //System.err.println ("in setData (String)");
    this.data = data;
  }

  public String getMessageType() {
    return (String)headers.get( String.valueOf( SMS_TP_MTI ) );
  }


  public String getSourceAddress() {
    return sourceAddress;
  }

  public String getMessageCenterAddress() {
    return smsc;
  }

  public int getIndexInMessage() {
    if (fragmentIndex == 0) {
      return 1;
    }
    else {
      return fragmentIndex;
    }
  }

  public int getMessageLength() {
    if (messageLength == 0) {
      return 1;
    }
    else {
      return messageLength;
    }
  }

  public String getDestination() {
    return (String)headers.get( String.valueOf( SMS_TP_DA ) );
  }

  public String getFormattedTimestamp() {
    return (String)headers.get( String.valueOf( SMS_TP_SCTS ) );
  }

  public Date getTimestamp() {
    return timestamp;
  }

  public byte [] getPDU() {
    return pdu;
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.device.DeviceMessageFragment#getHeader(java.lang.String)
   */
  public String getHeader( String name ) {
    return (String)headers.get( name );
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.device.DeviceMessageFragment#getHeaderName(java.lang.String)
   */
  public String getHeaderName( String key ) {
    String name = null;
    if (key.equals(String.valueOf(INDEX_ID_SHORT)) ||
        key.equals(String.valueOf(INDEX_ID_LONG))) {
      name = INDEX_ID_STR;
    }
    else if (key.equals(String.valueOf(PORT_NUMBER_SHORT)) ||
        key.equals(String.valueOf(PORT_NUMBER_LONG))) {
      name = PORT_NUMBER_STR;
    }
    else {
      int num= Integer.parseInt( key );
      if ( num <= headerNames.length ) {
        name = headerNames[ num - 1];
      }
    }
    return name;
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.device.DeviceMessageFragment#getHeaders()
   */
  public Map getHeaders() {
    return headers;
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.device.DeviceMessageFragment#getData()
   */
  public Object getData() {
    return data;
  }

 public String toString() {

    StringBuffer buf = new StringBuffer();
    buf.append("PDU length: " + pdu.length + "\r\n");
    buf.append( "PDU byte array:\r\n" );
    for ( int i = 0; i < pdu.length; i++ ) {
      buf.append( Integer.toHexString(pdu[i] & 0xFF) );
      buf.append( " " );
      if ( i % 10 == 0 ) {
        buf.append( "\r\n" );
      }
    }
    buf.append( "\r\nSMSC: " + smsc );
    buf.append( "\r\nSource Address: " + getSourceAddress() );
    buf.append( "\r\nDestination: " + getDestination());
    buf.append( "\r\nMessageType: " + getMessageType());
    buf.append( "\r\nMessageLength: " + getMessageLength());
    buf.append( "\r\nIndexInMessage: " + getIndexInMessage());
    buf.append( "\r\nHeaders:\n" );
    java.util.Iterator iterator = headers.keySet().iterator();
    String name = "??";
    while ( iterator.hasNext() ) {
      String key = (String)iterator.next();
      int num = Integer.parseInt( key);
      if (num == INDEX_ID_SHORT ||
          num == INDEX_ID_LONG) {
        name = INDEX_ID_STR;
      }
      else if (num == PORT_NUMBER_SHORT ||
          num == PORT_NUMBER_LONG) {
        name = PORT_NUMBER_STR;
      }
      else {
        if ( num <= headerNames.length ) {
          name = headerNames[ num - 1];
        }
        else {
          name = "??";
        }
      }
      buf.append( "\t" + key + "(" + name + ") = " + (String)headers.get( key ) + "\r\n" );
    }
    buf.append( "Data type is: " );
    buf.append( data.getClass().getName() );
    buf.append( "\r\n" );
    return buf.toString();
  }


  protected void serializeData( ObjectOutputStream stream ) throws IOException {
    if ( data != null ) {
      if ( data instanceof String ) {
        stream.writeInt( DATA_IS_STRING );
        stream.writeObject( data );
      } else {
        stream.writeInt( DATA_IS_BYTE_ARRAY );
        serializeByteArray( (byte [])data, stream );
      }
    } else {
      stream.writeInt( DATA_NOT_AVAILABLE );
    }
  }

  protected void deserializeData( ObjectInputStream stream ) throws IOException {
    int type = stream.readInt();
    if ( type != DATA_NOT_AVAILABLE ) {
      if ( type == DATA_IS_STRING ) {
        try {
          data = stream.readObject();
        } catch ( ClassNotFoundException e ) {
          // Safe to assume java.lang.String will be found
        }
      } else {
        data = deserializeByteArray( stream );
      }
    } else {
      data = null;
    }
  }

  protected void serializeByteArray( byte [] arr, ObjectOutputStream stream )
  throws IOException {
    if ( arr != null ) {
      stream.writeInt( arr.length );
      // Write and flush part of the byte stream. Found that writing an entire
      // large array failed, deserialzing revealed 0's instead of data.
      int pos = 0;
      int count;
      while( pos < arr.length ) {
        count = ( arr.length - pos ) > 500 ? 500 : ( arr.length - pos ) % 500;
        stream.write( arr, pos, count );
        stream.flush();
        pos += count;
      }
    } else {
      stream.writeInt( DATA_NOT_AVAILABLE );
    }
  }

  protected byte [] deserializeByteArray( ObjectInputStream stream ) throws
  IOException {
    byte [] arr = null;
    int size = stream.readInt();
    if ( size != DATA_NOT_AVAILABLE ) {
      arr = new byte[size];
      stream.readFully( arr );
    }

    return arr;
  }

  private void writeObject( ObjectOutputStream out ) throws IOException {
    out.defaultWriteObject();
    serializeByteArray( pdu, out );
    serializeData( out );
  }

  private void readObject( ObjectInputStream in ) throws IOException,
    ClassNotFoundException {
    in.defaultReadObject();
    pdu = deserializeByteArray( in );
    deserializeData( in );
  }

  /* Message Type Indicator; Values: "SMS-SUBMIT" */
  private static final int SMS_TP_MTI = 1;
  private static final String SMS_TP_MTI_STR = "Message Type Indicator";

  /* Reject duplicates; Values: "True" or "False" */
  private static final int SMS_TP_RD = 2;
  private static final String SMS_TP_RD_STR = "Reject duplicates";

  /* Reply path; Values: "True" or "False" */
  private static final int SMS_TP_RP = 3;
  private static final String SMS_TP_RP_STR = "Reply path";

  /* Status report request; Values: "True" or "False" */
  private static final int SMS_TP_SRR = 4;
  private static final String SMS_TP_SRR_STR = "Status report request";

  /* Message reference; Example value: "2" */
  private static final int SMS_TP_MR = 5;
  private static final String SMS_TP_MR_STR = "Message reference";

  /* Destination Address; Example value: "+17813547719" */
  private static final int SMS_TP_DA = 6;
  private static final String SMS_TP_DA_STR = "Destination Address";

  /* Protocol identifier; Values: "Text", "E-mail", "Fax" */
  private static final int SMS_TP_PID = 7;
  private static final String SMS_TP_PID_STR = "Protocol identifier";

  /* Data coding scheme; Values: "7 bit", "8 bit", "Unicode" */
  private static final int SMS_TP_DCS = 8;
  private static final String SMS_TP_DCS_STR = "Data coding scheme";

  /* Validity period relative: Example values: "5 minutes", "12 hours", "1 day", "1 week" */
  private static final int SMS_TP_VPR = 9;
  private static final String SMS_TP_VPR_STR = "Validity period relative";

  /* Validity period absolute: Example values: "Mar 13 2003 2:05:22 UTC" */
  private static final int SMS_TP_VPA = 10;
  private static final String SMS_TP_VPA_STR = "Validity period absolute";

  /* Indicates whether or not the TP-VP field is present */
  private static final int SMS_TP_VP = 11;
  private static final String SMS_TP_VP_STR = "Validity period present";

  /* Indicates whether the TP-UD field contains a header */
  private static final int SMS_TP_UDHI = 12;
  private static final String SMS_TP_UDHI_STR = "Header present in TP-UD";

  /* Indicates whether or not there are more messages to send */
  private static final int SMS_TP_MMS = 13;
  private static final String SMS_TP_MMS_STR = "More messages to send";

  /* Indicates whether the SME has requested a status report */
  private static final int SMS_TP_SRI = 14;
  private static final String SMS_TP_SRI_STR = "Status report indication";

  /* Address of the originating SME */
  private static final int SMS_TP_OA = 15;
  private static final String SMS_TP_OA_STR = "Source address";

    /* Timestamp */
  private static final int SMS_TP_SCTS = 16;
  private static final String SMS_TP_SCTS_STR = "Timestamp";

  private static final int INDEX_ID_SHORT = 100;
  private static final int INDEX_ID_LONG = 108;
  private static final String INDEX_ID_STR = "Index in message";

  private static final int PORT_NUMBER_SHORT = 104;
  private static final int PORT_NUMBER_LONG = 105;
  private static final String PORT_NUMBER_STR = "Port number";

  private static final int DATA_IS_STRING = -1;
  private static final int DATA_IS_BYTE_ARRAY = -2;
  private static final int DATA_NOT_AVAILABLE = -3;

  private static final String [] headerNames = {
    SMS_TP_MTI_STR, SMS_TP_RD_STR, SMS_TP_RP_STR, SMS_TP_SRR_STR, SMS_TP_MR_STR,
    SMS_TP_DA_STR, SMS_TP_PID_STR, SMS_TP_DCS_STR, SMS_TP_VPR_STR, SMS_TP_VPA_STR,
    SMS_TP_VP_STR, SMS_TP_UDHI_STR, SMS_TP_MMS_STR, SMS_TP_SRI_STR, SMS_TP_OA_STR,
    SMS_TP_SCTS_STR
  };

  protected int fragmentIndex;
  protected int messageLength;
  protected String smsc;
  protected String sourceAddress;
  protected Date timestamp;
  protected transient Object data;
  protected HashMap headers = new LinkedHashMap();
  protected transient byte [] pdu;
}
