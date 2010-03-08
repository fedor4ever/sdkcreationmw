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
import com.nokia.wtk.device.DeviceSocketListener;
import com.nokia.wtk.device.DeviceSocketMonitor;
import com.nokia.wtk.device.DeviceSocket;

/**
 * <p>Title: SdkSocketService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for socket activity monitoring.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkSocketService extends AbstractServiceAdapter
                             implements SocketAdapter,
                                        DeviceSocketMonitor {



  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkSocketService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceSocketListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- no methods to implement for com.nokia.wtk.device.DeviceSocketMonitor ---

  // --- implementation of SocketAdapter ---
  public void socketCreated( DeviceSocket socket ) {
    addCallback( SOCKET_CREATED, new Object [] { socket } );
  }

  public void socketConnected( DeviceSocket socket ) {
    addCallback( SOCKET_CONNECTED, new Object [] { socket } );
  }

  public void socketAccepted( DeviceSocket listeningSocket,
                              DeviceSocket acceptedSocket) {
    addCallback( SOCKET_ACCEPTED, new Object [] { listeningSocket, acceptedSocket } );
  }

  public void socketListening( DeviceSocket socket ) {
    addCallback( SOCKET_LISTENING, new Object [] { socket } );
  }

  public void socketClosed( DeviceSocket socket ) {
    addCallback( SOCKET_CLOSED, new Object [] { socket } );
  }

  public void messageSent( DeviceSocket socket, byte[] payload ) {
    addCallback( MESSAGE_SENT, new Object [] { socket, payload } );
  }

  public void messageReceived( DeviceSocket socket, byte[] payload ) {
    addCallback( MESSAGE_RECEIVED, new Object [] { socket, payload } );
  }


  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#methodForName(java.lang.String)
   */
  protected Method methodForName(String name) {
    return (Method)map.get( name );
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

  private static final String SOCKET_CREATED = "socketCreated";
  private static final String SOCKET_CONNECTED = "socketConnected";
  private static final String SOCKET_ACCEPTED = "socketAccepted";
  private static final String SOCKET_LISTENING = "socketListening";
  private static final String SOCKET_CLOSED = "socketClosed";
  private static final String MESSAGE_SENT = "messageSent";
  private static final String MESSAGE_RECEIVED = "messageReceived";

  private HashMap map;

}
