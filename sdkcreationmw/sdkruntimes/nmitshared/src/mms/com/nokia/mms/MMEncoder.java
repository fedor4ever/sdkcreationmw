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

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Vector;

import com.nokia.comm.wap.wps.HeaderCodec;
import com.nokia.comm.wap.wps.WpsException;

/**
 * The MMEncoder class encodes Multimedia Message object (MMMessage)
 * into an array of bytes according to the specification WAP-209-MMSEncapsulation
 * (WAP Forum).
 *
 */
public class MMEncoder implements IMMConstants {

  private static boolean FLAG_DEBUG = false;

  private MMMessage m_Message;
  private boolean m_bMessageAvailable;
  private boolean m_bMultipartRelated;
  private boolean m_bMessageEcoded;
  private ByteArrayOutputStream m_Out;

  // The content-type header we return on header encoding failure.

  private static String UNKNOWN_CONTENT = "application/octet-stream";
  private static byte[] UNKNOWN_CONTENT_HDR = UNKNOWN_CONTENT.getBytes();


  // Debugging output
  private static void dbgOut(String s) {
      if (FLAG_DEBUG) {
	  System.out.println("MMEncoder: " + s);
      }
  }

  // Format bytes for debugging output
  private static String formatBytes(byte[] b) {
     StringBuffer sb = new StringBuffer("[");
     for (int i = 0; i < b.length; i++) sb.append( (Integer.toHexString((0xff & b[i]))) + " ");
     sb.append("]");
     return sb.toString();
  }

  public MMEncoder() {
    super();
    reset();
  }

 /**
  * Resets the Decoder object.
  *
  */
  public void reset() {
    m_Message = null;
    m_bMultipartRelated = false;
    m_bMessageAvailable = false;
    m_bMessageEcoded = false;
    m_Out = null;
  }

 /**
  * Sets the Multimedia Message to be encoded.
  */
  public void setMessage(MMMessage msg) {
    m_Message = msg;
    m_bMessageAvailable = true;
  }

 /**
  * Retrieve the buffer of byte representing the encoded Multimedia Message.
  * This method has to be called after the calling to encodeMessasge()
  *
  * @return the array of bytes representing the Multmedia Message
  */
  public byte[] getMessage() {
    if (m_bMessageEcoded) {
      return m_Out.toByteArray();
    } else {
      return null;
    }
  }
 /**
  * Encode known content type assignments.
  * List of the content type assignments can be found from WAP-203-WSP, Table 40
  * This version is compliant with Approved version 4-May-2000
  *
  * @return assigned number
  */

