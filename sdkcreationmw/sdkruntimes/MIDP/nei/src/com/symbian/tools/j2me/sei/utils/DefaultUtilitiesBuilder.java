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

package com.symbian.tools.j2me.sei.utils;

import java.awt.*;
import javax.swing.JFrame;

import com.symbian.tools.j2me.sei.preferences.*;



/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

class DefaultUtilitiesBuilder extends UtilitiesBuilder{



  //
  // Operations
  //

  /**
   * Create UI mediator
   *
   * @param aModel user preferences
   */
  public void createPreferencesMediator(UserPreferences aModel){
    iMediator = new PreferencesMediator
        (aModel, new JFrame(UtilitiesResources.UTILS_DIALOG_TITLE), new ButtonsPanel());
  }


  /**
   * Create main utilities panel
   */
  public void createUserPreferencesCompositeView(){
    iUserPreferencesView = new UserPreferencesCompositeView();
    iUserPreferencesView.add(iMediator.getButtonsPanel(), BorderLayout.SOUTH);

    iMediator.setTopView(iUserPreferencesView);
    iMediator.getFrame().getContentPane().add(iUserPreferencesView);
  }

  /**
   * Create suite preferences panel
   */
  public void createSuitePreferencesView(SuitePreferences aModel){
    PreferencesView view = new SuitePreferencesView(aModel);
    view.setChangeListener(iMediator);
    iUserPreferencesView.addView(view);
  }

  /**
   * Create security preferences panel
   */
  public void createSecurityPreferencesView(SecurityPreferences aModel){
    PreferencesView view = new SecurityPreferencesView(aModel);
    view.setChangeListener(iMediator);
    iUserPreferencesView.addView(view);
  }

  /**
   * Create UI preferences panel
   */
  public void createUIPreferencesView(UIPreferences aModel){
    PreferencesView view = new UIPreferencesView(aModel);
    view.setChangeListener(iMediator);
    iUserPreferencesView.addView(view);
  }

  /**
   * Create KDP preferences panel
   */
  public void createKDPPreferencesView(KDPPreferences aModel){
    PreferencesView view = new KDPPreferencesView(aModel);
    view.setChangeListener(iMediator);
    iUserPreferencesView.addView(view);
  }

}

