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


package com.nokia.mms;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import java.util.Date;

import com.nokia.comm.wap.wps.HeaderCodec;
import com.nokia.comm.wap.wps.WpsException;

import com.nokia.wtk.http.HeaderList;
import com.nokia.wtk.http.MimeMessageParser;

import com.nokia.wtk.util.MimeType;

/**
 * The MMDecoder class decodes an array of bytes representing a Multimedia Message (MM)
 * according with the specification.
 * The class can be used to obtain a MMMessage object from which you can access
 * to each field and content of the MM.
 *
 */

public class MMDecoder implements IMMConstants {

  // Debug print facilities
  private static boolean FLAG_DEBUG = false;

  public static void setDebug(boolean flag) {
    FLAG_DEBUG = flag;
  }

  private MMMessage m_Message = null;
  private int m_i = 0;
  private boolean m_bMultipartRelated = false;
  private boolean m_bMessageAvailable = false;
  private boolean m_bHeaderDecoded = false;
  private byte[] m_In;

  // Unknown content type
  private static final String APP_OCTETS = MimeType.APP_OCTET_STREAM;

  // ------------------------------------------------------------- BASIC RULES
  private long readMultipleByteInt(int length) {
    long value = 0L;
    int start = m_i;
    int end = m_i + length - 1;

    for (int ii = end, weight = 1; ii >= start; ii--, weight *= 256) {
      int bv = unsignedByte(m_In[ii]);
      value  = value + bv*weight;
    }

    m_i = end+1;

    return value;
  }

  /**
   * Read input until we hit a 0x0 terminator
   * @return String
   */
  private String readTextString() {
    String value = "";

    if (m_In[m_i] == 0x22) {
      // in this case it's a "Quoted-string"
       m_i++;
    }

    while (m_In[m_i] > 0) {
      value = value + (char)m_In[m_i++];
    }

    m_i++;

    return value;
  }

  private int readUintvar() {
    int value = 0;
    int bv = unsignedByte(m_In[m_i]);

    if (bv < 0x80) {
      value = bv;
      m_i++;
    } else { // In this case the format is "Variable Length Unsigned Integer"
      boolean flag = true;
      short count = 0, inc = 0;

      // Count the number of byte needed for the number
      while (flag) {
	flag = (m_In[m_i + count] & 0x80) == 0x80;
	count++;
      }

      inc=count;
      count--;

      int weight = 1;
      while (count >= 0) {
	bv = decodeByte(m_In[m_i + count]) * weight;
        weight *= 128;
        value = value + bv;
	count--;
      }
      m_i += inc;
    }

    return value;
  }

  private int readValueLength() {
    int length = 0;
    int temp = m_In[m_i++];

    if (temp < 31) {
      length = temp;
    } else
    if (temp == 31) {
      length = readUintvar();
    }

    return length;
  }


