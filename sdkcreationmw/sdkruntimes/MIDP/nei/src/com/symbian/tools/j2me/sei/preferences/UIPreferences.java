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

import javax.swing.LookAndFeel;
import javax.swing.UIManager;
import java.io.*;

public class UIPreferences implements Serializable {

  //
  // Default members values
  // (used in setDefaultValues())
  //

  private static final boolean DEF_SHOW_VIEW = true;
  private static final boolean DEF_SHOW_TASK_BAR = true;
  private static final String DEF_PLAF_CLASS_NAME = UIManager.getLookAndFeel().getClass().getName();

  //
  // Members
  //

  /**
   * SEI view visibility indicator
   */
  private boolean iShowLauncherView;

  /**
   * Show in windows task bar (uses JNI plugin)
   */
  private boolean iShowInTaskBar;

  /**
   * Swing Look and Feel class name
   */
  private String iPlafClassName;


  //
  // Life cycle
  //

  /**
   * Default constructor.
   */
  public UIPreferences() {
    setDefaultValues();
  }

  //
  // Operation
  //

  /**
   * Set default values
   */
  protected void setDefaultValues(){
    iShowLauncherView = DEF_SHOW_VIEW;
    iShowInTaskBar = DEF_SHOW_TASK_BAR;
    iPlafClassName = DEF_PLAF_CLASS_NAME;
  }

  /**
   * Get SEI view visibility indicator
   *
   * @return true if to show the SEI view, false otherwise.
   */
  public boolean isShowLauncherView(){
    return iShowLauncherView;
  }


  /**
   * Set SEI view visibility indicator
   */
  public void setShowLauncherView(boolean aShowView){
    iShowLauncherView = aShowView;
  }

  /**
   * Get SEI show in task barindicator
   *
   * @return true if to show in the task bar, false otherwise.
   */
  public boolean isShowInTaskBar(){
    return iShowInTaskBar;
  }


  /**
   * Set SEI show in task bar indicator
   */
  public void setShowInTaskBar(boolean aShowInTaskBar){
    iShowInTaskBar = aShowInTaskBar;
  }


  /**
   * Get Swing Plaf class name
   *
   * @return Swing Plaf class name
   */
  public String getPlafClassName(){
    return iPlafClassName;
  }

  /**
   * Set Swing Plaf class name
   *
   * @param aPlaf Swing Plaf class name
   */
  public void setPlafClassName(String aPlafClassName){
    iPlafClassName = aPlafClassName;
  }
}
