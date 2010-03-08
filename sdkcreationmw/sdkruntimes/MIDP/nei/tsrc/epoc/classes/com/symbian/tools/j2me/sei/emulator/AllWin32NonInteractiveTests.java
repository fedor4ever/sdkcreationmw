/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import junit.framework.*;
import com.symbian.utils.Debug;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

public class AllWin32NonInteractiveTests extends TestSuite {


  static{
    //prevent calls to System.exit
    com.symbian.utils.SystemExitHandler.setExitOperation(false);
    Debug.init(SDKRuntimeProperties.SDK_HOME + java.io.File.separator + "testslogs");
    //ADD to history log file
    Debug.append("testshistory",
                 "\r\n" +
                 new java.util.Date(System.currentTimeMillis()).toString()
                 + ":\r\n");
    Debug.append("testshistory", "\r\n");

  }

  public AllWin32NonInteractiveTests() {
    this("all win32 NON INTERACTIVE tests");
  }

  public AllWin32NonInteractiveTests(String s) {
    super(s);
    addTestSuite(com.symbian.tools.j2me.sei.emulator.session.win32.TestWin32SessionImpl.class);
    addTestSuite(com.symbian.tools.j2me.sei.emulator.TestLauncher_WIN32_NONINTERACTIVE.class);
  }

  public static Test suite() {
    return new AllWin32NonInteractiveTests();
  }
}
