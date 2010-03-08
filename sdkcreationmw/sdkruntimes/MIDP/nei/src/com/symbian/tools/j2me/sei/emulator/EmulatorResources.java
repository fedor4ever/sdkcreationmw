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

package com.symbian.tools.j2me.sei.emulator;


import java.util.*;

import java.io.*;
import java.awt.*;
import javax.swing.*;

import com.symbian.utils.Debug;


/**
 * Place holder for all the static UI resources which could be localized.
 * such as strings, icons etc..
 *
 * Static resources should have static references, disregarding their loading
 * mechanism.
 *
 * <p>Title: PREQ 425</p>
 * <p>Description: Java debugging</p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 * @since 1.1
 */
public class EmulatorResources
{
  //
  // Constants
  //

  /**
   * Properties file name
   */
  private static final String PROPERTIES_FILE = "uiresources.properties";


  //
  // Strings
  //


  /**
   * Error dialog title
   */
  public static String sErrorDialogTitle = "ERROR_DIALOG_TITLE";

  /**
   * SEI Command parse error message
   */
  public static String sParseErrorMessage = "FAILED_TO_PARSE";

  /**
   * SEI Command parse error message
   */
  public static String sCreateVMCommandErrorMessage = "FAILED_TO_CREATE_VM_COMMAND";

  /**
   * Jad Initialization error message
   */
  public static String sInitJadErrorMessage = "FAILED_TO_INITIALIZE_JAD";

  /**
   * Jar Initialization error message
   */
  public static String sInitJarErrorMessage = "FAILED_TO_INITIALIZE_JAR";

  /**
   * Emulator execution error message
   */
  public static String sExecErrorMessage = "FAILED_TO_EXECUTE";

  /**
   * Debugging the emulator execution error message
   */
  public static String sDebugErrorMessage = "FAILED_TO_DEBUG";

  /**
   * Failed to answer query error message
   */
  public static String sQueryErrorMessage = "FAILED_TO_ANSWER_QUERY";

  /**
   * Unknown error message
   */
  public static String sUnknownErrorMessage = "UNKNOWN_ERROR";

  /**
   * Caught exception
   */
  public static String sCaughtException = "CAUGHT_EXCEPTION";


  /**
   * Choose a MIDlet dialog message
   */
  public static String sChooseMIDletMessage = "CHOOSE_MIDLET";

  /**
   * MIDlet selection dialog title
   */
  public static String sChooseMIDletTitle = "CHOOSE_MIDLET_DIALOG_TITLE";

  /**
   * Wait for debugged VM dialog title
   */
  public static String sWaitForVmTitle = "WAIT_FOR_VM_DIALOG_TITLE";

  /**
   * Wait for debugged VM dialog message
   */
  public static String sWaitForVmMessage = "WAIT_FOR_VM_DIALOG_MESSAGE";

  /**
   * JadTool is not in classpath error messege
   */
  public static String sJadToolMissing = "JADTOOL_MISSING";

  /**
   * Status bar progress message
   */
  public static String sOnProgress= "ON_PROGRESS";

  /**
   * Abort
   */
  public static String sAbort = "ABORT";

  /**
   * Hide
   */
  public static String sHide = "HIDE";

  /**
   * View frame title
   */
  public static String sViewTitle = "VIEW_TITLE";

  /**
   * Failed loading preferences
   */
  public static String sPrefsLoadFaild = "FAILED_TO_LOAD_PREFS";

  /**
   * Loading preferences
   */
  public static String sReadPrefs = "READ_USER_PREFS";

  /**
   * Loading preferences failed
   */
  public static String sReadPrefsFailed = "READ_USER_PREFS";

  /**
   * Create VM args
   */
  public static String sCreateVMArgs = "CREATE_VM_ARGS";

  /**
   * Create Jad
   */
  public static String sCreateJad = "CREATE_JAD";

  /**
   * Resolve application name failed
   */
  public static String sResolveAppFailed = "RESOLVE_APP_FAILED";

  /**
   * Resolve application name
   */
  public static String sResolveApp = "RESOLVE_APP";

  /**
   * Create Jar
   */
  public static String sCreateJar = "CREATE_JAR";

  /**
   * Sign suite failed
   */
  public static String sSignJadFailed = "SIGN_JAD_FAILED";

  /**
   * Sign suite
   */
  public static String sSignJad = "SIGN_JAD";

  /**
   * Init session failed
   */
  public static String sInitSessionFailed = "INIT_SESSION_FAILED";

  /**
   * Init session
   */
  public static String sInitSession = "INIT_SESSION";

  /**
   * Init session ports failed
   */
  public static String sInitPortsFailed = "INIT_PORTS_FAILED";

  /**
   * Init ports
   */
  public static String sInitPorts = "INIT_PORTS";

