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

public class TestMIDletSuiteAMSArguments extends TestCase {
  private MIDletSuiteAMSArguments iArgs = null;

  protected void setUp() throws Exception {
    super.setUp();
    iArgs = new MIDletSuiteAMSArguments();
  }

  protected void tearDown() throws Exception {
    iArgs = null;
    super.tearDown();
  }

  public void testSetSystemProperty() {
    String aKey = "key";
    String aValue = "value";
    iArgs.setSystemProperty(aKey, aValue);
    Assert.assertEquals("wrong system property string", iArgs.getArguments(), " -D" + aKey + "=" + aValue);
  }

}
