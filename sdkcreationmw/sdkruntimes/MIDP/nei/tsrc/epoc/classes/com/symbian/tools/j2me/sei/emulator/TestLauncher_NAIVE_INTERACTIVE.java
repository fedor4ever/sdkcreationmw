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

public class TestLauncher_NAIVE_INTERACTIVE extends TestLauncher_BASE {
  public TestLauncher_NAIVE_INTERACTIVE() {
    super(AllTests.NAIVE_SESSION, true);
  }

  public TestLauncher_NAIVE_INTERACTIVE(String aTestName) {
    super(aTestName, AllTests.NAIVE_SESSION, true);
  }

}
