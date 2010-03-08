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
import java.util.*;
import com.symbian.utils.Utilities;

public class TestJADFile extends TestCase {

  private File iTempJad;

  public static File createTempJad() throws IOException{
    File file = File.createTempFile("testjad", ".jad");
    file.deleteOnExit();
    Utilities.copyToFile(TestJADFile.class.getResourceAsStream("testjad.properties"), file.getAbsolutePath());
    return file;
  }

  public TestJADFile(String name) throws IOException{
    super(name);
    //create a temp JAD file
    iTempJad = File.createTempFile("testjad", ".jad");
    iTempJad.deleteOnExit();
    Utilities.copyToFile(getClass().getResourceAsStream("testjad.properties"), iTempJad.getAbsolutePath());
  }


  public void testJADFilePropertiesCTOR() throws IOException {
    Properties aProperties = new Properties();
    aProperties.load(getClass().getResourceAsStream("testjad.properties"));
    JADFile jadFile = new JADFile(aProperties);
  }


  public void testAddJ2MEPermission() throws IOException {
    //single
    String[] aPermission = new String[]{"javax.permissions.a"};
    JADFile jadFile1 = new JADFile(iTempJad);
    jadFile1.addJ2MEPermission(aPermission);
    String value = (String)jadFile1.getMainAttributes().get("MIDlet-Permissions");
    assertTrue("doesnt include (single) J2ME permission " + aPermission[0], jadFile1.getMainAttributes().containsKey("MIDlet-Permissions"));
    assertEquals("doesnt include (single) J2ME permission " + aPermission[0], aPermission[0], value);

    //list of permissions
    String[] aPermissionS = new String[]{"javax.permissions.a", "javax.permissions.b", "javax.permissions.c"};
    JADFile jadFile2 = new JADFile(iTempJad);
    jadFile2.addJ2MEPermission(aPermissionS);
    value = (String)jadFile2.getMainAttributes().get("MIDlet-Permissions");
    for(int i = 0; i < aPermissionS.length; i++){
      assertTrue("doesnt include J2ME permission " + aPermissionS[i], value.indexOf(aPermissionS[i]) != -1);
    }
  }

  public void testFlush() throws IOException {
    File flushedJad = File.createTempFile("testflushedjad", ".jad");
    flushedJad.deleteOnExit();
    Utilities.copyFiles(iTempJad.getAbsolutePath(), flushedJad.getAbsolutePath());
    JADFile jad = new JADFile(iTempJad);
    jad.flush();
    assertTrue("Temp jad file should exist after flush", jad.getFile().exists());
    assertTrue("Temp jad file should be a file", jad.getFile().isFile());
  }

  public void testGetFile() throws IOException{
    JADFile jadFile = new JADFile(iTempJad);
    assertEquals("inner file is not the same ", iTempJad, jadFile.getFile());
    /**@todo fill in the test code*/
  }

//  public void testGetJARUrl() throws IOException{
//    JADFile jadFile = new JADFile(iTempJad);
//    String expectedReturn = "MIDletJarURL.jar";
//    String actualReturn = jadFile.getJARUrl();
//    assertEquals("jar URL is not the same ", expectedReturn, actualReturn);
//  }

}