  private byte encodeContentType(String sContentType) {
    if (sContentType.equalsIgnoreCase("*/*"))
        return 0x00;
    else
    if (sContentType.equalsIgnoreCase("text/*"))
        return 0x01;
    else
    if (sContentType.equalsIgnoreCase("text/html"))
        return 0x02;
    else
    if (sContentType.equalsIgnoreCase("text/plain"))
        return 0x03;
    else
    if (sContentType.equalsIgnoreCase("text/x-hdml"))
        return 0x04;
    else
    if (sContentType.equalsIgnoreCase("text/x-ttml"))
        return 0x05;
    else
    if (sContentType.equalsIgnoreCase("text/x-vCalendar"))
        return 0x06;
    else
    if (sContentType.equalsIgnoreCase("text/x-vCard"))
        return 0x07;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.wml"))
        return 0x08;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.wmlscript"))
        return 0x09;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.channel"))
        return 0x0A;
    else
    if (sContentType.equalsIgnoreCase("multipart/*"))
        return 0x0B;
    else
    if (sContentType.equalsIgnoreCase("multipart/mixed"))
        return 0x0C;
    else
    if (sContentType.equalsIgnoreCase("multipart/form-data"))
        return 0x0D;
    else
    if (sContentType.equalsIgnoreCase("multipart/byteranges"))
        return 0x0E;
    else
    if (sContentType.equalsIgnoreCase("multipart/alternative"))
        return 0x0F;
    else
    if (sContentType.equalsIgnoreCase("application/*"))
        return 0x10;
    else
    if (sContentType.equalsIgnoreCase("application/java-vm"))
        return 0x11;
    else
    if (sContentType.equalsIgnoreCase("application/x-www-form-urlencoded"))
        return 0x12;
    else
    if (sContentType.equalsIgnoreCase("application/x-hdmlc"))
        return 0x13;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.wmlc"))
        return 0x14;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.wmlscriptc"))
        return 0x15;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.channelc"))
        return 0x16;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.uaprof"))
        return 0x17;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.wtls-ca-certificate"))
        return 0x18;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.wtls-user-certificate"))
        return 0x19;
    else
    if (sContentType.equalsIgnoreCase("application/x-x509-ca-cert"))
        return 0x1A;
    else
    if (sContentType.equalsIgnoreCase("application/x-x509-user-cert"))
        return 0x1B;
    else
    if (sContentType.equalsIgnoreCase("image/*"))
        return 0x1C;
    else
    if (sContentType.equalsIgnoreCase("image/gif"))
        return 0x1D;
    else
    if (sContentType.equalsIgnoreCase("image/jpeg"))
        return 0x1E;
    else
    if (sContentType.equalsIgnoreCase("image/tiff"))
        return 0x1F;
    else
    if (sContentType.equalsIgnoreCase("image/png"))
        return 0x20;
    else
    if (sContentType.equalsIgnoreCase("image/vnd.wap.wbmp"))
        return 0x21;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.multipart.*"))
        return 0x22;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.multipart.mixed"))
        return 0x23;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.multipart.form-data"))
        return 0x24;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.multipart.byteranges"))
        return 0x25;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.multipart.alternative"))
        return 0x26;
    else
    if (sContentType.equalsIgnoreCase("application/xml"))
        return 0x27;
    else
    if (sContentType.equalsIgnoreCase("text/xml"))
        return 0x28;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.wbxml"))
        return 0x29;
    else
    if (sContentType.equalsIgnoreCase("application/x-x968-cross-cert"))
        return 0x2A;
    else
    if (sContentType.equalsIgnoreCase("application/x-x968-ca-cert"))
        return 0x2B;
    else
    if (sContentType.equalsIgnoreCase("application/x-x968-user-cert"))
        return 0x2C;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.si"))
        return 0x2D;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.sic"))
        return 0x2E;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.sl"))
        return 0x2F;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.slc"))
        return 0x30;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.co"))
        return 0x31;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.coc"))
        return 0x32;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.multipart.related"))
        return 0x33;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.sia"))
        return 0x34;
    else
    if (sContentType.equalsIgnoreCase("text/vnd.wap.connectivity-xml"))
        return 0x35;
    else
    if (sContentType.equalsIgnoreCase("application/vnd.wap.connectivity-wbxml"))
        return 0x36;
    else
        return 0;
  }

  private int unsignedByte(byte value) {
    if (value<0) {
      return (value+256);
    } else {
      return value;
    }
  }

  private void writeValueLength(long value) {

    if (value <=30)
      m_Out.write((int)value);
    else {
      m_Out.write(31);
      int data[] = EncodeUintvarNumber(value);
      int numValue;
      for (int i = 1; i <= data[0]; i++) {
        numValue = data[i];
        m_Out.write(numValue);
      }
    }
  }

  private void writeUintvar(long value) {
    int data[] = EncodeUintvarNumber(value);
    int numValue;
    for (int i = 1; i <= data[0]; i++) {
      numValue = data[i];
      m_Out.write(numValue);
    }
  }

