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


package com.nokia.wtk.sdkSimulator.sampleSdk;

// java.io
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DeviceMessageFragment;
import com.nokia.wtk.device.DeviceSocket;
import com.nokia.wtk.device.DeviceBluetoothSend;
import com.nokia.wtk.device.DeviceBluetoothReceive;
import com.nokia.wtk.device.DeviceMessage;

// com.nokia.wtk.sdkSimulator.data
import com.nokia.wtk.sdkSimulator.data.BluetoothDataSend;
import com.nokia.wtk.sdkSimulator.data.BluetoothDataReceive;
import com.nokia.wtk.sdkSimulator.data.BrowserRequest;
import com.nokia.wtk.sdkSimulator.data.BrowserResponse;
import com.nokia.wtk.sdkSimulator.data.SocketData;
import com.nokia.wtk.sdkSimulator.data.SMSMessageFragment;
import com.nokia.wtk.sdkSimulator.data.SMSMessage;
import com.nokia.wtk.sdkSimulator.data.PushData;

/**
 * <p>Title: TrafficEventGenerator </p>
 * <p>Description: A demo class to generate events for the Traffic view
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class TrafficEventGenerator  {

  public TrafficEventGenerator() {
  }

// HTTP operations
  /**
  * send HTTP request
  */
  public DeviceContentRequest sendHttp() {

    BrowserRequest request =
      new BrowserRequest(url,
                         method,
                         httpRequestHeaders,
                         httpRequestData);
    return request;
  }

  /**
   * receive HTTP response
   */
  public BrowserResponse receiveHttp() {
    byte[] data = responseString.getBytes();

    BrowserResponse response =
      new BrowserResponse(url,
                          requestStatus,
                          url,
                          wmlContentType,
                          encoding,
                          httpResponseHeaders,
                          data,
                          cacheHit,
                          transactionId);
    return response;
  }

// MMS operations
  /**
   * send MMS
   */
  public PushData sendMms() {

    // if mmsFile is null, open it and load the data
    if (mmsFile == null || mmsData == null) {
      getMmsData();
    }

    PushData mmsSend =
      new PushData(mmsContentType,
                   encoding,
                   mmsHeaders,
                   mmsSent,
                   mmsData);
    return mmsSend;
  }

  /**
   * push MMS
   */
  public PushData pushMms() {

    // if mmsFile is null, open it and load the data
    if (mmsFile == null || mmsData == null) {
      getMmsData();
    }

    PushData mmsPush =
      new PushData(mmsContentType,
                   encoding,
                   mmsHeaders,
                   mmsPushed,
                   mmsData);
    return mmsPush;
  }


// SMS operations
  /**
   * send SMS
   */
  public DeviceMessage sendSMSMessage() {

    // create SMSMessageFragmentObject
    SMSMessageFragment smsFragment = new SMSMessageFragment();
    smsFragment.setMessageCenterAddress(smsc);
    smsFragment.setData(smsData);
    smsFragment.addHeader(SMS_TP_MTI, "SMS-SUBMIT");
    smsFragment.addHeader(SMS_TP_RD, "false");
    smsFragment.addHeader(SMS_TP_RP, "false");
    smsFragment.addHeader(SMS_TP_MR, "false");
    smsFragment.addHeader(SMS_TP_UDHI, "false");
    smsFragment.addHeader(SMS_TP_VP, "relative-format");
    smsFragment.addHeader(SMS_TP_DA, smsDestAddr);
    smsFragment.addHeader(SMS_TP_PID, "0");
    smsFragment.addHeader(SMS_TP_DCS, "7 bit");
    smsFragment.addHeader(SMS_TP_VPR, "1 day");
    smsFragment.addHeader(SMS_TP_MMS, "false");

    // create SMSMessageObject
    SMSMessage smsMessage = new SMSMessage();
    smsMessage.addFragment(smsFragment);
    return smsMessage;
  }

  /**
   * receive SMS
   */
  public DeviceMessage receiveSMSMessage() {
    // create SMSMessageFragmentObject
    SMSMessageFragment smsFragment = new SMSMessageFragment();
    smsFragment.setMessageCenterAddress(smsc);
    smsFragment.setSourceAddress(smsSrcAddr);
    smsFragment.setData(smsData);
    smsFragment.addHeader(SMS_TP_MTI, "SMS-DELIVER");
    smsFragment.addHeader(SMS_TP_MMS, "false");
    smsFragment.addHeader(SMS_TP_RP, "false");
    smsFragment.addHeader(SMS_TP_UDHI, "false");
    smsFragment.addHeader(SMS_TP_SRI, "false");
    smsFragment.addHeader(SMS_TP_OA, smsSrcAddr);
    smsFragment.addHeader(SMS_TP_PID, "0");
    smsFragment.addHeader(SMS_TP_DCS, "7 bit");
    long currentTime = System.currentTimeMillis();
    smsFragment.addHeader(SMS_TP_SCTS, String.valueOf(currentTime));


    // create SMSMessageObject
    SMSMessage smsMessage = new SMSMessage();
    smsMessage.addFragment(smsFragment);
    return smsMessage;
  }

