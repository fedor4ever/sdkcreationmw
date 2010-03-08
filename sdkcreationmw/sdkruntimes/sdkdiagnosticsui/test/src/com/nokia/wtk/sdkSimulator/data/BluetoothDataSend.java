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
import com.nokia.wtk.device.DeviceBluetoothSend;

/**
 * <p>Title: BluetoothDataSend</p>
 * <p>Description: Implements the com.nokia.wtk.device.DeviceBluetoothSend
 * interface.</p>
 * <p>Company: Nokia</p>
 */
public class BluetoothDataSend extends BluetoothData implements DeviceBluetoothSend {

  public BluetoothDataSend (String toAddress,
                            int packetBoundaryFlag,
                            int broadcastFlag,
                            int dataTotalLength,
                            byte[] data) {

    super (packetBoundaryFlag,
           broadcastFlag,
           dataTotalLength,
           data);

    this.toAddress = toAddress;
  }

  /**
   * Returns the send-to address.
   * @return the sent-to address.
   */
  public String getToAddress () {
    return toAddress;
  }

  private String toAddress;
}
