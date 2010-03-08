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
import java.io.*;

public class TestStreamsPiper extends TestCase {


  public static final String TEST_STRING = "this is a test string";
  private ByteArrayInputStream iBin;
  private ByteArrayOutputStream iBos;
  private StreamsPiper iPiper;


  public void setUp()
      throws Exception
  {
      super.setUp();
      iBin = new ByteArrayInputStream(TEST_STRING.getBytes());
      iBos = new ByteArrayOutputStream();
      iPiper = new StreamsPiper(iBin, iBos);
  }


  public void testPipe()
  {
    try{
      iPiper.pipe(TEST_STRING.getBytes().length);
      Assert.assertEquals("pipe(int): length of streams is different",
                          TEST_STRING.getBytes().length, iBos.size());
      Assert.assertEquals("pipeAll(): output string is different",
                          TEST_STRING, new String(iBos.toByteArray()));
    }
    catch(Exception e){
      e.printStackTrace();
      Assert.fail("caught: " + e.getMessage());
    }
  }

  public void testPipeAll()
  {
    try{
      iPiper.pipeAll();
      Assert.assertEquals("pipe(int): length of streams is different",
                          TEST_STRING.getBytes().length, iBos.size());
      Assert.assertEquals("pipeAll(): output string is different",
                          TEST_STRING, new String(iBos.toByteArray()));
    }
    catch(Exception e){
      e.printStackTrace();
      Assert.fail("caught: " + e.getMessage());
    }
  }

}
