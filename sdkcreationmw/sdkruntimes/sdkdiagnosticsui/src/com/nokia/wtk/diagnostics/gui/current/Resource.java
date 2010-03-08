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





package com.nokia.wtk.diagnostics.gui.current;

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


    // Resources for Variables view
    public static final String VAR_CLEARALL_BUTTON_LABEL = "variables.clear-all.label";
    public static final String VAR_CLEARALL_BUTTON_TOOLTIP = "variables.clear-all.tooltip";
    public static final String VAR_CLEAR_BUTTON_LABEL = "variables.clear.label";
    public static final String VAR_CLEAR_BUTTON_TOOLTIP = "variables.clear.tooltip";
    public static final String VAR_ADD_VARIABLE_LABEL = "variables.addVar.label";
    public static final String VAR_ADD_VARIABLE_TOOLTIP = "variables.addVar.tooltip";
    public static final String VAR_NAME_LABEL = "variables.name.label";
    public static final String VAR_NAME_TOOLTIP = "variables.table.name.tooltip";
    public static final String VAR_DIALOG_NAME_LABEL = "variables.dialog.name.label";
    public static final String VAR_DIALOG_VALUE_LABEL = "variables.dialog.value.label";
    public static final String VAR_VALUE_LABEL = "variables.value.label";
    public static final String VALUE_TOOLTIP = "variables.table.value.tooltip";
    public static final String VAR_BUTTON_ICON = "variables.variables.icon";
    public static final String VAR_BUTTON_LABEL = "variables.variables.label";
    public static final String VAR_BUTTON_TOOLTIP = "variables.variables.tooltip";
    public static final String VAR_TOOLTIP = "variables.tooltip";
    public final static String VAR_HELP_ID = "variables.helpID";
    public static final String VAR_NO_VARS_TEXT = "variables.novars.text";

    // DeckView Resource Names:
    public static final String CNTNT_URL_LABEL = "wmldeck.url.label";
    public static final String CNTNT_CARD_LABEL = "wmldeck.card.label";
    public static final String CNTNT_VIEW_LABEL = "wmldeck.view.label";
    public static final String CNTNT_REFRESH_BUTTON_LABEL = "wmldeck.refresh.label";
    public static final String CNTNT_VERSION_LABEL = "wmldeck.version.label";
    public static final String CNTNT_SOURCE_LABEL = "wmldeck.size.source.label";
    public static final String CNTNT_SOURCE_UNITS = "wmldeck.size.source.units";
    public static final String CNTNT_BINARY_LABEL = "wmldeck.size.binary.label";
    public static final String CNTNT_BINARY_UNITS = "wmldeck.size.binary.units";
    public static final String CNTNT_UNKNOWN_SIZE = "wmldeck.size.unknown";
    public static final String CNTNT_TOP_LABEL = "wmldeck.top.label";
    public static final String CNTNT_EDIT_TT = "wmldeck.edit.tooltip";
    public static final String CNTNT_EDIT_QUESTION_TEXT = "wmldeck.edit.question";
    public static final String CNTNT_EDIT_QUESTION_TITLE = "wmldeck.edit.title";
    public static final String CNTNT_EDIT_FAIL_TITLE = "wmldeck.edit.fail.title";
    public static final String CNTNT_EDIT_FAIL_TEXT = "wmldeck.edit.fail.text";
    public static final String CNTNT_DISCLAIMER = "wmldeck.decoder.disclaimer";

    public static final String CNTNT_NO_DEVICE_TEXT = "wmldeck.nodevice.text";
    public static final String CNTNT_NO_DECK_TEXT = "wmldeck.nodeck.text";
    public static final String CNTNT_NO_UPDATES_TEXT = "wmldeck.noupdates.text";

    public static final String CNTNT_CONTROL_PANEL_HELP_ID = "wmldeck.helpID";
    public static final String CNTNT_STATS_HELP_ID = "wmldeck.stats.helpID";

    public static final String CNTNT_SOURCE_VIEW = "wmldeck.view.source_view";
    public static final String CNTNT_DECODED_VIEW = "wmldeck.view.decoded_view";
    public static final String CNTNT_BYTECODE_VIEW = "wmldeck.view.bytecode_view";
    public static final String CNTNT_TREE_VIEW = "wmldeck.view.tree_view";

    public static final String CNTNT_SAVE_PREFIX = "wmldeck.save.prefix";
    
    public static final String DECKSUMM_VAR_TEXT = "decksummaryview.variables.text";
    public static final String DECKSUMM_NODECK_TEXT = "decksummaryview.nodeck.text";
    public static final String DECKSUMM_STATS_BYTES = "decksummaryview.stats.bytes";
}
