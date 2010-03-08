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

public class AllNaiveTests extends TestSuite {


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

  public AllNaiveTests() {
    this("all naive tests");
  }

  public AllNaiveTests(String s) {
    super(s);
    addTestSuite(com.symbian.tools.j2me.sei.emulator.TestLauncher_NAIVE_INTERACTIVE.class);
    //addTestSuite(com.symbian.tools.j2me.sei.emulator.TestLauncher_NAIVE_NONINTERACTIVE.class);
  }

  public static Test suite() {
    return new AllNaiveTests();
  }
}
