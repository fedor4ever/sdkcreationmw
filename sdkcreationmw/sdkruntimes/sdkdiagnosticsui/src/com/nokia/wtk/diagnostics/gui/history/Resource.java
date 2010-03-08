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





package com.nokia.wtk.diagnostics.gui.history;

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

    // Resource Names for HistoryView
    public static final String HIS_PREV_BUTTON_LABEL = "history.prev.label";
    public static final String HIS_PREV_BUTTON_TOOLTIP = "history.prev.tooltip";
    public static final String HIS_PREVCONTEXT_BUTTON_LABEL = "history.prevcontext.label";
    public static final String HIS_PREVCONTEXT_BUTTON_TOOLTIP = "history.prevcontext.tooltip";
    public static final String HIS_NEWCONTEXT_BUTTON_LABEL = "history.newcontext.label";
    public static final String HIS_NEWCONTEXT_BUTTON_TOOLTIP = "history.newcontext.tooltip";
    public static final String HIS_CLEARPREVCONTEXTS_BUTTON_LABEL = "history.clear-previous-contexts.label";
    public static final String HIS_CLEARPREVCONTEXTS_BUTTON_TOOLTIP = "history.clear-previous-contexts.tooltip";
    public static final String HIS_SIZE_LABEL = "history.size.label";
    public static final String HIS_SIZE_NONE = "history.size.none";
    public static final String HIS_BUTTON_LABEL = "history.history.label";
    public static final String HIS_BUTTON_TOOLTIP = "history.history.tooltip";
    public static final String HIS_TOOLTIP = "history.tooltip";
    public final static String HIS_HELP_ID = "history.helpID";
    public static final String HIS_NO_HISTORY_TEXT = "history.nohistory.text";
    public static final String HIS_ICON = "resources/History16.gif";

    public static final String HIST_SUMMARY_SIZE_TXT = "historySummary.text.size";
    public static final String HIST_SUMMARY_CLEARED_TXT = "historySummary.text.cleared";
    public static final String HIST_SUMMARY_BACK_TXT = "historySummary.text.back";
    public static final String HIST_SUMMARY_FORW_TXT = "historySummary.text.forward";
    public static final String HIST_SUMMARY_POS_TXT = "historySummary.text.position";
    public static final String HIST_SUMMARY_REM_TXT = "historySummary.text.removed";
    public static final String HIST_SUMMARY_NP_TXT = "historySummary.text.newPath";
}

