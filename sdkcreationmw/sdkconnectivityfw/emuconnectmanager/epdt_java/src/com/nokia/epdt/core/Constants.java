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

package com.nokia.epdt.core;

/**
 * Constants
 */
public interface Constants {
	// Tool configuration
	String CONFIG_HEADER = "Basic configuration properties file";
	String CONFIG_RESOURCE_BUNDLE = "config/config";
	String CONFIG_PROPERTIES = "config/config.properties";

	// Config keys
	String EPDT_CORE_UID = "epdt.core.uid";
	String EPDT_RESOURCE_BUNDLE = "epdt.resource.bundle";
	String EPDT_LANGUAGE = "epdt.language";
	String EPDT_COUNTRY = "epdt.country";
	String EPDT_VARIANT = "epdt.variant";
	String EPDT_LOOK_AND_FEEL = "epdt.look.and.feel";
	String EPDT_JAVA_VERSION_START = "epdt.java.version.start";

	// ** Resource keys ** //
	// Log configuration file
	String LOGGING_CONFIGURATION = "log.configuration.file";

	// Version dialog
	String VERSION_JDIALOG_TITLE = "version.jdialog.title";
	String MESSAGE_TAB_TITLE = "message.tab.title";
	String DETAILS_TAB_TITLE = "details.tab.title";
	// Message panel
	String MESSAGE_JPANEL_SENTENCE_0 = "message.jpanel.sentence_0";
	String MESSAGE_JPANEL_SENTENCE_1 = "message.jpanel.sentence_1";
	// Details panel
	String DETAILS_JPANEL_JAVA_VERSION = "details.jpanel.java.version";
	String DETAILS_JPANEL_JAVA_RUNTIME_VERSION = "details.jpanel.java.runtime.version";
	String DETAILS_JPANEL_JAVA_VM_VERSION = "details.jpanel.java.vm.version";

	// Preferences JFrame
	String PREFERENCES_JFRAME_TITLE = "preferences.jframe.title";
	String PREFERENCES_JFRAME_ICON_IMAGE = "preferences.jframe.icon.image";
	String PREFERENCES_OK_JBUTTON_TEXT = "preferences.ok.jbutton.text";
	String PREFERENCES_CANCEL_JBUTTON_TEXT = "preferences.cancel.jbutton.text";
	String PREFERENCES_APPLY_JBUTTON_TEXT = "preferences.apply.jbutton.text";
	// Diagnostics JFrame
	String DIAGNOSTICS_JFRAME_TITLE = "diagnostics.jframe.title";
	String DIAGNOSTICS_JFRAME_ICON_IMAGE = "diagnostics.jframe.icon.image";
	String DIAGNOSTICS_OK_JBUTTON_TEXT = "diagnostics.ok.jbutton.text";
	String DIAGNOSTICS_CANCEL_JBUTTON_TEXT = "diagnostics.cancel.jbutton.text";
	String DIAGNOSTICS_APPLY_JBUTTON_TEXT = "diagnostics.apply.jbutton.text";
	// Utilities JFrame
	String UTILITIES_JFRAME_TITLE = "utilities.jframe.title";
	String UTILITIES_JFRAME_ICON_IMAGE = "utilities.jframe.icon.image";
	String UTILITIES_OK_JBUTTON_TEXT = "utilities.ok.jbutton.text";
	String UTILITIES_CANCEL_JBUTTON_TEXT = "utilities.cancel.jbutton.text";
	String UTILITIES_APPLY_JBUTTON_TEXT = "utilities.apply.jbutton.text";