  /*
   * Read current byte and return content type string
   * @see WSP Specification
   */
  private String readWellKnownMedia() {
    String value="";
    switch (decodeByte(m_In[m_i])) {

      case 0x00: value= "*/*"; break;
      case 0x01: value= "text/*"; break;
      case 0x02: value= "text/html"; break;
      case 0x03: value= "text/plain"; break;
      case 0x04: value= "text/x-hdml"; break;
      case 0x05: value= "text/x-ttml"; break;
      case 0x06: value= "text/x-vCalendar"; break;
      case 0x07: value= "text/x-vCard"; break;
      case 0x08: value= "text/vnd.wap.wml"; break;
      case 0x09: value= "text/vnd.wap.wmlscript"; break;
      case 0x0A: value= "text/vnd.wap.channel"; break;
      case 0x0B: value= "multipart/*"; break;
      case 0x0C: value= "multipart/mixed"; break;
      case 0x0D: value= "multipart/form-data"; break;
      case 0x0E: value= "multipart/byteranges"; break;
      case 0x0F: value= "multipart/alternative"; break;
      case 0x10: value= "application/*"; break;
      case 0x11: value= "application/java-vm"; break;
      case 0x12: value= "application/x-www-form-urlencoded"; break;
      case 0x13: value= "application/x-hdmlc"; break;
      case 0x14: value= "application/vnd.wap.wmlc"; break;
      case 0x15: value= "application/vnd.wap.wmlscriptc"; break;
      case 0x16: value= "application/vnd.wap.channelc"; break;
      case 0x17: value= "application/vnd.wap.uaprof"; break;
      case 0x18: value= "application/vnd.wap.wtls-ca-certificate"; break;
      case 0x19: value= "application/vnd.wap.wtls-user-certificate"; break;
      case 0x1A: value= "application/x-x509-ca-cert"; break;
      case 0x1B: value= "application/x-x509-user-cert"; break;
      case 0x1C: value= "image/*"; break;
      case 0x1D: value= "image/gif"; break;
      case 0x1E: value= "image/jpeg"; break;
      case 0x1F: value= "image/tiff"; break;
      case 0x20: value= "image/png"; break;
      case 0x21: value= "image/vnd.wap.wbmp"; break;
      case 0x22: value= "application/vnd.wap.multipart.*"; break;
      case 0x23: value= "application/vnd.wap.multipart.mixed"; break;
      case 0x24: value= "application/vnd.wap.multipart.form-data"; break;
      case 0x25: value= "application/vnd.wap.multipart.byteranges"; break;
      case 0x26: value= "application/vnd.wap.multipart.alternative"; break;
      case 0x27: value= "application/xml"; break;
      case 0x28: value= "text/xml"; break;
      case 0x29: value= "application/vnd.wap.wbxml"; break;
      case 0x2A: value= "application/x-x968-cross-cert"; break;
      case 0x2B: value= "application/x-x968-ca-cert"; break;
      case 0x2C: value= "application/x-x968-user-cert"; break;
      case 0x2D: value= "text/vnd.wap.si"; break;
      case 0x2E: value= "application/vnd.wap.sic"; break;
      case 0x2F: value= "text/vnd.wap.sl"; break;
      case 0x30: value= "application/vnd.wap.slc"; break;
      case 0x31: value= "text/vnd.wap.co"; break;
      case 0x32: value= "application/vnd.wap.coc"; break;
      case 0x33: value= "application/vnd.wap.multipart.related";
               m_bMultipartRelated = true;
               break;
      case 0x34: value= "application/vnd.wap.sia"; break;
      case 0x35: value= "text/vnd.wap.connectivity-xml"; break;
      case 0x36: value= "application/vnd.wap.connectivity-wbxml"; break;

    }
    m_i++;

    return value;
  }


  // ------------------------------------------------------- MMS Header Encoding

  /**
   * Read content type value for the Multipart envelope headers.  This handles the
   * 'start' and 'type' parameters for multipart/related.
   */
  private String readContentTypeValue() {
    int bv = unsignedByte(m_In[m_i]);
    String value = "";

    if (bv >= 0x80) { /* Constrained-media - Short Integer*/
      //
      // Short-integer: the assigned number of the well-known encoding is
      // small enough to fit into Short-integer
      //
      value = readWellKnownMedia();
    }
    else /* Constrained-media - Extension-media*/
    if (bv >= 0x20 && bv < 0x80) {
      value = readTextString();
    }
    else /* Content-general-form */
    if (bv < 0x20) {
      int valueLength = readValueLength();
      bv = unsignedByte(m_In[m_i]);
      if (bv >= 0x80) { //Well-known-media
	    int i2 = m_i;
        value = readWellKnownMedia();
        if (value.equals("application/vnd.wap.multipart.related")) {
          //
          // Decode presentation type and start, in either order
          //
          bv = decodeByte(m_In[m_i]);
          if (bv == WKPA_TYPE) { // Type of the multipart/related
            m_i++;
            m_Message.setMultipartRelatedType( readTextString() );
            bv = decodeByte(m_In[m_i]);
            if (bv == WKPA_START) { // Start (it is the pointer to the presentation part)
              m_i++;
	      m_Message.setPresentationId( readTextString() );
            }
          } else if (bv == WKPA_START) { // Start (it is the pointer to the presentation part)
            m_i++;
	    m_Message.setPresentationId( readTextString() );
            bv = decodeByte(m_In[m_i]);
            if (bv == WKPA_TYPE) { // Type of the multipart/related
              m_i++;
              m_Message.setMultipartRelatedType( readTextString() );
            }
          }
        }
	m_i = i2 + valueLength;
      } else {
	int i2 = m_i;
        value = readTextString();
	m_i = i2 + valueLength;
      }
    }
    return(value);
  }

