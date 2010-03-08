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
import java.util.*;

import com.symbian.utils.Debug;

public class SecurityPreferences implements Serializable {

  //
  // Constants
  //

  /**
   * J2ME permissions
   */
  private static final String PERMISSIONS = "j2mepermissions.properties";

  /**
   * List of all optional J2ME permissions
   */
  private static String[] sJ2MEPermissions = null;

  //
  // Default members values
  // (used in setDefaultValues())
  //

  private static final String DEF_CHALLENGE = "";
  private static final String DEF_KEYSTORE_FILE = "";
  private static final char[] DEF_KEYSTORE_PASSWORD = new String().toCharArray();
  private static final boolean DEF_ADD_SIGNATURE = false;
  private static final String[] DEF_REQUIRED_PERMISSIONS = new String[0];
  private static final char[] DEF_KEY_PASSWORD = new String().toCharArray();
  private static final String DEF_KEY_ALIAS = null;
  private static final boolean DEF_IS_TRUSTED = false;




  //
  // Members
  //

  /**
   * Keystore file
   */
  private String iKeystoreFile = "";

  /**
   * Keystore password
   */
  private char[] iKeystorePassword = new String().toCharArray();

  /**
   * Indicates always to add Jar signature to Jad file.
   */
  private boolean iAlwaysAddSignature = false;

  /**
   * Required permissions
   */
  private String[] iRequiredPermissions = new String[0];

  /**
   * Key password
   */
  private char[] iKeyPassword = new String().toCharArray();

  /**
   * Key alias name
   */
  private String iKeyAlias = null;

  /**
   * Trust mode indicator
   */
  private boolean iIsTrusted = false;

  /**
   * Challenge for target debugging.
   * The challenge is set by the SEI-Slave and the user should set it on the PC
   * through the utilities panel.
   */
  private String iChallenge = "";



  //
  // Life cycle
  //


  /**
   * Load All J2ME permissions
   */
  static
  {
    BufferedReader reader = null;
    Set tmpSet = new HashSet(20);
    try {
      InputStream is = SecurityPreferences.class.getResourceAsStream(PERMISSIONS);
      reader = new BufferedReader
          (new InputStreamReader( (is)));
      String tmpPerm = reader.readLine();
      while (tmpPerm != null) {
        tmpSet.add(tmpPerm);
        tmpPerm = reader.readLine();
      }
    }
    catch (EOFException eof) {
      Debug.println(SecurityPreferences.class.getName(), "caught: " + eof);
    }
    catch (IOException e) {
      Debug.printStackTrace(SecurityPreferences.class.getName(), e);
    }
    finally {
      sJ2MEPermissions = new String[tmpSet.size()];
      tmpSet.toArray(sJ2MEPermissions);
      if (reader != null) {
        try {
          reader.close();
        }
        catch (Exception e) {
          Debug.printStackTrace(SecurityPreferences.class.getName(), e);
        }
      }
    }
  }

  /**
   * Default constructor.
   */
  public SecurityPreferences() {
    setDefaultValues();
  }





  //
  // Operation
  //


  /**
   * Set default values
   */
  protected void setDefaultValues(){
    iChallenge = DEF_CHALLENGE;
    iKeystoreFile = DEF_KEYSTORE_FILE;
    iKeystorePassword = DEF_KEYSTORE_PASSWORD;
    iAlwaysAddSignature = DEF_ADD_SIGNATURE;
    iRequiredPermissions = DEF_REQUIRED_PERMISSIONS;
    iKeyPassword = DEF_KEY_PASSWORD;
    iKeyAlias = DEF_KEY_ALIAS;
    iIsTrusted = DEF_IS_TRUSTED;
  }

  /**
   * Set keystore file
   *
   * @param aKeystoreFile new keystore file
   */
  public void setKeystoreFile(String aKeystoreFile) {
    iKeystoreFile = aKeystoreFile;
  }

  /**
   * Set keystore password
   *
   * @param aPassword new keystore password
   */
  public void setKeystorePassword(char[] aPassword) {
    iKeystorePassword = aPassword;
  }

  /**
   * Set key password
   *
   * @param aPassword password
   */
  public void setKeyPassword(char[] aPassword){
    iKeyPassword = aPassword;
  }

  /**
   * Set always add Jar signature
   *
   * @param aAlwaysSign new always add Jar signature state
   */
  public void setAlwaysAddSignature(boolean aAlwaysSign) {
    iAlwaysAddSignature = aAlwaysSign;
  }



  /**
   * Get all J2ME permissions
   *
   * @return array of J2ME permissions
   */
  public static String[] getJ2MEPermissions(){
    return sJ2MEPermissions;
  }

  /**
   * Set selected permissions indices
   *
   * @param aPermissions selected permissions
   */
  public void setRequiredPermissions(String[] aPermissions){
    iRequiredPermissions = aPermissions;
  }

  /**
   * Get array of selected permissions
   *
   * @return array of selected permissions
   */
  public String[] getRequiredPermissions(){
    return iRequiredPermissions;
  }

  /**
   * Get the keystore file
   *
   * @return keystore file
   */
  public String getKeystoreFile() {
    return iKeystoreFile;
  }

  /**
   * Get the keystore password
   *
   * @return keystore password
   */
  public char[] getKeystorePassword() {
    return iKeystorePassword;
  }

  /**
   * Indicate if always to add Jar signature
   *
   * @return true if to add Jar signature, false otherwise.
   */
  public boolean isAlwaysAddSignature() {
    return iAlwaysAddSignature;
  }


  /**
   * Get the key password
   *
   * @return key password
   */
  public char[] getKeyPassword(){
    return iKeyPassword;
  }

  /**
   * Get the key alias
   *
   * @return key alias
   */
  public String getKeyAlias(){
    return iKeyAlias;
  }

  /**
   * Set key alias
   *
   * @param aAlias key alias
   */
  public void setKeyAlias(String aAlias){
    iKeyAlias = aAlias;
  }

  /**
   * Get trust mode indicator
   *
   * @return trust mode indicator
   */
  public boolean isTrusted(){
    return iIsTrusted;
  }


  /**
   * Set trust mode indicator
   *
   * @param aTrusted trust mode indicator
   */
  public void setTrusted(boolean aTrusted){
    iIsTrusted = aTrusted;
  }


  /**
   * Get the target challenge
   *
   * @return target challenge
   */
  public String getChallenge(){
    return iChallenge;
  }

  /**
   * Set target challenge
   *
   * @param aChallenge target challenge
   */
  public void setChallenge(String aChallenge){
    iChallenge = aChallenge;
  }

}
