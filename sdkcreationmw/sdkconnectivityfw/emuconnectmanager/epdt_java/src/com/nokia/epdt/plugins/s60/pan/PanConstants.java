/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

/**
 * PanConstants
 *
 * @version 1.0
 */
public class PanConstants extends Object {
    protected static final String VALUE_TYPE_PAN_MESSAGE = "VALUE";
    protected static final String OK_TYPE_PAN_MESSAGE = "OK";
    protected static final String ERROR_TYPE_PAN_MESSAGE = "ERROR";
    
    // Resource bundle
    protected static final String PAN_PLUGIN_RESOURCE_BUNDLE_BASE_NAME = "com.nokia.epdt.plugins.s60.pan.pan";
    // Resource keys
    protected static final String PLUGIN_UID = "plugin.uid";
    protected static final String PLUGIN_NAME = "plugin.name";
    protected static final String TARGET_UID = "target.uid";
    protected static final String TITLE = "title";
    protected static final String MESSAGE = "message";
    protected static final String BLUETOOTH_BORDER_TITLE = "bluetooth.border.title";
    protected static final String BLUETOOTH_DISABLE_LABEL = "bluetooth.disable.label";
    protected static final String BLUETOOTH_ENABLE_LABEL = "bluetooth.enable.label";
    protected static final String BLUETOOTH_COM_PORT_LABEL = "bluetooth.com.port.label";
    protected static final String BLUETOOTH_COM_HCI_LABEL = "bluetooth.hci.label";
    
    /**
     * Creates a new instance of PanConstants
     */
    private PanConstants() {
        super();
    }
    
}
