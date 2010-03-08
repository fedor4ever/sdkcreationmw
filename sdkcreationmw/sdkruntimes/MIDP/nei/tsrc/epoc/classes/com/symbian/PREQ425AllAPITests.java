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

package com.symbian;

import com.symbian.utils.*;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

import junit.framework.*;

/**
 * Conatains only the API tests.
 * No session tests.
 *
 * <p>Title: PREQ 425</p>
 * <p>Description: Java debugging</p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class PREQ425AllAPITests extends TestCase {


  static{
    //init debug log folder
    Debug.init(SDKRuntimeProperties.SDK_HOME + java.io.File.separator + "testslogs");

    //ADD to history log file
    Debug.append("testshistory",
                 "\r\n" +
                 new java.util.Date(System.currentTimeMillis()).toString()
                 + ":\r\n");
    Debug.append("testshistory", "\r\n");

    com.symbian.utils.SystemExitHandler.setExitOperation(false);
  }

  public PREQ425AllAPITests(String s) {
    super(s);
  }

  public static Test suite() {
    TestSuite suite = new TestSuite();
    suite.addTest(com.symbian.tools.j2me.sei.AllTests.suite());
    suite.addTest(com.symbian.utils.AllTests.suite());
    suite.addTest(com.symbian.io.AllTests.suite());
    suite.addTest(com.symbian.vm.AllTests.suite());
    suite.addTest(com.symbian.tools.j2me.sei.emulator.commands.AllTests.suite());
    suite.addTest(com.symbian.tools.j2me.sei.emulator.event.AllTests.suite());
    suite.addTest(com.symbian.tools.j2me.sei.emulator.session.naive.AllTests.suite());
    return suite;
  }
}
