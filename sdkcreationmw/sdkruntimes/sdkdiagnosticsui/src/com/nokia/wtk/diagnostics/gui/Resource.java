/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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





package com.nokia.wtk.diagnostics.gui;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  Resources set and in use by the gui package. Stored and
 * retrieved using AppEnvironment.getAppEnvironment.GetProperties(). Override
 * for new language or look by changing the properties file used to initialize
 * the AppEnvironment PropertySet;
 * Company:      Nokia
 * @version 1.0
 */

public interface Resource {

    // Resource Names:

    // for browser main window -- through ToolkitUI
    public static final String DIALOG_PREFERENCES_TITLE = "dialog.preferences.title";

    // main window
    public static final String TOOLBAR_TILE = "mainWindow.toolbar.tile.image";
    public static final String TOOLBAR_LEFT_CAP = "mainWindow.toolbar.leftCap.image";
    public static final String TOOLBAR_RIGHT_CAP = "mainWindow.toolbar.rightCap.image";
    public static final String TOOLBAR_LEFT_MARGIN = "mainWindow.toolbar.leftCap.indent";
    public static final String TOOLBAR_RIGHT_MARGIN = "mainWindow.toolbar.rightCap.indent";
    public static final String TEXT_MENU_NAME = "mainWindow.textMenu.label";

    // Toolkit Preferences
    public final static String HOMEPAGE_LABEL = "prefs.homepage.label";
    public final static String HOME_LABEL = "prefs.homepage.home.label";
    public static final String PREF_PANEL_WIDTH = "prefs.width";
    public static final String PREF_PANEL_HEIGHT = "prefs.height";
}
