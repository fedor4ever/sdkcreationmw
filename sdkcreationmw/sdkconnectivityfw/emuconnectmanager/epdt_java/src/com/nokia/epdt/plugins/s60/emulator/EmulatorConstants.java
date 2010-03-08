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




package com.nokia.epdt.plugins.s60.emulator;

import java.lang.Object;

/**
 * EthernetConstants
 *
 * @version 1.0
 */
public class EmulatorConstants extends Object {
    public static final String VALUE_TYPE_EMULATOR_MESSAGE = "VALUE";
    public static final String OK_TYPE_EMULATOR_MESSAGE = "OK";
    public static final String ERROR_TYPE_EMULATOR_MESSAGE = "ERROR";
    
    // Resource bundle
    public static final String EMULATOR_PLUGIN_RESOURCE_BUNDLE_BASE_NAME = "com.nokia.epdt.plugins.s60.emulator.emulator";
    // Resource keys
    public static final String PLUGIN_UID = "plugin.uid";
    public static final String PLUGIN_NAME = "plugin.name";
    public static final String TARGET_UID = "target.uid";
    public static final String ERROR_DIALOG_TITLE = "error.dialog.title";
    public static final String INFORMATION_DIALOG_TITLE = "information.dialog.title";
    public static final String INFORMATION_DIALOG_MESSAGE = "information.dialog.message";
    public static final String VALIDATION_BAD_LOCATION_EXCEPTION = "validation.bad.location.exception";
    public static final String JUST_IN_TIME_ENABLE_LABEL = "just.in.time.debug.enable.label";
    public static final String JUST_IN_TIME_DISABLE_LABEL = "just.in.time.debug.disable.label";
    public static final String PANIC_ENABLE_LABEL = "panic.enable.label";
    public static final String PANIC_DISABLE_LABEL = "panic.disable.label";
    public static final String DEBUG_ENABLE_LABEL = "debug.enable.label";
    public static final String DEBUG_DISABLE_LABEL = "debug.disable.label";
    
    /**
     * Creates a new instance of EmulatorConstants
     */
    private EmulatorConstants() {
        super();
    }
    
}