  /*
   * Decode WSP headers in a Multipart part (Content-Type is always first).
   * Use the WSP HeaderCodec to decode parameters like 'charset'.
   */
  private String readHeaders(int len) {
      byte[] hdr = new byte[len];
      for (int i = 0; i < len; i++) {
          hdr[i] = m_In[m_i + i];
      }
      if (FLAG_DEBUG) {
          StringBuffer sb = new StringBuffer();
          for (int i = 0; i < len; i++) sb.append( (Integer.toHexString((0xff & hdr[i]))) + " ");
          System.out.println("MMSDECODER: DECODING " + len + " HEADER Bytes: " + sb);
      }
      HeaderCodec hc = new HeaderCodec();
      String hdrs = null;
      try { hdrs = hc.decode(hdr, HeaderCodec.OPT_CONTENT_TYPE_FIRST); }
      catch (WpsException e) {
          if (FLAG_DEBUG) System.out.println("CAN'T DECODE HEADERS; TRY OPT_NONE..");
          try { hdrs = hc.decode(hdr, HeaderCodec.OPT_NONE); }
          catch (WpsException ex2) {
             if (FLAG_DEBUG) System.out.println("NOPE. CAN'T DECODE HEADERS");
             return null;
          }
      }
      if (FLAG_DEBUG) System.out.println("DECODED HEADERS: " + hdrs);
      return hdrs;
  }

  /**
   * Find Content-Type string.  Defaults to application/octet-stream on error.
   * @param headers HeaderList to search
   */
  String getPartContentType(HeaderList headers) {
    String ct = headers.getHeaderValue(MimeType.CONTENT_TYPE_HEADER);
    if (ct == null) {
      ct = APP_OCTETS;
    }
    return ct;
  }

  /**
   * Find Content-ID string.
   * @param headers HeaderList to search
   * @param default value to use if not found
   */
  String getContentID(HeaderList headers, String defaultVal) {
    String cid = headers.getHeaderValue(MimeType.CONTENT_ID_HEADER);
    if (cid == null) {
      cid = headers.getHeaderValue(MimeType.CONTENT_LOCATION_HEADER);
    }
    if (cid == null) {
      cid = defaultVal;
    }
    return cid;
  }

  //
  // Debugging string formatting method
  //
  private String format(byte[] arr) {
    int len = arr.length;
    StringBuffer sb = new StringBuffer();
    sb.append("Headers (" + len + " bytes: ");
    for (int i = 0; i < len; i++)
      sb.append(Integer.toHexString((int)(arr[i] & 0x7F)) + " ");
    return sb.toString();
  }

  // ------------------------------------------------------------------ MMS Body
  //
  private void readMMBodyMultiPart() {
    int n = 0;
    int c_headerLen = 0, c_dataLen = 0;
    String c_type = "";
    String c_id = "";
    String c_headers = null;
    byte [] c_buf;
    int nEntries = m_In[m_i++];

    if (FLAG_DEBUG) System.out.println("PART_COUNT " + nEntries);

    while (n < nEntries) {
      MMContent mmc = new MMContent();

      // read length bytes
      c_headerLen = readUintvar();
      c_dataLen = readUintvar();

      // read and decode headers, save string
      c_headers = readHeaders(c_headerLen);
      m_i += c_headerLen;

      // Sanity check
      if (m_i > m_In.length) {
        m_i = m_In.length - 1;
      }

      // default values
      c_id = "A" + n;
      c_type = APP_OCTETS;

      if (c_headers == null) {
        if (FLAG_DEBUG) System.out.println("BAD HEADERS IN PART " + n);
      } else {
        HeaderList hl = new HeaderList();
        try { MimeMessageParser.parseHeaders(c_headers, hl); } catch (Exception ex) {}
        c_type = getPartContentType(hl);
        c_id = getContentID(hl, c_id);
      }

      // Set values on MMContent
      mmc.setType(c_type);
      mmc.setContentId(c_id);
      mmc.setContent(m_In, m_i, c_dataLen);
      mmc.setHeaderString(c_headers);
      m_Message.addContent(mmc);
      m_i += c_dataLen;
      if (m_i > m_In.length) {
      	break;
      }
      n++;
      if (FLAG_DEBUG) System.out.println("PART " + n + ": " + c_type + " (CID " + c_id + ") " +
        c_dataLen + " bytes");
    }
  }

  private void readMMBodyMultiPartRelated() {
    readMMBodyMultiPart();
  }

  private void readMMBodyMultiPartMixed() {
    readMMBodyMultiPart();
  }

