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


/*
 * Created on Aug 9, 2004
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

// java.util
import java.util.HashMap;

// java.lang.reflect
import java.lang.reflect.Method;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceBluetoothListener;
import com.nokia.wtk.device.DeviceBluetoothService;
import com.nokia.wtk.device.DeviceBluetoothSend;
import com.nokia.wtk.device.DeviceBluetoothReceive;

/**
 * <p>Title: SdkBluetoothService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for Bluetooth activity monitoring.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkBluetoothService extends AbstractServiceAdapter
                                  implements BluetoothAdapter,
                                             DeviceBluetoothService  {

  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkBluetoothService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceBluetoothListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- no methods to implement for com.nokia.wtk.device.DeviceBluetoothService ---

  // --- implementation of BluetoothAdapter ---
  /**
   * Called when a bluetooth packet has been sent.
   * @param bluetooth  a reference to the BluetoothDataSend object.
   */
  public void bluetoothSent( DeviceBluetoothSend bluetooth ) {
    addCallback( BLUETOOTH_SENT, new Object [] { bluetooth} );
  }

  /**
   * Called when a bluetooth packet has been received.
   * @param bluetooth  a reference to the BluetoothDataReceive object.
   */
  public void bluetoothReceived( DeviceBluetoothReceive bluetooth ) {
    addCallback( BLUETOOTH_RECEIVED, new Object [] { bluetooth} );
  }


  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#registerWithSdk()
   */
  protected void registerWithSdk() {
    if (sampleSdk != null) {
      sampleSdk.registerServer (serviceName, this);
    }
    // TBD - to be implemented by SDK developer

  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#unregisterWithSdk()
   */
  protected void unregisterWithSdk() {
    // TBD - to be implemented by SDK developer
  }


  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#methodForName(java.lang.String)
   */
  protected Method methodForName(String name) {
    return (Method)map.get( name );
  }

  private static final String BLUETOOTH_SENT = "bluetoothSent";
  private static final String BLUETOOTH_RECEIVED = "bluetoothReceived";

  private HashMap map;

}



