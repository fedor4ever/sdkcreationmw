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

import com.symbian.tools.j2me.sei.preferences.*;
import com.symbian.utils.Debug;
import com.symbian.tools.j2me.sei.SDKTestsProperties;
import javax.swing.*;
import java.io.*;
import java.net.*;

/**
 * Base class for confirmation
 */
public abstract class Confirmation {

  /**
   * Used when a double ping is required - i.e. tests with 2 MIDlets
   */
  private boolean iDoubleConfirmation = false;
  private int iConfirmationsCounter = 0;

  Confirmation(boolean aSignSuite) {
    try{
      //override the preferences.
      //this is required because the Non-Interactive confirmation must be signed
      //when reverting back to Interactive, need to remove this dependency.
      UserPreferences up = UserPreferences.load();
      SecurityPreferences sp = up.getSecurityPreferences();
      //set signature indicator
      sp.setAlwaysAddSignature(aSignSuite);
      //al the rest is ignored is (aSignSuite == false)
      sp.setKeyAlias("dummyca");
      JPasswordField jpf1 = new JPasswordField("keypwd");
      sp.setKeyPassword(jpf1.getPassword());
      sp.setKeystoreFile("V:\\Java\\tools\\jadtool\\midptck.ks");
      JPasswordField jpf2 = new JPasswordField("keystorepwd");
      sp.setKeystorePassword(jpf2.getPassword());
      sp.setRequiredPermissions(new String[] {
                                "javax.microedition.io.Connector.socket"});
      up.save();
    }
    catch(Exception e){
      e.printStackTrace();
    }
  }

  public abstract boolean confirm(String aTest);

  public abstract void abort();

  public void setDoubleConfirmation(){
    iDoubleConfirmation = true;
  }

  public boolean isDoubleConfirmation(){
    return iDoubleConfirmation;
  }

  public boolean isSingleConfirmation(){
    return !iDoubleConfirmation;
  }

  public void setConfirmationReceived(){
    iConfirmationsCounter++;
  }

  public boolean isFirstConfirmationReceived(){
    return iConfirmationsCounter >= 1;
  }

  public boolean isSecondConfirmationReceived(){
    return iConfirmationsCounter >= 2;
  }
}

/**
 * Interactive confirmation
 */
class InteractiveConfirmation extends Confirmation {
  private final String iMessage;

  public InteractiveConfirmation(String aMessage){
    super(false);
    iMessage = aMessage;
  }


  public synchronized boolean confirm(String aTest) {
    //TODO: no support for double confirmation at the moment
    boolean rc = JOptionPane.showConfirmDialog(null,
                                         iMessage,
                                         aTest,
                                         JOptionPane.YES_NO_CANCEL_OPTION)
        == JOptionPane.YES_OPTION;
    Debug.println(this, "received interactive confirmation [" + aTest + "] == " + rc);
    return rc;
  }

  public void abort(){
    synchronized (this) {
      notify();
    }
  }

}


/**
 * NON-Interactive confirmation
 */
class NonInteractiveConfirmation extends Confirmation {
  private static final int PING_INTERVAL = 60 * 1000;
  private static final int SO_TIMEOUT = 2 * 60 * 1000;
  private static final String REMOTE_HOST = SDKTestsProperties.CONFIRMATION_PING_REMOTE_HOST;
  private static int REMOTE_PORT;
  private static int LOCAL_PORT;
  private static final int INTERVAL = 100;
  private final boolean iServerMode = true;
  private ServerSocket iServerSocket;
  private Socket iSocket;
  private boolean iTestAborted;
  {
    try{
      REMOTE_PORT = Integer.parseInt(SDKTestsProperties.CONFIRMATION_PING_REMOTE_PORT);
    }
    catch(Exception e){}
    try{
      LOCAL_PORT = Integer.parseInt(SDKTestsProperties.CONFIRMATION_LISTEN_LOCAL_PORT);
    }
    catch(Exception e){}
  }

  public NonInteractiveConfirmation(){
    super(true);
  }

  public synchronized boolean confirm(String aTest) {
    if (iServerMode) {
      return listen(aTest);
    }
    else {
      return ping(aTest);
    }
  }

  public boolean listen(String aTest){

    boolean rc = false;
    Socket s = null;
    try {
      Debug.println(this, "+ non-interactive confirmation [" + aTest + "] listening on " + LOCAL_PORT);
      System.out.println("+ non-interactive confirmation [" + aTest + "] listening on " + LOCAL_PORT);

      if(!isFirstConfirmationReceived()){
        //this is the first confirmation out of 2 required
        //OR the single confirmation that we actually need
        iServerSocket = new ServerSocket(LOCAL_PORT);
        iServerSocket.setSoTimeout(SO_TIMEOUT);
      }
      synchronized (this) {
        if(!iTestAborted){
          s = iServerSocket.accept();
        }
      }
      s.getInputStream().read();
      setConfirmationReceived();
      Debug.println(this, "- received non-interactive confirmation [" + aTest + "]");
      System.out.println("- received non-interactive confirmation [" + aTest + "]");

      //set to passed
      rc = true;

      //cleanup
      s.close();
      if(isSingleConfirmation() ||
         (isDoubleConfirmation() && isSecondConfirmationReceived())){
        //Close the server socket only if no other confirmation is expected.
        //Otherwise, MIDlet might ping before new ServerSocket was created.
        Debug.print(this, "Closing Confirmation ServerSocket");
        iServerSocket.close();
      }
    }
    catch (Exception ex) {
      //set to failed
      rc = false;

      //cleanup
      try {
        Debug.print(this, "Closing Confirmation ServerSocket because of caught " + ex);
        iServerSocket.close();
        s.close();
      }
      catch (Exception e2) {}
      finally{
        iServerSocket = null;
      }

      //Log state and error
      if(iTestAborted){
        Debug.println(this, "Test was aborted.");
      }
      if(ex instanceof InterruptedIOException){
        Debug.println(this, "SO_TIMEOUT timed out for test " + aTest.toString());
      }
      ex.printStackTrace();
    }
    return rc;
  }


  /**
   * NOT USED!!
   * since in naive session the VM runs without SystemAMS and therefor cannot
   * open server connection which are managed by the missing SystemAMS
   */
  public boolean ping(String aTest) {
    Debug.println(this, "non-interactive confirmation [" + aTest + "] pings " + REMOTE_HOST + ":" + REMOTE_PORT);
    System.out.println("non-interactive confirmation [" + aTest + "] pings " + REMOTE_HOST + ":" + REMOTE_PORT);
    long end = System.currentTimeMillis() + PING_INTERVAL;
    boolean connected = false;
    Socket s = null;
    while(System.currentTimeMillis() < end && !connected){
      try {
       s = new Socket(REMOTE_HOST, REMOTE_PORT);
       connected = true;
      Debug.println(this, "received non-interactive confirmation [" + aTest + "]");
      System.out.println("received non-interactive confirmation [" + aTest + "]");
      }
      catch (Exception ex) {
        synchronized (this) {
          try {
            wait(INTERVAL);
          }
          catch (InterruptedException iex){}
        }
      }
      finally{
        try {
          s.close();
        }
        catch (Exception ex) {}
      }
    }
    return connected;
  }



  public synchronized void abort(){
    iTestAborted = true;
    if (iServerMode) {
      try {
       iServerSocket.close();
       iServerSocket = null;
      }
      catch (Exception ex) {
        Debug.printStackTrace(this, ex);
      }
    }
    else {
      //not supported
    }

  }

}
