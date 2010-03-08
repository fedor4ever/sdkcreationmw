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
import com.symbian.vm.*;


public class RemoveCommand extends SessionCommand {
  //
  // Constants
  //

  /**
   * Remove command identifier
   */
  private static final int REMOVE_CMD_ID = 2;

  //
  // Members
  //

  /**
   * MIDlet suite information
   */
  private final MIDletSuiteInfo iSuiteInfo;

  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aSuiteInfo suite info
   */
  public RemoveCommand(MIDletSuiteInfo aSuiteInfo) {
    super(REMOVE_CMD_ID);
    iSuiteInfo = aSuiteInfo;
  }

  //
  // Operations
  //

  /**
   * Dump the object to the output stream
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException {
    super.dump(aDos);
    aDos.write((byte)1); // "command exits" flag necessary to fulfill protocol
    aDos.writeUTF(iSuiteInfo.getSuiteName());
    aDos.writeUTF(iSuiteInfo.getVendor());
    aDos.flush();
  }

}
