/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.symbian.tools.j2me.sei.emulator.session;



import junit.framework.*;
import com.symbian.tools.j2me.sei.preferences.*;
import com.symbian.tools.j2me.sei.emulator.*;
import com.symbian.tools.j2me.sei.emulator.session.Session;
import com.symbian.io.*;
import com.symbian.vm.*;
import com.symbian.utils.*;
import java.io.*;
import javax.swing.*;
import java.net.*;
import com.symbian.tools.j2me.sei.*;

public abstract class TestSessionImpls_BASE extends TestCase
{

  private String iSession;
  private JADFile iJad;
  private JARFile iJar;
  private UserPreferences iUserPreferences;
  private TestMIDletSuite iTestSuite;
  protected String iHost;
  protected String iSessionName;


  public TestSessionImpls_BASE(String aSession)
  {
    iSession = aSession;
  }

  public void setUp() throws Exception
  {
    super.setUp();
    setupUserPreferences();
  }

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


  public void testInstallAndRemove() throws Exception
  {
    Debug.println(this, "+testInstallAndRemove");
    try {
      Debug.println(this, "create session");
      //create Session
      Session session = Session.create(iSessionName);
      Debug.println(this, "session=" + session);
      //init session
      Debug.println(this, "init session");
      iUserPreferences = UserPreferences.load();
      iTestSuite = TestMIDletSuitesFactory.getTestMIDletSuite(false);
      session.initialize(iUserPreferences);
      //init binaries
      initializeDescriptor();
      initializeJar();
      addJarSignature();
      //install
      Debug.println(this, "install");
      MIDletSuiteInfo suiteInfo =
          session.installMIDletSuite(iJar, iJad,
                                     iUserPreferences.getSecurityPreferences().
                                     isTrusted());
      //remove
      Debug.println(this, "remove");
      session.removeMIDletSuite(suiteInfo);

    }
    catch (Exception ex) {
      Debug.printStackTrace(this, ex);
      throw ex;
    }
    Debug.println(this, "-testInstallAndRemove");
  }


  private File getNewTempFile(String aSrc) throws IOException{
    String name = new File(aSrc).getName();
    Debug.println(this, name);
    File newFileName = new File(System.getProperty("java.home"), name);
    Debug.println(this, newFileName);
    Utilities.copyFiles(aSrc, newFileName.getAbsolutePath());
    Debug.println(this, "return " + newFileName.getAbsolutePath());
    return new File(newFileName.getAbsolutePath());
  }



  protected void initializeDescriptor() throws IOException
{
  Debug.println(this, "+initializeDescriptor()");
  //ensure all MUST properties exist
  iJad = new JADFile(iUserPreferences.getSuitePreferences().
                     getDefaultJadProperties());

  //add J2ME permissions
  iJad.addJ2MEPermission
      (iUserPreferences.getSecurityPreferences().getRequiredPermissions());

  iJad.addDefaultMIDlet("TestMIDlet1");
  iJad.flush();

}


/**
 * Initialize the jar file
 *
 * @throws IOException if some error occured
 */
protected void initializeJar() throws IOException
{
  Debug.println(this, "+initializeJar()");
  //remove non valid properties (i.e file size is invalid at this point)
  final String jarUrl = iJad.containsJarURL() ? iJad.getJarURL() : null;
  iJad.removeJarProperties();
  iJad.flush();

    iJar = new JARFile(iUserPreferences.getSuitePreferences().getFileTypes(),
                       new String[]{iTestSuite.getClassesFolder()},
                       iJad.getFile());
  iJad.setJarSize(iJar.getFile().length());
  iJad.setJarURL(iJar.getFile().getName());
  iJad.flush();
}

/**
 * Add Jar signature to Jad file
 *
 * @throws IOException if some error occured
 * @throws ClassNotFoundException if the JadTool.jar is not in the classpath
 * InterruptedException might be thrown by Process.waitFor()
 */
protected void addJarSignature() throws Exception
{
  Debug.println(this, "+addJarSignature()");
  iJad = new JadToolLauncher().launch
      (iUserPreferences.getSecurityPreferences(),
       iJar,
       iJad);
}


}
