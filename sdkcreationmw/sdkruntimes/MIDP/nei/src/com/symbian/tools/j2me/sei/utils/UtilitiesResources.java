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

import java.util.*;

/**
 * Place holder for all utils.exe strings
 *
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class UtilitiesResources {

  //
  // Constants
  //

  /**
   * Properties file name
   */
  private static final String PROPERTIES_FILE = "com.symbian.tools.j2me.sei.utils.utilitiesresources";


  //
  // Shared strings
  //


  public static String CLOSE = "CLOSE";
  public static String REVERT = "REVERT";
  public static String APPLY = "APPLY";
  public static String CANCEL = "CANCEL";
  public static String REMOVE = "REMOVE";
  public static String SELECT_ = "SELECT_";
  public static String ADD_ = "ADD_";
  public static String OK = "OK";

  //
  // ButtonsPanel resources
  //

  public static String RESET_DEFAULTS = "RESET_DEFAULTS";

  //
  // Utils.exe dialog resource
  //

  public static String UTILS_DIALOG_TITLE = "UTILS_DIALOG_TITLE";

  //
  // KDPPReferencesView resources
  //

  public static String KDP_TAB_TITLE = "KDP_TAB_TITLE";
  public static String UNLIMITED_TIMOUT = "UNLIMITED_TIMOUT";
  public static String ENABLE_ATTACHING_TO_VM = "ENABLE_ATTACHING_TO_VM";
  public static String DEFER_VM_EXECUTION = "DEFER_VM_EXECUTION";
  public static String DEFER_VM_EXECUTION_TIP = "DEFER_VM_EXECUTION_TIP";
  public static String DEBUG_CONNECTION_TIP = "DEBUG_CONNECTION_TIP";
  public static String MAX_DEBUG_TIMEOUT_TIP = "MAX_DEBUG_TIMEOUT_TIP";
  public static String ENABLE_ATTACHING = "ENABLE_ATTACHING";
  public static String ATTACH_OPTIONS = "ATTACH_OPTIONS";
  public static String KDP_ATTACH_PORT = "KDP_ATTACH_PORT";
  public static String VM_LAUNCH_OPTIONS = "VM_LAUNCH_OPTIONS";
  public static String SLAVE_PORT = "SLAVE_PORT";
  public static String SLAVE_HOST = "SLAVE_HOST";
  public static String AGENT_TAB = "AGENT_TAB";
  public static String EKA2_EMULATOR_DISCOVERY = "EKA2_EMULATOR_DISCOVERY";
  public static String EKA2_EMULATOR_DISCOVERY_TIP = "EKA2_EMULATOR_DISCOVERY_TIP";
  public static String AGENT_BUTTON = "AGENT_BUTTON";
  public static String EMULATOR = "EMULATOR";
  public static String REMINDER_MESSAGE  = "REMINDER_MESSAGE";
  public static String LAUNCH_AGENT_REMINDER_MESSAGE  = "LAUNCH_AGENT_REMINDER_MESSAGE";
  public static String REMINDER_TITLE = "REMINDER_TITLE";

  //
  // PermissiondDialog resources
  //

  public static String PERMISSIONS_DIALOG_TITLE = "PERMISSIONS_DIALOG_TITLE";


  //
  // SecurityPreferencesView resources
  //

  public static String SECURITY_TAB_TITLE = "SECURITY_TAB_TITLE";
  public static String ALWAYS_SIGN = "ALWAYS_SIGN";
  public static String TRUST_MODE = "TRUST_MODE";
  public static String TRUSTED = "TRUSTED";
  public static String SIGNING = "SIGNING";
  public static String KEYSTORE_FILE = "KEYSTORE_FILE";
  public static String KEYSTORE_PASSWORD = "KEYSTORE_PASSWORD";
  public static String KEY_ALIAS = "KEY_ALIAS";
  public static String KEY_PASSWORD = "KEY_PASSWORD";
  public static String MIDLET_PERMISSIONS = "MIDLET_PERMISSIONS";
  public static String CAUGHT_EXCEPTION = "CAUGHT_EXCEPTION";
  public static String FAILED_LOAD_ALIASES = "FAILED_LOAD_ALIASES";
  public static String WILDCARD = "WILDCARD";
  public static String CHALLENGE = "CHALLENGE";


  //
  // SuitePreferencesView resources
  //

  public static String SUITE_TAB = "SUITE_TAB";
  public static String DEFAULT_JAD_PROPS = "DEFAULT_JAD_PROPS";
  public static String JARRED_TYPES = "JARRED_TYPES";
  public static String ENTER_NEW_TYPE = "ENTER_NEW_TYPE";

  //
  // UIPreferencesView resources
  //

  public static String UI_TAB = "UI_TAB";
  public static String SHOW_PROGRESS = "SHOW_PROGRESS";
  public static String SHOW_IN_TASK_BAR = "SHOW_IN_TASK_BAR";
  public static String SHOW_PROGRESS_TIP = "SHOW_PROGRESS";
  public static String PLAF = "PLAF";
  public static String SELECT_PLAF_TIP = "SELECT_PLAF_TIP";


  //
  // Mediator resources
  //

  public static String CONFIRM_LOOSING_SETTINGS = "CONFIRM_LOOSING_SETTINGS";
  public static String FAILED_TO_SAVE = "FAILED_TO_SAVE";
  public static String CONFIRMATION = "CONFIRMATION";



  static
  {
    ResourceBundle res = ResourceBundle.getBundle(PROPERTIES_FILE);

    //shared
    CLOSE = res.getString(CLOSE);
    REVERT = res.getString(REVERT);
    APPLY = res.getString(APPLY);
    CANCEL = res.getString(CANCEL);
    REMOVE = res.getString(REMOVE);
    SELECT_ = res.getString(SELECT_);
    ADD_ = res.getString(ADD_);
    OK = res.getString(OK);

    //Utils.exe dialog resources
    UTILS_DIALOG_TITLE = res.getString(UTILS_DIALOG_TITLE);

    // ButtonsPanel resources
    RESET_DEFAULTS = res.getString(RESET_DEFAULTS);

    // KDPPReferencesView resources
    KDP_TAB_TITLE = res.getString(KDP_TAB_TITLE);
    UNLIMITED_TIMOUT = res.getString(UNLIMITED_TIMOUT);
    ENABLE_ATTACHING_TO_VM = res.getString(ENABLE_ATTACHING_TO_VM);
    DEFER_VM_EXECUTION = res.getString(DEFER_VM_EXECUTION);
    DEFER_VM_EXECUTION_TIP = res.getString(DEFER_VM_EXECUTION_TIP);
    DEBUG_CONNECTION_TIP = res.getString(DEBUG_CONNECTION_TIP);
    MAX_DEBUG_TIMEOUT_TIP = res.getString(MAX_DEBUG_TIMEOUT_TIP);
    ENABLE_ATTACHING = res.getString(ENABLE_ATTACHING);
    ATTACH_OPTIONS = res.getString(ATTACH_OPTIONS);
    KDP_ATTACH_PORT = res.getString(KDP_ATTACH_PORT);
    VM_LAUNCH_OPTIONS = res.getString(VM_LAUNCH_OPTIONS);
    SLAVE_PORT = res.getString(SLAVE_PORT);
    SLAVE_HOST = res.getString(SLAVE_HOST);
    AGENT_TAB = res.getString(AGENT_TAB);
    EKA2_EMULATOR_DISCOVERY = res.getString(EKA2_EMULATOR_DISCOVERY);
    EKA2_EMULATOR_DISCOVERY_TIP = res.getString(EKA2_EMULATOR_DISCOVERY_TIP);
    AGENT_BUTTON = res.getString(AGENT_BUTTON);
    EMULATOR = res.getString(EMULATOR);
    REMINDER_MESSAGE = res.getString(REMINDER_MESSAGE);
    LAUNCH_AGENT_REMINDER_MESSAGE = res.getString(LAUNCH_AGENT_REMINDER_MESSAGE);
    REMINDER_TITLE = res.getString(REMINDER_TITLE);

    // PermissiondDialog resources
    PERMISSIONS_DIALOG_TITLE = res.getString(PERMISSIONS_DIALOG_TITLE);

    // SecurityPreferencesView resources
    SECURITY_TAB_TITLE = res.getString(SECURITY_TAB_TITLE);
    ALWAYS_SIGN = res.getString(ALWAYS_SIGN);
    TRUST_MODE = res.getString(TRUST_MODE);
    TRUSTED = res.getString(TRUSTED);
    SIGNING = res.getString(SIGNING);
    KEYSTORE_FILE = res.getString(KEYSTORE_FILE);
    KEYSTORE_PASSWORD = res.getString(KEYSTORE_PASSWORD);
    KEY_ALIAS = res.getString(KEY_ALIAS);
    KEY_PASSWORD = res.getString(KEY_PASSWORD);
    MIDLET_PERMISSIONS = res.getString(MIDLET_PERMISSIONS);
    CAUGHT_EXCEPTION = res.getString(CAUGHT_EXCEPTION);
    FAILED_LOAD_ALIASES = res.getString(FAILED_LOAD_ALIASES);
    WILDCARD = res.getString(WILDCARD);
    CHALLENGE = res.getString(CHALLENGE);


    // SuitePreferencesView resources
    SUITE_TAB = res.getString(SUITE_TAB);
    DEFAULT_JAD_PROPS = res.getString(DEFAULT_JAD_PROPS);
    JARRED_TYPES = res.getString(JARRED_TYPES);
    ENTER_NEW_TYPE = res.getString(ENTER_NEW_TYPE);

    // UIPreferencesView resources
    UI_TAB = res.getString(UI_TAB);
    SHOW_PROGRESS = res.getString(SHOW_PROGRESS);
    SHOW_IN_TASK_BAR = res.getString(SHOW_IN_TASK_BAR);
    SHOW_PROGRESS_TIP = res.getString(SHOW_PROGRESS_TIP);
    PLAF = res.getString(PLAF);
    SELECT_PLAF_TIP = res.getString(SELECT_PLAF_TIP);

    // Mediator resources
    CONFIRM_LOOSING_SETTINGS = res.getString(CONFIRM_LOOSING_SETTINGS);
    FAILED_TO_SAVE = res.getString(FAILED_TO_SAVE);
    CONFIRMATION = res.getString(CONFIRMATION);

  }

}
