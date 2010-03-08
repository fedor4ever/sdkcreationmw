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

import java.io.*;
import java.net.*;

import junit.framework.*;
import com.symbian.vm.VMProcess;

public class TestVMVerbosePiper extends TestCase {

  static final String TEST_STRING = "*this is a test string*";

  public TestVMVerbosePiper(String name) {
    super(name);
  }


  public void testRun() throws Exception{
    ByteArrayOutputStream bos = new ByteArrayOutputStream(TEST_STRING.getBytes().length);
    PrintStream stream = new PrintStream(bos, true);
    VMVerbosePiper piper = new VMVerbosePiper(InetAddress.getLocalHost(), 7000, stream, new VMStubProcess(), 5);
    TestVMVerbosePiperServer server = new TestVMVerbosePiperServer();
    server.start();
    piper.start();
    //wait for the piper to finish and die
    piper.join();
    //check output
    bos.flush();
    String str = new String(bos.toByteArray());
    assertEquals("output string " + str + " does not match input string " + TEST_STRING,
                 TEST_STRING,
                 str);

    assertEquals("output length " + bos.toByteArray().length + " does not match input length " + TEST_STRING.getBytes().length,
                 TEST_STRING.getBytes().length,
                 bos.toByteArray().length);
    piper.stop();
    server.stop();
  }

}


class TestVMVerbosePiperServer extends Thread{
  public void run(){
    try{
      ServerSocket ss = new ServerSocket(7000);
      Socket s = ss.accept();
      s.getOutputStream().write(TestVMVerbosePiper.TEST_STRING.getBytes());
      s.getOutputStream().flush();
      s.close();
      ss.close();
    }
    catch(Exception e){
      e.printStackTrace();
    }

  }
}

class VMStubProcess extends VMProcess{
  public VMStubProcess(){
    super(null);
  }
  public void destroy(){

  }
}



