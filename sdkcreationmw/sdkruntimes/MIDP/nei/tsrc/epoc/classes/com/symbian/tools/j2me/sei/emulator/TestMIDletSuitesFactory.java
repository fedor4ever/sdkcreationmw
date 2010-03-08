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

import java.util.*;
import java.io.*;
import com.symbian.tools.j2me.sei.SDKTestsProperties;

public class TestMIDletSuitesFactory {

  //
  // Constants
  //


  /*dummyCA for 8.1*/
  public static final String KEY_ALIAS;

  /*"keypwd for 8.1"*/
  public static final String KEY_PASSWORD;

  /*"D:\JTWI-TCK_10\JTWI-TCK\lib\midptck.ks for 8.1"*/
  public static final String KEYSTORE_FILE;

  /*"D:\JTWI-TCK_10\JTWI-TCK\lib\midptck.ks for 8.1"*/
  public static final String KEYSTORE_PASSWORD;

  //
  // Life cycle
  //

  static {
    System.out.println("loading keystore properties from: " +
                       SDKTestsProperties.NON_INTERACTIVE_SUITES_LOCATION);
    Properties props = new Properties();
    try {
      FileInputStream fis = new FileInputStream(SDKTestsProperties.
                                                NON_INTERACTIVE_SUITES_LOCATION);
      props.load(fis);
      fis.close();
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit( -1);
    }
    KEY_ALIAS = props.getProperty("KEY_ALIAS__9_1");
    KEY_PASSWORD = props.getProperty("KEY_PASSWORD___9_1");
    KEYSTORE_FILE = props.getProperty("KEYSTORE_FILE__9_1");
    KEYSTORE_PASSWORD = props.getProperty("KEYSTORE_PASSWORD__9_1");
    System.out.println("KEY_ALIAS = " + KEY_ALIAS);
    System.out.println("KEY_PASSWORD = " + KEY_PASSWORD);
    System.out.println("KEYSTORE_FILE = " + KEYSTORE_FILE);
    System.out.println("KEYSTORE_PASSWORD = " + KEYSTORE_PASSWORD);
  }

  //
  // Operations
  //



  /**
   * Get interactive test suite
   */
  private static TestMIDletSuite getInteractiveTestMIDletSuite(String aFile) throws
      Exception {
    FileInputStream fis = null;
    try {
      fis = new FileInputStream(aFile);
      PropertyResourceBundle bundle = new PropertyResourceBundle(fis);

      return new TestMIDletSuite(new InteractiveConfirmation(
          "Please confirm you see the MIDlet running"), bundle);
    }
    finally {
      try {
        fis.close();
      }
      catch (Exception ex) {
      }
    }
  }

  /**
   * Get non-interactive test suite
   */
  private static TestMIDletSuite getNonInteractiveTestMIDletSuite(String aFile) throws
      Exception {
    FileInputStream fis = null;
    try {
      fis = new FileInputStream(aFile);
      PropertyResourceBundle bundle = new PropertyResourceBundle(fis);

      return new TestMIDletSuite(new NonInteractiveConfirmation(), bundle);
    }
    finally {
      try {
        fis.close();
      }
      catch (Exception ex) {
      }
    }
  }

  /**
   * Get test suite factory method
   */
  public static final TestMIDletSuite getTestMIDletSuite(boolean aInteractive) {
    try {
      if (aInteractive) {
        if ("true".equalsIgnoreCase(SDKTestsProperties.IS_INTERACTIVE_SUITES)) {
          return getInteractiveTestMIDletSuite
              (SDKTestsProperties.INTERACTIVE_SUITES_LOCATION);
        }
        else {
          throw new IllegalArgumentException(
              "cannot create interactive test suites");
        }
      }
      else {
        if ("false".equalsIgnoreCase(SDKTestsProperties.IS_INTERACTIVE_SUITES)) {
          return getNonInteractiveTestMIDletSuite
              (SDKTestsProperties.NON_INTERACTIVE_SUITES_LOCATION);
        }
        else {
          throw new IllegalArgumentException(
              "cannot create non-interactive test suites");
        }
      }
    }
    catch (Exception e) {
      System.out.println("--- could not create test midlet suite ----");
      e.printStackTrace();
      throw new RuntimeException(e.getMessage());
    }
  }

}
