// MIDletSuiteInfo.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.vm;


import java.util.Map;
import java.util.Iterator;
import java.io.IOException;
import java.io.DataInputStream;
import java.io.DataOutputStream;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

import com.symbian.io.*;
import com.symbian.utils.*;


public class MIDletSuiteInfo implements Loadable, Dumpable{


  //
  // Members
  //

  private int iID;

  /**
   * Suite name
   */
  private String iName;

  /**
   * Suite vendor
   */
  private String iVendor;

  /**
   * Suite version
   */
  private String iVersion;

  /**
   * Suite storage name
   */
  private String iStorageName;

  /**
   * Suite size in bytes
   */
  private String iSize;

  /**
   * Suite installation source
   */
  private String iInstalledFrom;

  /**
   * MIDlets in suite
   */
  private MIDletInfo[] iMIDlets;

  /**
   * Jad storage name
   */
  private String iJadStorageName;

  /**
   * Jar storage name
   */
  private String iJarStorageName;

  //
  // Life cycle
  //

  /**
   * Constructor for InstallCommant
   *
   * @param aJad JADFile containing MIDlets
   */
  public MIDletSuiteInfo(JADFile aJad){
    int num = aJad.getMIDlets().size();
    iName = aJad.getSuiteName();
    iVendor = aJad.getVendor();
    iMIDlets = new MIDletInfo[num];
    for(int i = 1; i <= num; i++){
      iMIDlets[i - 1] = new MIDletInfo(aJad.getMIDletName(i), i, this);
    }
  }

  /**
   * Create a new suite info from a local Jad and Jar.
   * This constructor is used only for the naive VM execution and for tests.
   *
   * @param aJadName Jad file name
   * @param aJarName Jar file name
   * @param aPath Path to files
   * @param aVirtualPath virtual path for storage name
   *
   * @throws IOException if some error occured
   */
  public MIDletSuiteInfo(String aJadName, String aJarName,
                         String aPath, String aVirtualPath) throws IOException{
    JADFile jad = new JADFile(aPath + aJadName);
    int num = jad.getMIDlets().size();
    iMIDlets = new MIDletInfo[num];
    for(int i = 1; i <= num; i++){
      iMIDlets[i - 1] = new MIDletInfo(jad.getMIDletName(i), i, this);
    }
    iJadStorageName = aVirtualPath + aJadName;
    iJarStorageName = aVirtualPath + aJarName;
  }



  //
  // Operations
  //


  /**
   * Get Jar storage name
   *
   * @return Jar storage name
   */
  public String getJarStorageName(){
    return iJarStorageName;
  }

  /**
   * Get Jad storage name
   *
   * @return Jad storage name
   */
  public String getJadStorageName(){
    return iJadStorageName;
  }

  /**
   * Set MIDlet suite ID.
   *
   * @param aID MIDlet suite ID assigned during installation
   */
  public void setId(int aID){
    iID = aID;
  }

  /**
   * Get MIDlet by name
   *
   * @param aName required MIDlet's name
   *
   * @return MIDlet info
   *
   * @exception IllegalArgumentException if failed to find such MIDlet
   */
  public MIDletInfo getMIDletInfo(String aName){
    for(int i = 0; i < iMIDlets.length; i++){
      if(iMIDlets[i].getMIDletName().equals(aName)){
         return iMIDlets[i];
      }
    }
    throw new IllegalArgumentException("MIDlet not found for ID: " + aName);
  }


  /**
   * Get suite vendor
   *
   * @return suite vendor
   */
  public String getVendor(){
    return iVendor;
  }

  /**
   * Get suite name
   *
   * @return suite name
   */
  public String getSuiteName(){
    return iName;
  }


  /**
   * Get array of MIDlet infos
   *
   * @return array of MIDlet infos
   */
  public MIDletInfo[] getMIDletsInfos(){
    return iMIDlets;
  }

  //
  // Loadable implementation
  //

  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException{
    iID = aDis.readInt();
  }


  //
  // Dumpable implementation
  //

  /**
   * Dump the object to the stream.
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException{
    aDos.writeInt(iID);
  }


}