  /**
   * Encodes the Multimedia Message set in setMessage()
   */
  public void encodeMessage() throws MMEncoderException {
    m_bMessageEcoded = false;
    m_bMultipartRelated = false;

    if (!m_bMessageAvailable)
        throw new MMEncoderException("No Multimedia Messages set in the encoder");

    try {
      m_Out = new ByteArrayOutputStream();

      if (!m_Message.isMessageTypeAvailable()) {
        m_Out.close();
        throw new MMEncoderException("Invalid Multimedia Message format: no message-type field!");
      }

      byte nMessageType = m_Message.getMessageType();

      switch(nMessageType) {
        default:
            {
            m_Out.close();
            throw new MMEncoderException("Invalid Multimedia Message format: message-type " + nMessageType);
            }

        case MESSAGE_TYPE_M_RETRIEVE_CONF: // ---------------------------- m-retrieve-conf

            // ------------------- MESSAGE TYPE --------
            m_Out.write(FN_MESSAGE_TYPE + 0x80);
            m_Out.write(nMessageType);

            // ------------------- TRANSACTION ID ------
            doTransactionId();

            // ------------------- VERSION -------------
            doVersion();

            // ------------------- MESSAGE ID ----------------
            doMessageId(false);

            // ------------------- DATE ----------------
            doDate();

            // ------------------- FROM ----------------
            doFrom();

            // ------------------- TO ------------------
            doTo(false);

            // ------------------- CC ------------------
            doCc();

            // ------------------- BCC ------------------
            // Not part of spec, but for MMS editor

            doBcc();

            // ------------------- SUBJECT ------------------
            doSubject();

            // ---------------- MESSAGE CLASS ---------
            doMessageClass();

            // ---------------- PRIORITY ----------------
            doPriority();

            // ------------------- DELIVERY-REPORT ----------------
            doDeliveryReport();

            // ------------------- READ-REPLY ----------------
            doReadReply();

            // ---------------- USER-DEFINED HEADERS ----------------
            doUserDefined();

            // ---------------- CONTENT-TYPE ----------------
            doContentType();

            // -------------------------- BODY -------------
            doBody();

            break;

        case MESSAGE_TYPE_M_DELIVERY_IND: // ---------------------------- m-delivery-ind

            // ------------------- MESSAGE TYPE --------
            m_Out.write(FN_MESSAGE_TYPE + 0x80);
            m_Out.write(nMessageType);

            doMessageId(true);

            // ------------------- VERSION -------------
            doVersion();

            // ------------------- DATE ----------------
            doDate();

            // ------------------- TO ------------------
            doTo(true);

            // ------------------- MESSAGE-STATUS ----------------
            doMessageStatus();

            // ---------------- USER-DEFINED HEADERS ----------------
            doUserDefined();

            break;

        case MESSAGE_TYPE_M_SEND_REQ: // ---------------------------- m-send-req

            // ------------------- MESSAGE TYPE --------
            m_Out.write(FN_MESSAGE_TYPE + 0x80);
            m_Out.write(nMessageType);

            // ------------------- TRANSACTION ID ------
            doTransactionId();

            // ------------------- VERSION -------------
            doVersion();

            // ------------------- DATE ----------------
            doDate();

            // ------------------- FROM ----------------
            doFrom();

            // ------------------- TO ------------------
            doTo(false);

            // ------------------- CC ------------------
            doCc();

            // ------------------- BCC ------------------
            doBcc();

			if (!(m_Message.isToAvailable() || m_Message.isCcAvailable() || m_Message.isBccAvailable())) {
              m_Out.close();
              throw new MMEncoderException("No recipient specified in the Multimedia Message.");
            }

            // ---------------- SUBJECT  --------------
            doSubject();

            // ------------------- DELIVERY-REPORT ----------------
            doDeliveryReport();

            // ------------------- SENDER-VISIBILITY ----------------
            if (m_Message.isSenderVisibilityAvailable()) {
              m_Out.write(FN_SENDER_VISIBILITY + 0x80);
                m_Out.write(m_Message.getSenderVisibility());
            }

            // ------------------- READ-REPLY ----------------
            doReadReply();

            // ---------------- MESSAGE CLASS ---------
            doMessageClass();

            // ---------------- EXPIRY ----------------
            doExpiry();

            // ---------------- DELIVERY TIME ----------------
            doDeliveryTime();

            // ---------------- PRIORITY ----------------
            doPriority();

            // ---------------- USER-DEFINED HEADERS ----------------
            doUserDefined();

            // ---------------- CONTENT TYPE ----------------
            doContentType();

            // -------------------------- BODY -------------
            doBody();

            break;

        case MESSAGE_TYPE_M_NOTIF_IND: // ------------------- m-notification-ind

            // ------------------- MESSAGE TYPE --------
            m_Out.write(FN_MESSAGE_TYPE + 0x80);
            m_Out.write(nMessageType);

            // ------------------- TRANSACTION ID ------
            doTransactionId();

            // ------------------- VERSION -------------
            doVersion();

            // ------------------- FROM ----------------
            doFrom();

            // ---------------- SUBJECT  --------------
            doSubject();

            // ------------------- DELIVERY-REPORT ----------------
            doDeliveryReport();

            // ---------------- MESSAGE CLASS ---------
            doMessageClass();

            // ---------------- MESSAGE SIZE ---------
            doMessageSize();

            // ---------------- EXPIRY ----------------
            doExpiry();

            // ---------------- CONTENT LOCATION ----------------
            doContentLocation();

      }
      m_Out.close();
      m_bMessageEcoded = true;
    } catch (IOException e) {
      throw new MMEncoderException("An IO error occurred encoding the Multimedia Message.");
    }
  }

