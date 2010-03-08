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

public class SuitePreferences implements Serializable {


  //
  // Constants
  //

  /**
   * Jarred files type properties file
   */
  private static final String JAR_FILES = "jarfiletypes.properties";

  /**
   * Default Jad properties file
   */
  private static final String DEF_JAD_PROPS = "defaultjad.properties";

  /**
   * Files types key
   */
  private static final String FILES_KEY = "FILE_TYPES";

  /**
   * Class file suffix
   */
  private static final String CLASS_SUFFIX = "class";


  //
  // Members
  //

  /**
   * Jarred file types
   */
  private TreeSet iJarredFileTypes = new TreeSet();

  /**
   * Template Jad.
   * This includes the default values for all MUST properties.
   */
  private Properties iDefaultJadProperties = new Properties();




  //
  // Life cycle
  //

  public SuitePreferences(){
  }




  //
  // Operation
  //


  /**
   * Ensure that "class" files are always included
   */
  private void ensureJarredTypesContainsClassSuffix(){
    iJarredFileTypes.add(CLASS_SUFFIX);
  }

  /**
   * Set default values
   *
   * @exception IOException if some error oocured
   */
  protected void setDefaultValues() throws IOException
  {
    //load all supported file types (a MUST - "class" files)
    iJarredFileTypes.clear();
    ensureJarredTypesContainsClassSuffix();
    Properties props = new Properties();
    props.load(getClass().getResourceAsStream(JAR_FILES));
    StringTokenizer tokenizer = new StringTokenizer(props.getProperty(
        FILES_KEY));
    while (tokenizer.hasMoreTokens()) {
      iJarredFileTypes.add(tokenizer.nextToken());
    }

    //load default Jad properties
    iDefaultJadProperties.clear();
    iDefaultJadProperties.load(getClass().getResourceAsStream(DEF_JAD_PROPS));
  }


  /**
   * Get the set of jarred file types
   *
   * @return set of jarred file types
   */
  public Set getFileTypes(){
    return iJarredFileTypes;
  }

  /**
   * Set the jarred file types
   *
   * @param aFileTypes new set of jarred file types
   */
  public void setFileTypes(Set aFileTypes){
    iJarredFileTypes = new TreeSet(aFileTypes);
    ensureJarredTypesContainsClassSuffix();
  }

  /**
   * Add new file type
   *
   * @param aFileType new file type
   */
  public void addFileType(String aFileType){
    iJarredFileTypes.add(aFileType);
  }

  /**
   * Remove the given file type
   *
   * @param aFileType removed file type
   */
  public void removeFileType(String aFileType){
    iJarredFileTypes.remove(aFileType);
    ensureJarredTypesContainsClassSuffix();
  }

  /**
   * Get default Jad properties.
   *
   * @return default Jad properties.
   */
  public Properties getDefaultJadProperties(){
    return iDefaultJadProperties;
  }

  /**
   * Set default Jad properties.
   *
   * @param aProperties default Jad properties.
   */
  public void setDefaultJadProperties(Properties aProperties){
    iDefaultJadProperties = aProperties;
  }


  /**
   * Put Jad default property
   *
   * @param aKey property key
   * @param aValue property value
   */
  public void putDefaultJadProperty(String aKey, String aValue){
    iDefaultJadProperties.put(aKey, aValue);
  }





}
