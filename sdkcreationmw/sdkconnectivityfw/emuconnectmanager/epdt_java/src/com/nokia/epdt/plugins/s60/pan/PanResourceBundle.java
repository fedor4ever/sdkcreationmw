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




package com.nokia.epdt.plugins.s60.pan;

import java.lang.Object;
import java.util.ResourceBundle;

/**
 * PanResourceBundle
 *
 * @version 1.0
 */
public class PanResourceBundle extends Object {
    private static PanResourceBundle panResourceBundle = null;
    private static ResourceBundle resourceBundle = null;
    
    /**
     * Creates a new instance of PanResourceBundle
     */
    private PanResourceBundle() {
        super();
    }
    
    /**
     * Returns a new PanResourceBundle instance and creates a new one if necessary.
     */
    public static PanResourceBundle getInstance() {
        if ( panResourceBundle == null ) {
            panResourceBundle = new PanResourceBundle();
            resourceBundle = ResourceBundle.getBundle(PanConstants.PAN_PLUGIN_RESOURCE_BUNDLE_BASE_NAME);
        }
        return panResourceBundle;
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
