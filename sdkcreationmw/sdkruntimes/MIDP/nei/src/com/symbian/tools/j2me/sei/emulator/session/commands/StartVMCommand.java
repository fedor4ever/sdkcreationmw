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
import com.symbian.tools.j2me.sei.emulator.commands.VMLaunchArguments;

public class StartVMCommand extends SessionCommand {
  //
  // Constants
  //

  /**
   * Start command identifier
   */
  private static final int START_CMD_ID = 3;


  //
  // Members
  //


  /**
   * VM launch arguments
   */
  private final VMLaunchArguments iVMLaunchArguments;

  /**
   * VM defered launch indicator
   */
  private final boolean iDefered;



  //
  // Life cycle
  //

  /**
   * Constructor for launching a MIDlet
   *
   * @param aVMLaunchArguments VM command line arguments
   */
  public StartVMCommand(VMLaunchArguments aVMLaunchArguments,
                      boolean aDefered) {
    super(START_CMD_ID);
    iVMLaunchArguments = aVMLaunchArguments;
    iDefered = aDefered;
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
  public void dump(DataOutputStream aDos) throws IOException{
    super.dump(aDos);
    //msid, mid, !defered
    iVMLaunchArguments.dump(aDos);
    aDos.write(iDefered? 1 : 0);
    aDos.flush();
  }



}
