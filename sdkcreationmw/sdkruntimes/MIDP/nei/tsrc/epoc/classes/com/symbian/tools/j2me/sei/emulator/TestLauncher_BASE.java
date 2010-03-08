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

import junit.framework.*;

import java.util.*;
import java.io.*;
import java.net.*;
import javax.swing.*;

import com.symbian.tools.j2me.sei.SDKTestsProperties;
import com.symbian.tools.j2me.sei.preferences.*;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;
import com.symbian.tools.j2me.sei.emulator.event.*;
import com.symbian.utils.*;







/**
 * Since JUnit uses reflection to create test cases, there is no other way but
 * to use a base class, and create concrete class for each variation of
 * <Session>/<Non/Interactive>
 */
public abstract class TestLauncher_BASE extends TestCase {
  public static final int TEST_PASSED = 100;
  public static final int TEST_FAILED = 199;

  private final boolean iInteractive;
  private final String iSession;
  protected EmulatorLauncher iLauncher;
  protected String iHost;
  protected String iSessionName;
  private TestMIDletSuite iMIDletSuite;
  private String iCurrentTest;
  private boolean iSupressExit = false;

  public TestLauncher_BASE(){
    this(null, true);

  }
  public TestLauncher_BASE(String aSession, boolean aInteractive) {
    iSession = aSession;
    iInteractive = aInteractive;
    iMIDletSuite = TestMIDletSuitesFactory.getTestMIDletSuite(aInteractive);
  }

  public TestLauncher_BASE(String str, String aSession, boolean aInteractive) {
    super(str);
    iSession = aSession;
    iInteractive = aInteractive;
    iMIDletSuite = TestMIDletSuitesFactory.getTestMIDletSuite(aInteractive);
  }


  /**
   * JUnit setup
   *
   * @throws Exception
   */
  public void setUp() throws Exception{
    System.out.println("\n\n\nsetup next test...\n\n\n");
    super.setUp();
    setupUserPreferences();
    iLauncher = new EmulatorLauncher();
    EmulatorLauncherListener listener = new TestLauncherEmulatorLauncherListener();
    iLauncher.setListener(listener);
    iSupressExit = false;
  }

  /**
   * Modify the user preferences cache according to test suite preferences
   *
   * @throws Exception
   */
  protected void setupUserPreferences() throws Exception{
    UserPreferences up = UserPreferences.load();
    //host
    InetAddress.getByName(iHost).getHostName(); //minimal sanity check before actual running
    up.getKDPPreferences().getSessionPreferences(iSessionName).setHost(iHost);
    up.getKDPPreferences().setAttachingToVMEnabled(true); //all tests are in debug ("-nosuspend") mode
    //increament IDE port (to avoid cleanup issues between tests. in real life each run is in a new J2SE VM anyway.)
    up.getKDPPreferences().setAttachedVMKDPPort(up.getKDPPreferences().getAttachedVMKDPPort() + 1);
    up.getKDPPreferences().setEKA2EmulatorDiscovery("eka2".equalsIgnoreCase(SDKTestsProperties.KERNEL));
    //signing - mandatory only for non-interactive tests
    setupSecurityUserPreferences(up);
    up.save();
  }

  /**
   * Setup security preferences
   */
  private void setupSecurityUserPreferences(UserPreferences up){
    SecurityPreferences sp = up.getSecurityPreferences();
    sp.setAlwaysAddSignature(true);
    sp.setKeyAlias(TestMIDletSuitesFactory.KEY_ALIAS/*manufacturer_1_tck*/);
    JPasswordField jpf1 = new JPasswordField(TestMIDletSuitesFactory.KEY_PASSWORD/*password*/);
    sp.setKeyPassword(jpf1.getPassword());
    sp.setKeystoreFile(TestMIDletSuitesFactory.KEYSTORE_FILE/*V:\Java\tools\jadtool\manufacturer_1.jks"*/);
    JPasswordField jpf2 = new JPasswordField(TestMIDletSuitesFactory.KEYSTORE_PASSWORD/*password*/);
    sp.setKeystorePassword(jpf2.getPassword());
    sp.setRequiredPermissions(new String[] {
                              "javax.microedition.io.Connector.socket"});

  }


  public void tearDown() throws Exception{
    System.out.println("\n\ntearDown test: " + iCurrentTest);

    super.tearDown();
    try{
      SystemExitHandler.invokeSystemExit(200);
      SystemExitHandler.setExitOperation(false);
    }
    catch(Exception e){}
    iLauncher = null;
    System.gc();
    //allow some timeout for SEI Threads to exit
    System.out.println("wait(2000)...");
    synchronized (this) {
      wait(2000);
    }
    System.gc();
  }

