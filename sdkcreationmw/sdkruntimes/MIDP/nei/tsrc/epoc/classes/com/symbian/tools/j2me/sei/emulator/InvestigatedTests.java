/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
import com.symbian.tools.j2me.sei.*;

public class InvestigatedTests
    extends TestCase {

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


  public InvestigatedTests(String s) {
    super(s);
  }

  public static Test suite() {
    TestCase c;
    TestSuite suite = new TestSuite();
    String testName = "test_Pattern_LaunchJadAndClassPath";
    String session = "naive";
    if(SDKTestsProperties.TEST_NAME != null){
      testName = SDKTestsProperties.TEST_NAME;
    }
    if(SDKTestsProperties.TEST_SESSION != null){
      session = SDKTestsProperties.TEST_SESSION;
    }
    Test test;
    if(session.equalsIgnoreCase("win32")){
      test = new TestLauncher_WIN32_NONINTERACTIVE(testName);
    }
    else if(session.equalsIgnoreCase("target")){
      test = new TestLauncher_TARGET_NONINTERACTIVE(testName);
    }
    else{
      test = new TestLauncher_NAIVE_NONINTERACTIVE(testName);
    }
    System.out.println("running single " + session + " session test: " + testName);

    suite.addTest(test);
    return suite;
//    suite.addTest(new TestLauncher_WIN32_INTERACTIVE("test_UseCase_DebugPushedMIDlet"));
//    suite.addTest(new TestLauncher_NAIVE_INTERACTIVE("test_Pattern_LaunchJadWithJarURL"));
//    suite.addTest(new TestLauncher_NAIVE_INTERACTIVE("test_UseCase_DebugWithBootstrapClasspath"));
  }
}


class MyTests extends TestCase{
  public MyTests(){
  }

}
