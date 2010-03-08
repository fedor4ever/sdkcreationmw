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
import java.util.jar.*;
import java.util.*;
import java.io.*;

import com.symbian.io.JADFile;

public class TestSEIManifest extends TestCase {
  private SEIManifest iManifest = null;
  private File iJad;


  public TestSEIManifest()throws Exception {
    super.setUp();
    iJad = File.createTempFile("testSEIjad", "jad");
    iJad.deleteOnExit();

    Properties props = new Properties();
    InputStream is = getClass().getResourceAsStream("testmanifest.properties");
    FileOutputStream fos = new FileOutputStream(iJad);
    Utilities.copyToFile(is, iJad.getAbsolutePath());
    is.close();

  }

  public void setUp() throws Exception {
    super.setUp();
    iManifest = new SEIManifest(new JADFile(iJad));
  }



  public void testContainsMIDletClass() {
    assertTrue("should contain class", iManifest.containsMIDletClass("MIDlet1Class"));
    assertTrue("should contain class", iManifest.containsMIDletClass("MIDlet2Class"));
    assertFalse("should not contain class", iManifest.containsMIDletClass("MIDlet3Class"));
  }

  public void testGetMIDletNumber() {
    String aClassName = "MIDlet1Class";
    int expectedReturn = 1;
    int actualReturn = iManifest.getMIDletNumber(aClassName);
    assertEquals("MIDlet number should be 1", expectedReturn, actualReturn);

    aClassName = "MIDlet2Class";
    expectedReturn = 2;
    actualReturn = iManifest.getMIDletNumber(aClassName);
    assertEquals("MIDlet number should be 2", expectedReturn, actualReturn);
  }

  public void testGetDefaultMIDletNames() {
    for(int aIndex = 0; aIndex < 5; aIndex++){
      String expectedReturn = "SDKMIDlet" + aIndex;
      String actualReturn = iManifest.getDefaultMIDletName("SDKMIDlet" + aIndex);
      assertEquals("return value", expectedReturn, actualReturn);
    }
  }

  public void testGetMIDlets() {
    Map map = iManifest.getMIDlets();
    assertTrue("should contain only 2 MIDlets", map.size() == 2);
    assertTrue("should contain MIDlet1Class", map.containsKey("MIDlet1Class"));
    assertTrue("should contain MIDlet2Class", map.containsKey("MIDlet2Class"));
    assertEquals("wrong MIDlet1Class name", map.get("MIDlet1Class"), "MIDlet1Name");
    assertEquals("wrong MIDlet2Class name", map.get("MIDlet2Class"), "MIDlet2Name");
  }

  public void testGetDefaultMIDletName() {
    String expectedReturn = "SDKMIDlet";
    String actualReturn = iManifest.getDefaultMIDletName("SDKMIDlet");
    assertEquals("return value", expectedReturn, actualReturn);
  }

  public void testAddDefaultMIDlet() {
    String aClassName = "DefaultMIDletClass";
    iManifest.addDefaultMIDlet(aClassName);
    Map map = iManifest.getMIDlets();
    assertTrue("should contain 3 MIDlets", map.size() == 3);
    assertTrue("should contain DefaultMIDletClass", map.containsKey("DefaultMIDletClass"));
    assertEquals("wrong DefaultMIDletClass name", map.get("DefaultMIDletClass"), "DefaultMIDletClass");
    assertTrue("should contain MIDlet1Class", map.containsKey("MIDlet1Class"));
    assertTrue("should contain MIDlet2Class", map.containsKey("MIDlet2Class"));
    assertEquals("wrong MIDlet1Class name", map.get("MIDlet1Class"), "MIDlet1Name");
    assertEquals("wrong MIDlet2Class name", map.get("MIDlet2Class"), "MIDlet2Name");
  }

  public void testGetMIDletName() {
    String expectedReturn = "MIDlet1Name";
    String actualReturn = iManifest.getMIDletName(1);
    assertEquals("return value", expectedReturn, actualReturn);
    expectedReturn = "MIDlet2Name";
    actualReturn = iManifest.getMIDletName(2);
    assertEquals("return value", expectedReturn, actualReturn);
  }

  public void testAddAttribute() {
    String aKey = "key";
    String aValue = "value";
    iManifest.addAttribute(aKey, aValue);
    assertTrue("should contain key", iManifest.getMainAttributes().containsKey("key"));
    assertTrue("should contain value", iManifest.getMainAttributes().containsValue("value"));
    assertEquals("should contain key->value", iManifest.getMainAttributes().get("key"), "value");
  }

  public void testOverrideAttributes() throws IOException {
    //override
    Properties props = new Properties();
    InputStream is = getClass().getResourceAsStream("overridingtestmanifest.properties");
    props.load(is);
    JADFile temp = new JADFile(props);
    is.close();
    SEIManifest New_manifest = new SEIManifest(new JADFile(iJad));
    New_manifest.overrideAttributes(temp);

    //test all entries
    assertEquals("should contain same number of entries", temp.getMainAttributes().size(), New_manifest.getMainAttributes().size());
    Set keys = iManifest.getMainAttributes().keySet();
    String key;
    String value;
    String new_value;
    for(Iterator iter = keys.iterator(); iter.hasNext(); ){
      key = (String)iter.next();
      value = (String)iManifest.getMainAttributes().get(key);
      assertTrue(New_manifest.getMainAttributes().containsKey(key));
      new_value = (String)New_manifest.getMainAttributes().get(key);
      assertEquals("new value should be New_" + value, new_value, "New_" + value);
    }
  }



}
