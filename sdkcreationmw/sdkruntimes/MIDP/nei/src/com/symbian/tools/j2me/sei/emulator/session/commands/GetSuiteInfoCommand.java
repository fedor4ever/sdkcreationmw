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
import com.symbian.io.*;
import com.symbian.vm.MIDletSuiteInfo;

public class GetSuiteInfoCommand extends SessionCommand{

  //
  // Constants
  //

  /**
   * List MIDlet request identifier
   */
  private static final int INFO_CMD_ID = 6;


  //
  // Members
  //

  private MIDletSuiteInfo iSuiteInfo;

  /**
   * The ID of the MIDlet suite we are requesting info for.
   */
  int iId;

  //
  // Life cycle
  //

  /**
   * Default constructor.
   */
  public GetSuiteInfoCommand(int aId){
    super(INFO_CMD_ID);
  }

  //
  // Operations
  //


  /**
   * Get installed suites
   *
   * @return array of installed suites info
   */
  public MIDletSuiteInfo getSuiteInfo(){
    return iSuiteInfo;
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
    aDos.writeInt(iId);
  }

  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException{
    super.load(aDis);
    String info = aDis.readUTF();
    JADFile jad = new JADFile(info,false);
    // this is jad
    iSuiteInfo = new MIDletSuiteInfo(jad);
    iSuiteInfo.setId(iId);
  }

}
