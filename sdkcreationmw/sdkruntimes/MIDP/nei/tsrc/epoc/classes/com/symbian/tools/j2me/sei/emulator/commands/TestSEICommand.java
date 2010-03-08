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

package com.symbian.tools.j2me.sei.emulator.commands;

import junit.framework.*;
import java.util.*;
import java.io.*;

public class TestSEICommand extends TestCase {
  public static final String FULL_COMMAND = "-classpath classpath -Xdescriptor:C:\\descriptor -Xdebug -Xrunjdwp:transport=dt_socket,address=5500,server=y -Dparam=value  -Xverbose  -unknownArg1 -unknownArg2 -version Application";
  private SEICommand iSEICmd;
  private String iArgs[];

  public TestSEICommand(String name) {
    super(name);
    iSEICmd = null;
  }

  protected void setUp() throws Exception {
    super.setUp();
    StringTokenizer tokenzier = new StringTokenizer("-classpath classpath -Xdescriptor:C:\\descriptor -Xdebug -Xrunjdwp:transport=dt_socket,address=5500,server=y -Dparam=value  -Xverbose  -unknownArg1 -unknownArg2 -version Application");
    iArgs = new String[tokenzier.countTokens()];
    int index = 0;
    while (tokenzier.hasMoreTokens())
    {
      iArgs[index++] = tokenzier.nextToken();
    }
    iSEICmd = new SEICommand(iArgs);
  }

  protected void tearDown() throws Exception {
    iSEICmd = null;
    super.tearDown();
  }

  public void testGetApplication() {
    String expectedReturn = "Application";
    String actualReturn = iSEICmd.getApplication();
    Assert.assertEquals("Application return value", expectedReturn,
                        actualReturn);
  }

  public void testGetArguments() {
    String actualReturn[] = iSEICmd.getArguments();
    for (int i = 0; i < iArgs.length; i++)
      Assert.assertEquals("Arguments array return value", actualReturn[i],
                          iArgs[i]);

  }

  public void testGetClassPath() {
    Assert.assertEquals("Class path return value", "classpath",
                        iSEICmd.getClassPath()[0]);
    Assert.assertEquals("number of Class path folders", 1,
                        iSEICmd.getClassPath().length);
  }

  public void testGetDebugOptions() {
    Map map = iSEICmd.getDebugOptions();
    Assert.assertNotNull("Map should not be null", map);
    Assert.assertEquals("transport type should be 'dt_socket'", "dt_socket",
                        map.get("transport"));
    Assert.assertEquals("address type should be '5500'", "5500",
                        map.get("address"));
    Assert.assertEquals("server type should be 'y'", "y", map.get("server"));
  }

  public void testGetDebugPort() {
    Assert.assertEquals("debug port return value", 5500,
                        iSEICmd.getKVMDebugProxyListeningPort());
  }

  public void testGetDescriptor() {
    Assert.assertEquals("descriptor return value", new File("C:\\descriptor"),
                        iSEICmd.getDescriptor());
  }

  public void testGetQuery() {
    Assert.assertEquals("return value", "-version", iSEICmd.getQuery());
  }

  public void testGetSystemProperties() {
    Map params = iSEICmd.getSystemProperties();
    Assert.assertTrue("should contain a paramter names 'param'",
                      params.containsKey("param"));
    Assert.assertEquals("should contain a single paramter", 1, params.size());
    Assert.assertEquals("paramter value is incorrect", "value",
                        params.get("param"));
  }

  public void testGetVerboseTypes() {
    Set actualReturn = iSEICmd.getVerboseTypes();
    Assert.assertEquals("verbose type should be 'all'", "all",
                        iSEICmd.getVerboseTypes().iterator().next());
  }

  public void testIsQuery() {
    Assert.assertTrue("should recognize as query", iSEICmd.isQuery());
  }

  public void testGetUnrecognizedArguments() {
    Assert.assertEquals("should have 2 unrecognized arguments", 2,
                        iSEICmd.getUnrecognizedArguments().length);
    Assert.assertEquals("-unknownArg1 is missing", "-unknownArg1",
                        iSEICmd.getUnrecognizedArguments()[0]);
    Assert.assertEquals("-unknownArg2 is missing", "-unknownArg2",
                        iSEICmd.getUnrecognizedArguments()[1]);
  }

}
