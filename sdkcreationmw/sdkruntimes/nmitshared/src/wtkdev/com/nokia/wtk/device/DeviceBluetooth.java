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

/**
 * This interface provides an abstraction for the bluetooth data type.
 *
 */
public interface DeviceBluetooth {

  /**
   * Returns the data in the bluetooth packet.
   * @return the data bytes.
   */
  public byte[] getData();

  /**
   * Returns the data length.
   * @return the data length.
   */
  public int getDataTotalLength();

  /**
   * Returns the value of the broadcast flag.
   * @return the value of the broadcast flag..
   */
  public int getBroadcastFlag();

  /**
   * Returns the value of the packet boundary flag.
   * @return the value of the packet boundary flag.
   */
  public int getPacketBoundaryFlag();


  /** Broadcast flag values */
  public static final int BROADCAST_RESERVED = 3;

  /** Send Broadcast flag values */
  public static final int NO_BROADCAST       = 0;
  public static final int ACTIVE_BROADCAST   = 1;
  public static final int PICONET_BROADCAST  = 2;

  /** Receive Broadcast flag values */
  public static final int POINT_TO_POINT       = 0;
  public static final int SLAVE_BROADCAST      = 1;
  public static final int SLAVE_PARK_BROADCAST = 2;

  /** Packet-boundary flag values */
  public static final int FIRST_PACKET       = 2;
  public static final int CONTINUING_PACKET  = 1;
  public static final int PACKET_RESERVED_00 = 0;
  public static final int PACKET_RESERVED_11 = 3;

}
