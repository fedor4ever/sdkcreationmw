/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.preferences;

import java.io.*;

import com.symbian.utils.*;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

public class UserPreferences implements Serializable {


  //
  // Constants
  //


  /**
   * User preferences file
   */
  private static final String PREFERENCES_FILE =
      SDKRuntimeProperties.SDK_HOME + File.separator + "symbian" +
      File.separator + "lib" + File.separator + "prefs.sdk";


  //
  // Members
  //

  /**
   * KDP preferences
   */
  private KDPPreferences iKDPPreferences = new KDPPreferences();

  /**
   * Security preferences
   */
  private SecurityPreferences iSecurityPreferences = new SecurityPreferences();

  /**
   * Suite preferences
   */
  private SuitePreferences iSuitePreferences = new SuitePreferences();

  /**
   * UI preferences
   */
  private UIPreferences iUIPreferences = new UIPreferences();

  /**
   * Command line prferences
   */
  private CommandLinePreferences iCmdLinePreferences = new CommandLinePreferences();




  //
  // Life cycle
  //

  /**
   * Default constructor.
   *
   * @exception IOException if some error occured
   */
  public UserPreferences() throws IOException {
    setDefaultValues();
  }

  //
  // Operations
  //


  /**
   * Get the KDP preferences
   *
   * @return KDP preferences
   */
  public KDPPreferences getKDPPreferences(){
    return iKDPPreferences;
  }



  /**
   * Get the security preferences
   *
   * @return security preferences
   */
  public SecurityPreferences getSecurityPreferences(){
    return iSecurityPreferences;
  }



  /**
   * Get the suite preferences
   *
   * @return suite preferences
   */
  public SuitePreferences getSuitePreferences(){
    return iSuitePreferences;
  }



  /**
   * Get the UI preferences
   *
   * @return UI preferences
   */
  public UIPreferences getUIPreferences(){
    return iUIPreferences;
  }

  /**
   * Get the command line preferences
   *
   * @return command line preferences
   */
  public CommandLinePreferences getCommandLinePreferences(){
    return iCmdLinePreferences;
  }

  /**
   * Factory method for reading user preferences using the fallback.
   *
   * First tries loading from the user file.
   * If fails, loads the defaults from the distribution unit.
   */
  public static UserPreferences load() throws IOException {
    ObjectInputStream os = null;
    UserPreferences prefs;
    try {
      //load from file
      Debug.println(UserPreferences.class.getName(), "loading SEI preferences from " + PREFERENCES_FILE);
      os = new ObjectInputStream(new FileInputStream(PREFERENCES_FILE));
      prefs = (UserPreferences)os.readObject();
    }
    catch (Exception e) {
      //load default settings
      Debug.println(UserPreferences.class.getName(), "caught exception:" + e + " - " + e.getMessage());
      Debug.println(UserPreferences.class.getName(), "using default user preferences values");
      prefs = new UserPreferences();

      //save
      prefs.save();
    }
    finally {
      if (os != null) {
        try {
          os.close();
        }
        catch (Exception e) {}
      }
    }
    return prefs;
  }


  /**
   * Serialize this into a cache file
   *
   * @throws IOException if some error occured
   */
  public void save() throws IOException{
    ObjectOutputStream os = null;
    try {
      Debug.println(UserPreferences.class.getName(), "saving SEI preferences to " + PREFERENCES_FILE);
      // Create prefs directory if it doesn't exist
      new File(PREFERENCES_FILE).getParentFile().mkdirs();
      os = new ObjectOutputStream(new FileOutputStream(PREFERENCES_FILE));
      os.writeObject(this);
      os.flush();
    }
    finally {
      if (os != null) {
        try {
          os.close();
        }
        catch (Exception e) {}
      }
    }
  }


  /**
   * Set default values
   *
   * @exception IOException if some error occured
   */
  public void setDefaultValues() throws IOException{
    iKDPPreferences.setDefaultValues();
    iSecurityPreferences.setDefaultValues();
    iSuitePreferences.setDefaultValues();
    iUIPreferences.setDefaultValues();
    iCmdLinePreferences.setDefaultValues();
  }
}


