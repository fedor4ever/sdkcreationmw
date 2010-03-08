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

package com.symbian.tools.j2me.sei.emulator.event;

import junit.framework.*;

public class AllTests extends TestSuite {

  static{
    com.symbian.utils.SystemExitHandler.setExitOperation(false);
  }



  public AllTests(String s) {
    super(s);
    addTestSuite(com.symbian.tools.j2me.sei.emulator.event.TestEmulatorLauncherListener.class);

  }

  public static Test suite() {
    return new AllTests("com.symbian.tools.j2me.sei.emulator tests");
  }
}
