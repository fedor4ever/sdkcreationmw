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



import java.util.*;

import com.symbian.utils.*;
import com.symbian.vm.*;
import com.symbian.tools.j2me.sei.emulator.commands.VMLaunchArguments;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

class ExecutableVMCommand {

  //
  // Members
  //

  /**
   * VM exe file
   */
  private String iVMExe;

  /**
   * VM launch arguments
   */
  private VMLaunchArguments iVMLaunchArguments;


  //
  // Life cycle
  //

  public ExecutableVMCommand(String aVMExe,
                             VMLaunchArguments aVMArgs)
  {
    iVMExe = aVMExe;
    iVMLaunchArguments = aVMArgs;
  }

  //
  // Operations
  //


  /**
   * Returns an executable command line string
   *
   * @return String executable command line string
   */
  public String getCommandLine()
  {
    //VM, AMS, Jar, Jar, MIDlet
    StringBuffer str = new StringBuffer
        (iVMLaunchArguments.getVMArguments().getArguments());
    str.append(" ").append
        (iVMLaunchArguments.getMIDletSuiteAMSArguments().getArguments());
    str.append(" -jad ").append(iVMLaunchArguments.getMIDletInfo().
                           getMIDletSuiteInfo().getJadStorageName());
    str.append(" -jar ").append(iVMLaunchArguments.getMIDletInfo().
                           getMIDletSuiteInfo().getJarStorageName());

    StringBuffer line = new StringBuffer();
    line.append(iVMExe).append(" ");
    StringTokenizer tokenizer = new StringTokenizer(str.toString());
    while(tokenizer.hasMoreTokens())
    {
      line.append(tokenizer.nextToken()).append(" ");
    }
    //command line VM uses the MIDlet name
    line.append(iVMLaunchArguments.getMIDletInfo().getMIDletName());
    Debug.println(this, "command line: " + line.toString());
    return line.toString();
  }

}



