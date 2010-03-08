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

package com.symbian.tools.j2me.sei.emulator.session.win32;

import java.io.*;
import java.net.*;

import com.symbian.tools.j2me.sei.SDKRuntimeProperties;
import com.symbian.tools.j2me.sei.preferences.*;

import com.symbian.tools.j2me.sei.emulator.session.*;
import com.symbian.tools.j2me.sei.emulator.session.commands.*;
import com.symbian.vm.*;
import com.symbian.tools.j2me.sei.emulator.commands.*;
import com.symbian.io.*;
import com.symbian.utils.Debug;

public class SessionImpl extends RemoteSession{

  //
  // Constants
  //

  /**
   * Directory under epoc32 home containing <platform>/<variant>/<product>
   */
  private static final String RELEASE_FOLDER = "release";


  /**
   * SEI-Slave win32 exe
   */
  private static final String SEI_SLAVE_APP = "DebugAgent.exe";

  //
  // Members
  //

  /**
   * Slave win32 process
   */
  private Process iSlaveProcess;

  /**
   * On EKA2 we can reuse the emulator for future sessions.
   */
  private boolean iEKA2EmulatorDiscovery = false;




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
  public synchronized void initialize(UserPreferences aUserPrefs) throws Exception{
    super.initialize(aUserPrefs);
    iEKA2EmulatorDiscovery = aUserPrefs.getKDPPreferences().isEKA2EmulatorDiscovery();
    //handle emulator discovery
    if(iEKA2EmulatorDiscovery){
      Debug.println(this, "detected emulator keep alive mode");
      connectToSlave();
    }
    else{
      //Incoming && !Keep-Alive is not supported
      if(!iPrefs.isOutgoing()){
        throw new IllegalStateException("Incoming connection from emulator is supported only with keep alive mode");
      }
      else{
      //launch win32 process if needed
      launchSlaveProcess(" " + iPrefs.getPort());
      boolean connected = false;
      while (!connected) {
        //TODO: apply some timeout
        //exit from this loop after connected or by throwing exception
        try
        {
          connectToSlave();
          connected = true;
        }
        catch (IOException ioe)
        {
          try
          {
            //will throw IllegalThreadStateException if process is still alive
            iSlaveProcess.exitValue();
            throw new IllegalStateException("win32 process is not alive");
          }
          catch (IllegalThreadStateException ie)
          {
          }
        }
      }
      }
    }
  }

  /**
   * Launch the SEI-Slave win32 executable
   *
   * @param aPort slave port argument
   * @throws FileNotFoundException if could not find the exe file
   * @throws IOException if some error occured
   */
  private void launchSlaveProcess(String aPortArg) throws IOException{
    //find exe
    File slaveDir = new File(SDKRuntimeProperties.EPOC_HOME + File.separator +
                             RELEASE_FOLDER + File.separator +
                             SDKRuntimeProperties.PLATFORM + File.separator +
                             SDKRuntimeProperties.VARIANT);
    File app = new File(slaveDir, SEI_SLAVE_APP);
    if (!app.exists() || !app.isFile()) {
      throw new FileNotFoundException
          ("cannot find debug agent win32 exe: " + app.getAbsolutePath());
    }

    //create process
    iSlaveProcess = Runtime.getRuntime().exec
        (app.getAbsolutePath() + aPortArg, null , slaveDir);
  }


  /**
   * Terminate the session.
   */
  public synchronized void terminate() {
    super.terminate(); //call super only in SOS > 9.1
    if (!iEKA2EmulatorDiscovery) {
      iSlaveProcess.destroy();
    }
  }

}