  // ------------------------------------------------------------------ MMS Body
  // The original Part readers.  These only handle content-type,
  // content-id, and content-location headers.  They also don't do char encoding.
  //
  private void oldReadMMBodyMultiPartRelated() {
    int n=0;
    int c_headerLen=0,c_dataLen=0;
    String c_type="",c_id="";
    byte [] c_buf;
    int nEntries = m_In[m_i++];

    while (n<nEntries) {
      c_headerLen=readUintvar();
      c_dataLen=readUintvar();
      int freeze_i=m_i;
      c_type=readContentTypeValue();
      int c_typeLen=m_i-freeze_i;

      c_id="A"+n;
      while (m_i < freeze_i + c_headerLen) {
        if ((decodeByte(m_In[m_i])==HFN_CONTENT_LOCATION) ||
           (decodeByte(m_In[m_i])==HFN_CONTENT_ID) ) {
            m_i++;
	    c_id = readTextString();
        }
      }

      MMContent mmc=new MMContent();
      mmc.setType(c_type);
      mmc.setContentId(c_id);
      mmc.setContent(m_In,m_i,c_dataLen);
      m_Message.addContent(mmc);
      m_i+=c_dataLen;
      n++;
    }
  }

  private void oldReadMMBodyMultiPartMixed() {
    int n=0;
    int c_headerLen=0,c_dataLen=0;
    String c_type="",c_id="";
    byte [] c_buf;
    int nEntries = m_In[m_i++];

    while (n<nEntries) {
      c_headerLen=readUintvar();
      c_dataLen=readUintvar();
      c_type=readContentTypeValue();
      c_id="A"+n;
      if (unsignedByte(m_In[m_i])==0x8E) {
	    m_i++;
        c_id=readTextString();
      }

      if (FLAG_DEBUG) System.out.println("c_type=(" + c_type + ") c_headerLen=(" + c_headerLen +
         ") c_dataLen=(" + c_dataLen + ") c_id=(" + c_id+")");

      MMContent mmc=new MMContent();
      mmc.setType(c_type);
      mmc.setContentId(c_id);
      mmc.setContent(m_In,m_i,c_dataLen);
      m_Message.addContent(mmc);
      m_i+=c_dataLen;
      n++;
    }
  }

