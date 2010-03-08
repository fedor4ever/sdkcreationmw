/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.preferences;

import java.io.*;
import java.util.*;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;
import com.symbian.utils.Debug;

public class KDPPreferences implements Serializable {

  public static final String EMULATOR_SESSION_NAME = "S60Emulator";
  public static final String DEVICE_SESSION_NAME = "S60Device";
  
  //
  // Constant default values
  // (used in setDefaultValues)
  //


  //Dima private static final int DEF_CONNECTION_ATTEMPTS = 60;
  private static final int DEF_CONNECTION_ATTEMPTS = 1060;
  private static final boolean DEF_ENABLE_ATTACHING = false;
  private static final boolean DEF_DEFER_LAUNCH = false;
  private static final int DEF_KDP_ATTACH_PORT = 1168;
  private static final boolean DEF_EKA2_EMUL_DISCOVER = true;
  private static final String SESSION_PREFS_FILE = "sessions.properties";


  //
  // Members
  //

  /**
   * Number of attempts to connect to the VM.
   */
  private int iAttemptsCounter;

  /**
   * Indicates if to enable attaching to the VM when launching a MIDlet.
   * That means the VM starts in non-suspended mode and the KDP agent will
   * connect to it.
   */
  private boolean iEnableAttachingToVM;

  /**
   * Indicates not to launch the VM.
   * This is used for debugging pushed MIDlets.
   */
  private boolean iDeferLaunchingVM;

  /**
   * Port to be used for VM when launch in "attaching enabled" mode.
   * In this case, we choose it not by any SEI command line parameters.
   */
  private int iAttachedVMKDPPort;

  /**
   * Session names to SessionPreferences table
   */
  private HashMap iSessionNames2Preferences;

  /**
   * EKA2 reusability indicator.
   * On EKA2 we can reuse the emulator for future sessions instead of launching
   * it every time. That should save the use the startup time.
   */
  private boolean iEKA2EmulatorDiscovery = DEF_EKA2_EMUL_DISCOVER;




  //
  // Life cycle
  //



  /**
   * Default constructor.
   */
  public KDPPreferences() {
    setDefaultValues();
  }


  //
  // Operations
  //


  /**
   * Set default values
   */
  protected void setDefaultValues(){
    iAttemptsCounter = DEF_CONNECTION_ATTEMPTS;
    iEnableAttachingToVM = DEF_ENABLE_ATTACHING;
    iDeferLaunchingVM = DEF_DEFER_LAUNCH;
    iAttachedVMKDPPort = DEF_KDP_ATTACH_PORT;
    iEKA2EmulatorDiscovery = DEF_EKA2_EMUL_DISCOVER;

    //init Session preferences
    initSessionPreferences();
  }


  /**
   * Initialize Session preferences with default values.
   *
   * CUSTOMISATION NOTE: This METHOD is intended to be used for customization
   * TODO: Licensees should use this method to init implemented sessions data
   */
  private void initSessionPreferences(){
    iSessionNames2Preferences = new HashMap();
    Properties prefs = new Properties();
    SessionPreferences win32Prefs = new SessionPreferences();
    SessionPreferences targetPrefs = new SessionPreferences();
    try
    {
      prefs.load(getClass().getResourceAsStream(SESSION_PREFS_FILE));
      win32Prefs.setHost((String)prefs.get("win32.agent.host"));
      win32Prefs.setPort(Integer.parseInt((String)prefs.get("win32.agent.port")));
      win32Prefs.setLocalPort(Integer.parseInt((String)prefs.get("win32.port")));
      win32Prefs.setOutgoing(
          Boolean.valueOf( (String) prefs.get("win32.outgoing")).booleanValue());
      targetPrefs.setHost((String)prefs.get("target.agent.host"));
      targetPrefs.setPort(Integer.parseInt((String)prefs.get("target.agent.port")));
      targetPrefs.setLocalPort(Integer.parseInt((String)prefs.get("target.port")));
      targetPrefs.setOutgoing(
          Boolean.valueOf( (String) prefs.get("target.outgoing")).booleanValue());
    }
    catch(Exception e){
      Debug.printStackTrace(this, e);
    }
    finally{
      iSessionNames2Preferences.put(EMULATOR_SESSION_NAME, win32Prefs);
      iSessionNames2Preferences.put(DEVICE_SESSION_NAME, targetPrefs);

    }
  }

  /**
   * Get max VM connection attempts
   *
   * @return max VM connection attempts
   */
  public int getConnectionAttempts(){
    return iAttemptsCounter;
  }


  /**
   * Set max VM connection attempts
   * Calculate number of connection attempts based on the fact that
   * Sun's KDP was changed to connect every 1000ms.
   *
   * @param aTimeout max connection timeout in seconds.
   */
  public void setConnectionTimeout(int aTimeoutSecs){
    iAttemptsCounter = aTimeoutSecs;
  }

  /**
   * Get max VM connection attempts
   * Calculate number of connection attempts based on the fact that
   * Sun's KDP was changed to connect every 1000ms.
   *
   * @return max connection timeout in seconds.
   */
  public int getConnectionTimeout(){
    return iAttemptsCounter;
  }

  /**
   * Indicates if VM launching is defered
   *
   * @return VM launching defered indicator
   */
  public boolean isVMLaunchingDeferred(){
    return iDeferLaunchingVM;
  }

  /**
   * Indicates if attaching to VM is enabled
   *
   * @return attaching to VM indicator
   */
  public boolean isAttachingToVMEnabled(){
    return iEnableAttachingToVM;
  }

  /**
   * Set VM launching defered indicator
   *
   * @param aIsDefered new VM launching defered indicator
   */
  public void setVMLaunchingDeferred(boolean aIsDeferred){
    iDeferLaunchingVM = aIsDeferred;
  }

  /**
   * Set enable attaching to VM indicator
   *
   * @param aIsAttachingEnabled new enable attaching to VM indicator
   */
  public void setAttachingToVMEnabled(boolean aIsAttachingEnabled){
    iEnableAttachingToVM = aIsAttachingEnabled;
  }

  /**
   * Set VM debug port when attaching is enabled
   *
   * @param aPort VM debug port when attaching is enabled
   */
  public void setAttachedVMKDPPort(int aPort){
    iAttachedVMKDPPort = aPort;
  }

  /**
   * Get VM debug port when attaching is enabled
   *
   * @return VM debug port when attaching is enabled
   */
  public int getAttachedVMKDPPort(){
    return iAttachedVMKDPPort;
  }

  /**
   * Indicates if should attempt EKA2 emulator discovery method
   *
   * @return EKA2 emulator discovery method indicator
   */
  public boolean isEKA2EmulatorDiscovery(){
    return iEKA2EmulatorDiscovery;
  }

  /**
   * Indicate if to attempt EKA2 emulator discovery method
   *
   * @param aEKA2EmulatorDiscovery EKA2 emulator discovery method indicator
   */
  public void setEKA2EmulatorDiscovery(boolean aEKA2EmulatorDiscovery){
    iEKA2EmulatorDiscovery = aEKA2EmulatorDiscovery;
  }


  /**
   * Get session preferences
   * @param aSessionName session name
   * @return session preferences
   */
  public SessionPreferences getSessionPreferences(String aSessionName){
  	return (SessionPreferences)iSessionNames2Preferences.get(aSessionName);
  }

}
