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

import java.io.InputStream;
import java.io.OutputStream;

import java.util.HashSet;
import java.util.Set;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public abstract class VMProcess {


  //
  // Members
  //

  /**
   * MIDlet suite info
   */
  protected MIDletSuiteInfo iMIDletSuiteInfo;

  /**
   * Set of active MIDlets
   */
  private Set iActiveMIDlets = new HashSet();

  /**
   * Indicates if the process is alive
   */
  private boolean iIsAlive = false;

  /**
   * Indicates if the process was already attached to
   * (implemented by the VMVerbosePiper)
   */
  private boolean iIsAttached = false;

  /**
   * Suite info
   */
  private MIDletSuiteInfo iSuiteInfo;



  //
  // Life cycle
  //

  /**
   * Default constructor.
   *
   * @param aSuiteInfo suite info
   */
  public VMProcess(MIDletSuiteInfo aSuiteInfo) {
    iSuiteInfo = aSuiteInfo;
  }


  //
  // Operations
  //




  /**
   * Indicates if this process is still alive
   *
   * @return true if this process is alive, false other wise.
   */
  public boolean isAlive(){
    return iIsAlive;
  }

  /**
   * Set the VM process state indicator
   *
   * @param aIsAlive state indicator
   */
  public void setAlive(boolean aIsAlive){
    iIsAlive = aIsAlive;
  }


  /**
   * Set we attached to the VM and the process is alive
   */
  public void setAttachedAndAlive(){
    iIsAttached = true;
    setAlive(true);
  }


  /**
   * Indicates if we already attached to the VM
   *
   * @return true if we already attached to the VM
   */
  public boolean isAttached(){
    return iIsAttached;
  }

  /**
   * Kills the subprocess.
   * The subprocess represented by this object is forcibly terminated.
   */
  public abstract void destroy();



  /**
   * Get VM process suite info
   *
   * @return MIDlet suite info
   */
  public MIDletSuiteInfo getMIDletSuiteInfo(){
    return iSuiteInfo;
  }


}
