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




package com.nokia.epdt.core.plugin;

import java.lang.Integer;
import java.awt.Image;
import javax.swing.JPanel;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.action.ActionInterface;

/**
 * Plugin
 *
 * The common interface for all plug-in modules.
 *
 * The plug-in module (manager component) object created from that class
 * either implements this interface (and all the methods it contains) or
 * extends the abstract AbstractPlugin class (overriding all the abstract
 * methods it contains and also the methods of interest).
 *
 * @version 1.0
 */
public interface Plugin {
    
    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID();
    
    /**
     * Getter for property pluginType.
     * @return Value of property pluginType.
     */
    public PluginType getPluginType();
    
    /**
     * Getter for property name.
     * @return Value of property name.
     */
    public String getName();
    
    /**
     * Getter for property icon.
     * @return Value of property icon.
     */
    public Image getIcon(int type);
    
    /**
     * Getter for property jPanelView.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView();
    
    /**
     * Bind the plugin's with the epdt core.
     * @param c instance of CoreController that is assosiated with this plugin
     */
    public void bindCore(CoreController c);

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m);
    
    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a);
    
}
