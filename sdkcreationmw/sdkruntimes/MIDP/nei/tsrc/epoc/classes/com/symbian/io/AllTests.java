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

package com.symbian.io;

import junit.framework.*;

public class AllTests extends TestSuite {

  public AllTests(String s) {
    super(s);
    addTestSuite(com.symbian.io.TestJADFile.class);
    addTestSuite(com.symbian.io.TestJARFile.class);
    addTestSuite(com.symbian.io.TestStreamsPiper.class);
    addTestSuite(com.symbian.io.TestVMVerbosePiper.class);
  }

  public static Test suite() {
    return new AllTests("com.symbian.io");
  }
}