  private int[] EncodeMultiByteNumber(long lData) {

    int data[] = new int[32];
    long lDivider = 1L;
    int nSize = 0;
    long lNumber = lData;

    for (int i = 0; i < 32; i++)
      data[i] = 0;

    for (int i = 4; i >= 0; i--) {
      lDivider = 1L;
      for (int j = 0; j < i; j++)
        lDivider *= 256L;

      int q = (int)(lNumber / lDivider);

      if (q != 0 || nSize != 0) {
        int r = (int)(lNumber % lDivider);
        data[nSize + 1] = q;
        lNumber = r;
        nSize++;
      }
    }
    data[0] = nSize;
    return data;
  }

  private int[] EncodeUintvarNumber(long lData) {
    int data[] = new int[32];
    long lDivider = 1L;
    int nSize = 0;
    long lNumber = lData;

    for (int i = 0; i < 32; i++)
      data[i] = 0;

    for (int i = 4; i >= 0; i--) {
      lDivider = 1L;
      for (int j = 0; j < i; j++)
          lDivider *= 128L;

      int q = (int)(lNumber / lDivider);
      if (q != 0 || nSize != 0) {
        int r = (int)(lNumber % lDivider);
        data[nSize + 1] = q;
        if (i != 0)
            data[nSize + 1] += 128;
        lNumber = r;
        nSize++;
      }
    }

    data[0] = nSize;
    return data;
  }

  private byte[] encodeHeaders(MMContent part, boolean isCtFirst) {
    String hdrs = part.getHeaderString();
    if (hdrs == null) {
      return UNKNOWN_CONTENT_HDR;
    }
    return encodeHeaders(hdrs, isCtFirst);
  }

  /*
   * Encode WSP headers (content-type always first in Multipart parts).
   */
  private byte[] encodeHeaders(String hdrs, boolean ctIsFirst) {
      byte[] encHdrs = null;
      if (FLAG_DEBUG) System.out.println("MMS: ENCODING HEADERS: " + hdrs);
      HeaderCodec hc = new HeaderCodec();
      int opt = (ctIsFirst ? HeaderCodec.OPT_CONTENT_TYPE_FIRST : HeaderCodec.OPT_NONE);
      try {
        encHdrs = hc.encode(hdrs, opt);
      }
      catch (WpsException e) {
          if (FLAG_DEBUG) System.out.println("MMS: CAN'T ENCODE HEADERS!");
          encHdrs = UNKNOWN_CONTENT_HDR;
      }
      if (FLAG_DEBUG) System.out.println("MMS: ENCODE HEADERS:" + formatBytes(encHdrs));
      return encHdrs;
  }

  /**
   * Encode part using HeaderEncoder
   */
  private boolean EncodePart(MMContent part) throws IOException {
    if (part == null) {
      dbgOut("EncodePart: No part!");
      return false;
    }
    byte[] encHdrs = encodeHeaders(part.getHeaderString(), false);
    int nHeadersLen = encHdrs.length;

    // -------- DataLen -------------
    long lDataLen = part.getLength();

    // write HeadersLen
    writeUintvar(nHeadersLen);

    // write DataLen
    writeUintvar(lDataLen);

    // write headers
    m_Out.write(encHdrs);

    // ----------- Data --------------
    byte data[] = part.getContent();
    m_Out.write(data);

    dbgOut("EncodePart: Type: " + part.getType() +
       "Wrote hdrLen: " + nHeadersLen + " DataLen: " + lDataLen);

    return true;
  }

  //==================================================
  // HEADER ENCODING SECTION
  //==================================================

  // ------------------- TRANSACTION ID ------

  private void doTransactionId() throws IOException {
    if (m_Message.isTransactionIdAvailable()) {
      dbgOut("	Trans-Id:" + m_Message.getTransactionId());
      m_Out.write(FN_TRANSACTION_ID + 0x80);
      m_Out.write(m_Message.getTransactionId().getBytes());
      m_Out.write(0x00);
    }
  }

  // ------------------- VERSION ------

  private void doVersion() {
    int numValue;
    m_Out.write(FN_MMS_VERSION + 0x80);
    if (!m_Message.isVersionAvailable()) {
      numValue = MMS_VERSION_10;
    } else {
      numValue = m_Message.getVersion();
    }
    m_Out.write(numValue);
  }

