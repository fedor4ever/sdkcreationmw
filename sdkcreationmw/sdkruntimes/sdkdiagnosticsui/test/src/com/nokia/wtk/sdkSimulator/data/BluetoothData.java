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

import java.io.Serializable;
import com.nokia.wtk.device.DeviceBluetooth;

/**
 * <p>Title: BluetoothData</p>
 * <p>Description: BluetoothData is an abstract, Serializable class that is the
 * common parent of the three bluetooth related data classes.
 * It implements the methods common to all three types. </p>
 * <p>Company: Nokia</p>
 */
abstract class BluetoothData implements Serializable, DeviceBluetooth {

  /**
   * Constructor
   * @param packetBoundaryFlag the value of the packet boundary flag
   * @param broadcastFlag the value of the broadcast flag
   * @param dataTotalLength the length of the packet data
   * @param data the packet data
   */
  BluetoothData (int packetBoundaryFlag,
                 int broadcastFlag,
                 int dataTotalLength,
                 byte[] data) {

    this.packetBoundaryFlag = packetBoundaryFlag;
    this.broadcastFlag = broadcastFlag;
    this.dataTotalLength = dataTotalLength;
    this.data = data;
  }

  /**
   * Returns the value of the packet boundary flag.
   * @return the value of the packet boundary flag.
   */
  public int getPacketBoundaryFlag() {
    return packetBoundaryFlag;
  }

  /**
   * Returns the value of the broadcast flag.
   * @return the value of the broadcast flag..
   */
  public int getBroadcastFlag() {
    return broadcastFlag;
  }

  /**
   * Returns the data length.
   * @return the data length.
   */
  public int getDataTotalLength() {
    return dataTotalLength;
  }

  /**
   * Returns the data in the bluetooth packet.
   * @return the data bytes.
   */
  public byte[] getData() {
    return data;
  }


  private byte[] data;
  private int dataTotalLength;
  private int broadcastFlag;
  private int packetBoundaryFlag;
}
