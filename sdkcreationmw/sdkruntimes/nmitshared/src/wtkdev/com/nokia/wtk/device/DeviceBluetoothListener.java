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
 * This interface provides an abstraction for bluetooth events.
 *
 */

public interface DeviceBluetoothListener {


    /**
     * Called when a Bluetooth packet has been sent.
     * @param bluetooth  a reference to the DeviceBluetoothSend object.
     */
  public void bluetoothSent (DeviceBluetoothSend bluetooth);


    /**
     * Called when a Bluetooth packet has been received.
     * @param bluetooth  a reference to the DeviceBluetoothReceived object.
     */
  public void bluetoothReceived (DeviceBluetoothReceive bluetooth);
}
