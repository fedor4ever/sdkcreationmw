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
import com.symbian.utils.*;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;


/**
 * All launcher tests - Interactive and Non-Interactive
 */
public class AllTests extends TestSuite {
  public static final String NAIVE_SESSION = "-Xdevice:naive";
  public static final String WIN32_SESSION = "-Xdevice:win32";
  public static final String TARGET_SESSION = "-Xdevice:target";


  static{
    com.symbian.utils.SystemExitHandler.setExitOperation(false);
  }


  public AllTests() {
    this("All launcher tests");
  }

  public AllTests(String s) {
    super(s);
    addTest(com.symbian.tools.j2me.sei.emulator.AllNaiveTests.suite());
    addTest(com.symbian.tools.j2me.sei.emulator.AllWin32Tests.suite());
    addTest(com.symbian.tools.j2me.sei.emulator.AllTargetTests.suite());
  }

  public static Test suite() {
    return new AllTests();
  }
}


