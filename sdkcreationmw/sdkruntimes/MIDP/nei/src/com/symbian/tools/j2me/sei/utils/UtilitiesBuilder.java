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

import java.util.Set;

import com.symbian.tools.j2me.sei.preferences.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

abstract class UtilitiesBuilder {


  //
  // Members
  //

  /**
   * User preferences view
   */
  UserPreferencesCompositeView iUserPreferencesView;

  /**
   * User preferences mediator
   */
  PreferencesMediator iMediator;

  //
  // Life cycle
  //

  UtilitiesBuilder() {
  }

  //
  // Operations
  //

  /**
   * Create UI mediator
   *
   * @param aModel user preferences
   */
  public abstract void createPreferencesMediator(UserPreferences aModel);

  /**
   * Create main utilities panel
   */
  public abstract void createUserPreferencesCompositeView();

  /**
   * Create suite preferences panel
   */
  public abstract void createSuitePreferencesView(SuitePreferences aModel);

  /**
   * Create security preferences panel
   */
  public abstract void createSecurityPreferencesView(SecurityPreferences aModel);

  /**
   * Create UI preferences panel
   */
  public abstract void createUIPreferencesView(UIPreferences aModel);

  /**
   * Create KDP preferences panel
   */
  public abstract void createKDPPreferencesView(KDPPreferences aModel);


  /**
   * Get User preferences view
   *
   * @return User preferences view
   */
  public PreferencesView getUserPreferencesView(){
    return iUserPreferencesView;
  }


  /**
   * Get preferences mediator
   *
   * @return preferences mediator
   */
  public PreferencesMediator getPreferencesMediator(){
    return iMediator;
  }
}