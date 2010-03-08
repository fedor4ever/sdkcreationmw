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

import java.io.*;
import com.symbian.vm.MIDletSuiteInfo;

public class ListSuitesCommand extends SessionCommand{

  //
  // Constants
  //

  /**
   * List MIDlet request identifier
   */
  private static final int LIST_CMD_ID = 5;


  //
  // Members
  //

  private MIDletSuiteInfo[] iInstalledSuites;

  /**
   * Necessary for recursive command. Framework only provides
   * DataInputStream in load but here we need the associated
   * DataOutputStream as well. The solution is to cache the
   * DataOutputStream in the call to dump().
   */
  DataOutputStream iDos = null;

  //
  // Life cycle
  //

  /**
   * Default constructor.
   */
  public ListSuitesCommand(){
    super(LIST_CMD_ID);
  }

  //
  // Operations
  //


  /**
   * Get installed suites
   *
   * @return array of installed suites info
   */
  public MIDletSuiteInfo[] getInstalledSuites(){
    return iInstalledSuites;
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
    iDos = aDos;
  }

  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException{
    super.load(aDis);
    // first we get a list of suite IDs
    int count = aDis.readInt();
    int[] ids = new int [count];
    for (int i = 0; i < count; i++) {
      ids[i] = aDis.readInt();
    }
    iInstalledSuites = new MIDletSuiteInfo[count];
    // now we get each suite's info
    for (int i = 0; i < count; i++) {
      GetSuiteInfoCommand command = new GetSuiteInfoCommand(ids[i]);
      command.execute(aDis,iDos);
      iInstalledSuites[i] = command.getSuiteInfo();
    }
  }
}