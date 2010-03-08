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
import com.symbian.vm.*;

/**
 * Command to terminate a SINGLE MIDlet.
 * On SOS>=9.1 (EKA2 and PlatSec) we have to close all MIDlets and let the VM
 * exit on its own.
 */
public class TerminateCommand
    extends SessionCommand {

  //
  // Constants
  //

  /**
   * Indicates the agent to kill the VM
   * see note above.
   */
  private static final int TERMINATE_VM = 1;

  /**
   * Terminate MIDlet request identifier
   */
  private static final int TERMINATE_CMD_ID = 4;

  /**
   * Activee MIDlet info
   */
  private final MIDletInfo iMIDletInfo;

  //
  // Life cycle
  //

  /**
   * Default constructor.
   */
  public TerminateCommand(MIDletInfo aMIDletInfo) {
    super(TERMINATE_CMD_ID);
    iMIDletInfo = aMIDletInfo;
  }

  /**
   * Dump the object to the output stream
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException {
    super.dump(aDos);
    aDos.write(TERMINATE_VM);
    iMIDletInfo.dump(aDos);
    aDos.flush();
  }

}
