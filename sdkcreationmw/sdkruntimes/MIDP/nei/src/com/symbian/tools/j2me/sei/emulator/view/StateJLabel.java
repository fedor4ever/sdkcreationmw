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

package com.symbian.tools.j2me.sei.emulator.view;


import java.awt.image.ImageObserver;
import java.awt.Image;

import javax.swing.*;

import com.symbian.tools.j2me.sei.emulator.EmulatorResources;


/**
 * Default UI component - Swing checkbox.
 *
 * <p>Title: PREQ 425</p>
 * <p>Description: Java debugging</p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
class StateJLabel extends JLabel implements StateView, ImageObserver{

  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aLabel text
   */
  StateJLabel(String aLabel){
    super(aLabel);
    setHorizontalTextPosition(SwingConstants.RIGHT);
    setIcon(EmulatorResources.sEmptyImage);
  }

  //
  // StateViewComponent implementation
  //

  /**
   * Set the state active mode
   *
   * @param aActive active state
   */
  public void setActive(boolean aActive){
    if(aActive){
      setBorder(BorderFactory.createRaisedBevelBorder());
      setIcon(EmulatorResources.sPointerImage);
    }
    else{
      setBorder(null);
      setIcon(EmulatorResources.sVImage);
    }
  }

  /**
   * Set an error occured
   */
  public void setError(){
    setBorder(null);
    setIcon(EmulatorResources.sXImage);
  }

  /**
   * Set skipped
   */
  public void setSkipped(){
    setEnabled(false);
  }




}
