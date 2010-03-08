

// ========================================================================
//  Name        : Resource.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.wtk.app;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  Resources set and in use by the app package. Stored and
 * retrieved using AppEnvironment.getAppEnvironment.GetProperties(). Override
 * for new language or look by changing the properties file used to initialize
 * the AppEnvironment PropertySet;
 * Company:      Nokia
 * @version 1.0
 */

public interface Resource {
    public static final String appTitle = "application.title";
    public static final String appIcon = "application.icon";
    public static final String appDefaultBounds =
        "application.window.default-bounds";
    public static final String appCorporateLogoImage =
        "application.corporate-logo.image";
    public static final String appCopyrightNotice =
        "application.copyright-notice";
    public static final String appPreferenceParticipants =
        "application.preferenceParticipants";
    public static final String appVersion = "application.version.name";
    public static final String appVersionExpiryDate =
        "application.version.expiry.date";
    public static final String appBuildDate = "application.build.date";

    public static final String undoIcon = "app.general.undo.icon";
    public static final String undoText = "app.general.undo.text";
    public static final String undoTooltip = "app.general.undo.tooltip";
    public static final String undoKeyStroke = "app.general.undo.keyStroke";
    public static final String redoIcon = "app.general.redo.icon";
    public static final String redoText = "app.general.redo.text";
    public static final String redoTooltip = "app.general.redo.tooltip";
    public static final String redoKeyStroke = "app.general.redo.keyStroke";
    public static final String cutIcon = "app.general.cut.icon";
    public static final String cutText = "app.general.cut.icon";
    public static final String cutTooltip = "app.general.cut.tooltip";
    public static final String cutKeyStroke = "app.general.cut.keyStroke";
    public static final String copyIcon = "app.general.copy.icon";
    public static final String copyText = "app.general.copy.text";
    public static final String copyTooltip = "app.general.copy.tooltip";
    public static final String copykeyStroke = "app.general.copy.keyStroke";
    public static final String pasteIcon = "app.general.paste.icon";
    public static final String pasteText = "app.general.paste.text";
    public static final String pasteTooltip = "app.general.paste.tooltip";
    public static final String pasteKeyStroke = "app.general.paste.keyStroke";
    public static final String findIcon = "app.general.find.icon";
    public static final String findText = "app.general.find.text";
    public static final String findTooltip = "app.general.find.tooltip";
    public static final String findKeyStroke = "app.general.find.keyStroke";
    public static final String findNextIcon = "app.general.findNext.icon";
    public static final String findNextText = "app.general.findNext.text";
    public static final String findNextTooltip = "app.general.findNext.tooltip";
    public static final String findNextKeyStroke =
        "app.general.findNext.keyStroke";
    public static final String newIcon = "app.general.new.icon";
    public static final String newText = "app.general.new.text";
    public static final String newTooltip = "app.general.new.tooltip";
    public static final String newKeyStroke = "app.general.new.keyStroke";
    public static final String printIcon = "app.general.print.icon";
    public static final String printText = "app.general.print.text";
    public static final String printTooltip = "app.general.print.tooltip";
    public static final String printKeyStroke = "app.general.print.keyStroke";
    public static final String printPreviewIcon =
        "app.general.printPreview.icon";
    public static final String printPreviewText =
        "app.general.printPreview.text";
    public static final String printPreviewTooltip =
        "app.general.printPreview.tooltip";
    public static final String printPreviewKeyStroke =
        "app.general.printPreview.keyStroke";
    public static final String saveIcon = "app.general.save.icon";
    public static final String saveText = "app.general.save.text";
    public static final String saveTooltip = "app.general.save.tooltip";
    public static final String saveKeyStroke = "app.general.save.keyStroke";
    public static final String editIcon = "app.general.edit.icon";
    public static final String editText = "app.general.edit.text";
    public static final String editTooltip = "app.general.edit.tooltip";
    public static final String editKeyStroke = "app.general.edit.keyStroke";
    public static final String saveAsIcon = "app.general.saveAs.icon";
    public static final String saveAsText = "app.general.saveAs.text";
    public static final String saveAsTooltip = "app.general.saveAs.tooltip";
    public static final String saveAsKeyStroke = "app.general.saveAs.keyStroke";
    public static final String openIcon = "app.general.open.icon";
    public static final String openText = "app.general.open.text";
    public static final String openTooltip = "app.general.open.tooltip";
    public static final String openKeyStroke = "app.general.open.keyStroke";

    public static final String helpIcon = "app.general.help.icon";

    public static final String aboutDialogTitle = "app.general.about.title";
    public static final String aboutDialogImage = "app.general.about.image";
    public static final String aboutDialogBodyText =
        "app.general.about.bodyText";
    public static final String ABOUT_LEFT_TOP_ICON = "app.general.about.left-top-icon";
    public static final String ABOUT_LEFT_BOTTOM_ICON = "app.general.about.left-bottom-icon";

    public static final String splashMessage = "application.splash.message";
    public static final String splashImage = "application.splash.image";
    public static final String splashImageSmall =
        "application.splash-tiny.image";

    public static final String VISUAL_GAP = "look.visual.gap";
    public final static String PLAIN_FONT_NAME = "look.font.plain.name";
    public final static String CODE_FONT_NAME = "look.font.code.name";
    public final static String PLAIN_FONT_SIZE = "look.font.plain.size";
    public final static String CODE_FONT_SIZE = "look.font.code.size";

    public static final String ARROW_UP = "app.general.arrow_up.icon";
    public static final String ARROW_DOWN = "app.general.arrow_down.icon";
    public static final String ARROW_LEFT = "app.general.arrow_left.icon";
    public static final String ARROW_RIGHT = "app.general.arrow_right.icon";

    public static final String EMPTY_ICON = "app.general.empty.icon";

    public final static String APP_DOWNLOAD_URL = "application.url.download";

    /* Dialog keys */
    public static final String DIALOG_OK = "dialog.ok";
    public static final String DIALOG_CANCEL = "dialog.cancel";
    public static final String DIALOG_APPLY = "dialog.apply";
    public static final String DIALOG_HELP = "dialog.help";
    public static final String DIALOG_OPEN = "dialog.open";
    public static final String DIALOG_CLOSE = "dialog.close";
    public static final String DIALOG_YES = "dialog.yes";
    public static final String DIALOG_NO = "dialog.no";

}
