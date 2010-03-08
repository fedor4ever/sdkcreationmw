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

package com.symbian.tools.j2me.sei.emulator.event;

import java.util.EventObject;

import com.symbian.tools.j2me.sei.emulator.EmulatorLauncher;

/**
 * Encapsulation of launcher events
 *
 * <p>Title: PREQ 425</p>
 * <p>Description: Java debugging</p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class EmulatorLauncherEvent extends EventObject{

  //
  // States Constants
  //

  /**
   * Entered state event type
   */
  public final static int ENTER_STATE = 0;

  /**
   * Exited state event type
   */
  public final static int EXIT_STATE = 1;

  /**
   * Skipped state event type
   */
  public final static int SKIPPED_STATE = 2;

  /**
   * Unhandled exception event type
   */
  public final static int UNHANDLED_EXCEPTION = 3;

  //
  // Modes Constants
  //

  /**
   * Launcher mode change type
   */
  public final static int MODE_CHANGE = 4;

  /**
   * Show the view.
   * The launcher dispatches this event after checking the user preferences.
   */
  public final static int SHOW_VIEW = 5;

  //
  // KDP events
  //

  /**
   * KDP agent attached to the VM
   */
  public static final int KDP_ATTACHED = 6;

  /**
   * KDP agent dettached from the VM
   */
  public static final int KDP_DETTACHED = 7;


  //
  // Members
  //

  /**
   * Event type
   */
  private int iType;

  /**
   * Launcher current state
   */
  private int iState;

  /**
   * Launcher current mode
   */
  private int iMode;

  /**
   * Exception
   */
  private Throwable iException;

  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aSource event source
   * @param aType event type
   * @param aState current launcher state
   */
  public EmulatorLauncherEvent(EmulatorLauncher aSource,
                               int aType,
                               int aMode,
                               int aState){
    super(aSource);
    iMode = aMode;
    iType = aType;
    iState = aState;
  }

  /**
   * Constructor
   *
   * @param aSource event source
   * @param aEx exception
   */
  public EmulatorLauncherEvent(EmulatorLauncher aSource,
                               Throwable aEx,
                               int aMode,
                               int aState){
    this(aSource, UNHANDLED_EXCEPTION, aMode, aState);
    iException = aEx;
  }

  //
  // Operations
  //

  /**
   * Get event type
   *
   * @return event type
   */
  public int getType(){
    return iType;
  }

  /**
   * Get launcher current mode
   *
   * @return launcher current mode
   */
  public int getMode(){
    return iMode;
  }


  /**
   * Get launcher current state
   *
   * @return launcher current state
   */
  public int getState(){
    return iState;
  }

  /**
   * Get exception
   *
   * @return exception
   */
  public Throwable getException(){
    return iException;
  }
}