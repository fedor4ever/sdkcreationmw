package com.symbian.tools.j2me.sei.emulator;

import com.symbian.tools.j2me.sei.preferences.SecurityPreferences;
import com.symbian.utils.Debug;
import com.symbian.io.*;

import java.io.*;/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

public class JadToolLauncher {

  //
  // Constants
  //

  /**
   * Jad name with certificate
   */
  private static final String CERT_JAD_NAME = ".Cert.jad";

  /**
   * Signed Jad name
   */
  private static final String SIGNED_JAD_NAME = ".Signed.jad";



  //
  // Operations
  //


  /**
   * Launch the JadTool
   *
   * @param aSecPrefs security preferences
   * @param aJar Jar file
   * @param aJad Jad file
   *
   * @param new signed Jad file
   *
   * @throws Exception is some error occured
   * @throws ClassNotFoundException if JadTool.jar not on CLASSPATH
   */
  public JADFile launch(SecurityPreferences aSecurityPrefs,
                        JARFile aJar,
                        JADFile aJad) throws Exception {
    final String jadFile = aJad.getFile().getPath();
    final String certJadFile = jadFile + CERT_JAD_NAME;
    final String signedJadFile = jadFile + SIGNED_JAD_NAME;
    addCertificate(aSecurityPrefs, aJar, jadFile, certJadFile);
    addJarSignature(aSecurityPrefs, aJar, certJadFile, signedJadFile);
    new File(jadFile).deleteOnExit();
    new File(certJadFile).deleteOnExit();
    JADFile jad = new JADFile(signedJadFile);
    jad.getFile().deleteOnExit();
    return jad;
  }



  /**
   * Add certificate
   *
   * @param aSecurityPrefs SecurityPreferences
   * @param aJar JARFile
   * @param aJad JADFile
   *
   * @return JADFile
   *
   * @throws Exception
   */
  public void addCertificate(SecurityPreferences aSecurityPrefs,
                                JARFile aJar,
                                String aSrcJad,
                                String aDestJad) throws Exception {
    Debug.println(this, "+JadToolLauncher.addJarSignature");
    launchJavaProcss(new String[] {
                     "java",
                     "com.sun.midp.jadtool.JadTool",
                     "-addcert",
                     "-alias", aSecurityPrefs.getKeyAlias(),
                     "-storepass",
                     new String(aSecurityPrefs.getKeystorePassword()),
                     "-keystore", aSecurityPrefs.getKeystoreFile(),
                     "-inputjad", aSrcJad,
                     "-outputjad", aDestJad});
  }

  /**
   * Add Jar signature
   *
   * @param aSecurityPrefs SecurityPreferences
   * @param aJar JARFile
   * @param aJad JADFile
   * @throws Exception
   * @return JADFile
   */
  public void addJarSignature(SecurityPreferences aSecurityPrefs,
                              JARFile aJar,
                              String aSrcJad,
                              String aDestJad) throws Exception {
    Debug.println(this, "+JadToolLauncher.addJarSignature");
    launchJavaProcss(new String[] {
                     "java",
                     "com.sun.midp.jadtool.JadTool",
                     "-addjarsig",
                     "-jarfile", aJar.getFile().getPath(),
                     "-keypass", new String(aSecurityPrefs.getKeyPassword()),
                     "-alias", aSecurityPrefs.getKeyAlias(),
                     "-storepass",
                     new String(aSecurityPrefs.getKeystorePassword()),
                     "-keystore", aSecurityPrefs.getKeystoreFile(),
                     "-inputjad", aSrcJad,
                     "-outputjad", aDestJad});

  }



  /**
   * Launch the JadTool
   *
   * @param aJadToolArgs command line arguments
   *
   * @throws Exception if some error occured
   * @throws NoClassDefFoundError if JadTool is not on CLASSPATH
   * @throws RuntimeException if JadTool failed
   */
  private void launchJavaProcss(String[] aJadToolArgs) throws Exception {
    Debug.println(this, "Launch JadTool with arguments:");
    Debug.println(this, aJadToolArgs);
    try {
      //cant use the main method which exits
      Class.forName("com.sun.midp.jadtool.JadTool");
      Debug.println(this, "verified loading com.sun.midp.jadtool.JadTool.class");
      Process p = Runtime.getRuntime().exec(aJadToolArgs);
      int rc = p.waitFor();
      if (rc != 0) {
        throw new RuntimeException("bad JadTool return code: " + rc);
      }
    }
    catch (NoClassDefFoundError e) {
      Debug.printStackTrace(this, e);
      throw new ClassNotFoundException
          ("JadTool.class not on CLASSAPTH: " + e.getMessage(), e);
    }
  }

/*
  public void pipe(Process p) throws Exception{
    Debug.println(this, "reading from process input stream");
    InputStream is = p.getInputStream();
    int read = is.read();
    while(read != -1){
      System.out.print((char)read);
      read = is.read();
    }

    Debug.println(this, "reading from process error stream");
    is = p.getErrorStream();
    read = is.read();
    while(read != -1){
      System.out.print((char)read);
      read = is.read();
    }
    Debug.println(this, "----------------------");
  }
*/
}