// Socket operations
  /**
   * send/receive TCP
   */
  public DeviceSocket send_receiveTcp() {
    DeviceSocket socket =
      new SocketData(tcpSocketId,
                     srcPort,
                     srcAddr,
                     destPort,
                     destAddr,
                     DeviceSocket.TCP_SOCKET);
    return socket;
  }


  /**
   * send/receive UDP
   */
  public DeviceSocket send_receiveUdp() {
    DeviceSocket socket =
      new SocketData(udpSocketId,
                     srcPort,
                     srcAddr,
                     destPort,
                     destAddr,
                     DeviceSocket.UDP_SOCKET);
    return socket;
  }

  /**
   * returns TCPSend payload
   */
  public byte[] getTcpSendPayload() {
    return tcpSendPayload;
  }
  /**
   * returns TCPReceive payload
   */
  public byte[] getTcpReceivePayload() {
    return tcpReceivePayload;
  }
  /**
   * returns UDPSend payload
   */
  public byte[] getUdpSendPayload() {
    return udpSendPayload;
  }
  /**
   * returns UDPReceive payload
   */
  public byte[] getUdpReceivePayload() {
    return udpReceivePayload;
  }


// Bluetooth operations
  /**
   * send bluetooth
   */
  public DeviceBluetoothSend sendBluetooth() {
    String toAddress = "0x000033127b35";
    int packetBoundaryFlag = 2;
    int broadcastFlag = 0;
    String dataString = "bluetoothdata";
    byte[] data = dataString.getBytes();
    int dataLength = data.length;

    // construct data object
    BluetoothDataSend btSend =
      new BluetoothDataSend (toAddress,
                             packetBoundaryFlag,
                             broadcastFlag,
                             dataLength,
                             data);
    return btSend;
  }

  /**
   * receive bluetooth
   */
  public DeviceBluetoothReceive receiveBluetooth() {
    String fromAddress = "0x000034127b35";
    int packetBoundaryFlag = 2;
    int broadcastFlag = 0;
    String dataString = "bluetoothdata";
    byte[] data = dataString.getBytes();
    int dataLength = data.length;

    // construct data object
    BluetoothDataReceive btReceive =
      new BluetoothDataReceive (fromAddress,
                                packetBoundaryFlag,
                                broadcastFlag,
                                dataLength,
                                data);
    return btReceive;
  }

  private void getMmsData() {
    try {
      mmsFile = new File (mmsFileName);
      if (mmsFile != null && mmsFile.exists()) {
        mmsData = new byte[(int)mmsFile.length()];
        FileInputStream fis = new FileInputStream(mmsFileName);
        fis.read(mmsData);
        fis.close();
      } else {
        mmsData = mmsBackupData;
      }
    } catch (IOException e) {
      mmsData = mmsBackupData;
    }
 }


  // HTTP demo data
  private static final String url = "http://wap.yahoo.com";
  private static final String location = "http://wap.yahoo.com";
  private static final String method = "GET";
  private static final String httpRequestHeaders = "host:wap.yahoo.com\naccept-language:en";
  private static final String httpResponseHeaders = "content-type:text/vnd.wap.wml\ncache-control:no-cache";
  private static final byte[] httpRequestData = null;
  private static final int requestStatus = 200;
  private static final String wmlContentType = "text/vnd.wap.wml";
  private static final String encoding = "ISO-8859-1";
  private static final boolean cacheHit = false;
  private static final int transactionId = -1;
  private static final String responseString =
    "<?xml version='1.0'?><wml><card id='card1' title='Hello!'></card></wml>";

  // TCP/UDP demo data
  private static final int tcpSocketId = 1;
  private static final int udpSocketId = 2;
  private static final String srcPort = "100";
  private static final String srcAddr = "1111111";
  private static final String destPort = "100";
  private static final String destAddr = "2222222";
  private static final byte[] tcpSendPayload =
    new String("this is the TCP send data").getBytes();
  private static final byte[] tcpReceivePayload =
    new String("this is the TCP receive data").getBytes();
  private static final byte[] udpSendPayload =
    new String("this is the UDP send data").getBytes();
  private static final byte[] udpReceivePayload =
    new String("this is the UDP receive data").getBytes();

  // SMS demo data
  private static final String smsSrcAddr = "1111111";
  private static final String smsDestAddr = "2222222";
  private static final String smsc = "9999";
  private static final String smsData = "this is a short message";

  private static final int SMS_TP_MTI = 1;
  /* Reject duplicates; Values: "True" or "False" */
  private static final int SMS_TP_RD = 2;
  /* Reply path; Values: "True" or "False" */
  private static final int SMS_TP_RP = 3;
  /* Status report request; Values: "True" or "False" */
  private static final int SMS_TP_SRR = 4;
  /* Message reference; Example value: "2" */
  private static final int SMS_TP_MR = 5;
  /* Destination Address; Example value: "+17813547719" */
  private static final int SMS_TP_DA = 6;
  /* Protocol identifier; Values: "Text", "E-mail", "Fax" */
  private static final int SMS_TP_PID = 7;
  /* Data coding scheme; Values: "7 bit", "8 bit", "Unicode" */
  private static final int SMS_TP_DCS = 8;
  /* Validity period relative: Example values: "5 minutes", "12 hours", "1 day", "1 week" */
  private static final int SMS_TP_VPR = 9;
  /* Validity period absolute: Example values: "Mar 13 2003 2:05:22 UTC" */
  private static final int SMS_TP_VPA = 10;
  /* Indicates whether or not the TP-VP field is present */
  private static final int SMS_TP_VP = 11;
  /* Indicates whether the TP-UD field contains a header */
  private static final int SMS_TP_UDHI = 12;
  /* Indicates whether or not there are more messages to send */
  private static final int SMS_TP_MMS = 13;
  /* Indicates whether the SME has requested a status report */
  private static final int SMS_TP_SRI = 14;
  /* Address of the originating SME */
  private static final int SMS_TP_OA = 15;
  /* Timestamp */
  private static final int SMS_TP_SCTS = 16;

  // MMS demo data
  private static final String mmsContentType = "application/vnd.wap.mms-message";
  private static final String mmsPushed = "pushed";
  private static final String mmsSent= "sent";
  private static final byte[] mmsBackupData =
    new String ("this is a MMS message").getBytes();
  private static final String mmsHeaders =
    "Content-Type:application/vnd.wap.mms-message";
  private static final String mmsFileName =
    System.getProperty( "user.dir" ) + "\\demoData\\trafficView\\cats.mms";
  private File mmsFile = null;
  private byte[] mmsData = null;


}

