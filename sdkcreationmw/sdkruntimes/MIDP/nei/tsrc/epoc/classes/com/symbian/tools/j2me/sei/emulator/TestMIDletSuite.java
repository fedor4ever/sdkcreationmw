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

import java.io.*;
import java.util.*;
import com.symbian.utils.Debug;
import com.symbian.tools.j2me.sei.SDKTestsProperties;

public class TestMIDletSuite {

  /**
   * Jad file
   */
  private final String iJAD;

  /**
   * Jar file
   */
  private final String iJAR;

  /**
   * Classes directory
   */
  private final String iClasses;

  /**
   * Class name of the primary test MIDlet
   */
  private final String iMIDlet1ClassName;

  /**
   * Name (as appears in the Jad) of the 2nd MIDlet.
   * (used in some Use-Cases tests only)
   */
  private final String iMIDlet2Name;

  /**
   * Non/Interactive confirmation
   */
  private final Confirmation iConfirmation;


  /**
   * C'tor for midlet suites from the V drive
   *
   * @param aConfirm Confirmation
   * @param aBundle PropertyResourceBundle
   */
  public TestMIDletSuite(Confirmation aConfirm, ResourceBundle aBundle) throws FileNotFoundException{
    String pathPrefix = SDKTestsProperties.SUITES_LOCATION_PREFIX;
    if(pathPrefix == null){
      pathPrefix = "";
    }
    iJAD = pathPrefix + aBundle.getString("JAD_FILE");
    if(!new File(iJAD).exists()){
      throw new FileNotFoundException(iJAD + " not found");
    }
    iJAR = pathPrefix + aBundle.getString("JAR_FILE");
    if(!new File(iJAR).exists()){
      throw new FileNotFoundException(iJAR + " not found");
    }
    iClasses = pathPrefix + aBundle.getString("CLASSES_FOLDER");
    if(!new File(iClasses).exists()){
      throw new FileNotFoundException(iClasses + " not found");
    }
    iMIDlet1ClassName = aBundle.getString("MIDLET1_CLASS_NAME");
    iMIDlet2Name = aBundle.getString("MIDLET2_NAME");
    iConfirmation = aConfirm;
  }



  public String getJAD() {
    return iJAD;
  }

  public String getJAR() {
    return iJAR;
  }

  public String getClassesFolder() {
    return iClasses;
  }

  public String getMIDlet1ClassName() {
    return iMIDlet1ClassName;
  }

  public String getMIDlet2Name() {
    return iMIDlet2Name;
  }

  public Confirmation getConfirmation() {
    return iConfirmation;
  }
}
