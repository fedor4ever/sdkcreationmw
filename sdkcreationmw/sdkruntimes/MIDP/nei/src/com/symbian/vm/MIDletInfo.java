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

package com.symbian.vm;

import java.io.*;

import com.symbian.io.*;

import com.symbian.utils.*;

public class MIDletInfo implements Loadable, Dumpable{

  //
  // Members
  //


  /**
   * MIDlet storage ID
   */
  private int iID;

  /**
   * MIDlet name
   */
  private final String iName;

  /**
   * MIDlet suite
   */
  private final MIDletSuiteInfo iSuiteInfo;

  /**
   * Indicates if the MIDlet is running
   */
  private boolean iActive = false;


  //
  // Life cycle
  //

  /**
   * Constructor.
   *
   * @param aID MIDlet ID
   * @param aSuiteInfo Parent MIDlet suite
   */
  public MIDletInfo(String aName, int aID, MIDletSuiteInfo aSuiteInfo){
    iName = aName;
    iID = aID;
    iSuiteInfo = aSuiteInfo;
  }


  //
  // Operations
  //


  /**
   * Get the MIDlet ID
   *
   * @return MIDlet ID
   */
  public int getID(){
    return iID;
  }


  /**
   * Get the MIDlet name
   *
   * @return MIDlet name
   */
  public String getMIDletName(){
    return iName;
  }


  /**
   * Get MIDlet suite info
   *
   * @return MIDlet suite info
   */
  public MIDletSuiteInfo getMIDletSuiteInfo(){
    return iSuiteInfo;
  }

  /**
   * Set the MIDlet active state
   *
   * @param aActive the MIDlet active state
   */
  public void setActive(boolean aActive){
    iActive = aActive;
  }

  /**
   * Indicates if the MIDlet is active
   *
   * @return boolean if the MIDlet is active
   */
  public boolean isActive(){
    return iActive;
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
    //this method is not used. for future needs only.
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
    iSuiteInfo.dump(aDos);
    aDos.writeInt(iID);
  }

}
