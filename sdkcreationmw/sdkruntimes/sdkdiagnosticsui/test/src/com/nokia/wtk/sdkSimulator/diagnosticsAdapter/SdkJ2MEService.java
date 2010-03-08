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
import com.nokia.wtk.device.DeviceJ2MEListener;
import com.nokia.wtk.device.DeviceJ2ME;
import com.nokia.wtk.device.DeviceMemory;

/**
 * <p>Title: SdkJ2MEService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for KVM memory usage.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkJ2MEService extends AbstractServiceAdapter
                            implements J2MEAdapter,
                                       DeviceJ2ME{


  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkJ2MEService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceJ2MEListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- no methods to implement for com.nokia.wtk.device.DeviceJ2ME ---

  // --- implementation of J2MEAdapter ---
  /**
   * Called when a the KVM memory has changed
   * @param memory   a reference to the DeviceMemory, that gives a memory snapshot
   *                 of the KVM.
   */
  public void KVMMemoryChanged( DeviceMemory memory ) {
    addCallback( KVM_MEMORY_CHANGED, new Object [] { memory } );
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

  private static final String KVM_MEMORY_CHANGED = "KVMMemoryChanged";

  private HashMap map;


}