	// emulator JPanel
	String EMULATOR_TITLE = "emulator.card.title";
	String EMULATOR_LIST_TITLE = "emulator.port.list.title";
	String EMULATOR_ADD_BUTTON_TEXT = "emulator.add.button.text";
	String EMULATOR_CONNECT_BUTTON_TEXT = "emulator.connect.button.text";
	String EMULATOR_PREFERENCES_BUTTON_TEXT = "emulator.preferences.button.text";
	String EMULATOR_DIAGNOSTICS_BUTTON_TEXT = "emulator.diagnostics.button.text";
	String EMULATOR_UTILITIES_BUTTON_TEXT = "emulator.utilities.button.text";
	// phone JPanel
	String PHONE_TITLE = "phone.card.title";
	String PHONE_LIST_TITLE = "phone.connection.list.title";
	String PHONE_CONNECT_BUTTON_TEXT = "phone.connect.button.text";
	String PHONE_DIAGNOSTICS_BUTTON_TEXT = "phone.diagnostics.button.text";

	// Plugin
	String PLUGIN_DIRECTORY = "plugin.directory";
	String PLUGIN_FILE_NAME_EXTENSION = "plugin.file.name.extension";
	String PLUGIN_MODULE_KEY = "plugin.module.key";
	String PLUGIN_PROPERTIES = "plugin.properties";
	String PLUGIN_SMALL_CLASS = "plugin.small.class";
	String PLUGIN_TITLE_LONG = "plugin.title.long";
	String PLUGIN_TITLE_SHORT = "plugin.title.short";
	String PLUGIN_OPEN_DEFAULT = "plugin.open.default";
	String PLUGIN_WIDTH_DEFAULT = "plugin.width.default";
	String PLUGIN_HEIGHT_DEFAULT = "plugin.height.default";

	// Plugin set
	String MIDP_PLUGIN_SET = "midp.plugin.set";
	String CPP_PLUGIN_SET = "cpp.plugin.set";

	// preferences
	String CPPDEBUG_PLUGIN = "cppdebug";
	String ETHERNET_PLUGIN = "ethernet";
	String GENERAL_PLUGIN = "general";
	String PAN_PLUGIN = "pan";
	String PLATSEC_PLUGIN = "platsec";
	String ROUTE_PLUGIN = "route";
	String SECURITYCONF_PLUGIN = "securityconf";

	// diagnostics
	String OUTPUT_PLUGIN = "output";
	String TASKMAN_PLUGIN = "taskman";

	// Messaging
	int MESSAGE_LENGTH_RADIX = 16;
	int MESSAGE_LENGTH_HEX_NUMERAL_LENGTH = 10;
	int MESSAGE_LENGTH_LEADING_CHARACTER_LENGTH = 2;
	int MESSAGE_LENGTH_HEX_DIGIT_LENGTH = 8;
	int MESSAGE_UID_RADIX = 16;
	int MESSAGE_UID_HEX_NUMERAL_LENGTH = 10;
	int MESSAGE_UID_LEADING_CHARACTER_LENGTH = 2;
	int MESSAGE_UID_HEX_DIGIT_LENGTH = 8;
	int MESSAGE_INPUT_QUEUE_CAPACITY = 1000;
	int MESSAGE_OUTPUT_QUEUE_CAPACITY = 1000;

	// Action
	int ACTION_EVENT_QUEUE_CAPACITY = 50;

	// Command
	int COMMAND_QUEUE_CAPACITY = 50;

	// Proxy request keys and values
	String COMMAND_KEY = "command";
	String EXIT_VALUE = "exit";
	String OPEN_VALUE = "open";
	String SHOW_PREFERENCES_VALUE = "show_preferences";
	String HIDE_PREFERENCES_VALUE = "hide_preferences";
	String SHOW_DIAGNOSTICS_VALUE = "show_diagnostics";
	String HIDE_DIAGNOSTICS_VALUE = "hide_diagnostics";
	String SHOW_UTILITIES_VALUE = "show_utilities";
	String HIDE_UTILITIES_VALUE = "hide_utilities";
	String HOST_KEY = "host";
	String PORT_KEY = "port";
	String POS_TOP_KEY = "top";
	String POS_LEFT_KEY = "left";

	// Configuration keys and values
	String EPOC32_DIR = "epoc32Dir";
	String PLUGIN_DIR = "pluginDir";
	String LOG_LEVEL = "logLevel";
	String EPOC32 = "epdt.dir.epoc32";
}
