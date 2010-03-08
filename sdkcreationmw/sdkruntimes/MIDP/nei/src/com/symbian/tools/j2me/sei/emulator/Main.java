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

package com.symbian.tools.j2me.sei.emulator;

import com.symbian.tools.j2me.sei.SDKMainBase;
import com.symbian.tools.j2me.sei.emulator.view.*;
import com.symbian.tools.j2me.sei.emulator.event.*;
import com.symbian.utils.*;
import javax.swing.UIManager;


/**
 * SEI emulator application entry point.
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class Main extends SDKMainBase
{



  //
  // Life cycle
  //

  /**
   * Application entry point
   *
   * @param aArgs IDE's command line arguments
   */
  public static void main(String[] aArgs)
  {
    //init logging
    initDebugLogs(aArgs);

    //launch
    try
    {
      UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
      EmulatorLauncher launcher = new EmulatorLauncher();
      EmulatorLauncherView view = new EmulatorLauncherView(launcher);
      Utilities.centerWindow(view);
      launcher.setListener(view);
      launcher.launch(aArgs);
    }
    catch (Throwable e)
    {
      Debug.println(Main.class.getName(), e);
      SystemExitHandler.invokeSystemExit(-41);
    }
  }




}