  private String getCurrentTest(){
    return iCurrentTest + " (" + iSession.substring(iSession.indexOf(":") + 1) + ")";
  }

  /**
   * Ensure the command contains the midlet class name,
   *  if this is a non-interactive test
   */
  private String ensureNonInteractiveTestCommandContainsClassName(String aCommand){
    if("false".equalsIgnoreCase(SDKTestsProperties.IS_INTERACTIVE_SUITES))
    {
      final String midletName = iMIDletSuite.getMIDlet1ClassName();
      if (!aCommand.endsWith(midletName)) {
        Debug.println(this, "adding midlet name [" + midletName + "] to non-interactive command ");
        return aCommand + midletName;
      }
    }
    return aCommand;
  }

  private String[] getStringArray(String aCommand){
    StringTokenizer tokenizer = new StringTokenizer(ensureNonInteractiveTestCommandContainsClassName(aCommand));
    String[] tokens = new String[tokenizer.countTokens()];
    for (int i = 0; i < tokens.length; i++) {
      tokens[i] = tokenizer.nextToken();
    }
    return tokens;
  }

  private void doTestImpl(String aCommand) {
    final String[] arr = getStringArray(" " + iSession + " " + aCommand);
    new Thread(iCurrentTest + "-Thread"){
      public void run(){
        try{
          System.out.println("launching test: " +
                             TestLauncher_BASE.this.iCurrentTest);
          iLauncher.launch(arr);
        }
        catch(Exception e){
          Debug.println(this, "call iMIDletSuite.getConfirmation().abort()");
          iMIDletSuite.getConfirmation().abort();
          }
        }
      }.start();
    boolean passed = iMIDletSuite.getConfirmation().confirm(getCurrentTest());
    if(!iSupressExit)//applicable only to 2nd midlet tests
    {
      if (passed)
      {
        SystemExitHandler.invokeSystemExit(TEST_PASSED);
      }
      else
      {
        SystemExitHandler.invokeSystemExit(TEST_FAILED);
      }
    }
    Assert.assertTrue("did not receive confirmation", passed);
  }

  private String getClassPathArg(){
    return " -classpath " + iMIDletSuite.getClassesFolder() + " ";
  }

  private String getJADArg(){
    return " -Xdescriptor:" + iMIDletSuite.getJAD() + " ";
  }

  private void assertNotNaiveSession(){
    if(iSession == AllTests.NAIVE_SESSION){
      throw new UnsupportedOperationException("Naive Sessions does not supported this test");
    }
  }


  //
  // Patterns (With no jad)
  //


  public void test_Pattern_LaunchClassPathAndClassName() throws Exception {
    //SunONE-4 and Eclipse style
    iCurrentTest = "Launch with classpath and class name";
    String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }

  public void test_Pattern_LaunchJadWithJarURL() throws Exception {
    //SunONE-5
    iCurrentTest = "Launch with Jad with Jar URL";
    String str = getJADArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }

  //
  // Patterns (With jad)
  //

  public void test_Pattern_LaunchJadOnly() throws Exception {
    iCurrentTest = "Launch with Jad only";
    String str = getJADArg();
    doTestImpl(str);
  }

  public void test_Pattern_LaunchJadAndClassPath() throws Exception {
    //JBuilder style
    iCurrentTest = "Launch with Jad and class path";
    String str = getClassPathArg() + getJADArg();
    doTestImpl(str);
  }

  public void test_Pattern_LaunchJadAndClassPathAndClassName() throws Exception {
    iCurrentTest = "Launch with classpath, Jad and class name";
    String str = getClassPathArg() + getJADArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }


  //
  // IDEs
  //

  public void test_IDE_SunONE4() throws Exception {
    iCurrentTest = "Launch in SunONE-4 style";
    String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }

  public void test_IDE_SunONE5() throws Exception {
    iCurrentTest = "Launch in SunONE-5 style";
    String str = getJADArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }

  public void test_IDE_SunONE6() throws Exception {
    iCurrentTest = "Launch in SunONE-6 style";
    String str = getJADArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }
  public void test_IDE_JBuilder9() throws Exception {
    iCurrentTest = "Launch in JBuilder style";
    String str = getClassPathArg() + getJADArg();
    doTestImpl(str);
  }

  public void test_IDE_EclipseME() throws Exception{
    iCurrentTest = "Launch in EclipseME style";
    String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }








  //
  // Use-Cases
  //


