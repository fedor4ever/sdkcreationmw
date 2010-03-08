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

package com.symbian.utils;

import junit.framework.*;
import java.io.*;
import java.net.*;
import java.awt.*;

public class TestUtilities extends TestCase {
  public static final String VALID_NAMES[] = {
      "Name", "Name$Inner", "a.Name", "a.Name$Inner", "a.b.Name"
  };
  public static final String INVALID_NAMES[] = {
      "-Name", "a.-Name", "a+.b.Name", "C:\\"
  };

  public TestUtilities(String name) {
    super(name);
  }


  public void testIsValidClassName()
  {
      for(int i = 0; i < VALID_NAMES.length; i++){
        Assert.assertTrue(VALID_NAMES[i] + " should be a valid name",
                          Utilities.isValidClassName(VALID_NAMES[i]));
      }
      for(int i = 0; i < INVALID_NAMES.length; i++){
        Assert.assertFalse(INVALID_NAMES[i] + " should not be a valid name",
                           Utilities.isValidClassName(INVALID_NAMES[i]));
      }

  }

  public void testCopyFiles() throws IOException
  {
    File src = File.createTempFile("testutlities1", "temp");
    src.deleteOnExit();
    File dest = File.createTempFile("testutlities2", "temp");
    dest.delete();
    dest.deleteOnExit();
    Utilities.copyFiles(src.getAbsolutePath(), dest.getAbsolutePath());
    Assert.assertTrue("File must exists", dest.exists());
    Assert.assertTrue("File must be a file type", dest.isFile());
    dest.delete();
    dest.deleteOnExit();
  }





  public void testGetFreePort1() throws IOException {
    ServerSocket ss = new ServerSocket(8000);
    int actualReturn = Utilities.getFreePort(InetAddress.getLocalHost(), 8000);
    assertEquals("8001 should be a free port", 8001, actualReturn);
    ss.close();
  }

  public void testGetFreePort2() throws IOException {
    ServerSocket ss = new ServerSocket(8000);
    try{
      int actualReturn = Utilities.getFreePort(InetAddress.getLocalHost(), 8000, 8000);
      fail("should have thrown ConnectException");
    }
    catch(ConnectException e){
      //this is the correct behaviour
    }
    ss.close();
  }

  public void testGetFreePort3() throws IOException {
    try{
      int port = Utilities.getFreePort();
      ServerSocket ss = new ServerSocket(port);
      ss.close();
    }
    catch(Exception e){
      e.printStackTrace();
      Assert.fail("caught exception: " + e.getMessage());
    }
  }


}