  // ----------------------------------------------------------------
  // Main routine that decodes MMS Headers
  // ----------------------------------------------------------------
  private void readMMHeader() {
    boolean flag_continue = true;

    while (flag_continue && m_i < m_In.length) {
      byte currentByte = decodeByte(m_In[m_i++]);

      // System.out.println("[Byte " + m_i + "] " + Integer.toHexString((int)currentByte));

      switch (currentByte) {
         case FN_MESSAGE_TYPE:
             m_Message.setMessageType(m_In[m_i]);
             if (FLAG_DEBUG) System.out.println("FN_MESSAGE_TYPE (0C): " +
                                   m_Message.getMessageType());
             m_i++;
             break;
         case FN_TRANSACTION_ID:
             m_Message.setTransactionId(readTextString());
             if (FLAG_DEBUG) System.out.println("FN_TRANSACTION_ID (18): " +
                                   m_Message.getTransactionId());
             break;
         case FN_MESSAGE_ID:
             m_Message.setMessageId(readTextString());
             if (FLAG_DEBUG) System.out.println("FN_MESSAGE_ID (0B): " +
                                   m_Message.getMessageId());
             break;
         case FN_STATUS:
             if (FLAG_DEBUG) System.out.println("FN_STATUS (15)");
             m_Message.setMessageStatus(m_In[m_i]);
             m_i++;
             break;
         case FN_MMS_VERSION:
             if (FLAG_DEBUG) System.out.println("FN_MMS_VERSION (0D)");
	     m_Message.setVersion(m_In[m_i]);
             m_i++;
             break;
         case FN_TO:
             if (FLAG_DEBUG) System.out.println("FN_TO (17)");
	     m_Message.addToAddress(new String (readTextString()) );
             break;
         case FN_CC:
             if (FLAG_DEBUG) System.out.println("FN_CC (02)");
	     m_Message.addCcAddress(new String (readTextString()) );
             break;
         case FN_BCC:
             if (FLAG_DEBUG) System.out.println("FN_BCC (01)");
	     m_Message.addBccAddress(new String (readTextString()) );
             break;

         case FN_DATE:
             {
             if (FLAG_DEBUG) System.out.println("FN_DATE (05)");
	     int length = m_In[m_i++];
             long msecs = readMultipleByteInt(length)*1000;
             m_Message.setDate( new Date(msecs) );
             }
             break;

        case FN_DELIVERY_REPORT:
             {
             int value = unsignedByte(m_In[m_i++]);
             if (value==0x80)
                m_Message.setDeliveryReport(true);
             else
                m_Message.setDeliveryReport(false);
    	     if (FLAG_DEBUG) System.out.println("FN_DELIVERY_REPORT (06): " +
                                    m_Message.getDeliveryReport());
             break;
             }
         case FN_SENDER_VISIBILITY:
             m_Message.setSenderVisibility(m_In[m_i]);
             if (FLAG_DEBUG) System.out.println("FN_SENDER_VIS (14): " +
                                    m_Message.getSenderVisibility());
             m_i++;
             break;
        case FN_READ_REPLY:
             {
             int value = unsignedByte(m_In[m_i++]);
             if (value == 0x80)
                m_Message.setReadReply(true);
             else
                m_Message.setReadReply(false);
             if (FLAG_DEBUG) System.out.println("FN_READ_REPLY (10)" +
                                    m_Message.getReadReply());
             break;
             }

        case FN_FROM:
	     {
	      int valueLength = readValueLength();
	      int addressPresentToken = unsignedByte(m_In[m_i++]);
	      if (addressPresentToken == 0x80) { // Address-present-token
                m_Message.setFrom(new String (readTextString()) );
	      }
              if (FLAG_DEBUG) System.out.println("FN_FROM (09): " +
                                    m_Message.getFrom().getAddress());
	     }
             break;
        case FN_SUBJECT:
             {
             m_Message.setSubject( readTextString() );
             if (FLAG_DEBUG) System.out.println("FN_SUBJECT (16): " +
                                    m_Message.getSubject());
	     }
             break;
        case FN_MESSAGE_CLASS:
             {
             m_Message.setMessageClass(m_In[m_i++]);
             if (FLAG_DEBUG) System.out.println("FN_MESSAGE_CLASS (0A): " +
                                    m_Message.getMessageClass());
	     }
             break;
        case FN_EXPIRY:
             {
             if (FLAG_DEBUG) System.out.println("FN_EXPIRY (08)");
             int valueLength = readValueLength();
             int tokenType = unsignedByte(m_In[m_i++]);
	     long expiry = 0;

             if (tokenType == 128) { // Absolute-token
               int length=m_In[m_i++];
               expiry=readMultipleByteInt(length)*1000;
	           m_Message.setExpiryAbsolute(true);
             }

             if (tokenType==129) { // Relative-token
	           m_Message.setExpiryAbsolute(false);
               // Read the Delta-seconds-value
               if (valueLength>3) { // Long Integer
                 int length=m_In[m_i++];
                 expiry=readMultipleByteInt(length)*1000;
               }
               else { // Short Integhet (1 OCTECT)
                 int b=m_In[m_i]&0x7F;
                 expiry=b*1000;
                 m_i++;
               }
              }
	          m_Message.setExpiry( new Date(expiry) );
             }
             break;
        case FN_DELIVERY_TIME:
             {
             if (FLAG_DEBUG) System.out.println("FN_DELIVERY_TIME (07)");
             int valueLength=readValueLength();
             int tokenType=unsignedByte(m_In[m_i++]);
	     long deliveryTime=0;

             if (tokenType==128) { // Absolute-token
	       m_Message.setDeliveryTimeAbsolute(true);
               int length=m_In[m_i++];
               deliveryTime=readMultipleByteInt(length)*1000;
             }

             if (tokenType==129) { // Relative-token
	       m_Message.setDeliveryTimeAbsolute(false);
               // Read the Delta-seconds-value
               if (valueLength>3) { // Long Integer
                 int length=m_In[m_i++];
                 deliveryTime=readMultipleByteInt(length)*1000;
               }
               else { // Short Integhet (1 OCTECT)
                 int b=m_In[m_i]&0x7F;
                 deliveryTime=b*1000;
                 m_i++;
               }
               }
	      m_Message.setDeliveryTime( new Date(deliveryTime) );
             }
	     break;
        case FN_PRIORITY:
             {
             m_Message.setPriority( m_In[m_i++] );
    	     if (FLAG_DEBUG) System.out.println("FN_PRIORITY (0F): " +
                                    m_Message.getPriority());
             }
             break;
        case FN_CONTENT_TYPE:
             {
             m_Message.setContentType( readContentTypeValue() );
             if (FLAG_DEBUG) System.out.println("FN_CONTENT_TYPE (04): " +
                                    m_Message.getContentType());
             flag_continue = false;
             }
             break;
        case FN_CONTENT_LOCATION:
             {
             m_Message.setContentLocation( readTextString() );
             if (FLAG_DEBUG) System.out.println("FN_CONTENT_LOCATION (03): " +
                                    m_Message.getContentLocation());
	     }
             break;
        default:
           //
           // User defined header
           //
           if (currentByte > 31 && currentByte < 127) {
               m_i--;
               String name = readTextString();
               String val = readTextString();
               if (FLAG_DEBUG) System.out.println("USER DEFINED HEADER (" + name + ":" + val + ")");
               m_Message.addUserDefinedHeader(name, val);
           } else {
               if (FLAG_DEBUG) System.out.println("UNKNOWN HEADER TOKEN: " + currentByte);
           }
           break;
      }
    }
  }

