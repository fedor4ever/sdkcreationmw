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
import com.nokia.wtk.device.DeviceBluetoothReceive;

/**
 * <p>Title: BluetoothDataReceive</p>
 * <p>Description: Implements the com.nokia.wtk.device.DeviceBluetoothReceive
 * interface.</p>
 * <p>Company: Nokia</p>
 */
public class BluetoothDataReceive extends BluetoothData implements DeviceBluetoothReceive {

  public BluetoothDataReceive (String fromAddress,
                               int packetBoundaryFlag,
                               int broadcastFlag,
                               int dataTotalLength,
                               byte[] data) {

    super (packetBoundaryFlag,
           broadcastFlag,
           dataTotalLength,
           data);

    this.fromAddress = fromAddress;
  }

  /**
   * Returns the received-from address.
   * @return the received-from address.
   */
  public String getFromAddress () {
    return fromAddress;
  }

  private String fromAddress;
}