  // ------------------- MESSAGE ID ------

  private void doMessageId(boolean isRequired) throws MMEncoderException, IOException {
    if (m_Message.isMessageIdAvailable()) {
      dbgOut("	Message-Id: " + m_Message.getMessageId());
      m_Out.write(FN_MESSAGE_ID + 0x80);
      m_Out.write(m_Message.getMessageId().getBytes());
      m_Out.write(0x00);
    } else {
      if (isRequired) {
        m_Out.close();
        throw new MMEncoderException ("The required field Message-ID of the Multimedia Message is null");
      }
    }
  }

  // ------------------- DATE ----------------

  private void doDate() throws MMEncoderException, IOException {
    if (m_Message.isDateAvailable()) {
      dbgOut("	Date:" + m_Message.getDate());
      long secs = (m_Message.getDate()).getTime() / 1000;
      int data[] = EncodeMultiByteNumber(secs);
      if (data == null) {
        m_Out.close();
        throw new MMEncoderException("An error occurred encoding the sending date of the Multimedia Message");
      }
      m_Out.write(FN_DATE + 0x80);
      int nCount = data[0];
      m_Out.write(nCount);
      for (int i = 1; i <= nCount; i++) {
        m_Out.write(data[i]);
      }
    }
  }

  // ------------------- FROM ------

  private void doFrom() throws MMEncoderException, IOException {

    if (m_Message.isFromAvailable()) {
      m_Out.write(FN_FROM + 0x80);

      String strValue = (m_Message.getFrom()).getFullAddress();
      dbgOut("	From:" + strValue);
      if (strValue == null) {
        m_Out.close();
        throw new MMEncoderException("The field from is assigned to null");
      }

      // Value-length
      writeValueLength(strValue.length()+2);
      // Address-present-token
      m_Out.write(0x80);

      // Encoded-string-value
      m_Out.write(strValue.getBytes());
      m_Out.write(0x00);
    }
  }

  // ------------------- TO ------------------

  private void doTo(boolean isRequired) throws MMEncoderException, IOException {
    if (m_Message.isToAvailable()) {
      dbgOut("	To:");
      Vector sAddress = m_Message.getTo();
      int nAddressCount = sAddress.size();
      if (sAddress == null) {
        m_Out.close();
        throw new MMEncoderException("The required TO field of the Multimedia Message is null.");
      }
      for (int i = 0; i < nAddressCount; i++) {
        String strValue = ((MMAddress)sAddress.elementAt(i)).getFullAddress();
        dbgOut("	   ->" + strValue);
        if (strValue != null) {
          m_Out.write(FN_TO + 0x80);
          m_Out.write(strValue.getBytes());
          m_Out.write(0x00);
        }
      }
    } else {
      if (isRequired) {
        m_Out.close();
        throw new MMEncoderException("The To recipient field is required in this Multimedia Message.");
      }
    }
  }

  // ------------------- CC ------------------

  private void doCc() throws MMEncoderException, IOException {

    if (m_Message.isCcAvailable()) {
      dbgOut("	Cc:");
      Vector sAddress = m_Message.getCc();
      int nAddressCount = sAddress.size();

      if (sAddress == null) {
        m_Out.close();
        throw new MMEncoderException("The field CC of the Multimedia Message is set to null.");
      }

      for (int i = 0; i < nAddressCount; i++) {
        String strValue = ((MMAddress)sAddress.elementAt(i)).getFullAddress();
        dbgOut("	   ->" + strValue);
        if (strValue != null) {
          m_Out.write(FN_CC + 0x80);
          m_Out.write(strValue.getBytes());
          m_Out.write(0x00);
        }
      }
    }
  }

  // ------------------- BCC ------------------

  private void doBcc() throws MMEncoderException, IOException {
    if (m_Message.isBccAvailable()) {
      dbgOut("	Bcc:");
      Vector sAddress = m_Message.getBcc();
      int nAddressCount = sAddress.size();

      if (sAddress == null) {
        m_Out.close();
        throw new MMEncoderException("The field BCC of the Multimedia Message is set to null.");
      }

      for (int i = 0; i < nAddressCount; i++) {
        String strValue = ((MMAddress)sAddress.elementAt(i)).getFullAddress();
        dbgOut("	   ->" + strValue);
        if (strValue != null) {
          m_Out.write(FN_BCC + 0x80);
          m_Out.write(strValue.getBytes());
          m_Out.write(0x00);
        }
      }
    }
  }

