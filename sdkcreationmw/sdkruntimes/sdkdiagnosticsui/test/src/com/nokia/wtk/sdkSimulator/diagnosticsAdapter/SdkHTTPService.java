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
import com.nokia.wtk.device.DeviceHTTPContentListener;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceContentReceived;

/**
 * <p>Title: SdkHTTPService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for HTTP activity monitoring.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkHTTPService extends AbstractServiceAdapter
                            implements HTTPAdapter {


  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkHTTPService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceHTTPContentListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- implementation of HTTPAdapter ---
  /**
   * Called when an HTTP content has been requested.
   * @param request   the content that was requested.
   */
  public void contentRequested( DeviceContentRequest request ) {
    addCallback( CONTENT_REQUESTED, new Object [] { request } );
  }

  /**
   * Called when any content has been received as a response to a request.
   * This method should be called for all request responses, including
   * error responses and cached responses.
   * @param content   the content that was received.
   * @param cacheHit  if <code>true</code> then content was retrieved from the cache.
   */
  public void contentReceived( DeviceContentReceived response, boolean cached)  {
    addCallback( CONTENT_RECEIVED, new Object [] { response, new Boolean( cached ) } );
  }

  /**
   * Called when any content has been pushed onto the device.
   * @param content   the content that was pushed.
   */
  public void contentPushed( DeviceContentReceived data ) {
    addCallback( CONTENT_PUSHED, new Object [] { data } );
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

  private static final String CONTENT_REQUESTED = "contentRequested";
  private static final String CONTENT_RECEIVED = "contentReceived";
  private static final String CONTENT_PUSHED = "contentPushed";

  private HashMap map;

}
