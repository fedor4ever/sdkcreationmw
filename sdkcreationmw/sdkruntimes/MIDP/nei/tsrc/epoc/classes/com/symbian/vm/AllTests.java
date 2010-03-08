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

package com.symbian.vm;

import junit.framework.*;

public class AllTests extends TestSuite {

  public AllTests(String s) {
    super(s);
    addTestSuite(com.symbian.vm.TestMIDletSuiteInfo.class);
    addTestSuite(com.symbian.vm.TestMIDletInfo.class);
    addTestSuite(com.symbian.vm.TestMIDletSuiteAMSArguments.class);
    addTestSuite(com.symbian.vm.TestVMArguments.class);
  }

  public static Test suite() {
    return new AllTests("com.symbian.vm");
  }
}