  // ---------------- SUBJECT  --------------

  private void doSubject() throws MMEncoderException, IOException {
    if (m_Message.isSubjectAvailable()) {
      dbgOut("	Subject:" + m_Message.getSubject());
      m_Out.write(FN_SUBJECT + 0x80);
      m_Out.write(m_Message.getSubject().getBytes());
      m_Out.write(0x00);
    }
  }

  // ------------------- DELIVERY-REPORT ----------------

  private void doDeliveryReport() {
      dbgOut("	Deliv-Report:" + (m_Message.getDeliveryReport()==true ? "TRUE" : "FALSE"));
    if (m_Message.isDeliveryReportAvailable()) {
      m_Out.write(FN_DELIVERY_REPORT + 0x80);
      if (m_Message.getDeliveryReport()==true)
        m_Out.write(0x80);
      else
        m_Out.write(0x81);
    }
  }

  // ---------------- MESSAGE CLASS ---------

  private void doMessageClass() {
    if (m_Message.isMessageClassAvailable()) {
      dbgOut("	Message-Class:" + (m_Message.getMessageClass() & 0xFF));
      m_Out.write(FN_MESSAGE_CLASS + 0x80);
      m_Out.write(m_Message.getMessageClass());
    }
  }

  // ------------------- READ-REPLY ----------------

  private void doReadReply() {
    if (m_Message.isReadReplyAvailable()) {
      dbgOut("	ReadReply");
      m_Out.write(FN_READ_REPLY + 0x80);
      if (m_Message.getReadReply() == true)
        m_Out.write(0x80);
      else
        m_Out.write(0x81);
    }
  }

  // ---------------- PRIORITY ----------------

  private void doPriority() {
    if (m_Message.isPriorityAvailable()) {
      dbgOut("	Priority:" + (m_Message.getPriority() & 0xFF));
      m_Out.write(FN_PRIORITY + 0x80);
      m_Out.write(m_Message.getPriority());
    }
  }

  // ---------------- CONTENT TYPE ----------------

  private void doContentType() throws MMEncoderException, IOException {
    if (m_Message.isContentTypeAvailable()) {
      m_bMultipartRelated = false;
      m_Out.write(FN_CONTENT_TYPE + 0x80);

      byte ctype = encodeContentType(m_Message.getContentType());
      dbgOut("	ContentType "  + m_Message.getContentType() + "(" + (ctype&0xFF) + ")");

      if (ctype == 0x33) {
        // application/vnd.wap.multipart.related
        m_bMultipartRelated = true;
        int valueLength=1;
        String mprt = m_Message.getMultipartRelatedType();
        if (mprt == null) mprt = "";
        valueLength += mprt.length()+2;
        String start = m_Message.getPresentationId();
        if (start == null) start = "";
        dbgOut("	start="  + start + " type=" + mprt);
        valueLength += start.length()+2;
        // Value-length
        writeValueLength(valueLength);
        // Well-known-media
        m_Out.write(0x33+0x80);
        // Parameters
        // Type
        m_Out.write(0x09+0x80);
        m_Out.write(mprt.getBytes());
        m_Out.write(0x00);
        // Start
        m_Out.write(0x0A+0x80);
        m_Out.write(start.getBytes());
        m_Out.write(0x00);
      } else {
        if (ctype > 0x00)
          m_Out.write(ctype + 0x80);
        else {
          m_Out.write(m_Message.getContentType().getBytes());
          m_Out.write(0x00);
        }
      }
    } else {
        m_Out.close();
        throw new MMEncoderException("The field CONTENT TYPE of the Multimedia Message is not specified.");
    }
  }

  // ---------------- DELIVERY TIME ----------------

  private void doDeliveryTime() throws MMEncoderException, IOException {
    if (m_Message.isDeliveryTimeAvailable()) {
      dbgOut("	DeliveryTime:" + m_Message.getDeliveryTime());
      long secs=(m_Message.getDeliveryTime()).getTime() / 1000;
      int data[] = EncodeMultiByteNumber(secs);
      if (data == null) {
        m_Out.close();
        throw new MMEncoderException("The field DELIVERY TIME of the Multimedia Message is set to null.");
      }
      int nCount = data[0];

      m_Out.write(FN_DELIVERY_TIME + 0x80);

      // Value-length
      writeValueLength(nCount+2);

      if (m_Message.isDeliveryTimeAbsolute()) {
        // Absolute-token
        m_Out.write(0x80);
      } else {
        // Relative-token
        m_Out.write(0x81);
      }

      // Date-value or Delta-seconds-value
      for (int i = 0; i <= nCount; i++) {
        m_Out.write(data[i]);
      }
    }
  }

