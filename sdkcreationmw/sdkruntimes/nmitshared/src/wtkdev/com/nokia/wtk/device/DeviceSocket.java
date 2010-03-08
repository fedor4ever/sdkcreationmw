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



package com.nokia.wtk.device;

import java.util.Map;

/**
 * This interface provides an abstraction for the socket (UDP or TCP) data type.
 *
 */
public interface DeviceSocket {
  
  /**
   * Returns the source port of the socket.
   * @return the source port of the socket.
   */
  public String getSourcePort();

  /**
   * Returns the destination port of the socket.
   * @return the destination port of the socket.
   */
  public String getDestinationPort();


  /**
   * Returns the source address of the socket.
   * @return the source address of the socket.
   */
  public String getSourceAddress();
  

  /**
   * Returns the destination address of the socket.
   * @return the destination address of the socket.
   */
  public String getDestinationAddress();


  /**
   * Returns the type of the socket.
   * @return the type of the socket.
   *                  Possible values are:
   *                  <ul>
   *                    <li> {@link #TCP_SOCKET}
   *                    <li> {@link #UDP_SOCKET}
   *                  </ul>
   */
  public int getType();

 
   /** TCP Socket Identifier */
   public static final int TCP_SOCKET = 0;

   /** UDP Socket Identifier */
   public static final int UDP_SOCKET = 1;
}
