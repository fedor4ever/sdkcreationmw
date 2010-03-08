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



package com.nokia.epdt.plugins.s60.route;

import java.lang.Object;
import java.util.ResourceBundle;
import java.lang.NullPointerException;
import org.apache.log4j.Logger;

/**
 * RouteResourceBundle
 *
 * @version 1.0
 */
public class RouteResourceBundle extends Object {
    private static Logger log = Logger.getLogger(RouteResourceBundle.class);
    private static RouteResourceBundle routeResourceBundle = null;
    private static ResourceBundle resourceBundle = null;
    
    /**
     * Creates a new instance of RouteResourceBundle
     */
    private RouteResourceBundle() {
        super();
    }
    
    /**
     * Returns a new RouteResourceBundle instance and creates a new one if necessary.
     */
    public static RouteResourceBundle getInstance() {
        if ( routeResourceBundle == null ) {
            routeResourceBundle = new RouteResourceBundle();
            resourceBundle = ResourceBundle.getBundle(RouteConstants.ROUTE_PLUGIN_RESOURCE_BUNDLE_BASE_NAME);
        }
        return routeResourceBundle;
    }
    
    /**
     * Get an object from a ResourceBundle
     * Convenience method to save casting.
     * @param key see class description.
     * @return a value of an object from a ResourceBundle
     * @throws NullPointerException if key is null.
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
