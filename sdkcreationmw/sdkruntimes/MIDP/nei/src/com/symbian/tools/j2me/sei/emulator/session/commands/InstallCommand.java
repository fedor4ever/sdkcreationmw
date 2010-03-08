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

package com.symbian.tools.j2me.sei.emulator.session.commands;

import com.symbian.io.*;
import java.io.*;
import com.symbian.io.*;
import com.symbian.vm.*;

public class InstallCommand extends SessionCommand {


  //
  // Constants
  //

  /**
   * Install commmand ID
   */
  public static final int INSTALL_CMD_ID = 1;


  //
  // Members
  //

  /**
   * Trust mode indicator
   */
  private final boolean iTrusted;

  /**
   * Suite Jad file
   */
  private final JADFile iJad;

  /**
   * Suite jar file
   */
  private final JARFile iJar;

  /**
   * Installed suite info
   */
  private MIDletSuiteInfo iInstalledSuiteInfo;

  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aTrusted trust mode indicator
   * @param aJad jad file
   * @param aJar jar file
   */
  public InstallCommand(boolean aTrusted, JADFile aJad, JARFile aJar){
    super(INSTALL_CMD_ID);
    iTrusted = aTrusted;
    iJad = aJad;
    iJar = aJar;
  }

  //
  // Operations
  //


  /**
   * Get the suite info
   *
   * @return suite info
   */
  public MIDletSuiteInfo getMIDletSuiteInfo(){
    return iInstalledSuiteInfo;
  }

  /**
   * Dump the object to the output stream
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException{
    super.dump(aDos);
    //trsuted, jad, jar
    aDos.write(iTrusted? 1 : 0);
    aDos.writeUTF(iJad.getFile().getName());
    aDos.writeShort((short)iJad.getFile().length());
    FileInputStream fis = new FileInputStream(iJad.getFile());
    StreamsPiper piper = new StreamsPiper(fis, aDos);
    piper.pipeAll();
    fis.close();
    aDos.writeUTF(iJar.getFile().getName());
    aDos.writeInt((int)iJar.getFile().length());
    fis = new FileInputStream(iJar.getFile());
    piper.setInputStream(fis);
    piper.pipeAll();
    fis.close();
    aDos.flush();
  }



  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException {
    super.load(aDis);
    iInstalledSuiteInfo = new MIDletSuiteInfo(iJad);
    iInstalledSuiteInfo.load(aDis);
  }
}

