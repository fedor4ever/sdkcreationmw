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

package com.symbian.io;


import java.util.*;

import java.io.*;

import com.symbian.utils.Debug;

import com.symbian.utils.SEIManifest;


/**
 * Descriptor file and properties
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class JADFile extends SEIManifest
{


  //
  // Constants
  //

  /**
   * J2ME permissions property
   */
  private static final String PERMISSIONS_PROPERTY = "MIDlet-Permissions";

  /**
   * MIDlet JAR URL property
   */
  private static final String JAR_URL_PROPERTY = "MIDlet-Jar-URL";

  /**
   * MIDlet JAR size property
   */
  private static final String JAR_SIZE_PROPERTY = "MIDlet-Jar-Size";

  /**
   * Suite name property
   */
  private static final String SUITE_NAME_PROPERTY = "MIDlet-Name";

  /**
   * Suite vendor property
   */
  private static final String VENDOR_NAME_PROPERTY = "MIDlet-Vendor";

  /**
   * Jad file suffix
   */
  public static final String JAD_SUFFIX = ".jad";

  /**
   * Jad prefix (required for File.createTempFile)
   */
  private static final String JAD_PREFIX = "seijad";


  //
  // Members
  //



  /**
   * Jar URL
   */
  private String iJarURL;

  /**
   * Jad file
   */
  private File iFile;


  //
  // Constructors
  //








  /**
   * Constructor.
   * Parse only the headers we're interested in and that can be accessed with
   * Getter methods.
   *
   * @param aFile Descriptor file
   *
   * @exception IOException if some error occurs
   */
  public JADFile(File aFile) throws IOException
  {
    iFile = aFile;
    init();
  }


  /**
   * Constructor.
   *
   * @param aJadFileName exisiting Jad file name
   *
   * @exception IOException if some error occured
   */
  public JADFile(String aJadFileName) throws IOException
  {
    this(new File(aJadFileName));
  }


  public JADFile(String aJadContents, boolean aIgnore) throws IOException {
    FileOutputStream fos = null;
    try {
      iFile = File.createTempFile(JAD_PREFIX, JAD_SUFFIX);
      iFile.deleteOnExit();
      Debug.println(this, "created Jad file: " + iFile.getPath());
      fos = new FileOutputStream(iFile);
      fos.write(aJadContents.getBytes());
    }
    catch (IOException ex) {
      throw ex;
    } finally {
      if ( fos != null ){
        try {
          fos.close();
        }
        catch (IOException ex1) {
        }
      }
    }
    init();
  }

  /**
   * Create a JAD file with the given properties
   *
   * @param aProperties JAD properties
   *
   * @exception IOException if some error occurs
   */
  public JADFile(Properties aProperties) throws IOException
  {
    FileOutputStream fos = null;
    try {
      iFile = File.createTempFile(JAD_PREFIX, JAD_SUFFIX);
      iFile.deleteOnExit();
      Debug.println(this, "created Jad file: " + iFile.getPath());
      fos = new FileOutputStream(iFile);
      String key;
      for(Iterator iter = aProperties.keySet().iterator(); iter.hasNext(); ){
        key = (String)iter.next();
        getMainAttributes().putAll(aProperties);// entrySet().add(new MapEntry ) putValue((String)key, (String)aProperties.get(key));
      }
      write(fos);
      fos.flush();
      init();
    }
    finally {
      if(fos != null){
        try {
          fos.close();
        }
        catch (Exception e) {}
      }
    }
  }


  /**
   * Init the JAD properties
   *
   * @throws IOException if some error occured
   */
  private void init() throws IOException
  {
    FileInputStream fis = null;
    try
    {
      fis = new FileInputStream(iFile);
      read(fis);
      parseMIDletsProperties();
      //parse JAR url property
      iJarURL = (String)getMainAttributes().get(JAR_URL_PROPERTY);
      if(iJarURL != null)
      {
        iJarURL = iJarURL.trim();
      }
    }
    finally
    {
      if(fis != null)
      {
        fis.close();
      }
    }
  }

  //
  // Methods
  //




  /**
   * Add J2ME permissions to the Jad
   *
   * @param aPermissions array of J2ME permissions
   */
  public void addJ2MEPermission(String[] aPermissions){
    if(aPermissions.length > 0){
      StringBuffer tmp = new StringBuffer();
      for (int i = 0; i < aPermissions.length; i++) {
        tmp.append(aPermissions[i] + ",");
      }
      addAttribute(PERMISSIONS_PROPERTY,
                   tmp.toString().substring(0, tmp.length() - 1));
    }
  }


  /**
   * Removes the following headers: MIDlet-Jar-Size, MIDlet-Jar-URL
   */
  public void removeJarProperties(){
    getMainAttributes().remove(JAR_SIZE_PROPERTY);
    getMainAttributes().remove(JAR_URL_PROPERTY);
  }


  /**
   * Set the jar URL
   *
   * @param the jar URL
   */
  public void setJarURL(String aURL)
  {
    iJarURL = aURL;
    getMainAttributes().put(JAR_URL_PROPERTY, aURL);
  }

  /**
   * Get Jar URL
   *
   * @return Jar URL
   */
  public String getJarURL(){
    return (String)getMainAttributes().get(JAR_URL_PROPERTY);
  }

  /**
   * Indicates if has Jar URL property
   *
   * @return true if has Jar URL property
   */
  public boolean containsJarURL(){
    return getMainAttributes().get(JAR_URL_PROPERTY) != null;
  }

  /**
   * Set the jar size
   *
   * @param the jar size
   */
  public void setJarSize(long aSize)
  {
    getMainAttributes().put(JAR_SIZE_PROPERTY, Long.toString(aSize));
  }

  /**
   * Get Jad file
   *
   * @return Jad file
   */
  public File getFile(){
    return iFile;
  }


  /**
   * Ensures the data is saved to the file
   *
   * @throws IOException if some error occured
   */
  public void flush() throws IOException{
    FileOutputStream fos = null;
    try
    {
      fos = new FileOutputStream(iFile);
      write(fos);
      fos.flush();
    }
    finally{
      if(fos != null){
        try {
          fos.close();
        }
        catch (Exception ex) {}
      }
    }
  }

  /**
   * Get suite name
   *
   * @return suite name
   */
  public String getSuiteName(){
    return (String)getMainAttributes().get(SUITE_NAME_PROPERTY);
  }

  /**
   * Get vendor name
   *
   * @return vendor name
   */
  public String getVendor(){
    return (String)getMainAttributes().get(VENDOR_NAME_PROPERTY);
  }

}
