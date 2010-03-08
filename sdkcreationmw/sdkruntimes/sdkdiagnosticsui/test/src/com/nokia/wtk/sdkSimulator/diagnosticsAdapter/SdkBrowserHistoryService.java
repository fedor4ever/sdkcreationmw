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
import com.nokia.wtk.device.DeviceBrowser;
import com.nokia.wtk.device.DeviceBrowserHistoryListener;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceContentReceived;

/**
 * <p>Title: SdkBrowserHistoryService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for Browser history monitoring.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkBrowserHistoryService extends AbstractServiceAdapter
                                          implements BrowserHistoryAdapter,
                                                     DeviceBrowser {


  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkBrowserHistoryService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceBrowserHistoryListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }


  // --- implementation of BrowserHistoryAdpater interface ---
  /**
   * Called when a new document URL has been pushed onto the history stack.
   * It is expected that the most recently received content, whose request URL
   * matches the URL parameter in this method, is the same content that was pushed.
   * @param url       the request URL of the item that was pushed onto the history stack.
   */
  public void historyPushed(String url) {
    addCallback( HISTORY_PUSHED, new Object [] { url } );
  }

  /**
   * Called when a document has been reloaded,
   * either explicitly or as a side-effect of history navigation.
   * It is expected that the most recently received content, whose request URL
   * matches the URL parameter in this method, is the same content that was reloaded.
   * @param url       the request URL of the item that was reloaded.
   */
  public void historyRefresh(String url) {
    addCallback( HISTORY_REFRESHED, new Object [] { url } );
  }

  /**
   * Called when the top of the history stack has been popped.
   */
  public void historyPopped() {
    addCallback( HISTORY_POPPED, new Object [0] );
  }

  /**
   * Called when history stack position index has been decremented without a 'pop'.
   */
  public void historyBack() {
    addCallback( HISTORY_BACK, new Object [0] );
  }


  /**
   * Called when history stack position index has been incremented without a 'push'.
   */
  public void historyForward() {
    addCallback( HISTORY_FORWARD, new Object [0] );
  }

  /**
   * Called when the history stack has been cleared.
   */
  public void historyCleared() {
    addCallback( HISTORY_CLEARED, new Object [0] );
  }

  /**
   * Called when the bottom-most item in the history stack has been removed.
   */
  public void historyTrimmed() {
    addCallback( HISTORY_TRIMMED, new Object [0] );
  }


  //--- implementation of com.nokia.wtk.device.DeviceBrowser ---
  /**
   * Loads a new document.
   *
   * @param url       the document location.
   * @param nocache   if <code>true</code> then ignore possibly-cached content.
   */
  public void load(String url, boolean nocache) {
  }

  /**
   * Reloads the current document, ignoring possibly-cached content.
   * This operation must not change the state of browser history,
   * i.e. no "push" or "pop" events.
   *
   * @exception       UnsupportedOperationException
   *                  if this method is not supported.
   *                  Implementations that do not support this method
   *                  should explicitly declare this exception.
   */
  public void reload() {
  }

   /**
    * Navigates back to the previous document in the browser history stack.
    * This operation may or may not result in a
    * {@link DeviceBrowserHistoryListener#historyPopped} notification.
    *
    * @exception       UnsupportedOperationException
    *                  if this method is not supported.
    *                  Implementations that do not support this method
    *                  should explicitly declare this exception.
    */
   public void back() {
   }

   /**
    * Navigates forward to the next document in the browser history stack.
    *
    * @exception       UnsupportedOperationException
    *                  if this method is not supported.
    *                  Implementations that do not support this method
    *                  should explicitly declare this exception.
    */
   public void forward() {
   }

   /**
    * Halts any current browser activity.
    * (e.g. pending load operation, script execution, timers)
    */
   public void stop() {
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

  private static final String HISTORY_PUSHED = "historyPushed";
  private static final String HISTORY_REFRESHED = "historyRefreshed";
  private static final String HISTORY_POPPED = "historyPopped";
  private static final String HISTORY_BACK = "historyBack";
  private static final String HISTORY_FORWARD = "historyForward";
  private static final String HISTORY_CLEARED = "historyCleared";
  private static final String HISTORY_TRIMMED = "historyTrimmed";

  private HashMap map;

}
