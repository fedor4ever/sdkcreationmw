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




package com.nokia.epdt.plugins.s60.taskman;

import java.util.ResourceBundle;
import java.util.MissingResourceException;
import org.apache.log4j.Logger;

/**
 * TaskManagerResourceBundle
 *
 * @version 1.0
 */
class TaskManagerResourceBundle {
    private static Logger log = Logger.getLogger(TaskManagerResourceBundle.class);
    private static TaskManagerResourceBundle taskManagerResourceBundle = null;
    private ResourceBundle resourceBundle;

    /**
     * Creates a new instance of TaskManagerResourceBundle
     */
    private TaskManagerResourceBundle() {
        resourceBundle = ResourceBundle.getBundle(TaskManagerConstants.TASK_MANAGER_PLUGIN_RESOURCE_BUNDLE_BASE_NAME);
    }

    /**
     * Returns a new TaskManagerResourceBundle instance and creates a new one if necessary.
     */
    static TaskManagerResourceBundle getInstance() {
        if ( taskManagerResourceBundle == null ) {
            taskManagerResourceBundle = new TaskManagerResourceBundle();
        }
        return taskManagerResourceBundle;
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
        } catch (MissingResourceException mre) {
            log.info(key);
            log.warn(mre);
        } catch (NullPointerException npe) {
            log.info(key);
            log.warn(npe);
        }
        return result;
    }
}
