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

package com.symbian.tools.j2me.sei.emulator.session.naive;

import java.net.*;
import java.io.*;
import java.util.*;

import com.symbian.utils.*;
import com.symbian.tools.j2me.sei.preferences.*;
import com.symbian.tools.j2me.sei.emulator.commands.VMLaunchArguments;
import com.symbian.tools.j2me.sei.emulator.session.*;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;


import com.symbian.io.*;
import com.symbian.vm.*;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public class SessionImpl extends Session {

  //
  // Constants
  //

  /**
   * Naive session/device should be used for tests only. not for any SDK product.
   */
  private static final String WARNING =
      "\n#\n#WARNING!\n#Naive device/session should not be used for any SDK product!\n#it should be used for tests only.\n#\n";

  /**
   * Work folder under epoc32home/<platform>/
   */
  private static final String WORK_FOLDER = File.separator + "c" + File.separator;

  /**
   * Emulator C drive
   */
  private static final String C_DRIVE = "C:\\";

  /**
   * .exe
   */
  private static final String EXE = ".exe";

  /**
   * Directory under epoc32 home containing <platform>/<variant>/<product>
   */
  private static final String RELEASE_FOLDER = "release";

  //
  // Static Members
  //


  /**
   * Next port to check if free
   */
  private static int sSessionPorts = 52000;




  //
  // Life cycle
  //

  static{
    Debug.println(SessionImpl.class.getName(), WARNING);
    System.out.println(WARNING);
  }

  //
  // Operations
  //


  /**
   * Initialize the session.
   * The intialization code shouldnt be in the constructor to allow shutdown
   * during initialization, coming from the user.
   *
   * @param aUserPrefs user preferences used for initializing the connection
   *
   * @throws Exception if some error occured
   */
  public void initialize(UserPreferences aUserPrefs) throws Exception{
  }




  /**
   * Get a free port on the device
   *
   * @return int free port
   *
   * @throws Exception if some error occured
   */
  public int getFreePort() throws Exception {
    try {
      sSessionPorts =
          Utilities.getFreePort(InetAddress.getLocalHost(), sSessionPorts);
      return sSessionPorts;
    }
    finally {
      sSessionPorts = sSessionPorts + 1;
    }

  }


  /**
   * Start the MIDlet
   *
   * @param aMIDletInfo MIDlet to start
   *
   * @return VM process
   *
   * @throws Exception if some error occured
   */
  public VMProcess startMIDlet(VMLaunchArguments aVMLaunchArguments, boolean aDefered)
      throws Exception
  {
    //find VM exe
    File vmDir = new File(SDKRuntimeProperties.EPOC_HOME + File.separator +
                          RELEASE_FOLDER + File.separator +
                          SDKRuntimeProperties.PLATFORM + File.separator +
                          SDKRuntimeProperties.VARIANT);
    File vmFile = new File(vmDir, SDKRuntimeProperties.PRODUCT + EXE);
    if (!vmDir.exists() || !vmFile.exists() || !vmFile.isFile()) {
      throw new IllegalArgumentException("illegal VM executable: " + vmFile);
    }

    //create process
    Process win32Process = null;
    if (!aDefered)
    {
      win32Process = Runtime.getRuntime().exec
          (new ExecutableVMCommand(vmFile.getAbsolutePath(), aVMLaunchArguments).
           getCommandLine(),
           null,
           vmDir);
    }
    VMProcess process = new NaiveVMProcessImpl
        (win32Process, aVMLaunchArguments.getMIDletInfo().getMIDletSuiteInfo());
    iSuiteInfo2Process.put
        (aVMLaunchArguments.getMIDletInfo().getMIDletSuiteInfo(), process);

    return process;
  }

  /**
   * Start another MIDlet in the same suite VM which must be already running
   *
   * @param aMIDletInfo MIDlet info
   *
   * @throws Exception if some error occured
   */
  public void startMIDlet(MIDletInfo aMIDletInfo) throws Exception{
    throw new UnsupportedOperationException("Naive session does not support this option");
  }

  /**
   * Install the MIDlet suite
   *
   * @param aJar MIDlet suite Jar file
   * @param aJad MIDlet suite Jad file
   * @param aTrusted trust mode indicator
   *
   * @return MIDletSuiteInfo MIDlet suite information
   *
   * @throws Exception if some error occured
   */
  public MIDletSuiteInfo installMIDletSuite
      (JARFile aJar, JADFile aJad, boolean aTrusted) throws Exception
  {
    //work folder for creating files
    String destFolder = SDKRuntimeProperties.EPOC_HOME + File.separator
        + SDKRuntimeProperties.PLATFORM + WORK_FOLDER;
    Debug.println(getClass().getName(),
                  "local \"installation\" folder: " + destFolder);
    File tmp = new File(destFolder);
    if (!tmp.exists() || !tmp.isDirectory()) {
      throw new IllegalArgumentException
          ("illegal \"installation\" folder: " + destFolder);
    }

    //copy Jar and Jad to work folder
    Utilities.copyFiles(aJar.getFile().getPath(),
                        destFolder + aJar.getFile().getName());
    Utilities.copyFiles(aJad.getFile().getPath(),
                        destFolder + aJad.getFile().getName());

    //create suite and MIDlet info
    return new MIDletSuiteInfo(aJad.getFile().getName(),
                               aJar.getFile().getName(),
                               destFolder,
                               C_DRIVE);
  }



  /**
   * Remove the MIDlet suite
   *
   * @param aSuiteInfo removed MIDlet suite info
   *
   * @throws Exception if some error occured
   */
  public void removeMIDletSuite(MIDletSuiteInfo aSuiteInfo) throws Exception
  {
    new File(aSuiteInfo.getJadStorageName()).delete();
    new File(aSuiteInfo.getJarStorageName()).delete();
  }


  /**
   * Get the list of installed MIDlet suites
   *
   * @return list of installed MIDlet suites
   *
   * @throws Exception if some error occured
   */
  public MIDletSuiteInfo[] getInstalledMIDletSuitesInfo() throws Exception
  {
    throw new UnsupportedOperationException("get installed midlets is not supported by " + getClass());
  }

  /**
   * Brute force terminate the sessions
   */
  public void terminate(){
    for(Iterator iter = iSuiteInfo2Process.values().iterator();
        iter.hasNext(); ){
      ((VMProcess)iter.next()).destroy();
    }
  }


   public InetAddress getSlaveInetAddress() throws UnknownHostException{
	throw new RuntimeException("naive session not updated with 2 way connection");	
}



}



