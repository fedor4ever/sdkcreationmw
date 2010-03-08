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
import com.nokia.wtk.device.DeviceMessageListener;
import com.nokia.wtk.device.DeviceMessageSystem;
import com.nokia.wtk.device.DeviceMessage;
import com.nokia.wtk.device.DeviceMessageFragment;

/**
 * <p>Title: SdkSMSService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for SMS traffic.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkSMSService extends AbstractServiceAdapter
                           implements SMSAdapter,
                                      DeviceMessageSystem {

  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkSMSService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceMessageListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- no methods to implement for com.nokia.wtk.device.DeviceMessageSystem ---
  // --- implementation of SMSAdapter ---
  /**
   * Notifies the listener that a fragment has been sent.
   * @param fragment the fragment that was sent.
   */
  public void fragmentSent( DeviceMessageFragment fragment ) {
    addCallback( FRAGMENT_SENT, new Object [] { fragment } );
  }

  /**
   * Notifies the listener that a fragment has been received.
   * @param fragment the fragment that was received.
   */
  public void fragmentReceived( DeviceMessageFragment fragment ) {
    addCallback( FRAGMENT_RECEIVED, new Object [] { fragment } );
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.plugin.DeviceMessageListener#messageSent(com.nokia.wtk.data.SMSMessage)
   */
  public void messageSent( DeviceMessage message ) {
    addCallback( MESSAGE_SENT, new Object [] { message } );
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.plugin.DeviceMessageListener#messageReceived(com.nokia.wtk.data.SMSMessage)
   */
  public void messageReceived( DeviceMessage message ) {
    addCallback( MESSAGE_RECEIVED, new Object [] { message } );
  }


  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#registerWithSdk()
   */
  protected void registerWithSdk() {
    // TBD - to be implemented by SDK developer
    if (sampleSdk != null) {
      sampleSdk.registerServer (serviceName, this);
    }
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

  private static final String FRAGMENT_SENT = "fragmentSent";
  private static final String FRAGMENT_RECEIVED = "fragmentReceived";
  private static final String MESSAGE_SENT = "messageSent";
  private static final String MESSAGE_RECEIVED = "messageReceived";

  private HashMap map;


}