  /**
   * Test the signature was properly added to the Jad file
   */
  public void test_UseCase_AddJarSignature() throws Exception {
    iCurrentTest = "Test Use-Case Add-Jar-Signature";
    UserPreferences up = UserPreferences.load();
    setupSecurityUserPreferences(up);
    up.save();
    String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
  }

  public void test_UseCase_DebugWithBootstrapClasspath() throws Exception {
    iLauncher = new EmulatorLauncher(){
     public void readUserPreferences() throws IOException{
       super.readUserPreferences();
       //NOTE: if the classes file is not found, the VM will crash.
       iUserPreferences.getCommandLinePreferences().
		   setBootStrapClasspath("C:\\" + SDKRuntimeProperties.PRODUCT + ".jar");
       iUserPreferences.getKDPPreferences().setAttachingToVMEnabled(true);
     }
   };
   final StringBuffer attached = new StringBuffer();
   iLauncher.setListener(new EmulatorLauncherListener()
   {
     public void handleEmulatorLauncherEvent(EmulatorLauncherEvent aEvent) {
       //Goal 1> will verify the KDP was attached.
       if (aEvent.getType() == EmulatorLauncherEvent.KDP_ATTACHED) {
           attached.append("true");
           Debug.println(this, "JUnit tests received EmulatorLauncherEvent.KDP_ATTACHED");
       }
       else if (aEvent.getType() == EmulatorLauncherEvent.UNHANDLED_EXCEPTION) {
         attached.append("false");
       }
     }
   });

   //Goal 2> will verify the MIDlet runs
   iCurrentTest = "Test Use-Case Debug-With-BootstrapClassPath";
   String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
   doTestImpl(str);
  }


  public void test_UseCase_Add2ndMIDlet() throws Exception {
    assertNotNaiveSession();
    //Notify the Confirmation not to close the server socket after
    //the first ping because we'll need 2 of those.
    iMIDletSuite.getConfirmation().setDoubleConfirmation();
    iCurrentTest = "Test Use-Case Add-2nd-MIDlet (1st MIDlet)";
    String str = getClassPathArg() + getJADArg() +
        iMIDletSuite.getMIDlet1ClassName();
    iSupressExit = true;
    doTestImpl(str);
    iSupressExit = false;
    iCurrentTest = "Test Use-Case Add-2nd-MIDlet (2nd MIDlet)";
    iLauncher.addMIDlet(iMIDletSuite.getMIDlet2Name());
    boolean passed = iMIDletSuite.getConfirmation().confirm(getCurrentTest());
    if (passed)
    {
      SystemExitHandler.invokeSystemExit(TEST_PASSED);
    }
    else
    {
      SystemExitHandler.invokeSystemExit(TEST_FAILED);
    }
    Assert.assertTrue("did not receive confirmation for 2nd midlet",  passed);
  }


  /**
   * Tests 2 cases:
   *   Goal 1> enable attaching to the VM
   *   Goal 2> launch the VM in unsuspended debug mode after VMInit event.
   */
  public void test_UseCase_EnableAttachingToRunningVM() throws Exception {
    iLauncher = new EmulatorLauncher(){
     public void readUserPreferences() throws IOException{
       super.readUserPreferences();
       KDPPreferences cp = iUserPreferences.getKDPPreferences();
       cp.setAttachingToVMEnabled(true);
     }
   };
   final StringBuffer attached = new StringBuffer();
   iLauncher.setListener(new EmulatorLauncherListener()
   {
     public void handleEmulatorLauncherEvent(EmulatorLauncherEvent aEvent) {
       //Goal 1> will verify the KDP was attached.
       if (aEvent.getType() == EmulatorLauncherEvent.KDP_ATTACHED) {
           attached.append("true");
       }
       else if (aEvent.getType() == EmulatorLauncherEvent.UNHANDLED_EXCEPTION) {
         attached.append("false");
       }
     }
   });

   //Goal 2> will verify the MIDlet runs
   iCurrentTest = "Test Use-Case Enable-Attaching";
   String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
   doTestImpl(str);

   boolean testPassed = new Boolean(attached.toString()).booleanValue();
   Assert.assertTrue("did not receive KDP_ATTACHED event", testPassed);


 }




// /**
//  * Tests 3 cases:
//  *   Goal 1> enable attaching to the VM
//  *   Goal 2> launch the VM in unsuspended debug mode after VMInit event.
//  *   Goal 3> add 2nd MIDlet
//  */
//  public void test_UseCase_Debugging2MIDlets() throws Exception {
//    assertNotNaiveSession();
//
//    iLauncher = new EmulatorLauncher(){
//     public void readUserPreferences() throws IOException{
//       super.readUserPreferences();
//       KDPPreferences cp = iUserPreferences.getKDPPreferences();
//       cp.setAttachingToVMEnabled(true);
//     }
//   };
//   final StringBuffer attached = new StringBuffer();
//   iLauncher.setListener(new EmulatorLauncherListener()
//   {
//     public void handleEmulatorLauncherEvent(EmulatorLauncherEvent aEvent) {
//       //Goal 1> will verify the KDP was attached.
//       if (aEvent.getType() == EmulatorLauncherEvent.KDP_ATTACHED) {
//           attached.append("true");
//       }
//       else if (aEvent.getType() == EmulatorLauncherEvent.UNHANDLED_EXCEPTION) {
//         attached.append("false");
//       }
//     }
//   });
//
//   //Goal 2> will verify the 1st MIDlet runs (MUST use Jad with both MIDlets!)
//   iCurrentTest = "Test Use-Case Debugging-2-MIDlets (1st MIDlet)";
//   String str = getClassPathArg() + getJADArg() + iMIDletSuite.getMIDlet1ClassName();
//
//   doTestImpl(str);
//
//   //Goal 3> will verify the 1st MIDlet runs
//   iCurrentTest = "Test Use-Case Debugging-2-MIDlets (2nd MIDlet)";
//   iLauncher.addMIDlet(iMIDletSuite.getMIDlet2Name());
//   Assert.assertTrue("did not receive confirmation",
//                     iMIDletSuite.getConfirmation().confirm(getCurrentTest()));
//
//   boolean testPassed = new Boolean(attached.toString()).booleanValue();
//   Assert.assertTrue("did not receive KDP_ATTACHED event", testPassed);
//  }


