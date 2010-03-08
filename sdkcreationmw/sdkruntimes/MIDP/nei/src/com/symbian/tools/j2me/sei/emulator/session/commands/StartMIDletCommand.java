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
import com.symbian.vm.MIDletInfo;

public class StartMIDletCommand extends SessionCommand{

  //
  // Constants
  //

  /**
   * Start MIDlet commmand ID
   */
  public static final int START_MIDLET_CMD_ID = 3;


  //
  // Members
  //


  /**
   * MIDlet to add to a running VM
   */
  private final MIDletInfo iMIDletInfo;


  //
  // Life cycle
  //

  /**
   * Constructor for adding a MIDlet to an already running VM
   *
   * @param aMIDletInfo MIDlet to add
   */
  public StartMIDletCommand(MIDletInfo aMIDletInfo){
    super(START_MIDLET_CMD_ID);
    iMIDletInfo = aMIDletInfo;
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
    iMIDletInfo.dump(aDos); // msid, mid
    aDos.writeUTF(""); // command line necessary to fulfill protocol
    aDos.write((byte)0); // defer flag necessary to fulfill protocol
    aDos.flush();
  }
}