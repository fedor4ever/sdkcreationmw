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

public class TestVMArguments extends TestCase {

  private VMArguments iArgs;

  protected void setUp() throws Exception {
    super.setUp();
    iArgs = new VMArguments();
  }

  protected void tearDown() throws Exception {
    iArgs = null;
    super.tearDown();
  }

  public void testSetUnsuspendedMode() {
    iArgs.setUnsuspendedMode();
    Assert.assertEquals("wrong unsuspended mode string", iArgs.getArguments(), " -nosuspend");
  }

  public void testSetVMConsolePort() {
    int port = 9000;
    iArgs.setVMConsolePort(port);
    Assert.assertEquals("wrong VM console port string", iArgs.getArguments(), " -vp " + port);
  }

  public void testSetVMDebugPort() {
    int port = 9000;
    iArgs.setVMDebugPort(port);
    Assert.assertEquals("wrong VM debug port string", iArgs.getArguments(), " -debugger -port " + port);
  }

  public void testSetVerboseType() {
    String type = " -verbose";
    iArgs.setVerboseType(type);
    Assert.assertEquals("wrong verbose type string", iArgs.getArguments(), " -verbose");
  }

}

