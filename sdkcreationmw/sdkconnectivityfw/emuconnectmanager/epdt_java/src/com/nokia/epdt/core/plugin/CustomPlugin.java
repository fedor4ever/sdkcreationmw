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

import java.awt.Image;

import javax.swing.ImageIcon;
import javax.swing.JPanel;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageQueue;

/**
 * 
 * CustomPlugin implements default behaviour of Plugin and that way simplifies actual Plugin writing.  
 */
public class CustomPlugin implements Plugin, MessageListener, ActionListener {
    private Integer UID = Integer.valueOf("00000000", 16);
    private JPanel panel = new JPanel();
    private String name = "Custom";
    private PluginType type = PluginType.PREFERENCES;
    private Image image = (new ImageIcon()).getImage();
    private MessageQueue messageOutputQueue = null;
    private ActionEventQueue actionEventQueue = null;
    private boolean actionListener = false;
    private boolean messageListener = false;
    
    /**
     * Simple CustomPlugin constructor with default initiliaziation
     */
    public CustomPlugin() {
    	super();
    }

    /**
     * CustomPlugin constructor
     * 
     * @param uid plugin UID
     * @param name plugin name
     * @param type plugin type
     */
    public CustomPlugin(Integer uid, String name, PluginType type) {
    	super();
		this.UID = uid;
		this.name = name;
		this.type = type;
		this.actionListener = false;
		this.messageListener = false;
    }

    /**
     * CustomPlugin constructor
     * 
     * @param uid plugin UID
     * @param panel plugin panel
     * @param name plugin name
     * @param type plugin type
     * @param image plugin icon 
     * @param actionListener setting default ActionListener property true/false
     * @param messageListener setting default MessageListener property true/false
     */
    public CustomPlugin(Integer uid, JPanel panel, String name, PluginType type, Image image, boolean actionListener, boolean messageListener) {
		super();
		this.UID = uid;
		this.panel = panel;
		this.name = name;
		this.type = type;
		this.image = image;
		this.actionListener = actionListener;
		this.messageListener = messageListener;
    }
	
    /**
     * Getter for UID
     */
    public Integer getPluginUID() {
        return UID;
    }
    
    /**
     * Setter for panel
     */
    public void setJPanelView(JPanel panel) {
    	this.panel = panel;
    }

    /**
     * Getter for panel
     */
    public JPanel getJPanelView() {
        return panel;
    }

    /**
     * Getter for plugin type
     */
    public PluginType getPluginType() {
        return type;
    }

    /**
     * Getter for icon
     */
    public Image getIcon(int type) {
        return image;
    }

    /**
     * Getter for name
     */
    public String getName() {
        return name;
    }
    
    /**
     * Setting default MessageListener behaviour
     */
    public void bindMessageInterface(MessageInterface m) {
    	if(messageListener) {
    		m.setMessageListener(this, UID);
    		this.messageOutputQueue = m.getMessageOutputQueue();
    	}
    }	

    /**
     * Setting default ActionListener behaviour
     */
    public void bindActionInterface(ActionInterface a) {
    	if(actionListener) {
    		a.setActionListener(this, UID, ActionListener.PLUGIN);
    		this.actionEventQueue = a.getActionEventQueue();
    	}
    }
    
    /**
     * Bind the plugin's with the epdt core.
     * @param c instance of CoreController that is assosiated with this plugin
     */
    public void bindCore(CoreController c) {
    	
    }

    /**
     * Implementing ActionListener
     */
    public void actionPerformed(Action action) {
    	
    }
    
    /**
     * Implementing MessageListener
     */
    public void messageReceived(Message event) {
    	
    }
}
