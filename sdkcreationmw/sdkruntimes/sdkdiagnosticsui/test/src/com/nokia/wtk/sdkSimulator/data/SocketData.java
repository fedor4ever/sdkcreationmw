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

import java.util.Map;
import com.nokia.wtk.device.DeviceSocket;

/**
 * This class represents provides the socket (UDP or TCP) data type.
 *
 */
public class SocketData implements DeviceSocket {

  public SocketData() {
  }


  public SocketData(int id,
                    String srcPort,
                    String srcAddr,
                    String destPort,
                    String destAddr,
                    int type) {
    this.id = id;
    this.srcAddr = srcAddr;
    this.srcPort = srcPort;
    this.destAddr = destAddr;
    this.destPort = destPort;
    this.type = type;
  }

  /**
   * Returns the source port of the socket.
   * @return the source port of the socket.
   */
  public int getID() {
    return id;
  }

  /**
   * Returns the source port of the socket.
   * @return the source port of the socket.
   */
  public String getSourcePort() {
    return srcPort;
  }

  /**
   * Returns the destination port of the socket.
   * @return the destination port of the socket.
   */
  public String getDestinationPort() {
    return destPort;
  }


  /**
   * Returns the source address of the socket.
   * @return the source address of the socket.
   */
  public String getSourceAddress() {
    return srcAddr;
  }


  /**
   * Returns the destination address of the socket.
   * @return the destination address of the socket.
   */
  public String getDestinationAddress() {
    return destAddr;
  }


  /**
   * Returns the type of the socket.
   * @return the type of the socket.
   *                  Possible values are:
   *                  <ul>
   *                    <li> {@link #TCP_SOCKET}
   *                    <li> {@link #UDP_SOCKET}
   *                  </ul>
   */
  public int getType() {
    return type;
  }

  /**
  * Returns a String representation of the SocketData object
  */
  public String toString() {

    StringBuffer buf = new StringBuffer();
    buf.append("Socket ID: " + getID() + "\n");
    buf.append("Source port: " + getSourcePort() + "\n");
    buf.append("Source address " + getSourceAddress() + "\n");
    buf.append("Destination port: " + getDestinationPort() + "\n");
    buf.append("Destination address: " + getDestinationAddress() + "\n");
    String typeStr;
    if (getType() == DeviceSocket.TCP_SOCKET) {
      typeStr = "TCP";
    }
    else if (getType() == DeviceSocket.UDP_SOCKET) {
      typeStr = "UDP";
    }
    else {
      typeStr = "unknown";
    }
    buf.append("Protocol type: " + typeStr + "\n");

    return buf.toString();
  }


   /** TCP Socket Identifier (TCP_SOCKET (0)/ UDP_SOCKET(1) */
   private int id;
   private int type;
   private String srcPort;
   private String srcAddr;
   private String destPort;
   private String destAddr;
}