  private byte decodeByte(byte value) {
    return ( (byte)(value & 0x7F) );
  }

  private int unsignedByte(byte value) {
    if (value<0) {
      return (value+256);
    } else
      return value;
  }

 /**
  * Resets the Decoder object.
  *
  */
  public void reset() {
    m_Message = null;
    m_bMultipartRelated = false;
    m_bMessageAvailable = false;
    m_bHeaderDecoded = false;
    m_In = null;
    m_i = 0;
  }

 /**
  * Sets the buffer representing the Multimedia Message to be decoded.
  */
  public void setMessage(byte buf[]) {
    m_Message = new MMMessage();
    m_In = buf;
    m_bMessageAvailable = true;
  }

 /**
  * Decodes the header of the Multimedia Message. After calling this method
  * a MMMessage object, containing just the information related to the header and
  * without the contents, can be obtained by calling getMessage().
  * This method has to be called after setMessage(byte buf[).
  */
  public void decodeHeader() throws MMDecoderException {
    if (m_bMessageAvailable) {
      readMMHeader();
      m_bHeaderDecoded = true;
    } else {
      throw new MMDecoderException("Message unavailable. You must call " +
        "setMessage(byte[] buf) before calling this method.");
    }
  }

 /**
  * Decodes the body of the Multimedia Message. This method has to be called
  * after the method decodeHeader()
  */
  public void decodeBody() throws MMDecoderException {
    if (!m_bHeaderDecoded)
      throw new MMDecoderException("You must call decodeHeader() before calling decodeBody()");

    if ((m_Message.getContentType()).compareTo("application/vnd.wap.multipart.related") == 0)
       readMMBodyMultiPartRelated();
    else
       readMMBodyMultiPartMixed();
  }

 /**
  * Decodes the whole Multimedia Message. After calling this method
  * a MMMessage object, containing the information related to the header and
  * the all contents, can be obtained by calling the method getMessage().
  * This method has to be called after setMessage(byte buf[).
  */
  public void decodeMessage() throws MMDecoderException {
    decodeHeader();
    if (m_Message.getMessageType() == IMMConstants.MESSAGE_TYPE_M_SEND_REQ ||
        m_Message.getMessageType() == IMMConstants.MESSAGE_TYPE_M_RETRIEVE_CONF) {
      decodeBody();
    }
  }

  public MMDecoder(byte buf[]) {
    setMessage(buf);
  }

  public MMDecoder() {
  }

 /**
  * Retrieves the MMMessage object. This method has to be called after the calling
  * of methods decodeMessage() or decodeHeader().
  *
  * @return An object representing the decoded Multimedia Message
  */
  public MMMessage getMessage() {
    if (m_bHeaderDecoded)
      return m_Message;
   else
      return null;
  }

  public static void main(String args[]) {
    if (args.length == 0) {
      System.out.println("Usage: MMDecoder <file>");
      System.exit(0);
    }
    setDebug(true);

    File f = new File(args[0]);
    byte[] mmfile = new byte[(int)f.length()];
    try {
      FileInputStream fis = new FileInputStream(f);
      fis.read(mmfile);
      fis.close();
    } catch (IOException ex1) {
      System.out.println("IOexception: " + ex1);
      System.exit(1);
    }
    MMDecoder decoder = new MMDecoder(mmfile);
    try {
      decoder.decodeMessage();
    } catch (MMDecoderException ex) {
      System.out.println("Decoder exception: " + ex);
      System.exit(1);
    }
    System.out.println("Decoded " + args[0] + " OK");
    System.exit(0);
  }
}
