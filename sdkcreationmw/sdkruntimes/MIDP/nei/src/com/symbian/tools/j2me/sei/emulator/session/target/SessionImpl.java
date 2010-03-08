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

package com.symbian.tools.j2me.sei.emulator.session.target;


import com.symbian.tools.j2me.sei.emulator.session.*;
import com.symbian.tools.j2me.sei.emulator.session.commands.HandshakeCommand;
import com.symbian.tools.j2me.sei.preferences.*;
import java.io.*;

public class SessionImpl extends RemoteSession {


  /**
   * Initialize the session.
   * The intialization code shouldnt be in the constructor to allow shutdown
   * during initialization, coming from the user.
   *
   * @param aUserPrefs user preferences used for initializing the connection
   *
   * @throws Exception if some error occured
   */
  public synchronized  void initialize(UserPreferences aUserPrefs) throws Exception{
    super.initialize(aUserPrefs);
    boolean connected = false;
    while (!connected) {
      //TODO: apply some timeout
      //exit from this loop after connected or by throwing exception
      try {
        connectToSlave();
        doHandshake(aUserPrefs.getSecurityPreferences().getChallenge());
        connected = true;
      }
      catch (IOException ioe) {
      }
    }
  }



}

