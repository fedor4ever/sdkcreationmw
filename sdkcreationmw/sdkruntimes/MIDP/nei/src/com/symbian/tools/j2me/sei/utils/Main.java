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

import java.io.*;
import com.symbian.tools.j2me.sei.SDKMainBase;
import com.symbian.tools.j2me.sei.preferences.*;

import com.symbian.utils.Debug;
import com.symbian.utils.SystemExitHandler;


/**
 * SEI application entry point.
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class Main extends SDKMainBase
{

  //
  // Life cycle
  //

  /**
   * Application entry point
   *
   * @param aArgs IDE's command line arguments
   */
  public static void main(String[] aArgs)
  {
    initDebugLogs(aArgs);

    //launch
    try
    {
      //create user preferences
      UserPreferences prefs = UserPreferences.load();

      //create concrete builder
      UtilitiesBuilder builder = new DefaultUtilitiesBuilder();

      //create components
      builder.createPreferencesMediator(prefs);
      builder.createUserPreferencesCompositeView();
      builder.createKDPPreferencesView(prefs.getKDPPreferences());
      builder.createSecurityPreferencesView(prefs.getSecurityPreferences());
      builder.createSuitePreferencesView(prefs.getSuitePreferences());
      //builder.createUIPreferencesView(prefs.getUIPreferences());

      //show frame
      builder.getPreferencesMediator().showViewWindow();
    }
    catch (Exception e)
    {
      Debug.println(Main.class.getName(), e);
      SystemExitHandler.invokeSystemExit(-31);
    }

  }

}
