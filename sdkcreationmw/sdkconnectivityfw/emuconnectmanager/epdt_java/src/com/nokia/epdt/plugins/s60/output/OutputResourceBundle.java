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



package com.nokia.epdt.plugins.s60.output;

/* java.util */
import java.util.Locale;
import java.util.ResourceBundle;

/* org.apache.log4j */
import org.apache.log4j.Logger;

/**
 * OutputResourceBundle
 */
class OutputResourceBundle {
    private static final Logger log = Logger.getLogger(OutputResourceBundle.class);
    private static OutputResourceBundle outputResourceBundle = null;
    private ResourceBundle resourceBundle;

    /**
     * Creates a new instance of OutputResourceBundle
     */
    private OutputResourceBundle() {
        resourceBundle = ResourceBundle.getBundle(
            getClass().getPackage().getName() + ".output",
            Locale.getDefault(),
            getClass().getClassLoader());
    }

    /**
     * Returns a new OutputResourceBundle instance and creates a new one if necessary.
     */
    public static OutputResourceBundle getInstance() {
        if (outputResourceBundle == null ) {
            outputResourceBundle = new OutputResourceBundle();
        }
        return outputResourceBundle;
    }

    /**
     * Get an object from a ResourceBundle
     * Convenience method to save casting.
     * @param key see class description.
     * @return a value of an object from a ResourceBundle
     */
    public String getValue(String key) {
        String result = "";
        try {
            result = resourceBundle.getString(key);
        } catch (java.util.MissingResourceException mre) {
            log.warn(mre);
        } catch (NullPointerException npe) {
            log.warn(npe);
        }
        return result;
    }
}