  /**
   * Launch session
   */
  public static String sLaunchSessions = "LAUNCH_SESSION";

  /**
   * Debug session
   */
  public static String sDebugSession = "DEBUG_SESSION";

  /**
   * Status bar - Done
   */
  public static String sDone = "DONE";

  /**
   * Status bar - Error
   */
  public static String sError = "ERROR";

  /**
   * Add MIDlet popup label
   */
  public static String sAddMIDletLabel = "CHOOSE_A_MIDLET";

  /**
   * Add MIDlet popup title
   */
  public static String sAddMIDletTitle = "ADD_MIDLET";

  /**
   * Add MIDlet button label
   */
  public static String sAdd__ = "ADD__";

  //
  // Icons
  //

  /**
   * Pointer icon
   */
  public static ImageIcon sPointerImage = new ImageIcon();

  /**
   * X icon
   */
  public static ImageIcon sXImage = new ImageIcon();

  /**
   * V icon
   */
  public static ImageIcon sVImage = new ImageIcon();

  /**
   * Empty icon
   */
  public static ImageIcon sEmptyImage = new ImageIcon();


  //
  // Static initializer
  //




  static
  {
    try
    {
      //strings
      Properties props = new Properties();
      props.load(EmulatorResources.class.getResourceAsStream(PROPERTIES_FILE));
      sErrorDialogTitle = props.getProperty(sErrorDialogTitle);
      sParseErrorMessage = props.getProperty(sParseErrorMessage);
      sInitJadErrorMessage = props.getProperty(sInitJadErrorMessage);
      sInitJarErrorMessage = props.getProperty(sInitJarErrorMessage);
      sExecErrorMessage = props.getProperty(sExecErrorMessage);
      sDebugErrorMessage = props.getProperty(sDebugErrorMessage);
      sQueryErrorMessage = props.getProperty(sQueryErrorMessage);
      sUnknownErrorMessage = props.getProperty(sUnknownErrorMessage);
      sCaughtException = props.getProperty(sCaughtException);
      sChooseMIDletMessage = props.getProperty(sChooseMIDletMessage);
      sChooseMIDletTitle = props.getProperty(sChooseMIDletTitle);
      sCreateVMCommandErrorMessage = props.getProperty(sCreateVMCommandErrorMessage);
      sWaitForVmTitle = props.getProperty(sWaitForVmTitle);
      sWaitForVmMessage = props.getProperty(sWaitForVmMessage);
      sJadToolMissing = props.getProperty(sJadToolMissing);

      sAbort = props.getProperty(sAbort);
      sHide = props.getProperty(sHide);
      sViewTitle = props.getProperty(sViewTitle);
      sPrefsLoadFaild = props.getProperty(sPrefsLoadFaild);
      sReadPrefs = props.getProperty(sReadPrefs);
      sCreateVMArgs = props.getProperty(sCreateVMArgs);
      sCreateJad = props.getProperty(sCreateJad);
      sResolveAppFailed = props.getProperty(sResolveAppFailed);
      sResolveApp = props.getProperty(sResolveApp);
      sCreateJar = props.getProperty(sCreateJar);
      sSignJadFailed = props.getProperty(sSignJadFailed);
      sSignJad = props.getProperty(sSignJad);
      sInitSessionFailed = props.getProperty(sInitSessionFailed);
      sInitSession = props.getProperty(sInitSession);
      sInitPortsFailed = props.getProperty(sInitPortsFailed);
      sInitPorts = props.getProperty(sInitPorts);
      sLaunchSessions = props.getProperty(sLaunchSessions);
      sDebugSession = props.getProperty(sDebugSession);
      sDone = props.getProperty(sDone);
      sError = props.getProperty(sError);
      sAddMIDletLabel = props.getProperty(sAddMIDletLabel);
      sAddMIDletTitle = props.getProperty(sAddMIDletTitle);
      sAdd__ = props.getProperty(sAdd__);

      //images
      loadImage(sPointerImage, "pointer.gif");
      loadImage(sXImage, "x.gif");
      loadImage(sVImage, "v.gif");
      loadImage(sEmptyImage, "empty.gif");
    }
    catch (Throwable e)
    {
      //btw, string are already initialized to hard coded default.
      Debug.printStackTrace(EmulatorResources.class.getName(), e);
    }
  }


  /**
   * Load the image manually to avoid Swing/AWT loading mechanism.
   *
   * @param aIcon Image icon
   * @param aName resource name
   * @throws IOException if some error occured
   */
  private static void loadImage(ImageIcon aIcon, String aName) throws IOException{
    InputStream is = EmulatorResources.class.getResourceAsStream(aName);
    byte[] data = new byte[is.available()];
    is.read(data);
    aIcon.setImage(Toolkit.getDefaultToolkit().createImage(data));
    is.close();
  }
}
