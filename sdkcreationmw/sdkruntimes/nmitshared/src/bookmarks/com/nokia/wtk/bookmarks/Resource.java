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



package com.nokia.wtk.bookmarks;

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

    // Resource names for bookmark
    public static final String BOOKMARK_DEF_TITLE = "bookmarks.title.default";
    public static final String EDIT_BUTTON_LABEL = "bookmarks.edit.label";
    public static final String NEW_BUTTON_LABEL = "bookmarks.new.label";
    public static final String NEW_BUTTON_ICON = "bookmarks.new.icon";
    public static final String NEW_BOOKMARK_BUTTON_LABEL = "bookmarks.new_bookmark.label";
    public static final String NEW_FOLDER_BUTTON_LABEL = "bookmarks.new_folder.label";
    public static final String GO_BUTTON_LABEL = "bookmarks.go.label";
    public static final String DELETE_BUTTON_LABEL = "bookmarks.delete.label";
    public static final String OK_BUTTON_LABEL = "bookmarks.edit.done.label";
    public static final String CANCEL_BUTTON_LABEL = "bookmarks.edit.cancel.label";
    public static final String LOCATION_LABEL = "bookmarks.edit.location.label";
    public static final String TITLE_LABEL = "bookmarks.edit.title.label";
    public static final String COMMENT_LABEL = "bookmarks.edit.comment.label";
    public static final String ADD_TEXT = "bookmarks.edit.add.text";
    public static final String ADD_FOLDER_TEXT = "bookmarks.edit.addFolder.text";
    public static final String USER_BOOKMARKS_NAME = "bookmarks.branch-userFolder";
    public static final String SAMPLES_DIRECTORY_NAME = "bookmarks.branch-sampleFolder";
    public static final String OPTIONS_BUTTON_LABEL = "bookmarks.options.label";
    public static final String VIEW_LOCATIONS_LABEL = "bookmarks.viewLocations.label";
    public static final String VIEW_SAMPLES_LABEL = "bookmarks.viewSamples.label";
    public static final String SAMPLES_WIPING_WARNING_TEXT = "bookmarks.samples.warning";
    public static final String SAMPLES_WIPING_WARNING_TITLE = "bookmarks.samples.title";
    public static final String SAMPLES_WIPING_WARNING_DONT_ASK = "bookmarks.samples.dontWarn";

    // Resource names for BrowserComponentsFactory
    public static final String BOOKMARK_BUTTON_LABEL = "toolbar.bookmarks.label";
    public static final String BOOKMARK_MENU_EDIT_LABEL = "bookmarkMenu.edit.label";
    public static final String BOOKMARK_MENU_ADD_LABEL = "bookmarkMenu.add.label";



}