  // ---------------- EXPIRY ----------------

  private void doExpiry() throws MMEncoderException, IOException {
    if (m_Message.isExpiryAvailable()) {
      dbgOut("	Expiry:" + m_Message.getExpiry());
      long secs=(m_Message.getExpiry()).getTime() / 1000;
      int data[] = EncodeMultiByteNumber(secs);
      if (data == null) {
        m_Out.close();
        throw new MMEncoderException("An error occurred encoding the EXPIRY " +
            "field of the Multimedia Message. The field is set to null");
      }
      int nCount = data[0];

      m_Out.write(FN_EXPIRY + 0x80);

      // Value-length
      writeValueLength(nCount+2);

      if (m_Message.isExpiryAbsolute()) {
        // Absolute-token
        m_Out.write(0x80);
      } else {
        // Relative-token
        m_Out.write(0x81);
      }

      // Date-value or Delta-seconds-value
      for (int i = 0; i <= nCount; i++) {
        m_Out.write(data[i]);
      }
    }
  }

  // ---------------- CONTENT LOCATION  --------------

  private void doContentLocation() throws MMEncoderException, IOException {
    if (m_Message.isContentLocAvailable()) {
      dbgOut("	ContentLoc:" + m_Message.getContentLocation());
      m_Out.write(FN_CONTENT_LOCATION + 0x80);
      m_Out.write(m_Message.getContentLocation().getBytes());
      m_Out.write(0x00);
    }
  }

  // ------------------- MESSAGE SIZE ----------------

  private void doMessageSize() throws MMEncoderException, IOException {
    if (m_Message.isMessageSizeAvailable()) {
      dbgOut("	Date:" + m_Message.getMessageSize());
      long size = m_Message.getMessageSize();
      int data[] = EncodeMultiByteNumber(size);
      if (data == null) {
        m_Out.close();
        throw new MMEncoderException("An error occurred encoding the message size of the Multimedia Message");
      }
      m_Out.write(FN_MMS_MESSAGE_SIZE + 0x80);
      int nCount = data[0];
      m_Out.write(nCount);
      for (int i = 1; i <= nCount; i++) {
        m_Out.write(data[i]);
      }
    }
  }

  // ------------------- MESSAGE-STATUS ----------------

  private void doMessageStatus() throws MMEncoderException, IOException {
    if (m_Message.isStatusAvailable()) {
      dbgOut("	MessageStatus:" + (m_Message.isStatusAvailable() ? "TRUE" : "FALSE"));
      m_Out.write(FN_STATUS + 0x80);
      m_Out.write(m_Message.getMessageStatus());
    } else {
       m_Out.close();
       throw new MMEncoderException ("The field Message-ID of the Multimedia Message is null");
    }
  }

  // ---------------- USER-DEFINED HEADERS  --------------

  private void doUserDefined() throws MMEncoderException, IOException {
    if (m_Message.isUserDefinedAvailable()) {
      dbgOut("	User Defined:");
      String[] names = m_Message.getUserDefinedHeaderNames();
      for (int i = 0; i < names.length; i++) {
        m_Out.write(names[i].getBytes());
        m_Out.write(0x00);
        String val = m_Message.getUserDefinedHeaderValue(names[i]);
        if (val == null) val = " ";
        dbgOut("	-> " + names[i] + ":" + val);
        m_Out.write(val.getBytes());
        m_Out.write(0x00);
      }
    }
  }

  // ------------------- BODY ----------------

  private void doBody() throws MMEncoderException, IOException {
    int nPartsCount = m_Message.getNumContents();
    m_Out.write(nPartsCount);
    MMContent part = null;
    for (int i = 0; i < nPartsCount; i++) {
      dbgOut("== EncodePart #" + i + " ==");
      part = m_Message.getContent(i);
      boolean bRetVal = EncodePart(part);
      if (!bRetVal) {
        m_Out.close();
        throw new MMEncoderException("The entry having Content-id = " + part.getContentId() +
            " cannot be encoded.");
      }
    }
  }
}
