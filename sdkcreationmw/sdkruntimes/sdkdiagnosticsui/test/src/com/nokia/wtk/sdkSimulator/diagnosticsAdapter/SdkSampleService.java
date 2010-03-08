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
import com.nokia.wtk.device.DeviceSampleListener;
import com.nokia.wtk.device.DeviceSampleData;

/**
 * <p>Title: SdkSampleService</p>
 * <p>Description: A simple sample service object
 * (this class is for sample purpose only!).</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkSampleService extends AbstractServiceAdapter
                              implements SampleAdapter {

  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkSampleService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceSampleListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- implementation of SampleAdapter ---
  /**
   * Called when a sample event is generated.
   * @param event the object that carries the sample event data
   */
  public void sampleEventGenerated (DeviceSampleData event) {
    addCallback (SAMPLE_EVENT_GENERATED, new Object [] { event} );
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

  private static final String SAMPLE_EVENT_GENERATED = "sampleEventGenerated";

  private HashMap map;

}



