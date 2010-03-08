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
import java.util.ResourceBundle;

/**
 * EmulatorResourceBundle
 *
 * @version 1.0
 */
public class EmulatorResourceBundle extends Object {
    private static EmulatorResourceBundle emulatorResourceBundle = null;
    private static ResourceBundle resourceBundle = null;
    
    /**
     * Creates a new instance of EmulatorResourceBundle
     */
    private EmulatorResourceBundle() {
        super();
    }
    
    /**
     * Returns a new EmulatorResourceBundle instance and creates a new one if necessary.
     */
    public static EmulatorResourceBundle getInstance() {
        if ( emulatorResourceBundle == null ) {
            emulatorResourceBundle = new EmulatorResourceBundle();
            resourceBundle = ResourceBundle.getBundle(EmulatorConstants.EMULATOR_PLUGIN_RESOURCE_BUNDLE_BASE_NAME);
        }
        return emulatorResourceBundle;
    }
    
    /**
     * Get an object from a ResourceBundle
     * Convenience method to save casting.
     * @param key see class description.
     * @return a value of an object from a ResourceBundle
     * @throws NullPointerException if key is null.
     */
    public String getValue(String key) {
        return resourceBundle.getString(key);
    }
    
}
