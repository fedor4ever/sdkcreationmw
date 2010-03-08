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

import com.symbian.tools.j2me.sei.SDKTestsProperties;


public class TestLauncher_WIN32_NONINTERACTIVE extends TestLauncher_BASE{
  public TestLauncher_WIN32_NONINTERACTIVE(){
    super(AllTests.WIN32_SESSION, false);
    iHost = SDKTestsProperties.WIN32_SESSION_HOST;
    iSessionName = "win32";
  }

  public TestLauncher_WIN32_NONINTERACTIVE(String aTestName){
    super(aTestName, AllTests.WIN32_SESSION, false);
    iHost = SDKTestsProperties.WIN32_SESSION_HOST;
    iSessionName = "win32";
  }

}