  public void test_UseCase_DebugPushedMIDlet() throws Exception {
    assertNotNaiveSession();

    iCurrentTest = "Test Use-Case Debug-Pused-MIDlet (TCP push)";
    final int pushPort = 11911;
    final StringBuffer remoteHost = new StringBuffer();
    iLauncher = new EmulatorLauncher() {
      public void readUserPreferences() throws IOException {
        super.readUserPreferences();

        //set remote host
	String host = iUserPreferences.getKDPPreferences().getSessionPreferences(iSessionName).getHost();
        remoteHost.append(host);

        //set enable attaching
        iUserPreferences.getKDPPreferences().setAttachingToVMEnabled(true);

        //defer launch
        iUserPreferences.getKDPPreferences().setVMLaunchingDeferred(true);

        //security
        setupSecurityUserPreferences(iUserPreferences);
        iUserPreferences.getSecurityPreferences().setRequiredPermissions
            (new String[] {
             "javax.microedition.io.PushRegistry",
             "javax.microedition.io.Connector.serversocket",
             "javax.microedition.io.Connector.socket",
        });

        //add static registration properties
        String pushKey = "MIDlet-Push-1";
        String pushValue =
            "socket://:" + pushPort +
            ", " + iMIDletSuite.getMIDlet1ClassName() +
            ", *";
        iUserPreferences.getSuitePreferences().putDefaultJadProperty(pushKey, pushValue);
      }
    };
    Thread pinger = new Thread(){
      public void run(){
        Debug.println(this, "+Activating pushed MIDlet");
        Socket s = null;
        while(s == null){
          try{
            //attempt to activate the MIDlet
            Debug.println(this, "+ open TCP push connection " + remoteHost.toString() + ":" + pushPort);
            s = new Socket(remoteHost.toString(), pushPort);
            Debug.println(this, "--- connection established with TCP pushed MIDlet ---");
            s.close();
          }
          catch(Exception e){
            synchronized (this) {
              try{ wait(100); } catch(Exception e2){};
            }
          }
        }
        System.out.println("-Activating pushed MIDlet");
      }
    };
    pinger.start();

    //Goal 2> will verify the MIDlet runs
    String str = getClassPathArg() + iMIDletSuite.getMIDlet1ClassName();
    doTestImpl(str);
    pinger.stop();

  }
}




/**
 * Listener to events to receive errors
 */
class TestLauncherEmulatorLauncherListener implements EmulatorLauncherListener {
  public void handleEmulatorLauncherEvent(EmulatorLauncherEvent aEvent) {
    if (aEvent.getType() == EmulatorLauncherEvent.UNHANDLED_EXCEPTION) {
      throw new RuntimeException
          ("Exception " + aEvent.getException() +
           " in state " + aEvent.getState() +
           "in mode " + aEvent.getMode());
    }
  }

}



