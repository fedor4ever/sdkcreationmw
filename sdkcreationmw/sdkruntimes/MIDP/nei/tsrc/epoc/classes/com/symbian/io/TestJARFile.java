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
import java.util.*;
import java.util.jar.*;
import junit.framework.Assert;
import junit.framework.TestCase;




public class TestJARFile extends TestCase {
  private static File sManifestFile;

  private static final String JARRED_FILE_TYPES[] = {
      "class", "properties", "png", "wav"
  };

  private static final Set JARRED_FILE_TYPES_SET = arrayToSet(JARRED_FILE_TYPES);

  private static final String UNJARRED_FILE_TYPES[] = {
      "java", "cpp", "jar", "jad", "mf"
  };
  private String jarredClassPath;
  private String unJarredClassPath;
  private String singleFileClassPath;
  private Set expected;

  public TestJARFile(String name) throws IOException {
    super(name);
    jarredClassPath = System.getProperty("java.io.tmpdir") + "jarredclasspath\\";
    unJarredClassPath = System.getProperty("java.io.tmpdir") + "unjarredclasspath\\";
    singleFileClassPath = System.getProperty("java.io.tmpdir") + "singlefileclasspath\\";
    sManifestFile = File.createTempFile("testjar", "mf");
    sManifestFile.deleteOnExit();
  }

  private void initTestClassPath() throws IOException {
    expected = new HashSet();
    File dir1 = new File(String.valueOf(String.valueOf(jarredClassPath)).concat(
        "a\\"));
    dir1.mkdirs();
    dir1.deleteOnExit();
    File tmp;
    for (int i = 0; i < JARRED_FILE_TYPES.length; i++) {
      tmp = new File(String.valueOf(String.valueOf( (new StringBuffer(String.
          valueOf(String.valueOf(jarredClassPath)))).append("a\\test.").append(
          JARRED_FILE_TYPES[i]))));
      expected.add("a/test.".concat(String.valueOf(String.valueOf(
          JARRED_FILE_TYPES[i]))));
      tmp.createNewFile();
      tmp.deleteOnExit();
    }

    File dir2 = new File(String.valueOf(String.valueOf(unJarredClassPath)).
                         concat("b\\"));
    dir2.mkdirs();
    dir2.deleteOnExit();
    for (int i = 0; i < UNJARRED_FILE_TYPES.length; i++) {
      tmp = new File(String.valueOf(String.valueOf( (new StringBuffer(String.
          valueOf(String.valueOf(unJarredClassPath)))).append("b\\test.").
          append(UNJARRED_FILE_TYPES[i]))));
      tmp.createNewFile();
      tmp.deleteOnExit();
    }

    File dir3 = new File(singleFileClassPath);
    dir3.mkdirs();
    dir3.deleteOnExit();
    tmp = new File(String.valueOf(String.valueOf(singleFileClassPath)).concat(
        "single.class"));
    expected.add("single.class");
    tmp.createNewFile();
    tmp.deleteOnExit();
  }

  protected void setUp() throws Exception {
    super.setUp();
    initTestClassPath();
  }

  protected void tearDown() throws Exception {
    super.tearDown();
  }

  public void testAccept() throws IOException {
    JARFile jar = new JARFile(JARRED_FILE_TYPES_SET , new String[] {singleFileClassPath}, sManifestFile);
    FilenameFilter filter = jar;
    File dir = new File("C:\\");
    for (int i = 0; i < JARRED_FILE_TYPES.length; i++)
      Assert.assertTrue("must accept " + JARRED_FILE_TYPES[i] + " files",
                        filter.accept(dir, "." + JARRED_FILE_TYPES[i]));

    for (int i = 0; i < UNJARRED_FILE_TYPES.length; i++)
      Assert.assertFalse("probabaly shouldnt accept " + UNJARRED_FILE_TYPES[i] +
                         " files", filter.accept(dir, "." + UNJARRED_FILE_TYPES[i]));

    Assert.assertTrue("must accept directories for recursion", filter.accept(dir, ""));
  }

  public void testJarSingleClass() throws IOException {
    JARFile jar = new JARFile(JARRED_FILE_TYPES_SET , new String[] {singleFileClassPath}, sManifestFile);
    JarInputStream jis = new JarInputStream(new FileInputStream(jar.getFile()));
    JarEntry entry = jis.getNextJarEntry();
    Assert.assertNotNull(entry);
    Assert.assertEquals("single.class entry not found", "single.class",
                        entry.getName());
  }

  public void testJarRecursive() throws IOException {
    JARFile jar = new JARFile(JARRED_FILE_TYPES_SET,
                              new String[] {singleFileClassPath, jarredClassPath,unJarredClassPath},
                              sManifestFile);
    JarInputStream jis = new JarInputStream(new FileInputStream(jar.getFile()));
    JarEntry entry = jis.getNextJarEntry();
    do {
      Assert.assertNotNull("should not be null", entry);
      Assert.assertTrue(String.valueOf(String.valueOf(entry.getName())).concat(
          " is unexpected file in Jar"), expected.contains(entry.getName()));
      entry = jis.getNextJarEntry();
    }
    while (entry != null);
  }

  public void testRemoveDuplicateFolders() throws IOException {
    JARFile jar = new JARFile(JARRED_FILE_TYPES_SET , new String[] {singleFileClassPath, singleFileClassPath}, sManifestFile);
    JarInputStream jis = new JarInputStream(new FileInputStream(jar.getFile()));
    JarEntry entry = jis.getNextJarEntry();
    Assert.assertNotNull(entry);
    Assert.assertNull(jis.getNextJarEntry());
    Assert.assertEquals("single.class entry not found", "single.class", entry.getName());
  }

  private static Set arrayToSet(String[] array){
    Set s = new HashSet(array.length);
    for(int i = 0; i < array.length; i++){
      s.add(array[i]);
    }
    return s;
  }


}
