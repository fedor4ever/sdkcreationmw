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

import java.awt.Image;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.action.ActionEventQueue;
import org.apache.log4j.Logger;

/**
 * Provides an epdt-plugin (formerly ecmt) that makes it possible to graphically
 * define geographical routes for use in the location simulator of the emulator.
 *
 * Provides "bootstrapping" and message handling code for the plugin.
 */
public class RoutePlugin extends CustomPlugin {
    private static Logger log = Logger.getLogger(RoutePlugin.class);
    private static final Integer UID = Integer.valueOf(RouteResourceBundle.getInstance().getValue(RouteConstants.PLUGIN_UID), 16);
    private MessageQueue messageOutputQueue = null;
    private ActionEventQueue actionEventQueue = null;
    private JPanel mainPanel = new RoutePanel(this);
    /**
     * A ActionListener object is used to receive synchronously delivered actions.
     */
    private class RoutePluginActionListener implements ActionListener {
        public void actionPerformed(Action e) {
            if ( e.getName() == Action.UTILITIES_OK) {
                messageOutputQueue.put(PositionData.getInstance().createMessage(RoutePlugin.UID.intValue()));
                actionEventQueue.put(new Action(Action.UTILITIES_REMOVE_PLUGIN_MODEL_CHANGED, RoutePlugin.UID, Action.CORE, new int[0]));
                actionEventQueue.put(new Action(Action.UTILITIES_HIDE_PLUGIN, RoutePlugin.UID, Action.CORE, new int[0]));
            } else if ( e.getName() == Action.UTILITIES_APPLY) {
                messageOutputQueue.put(PositionData.getInstance().createMessage(RoutePlugin.UID.intValue()));
                actionEventQueue.put(new Action(Action.UTILITIES_REMOVE_PLUGIN_MODEL_CHANGED, RoutePlugin.UID, Action.CORE, new int[0]));
            }
        }
    }
    /**
     * A MessageListener object is used to receive asynchronously delivered messages.
     */
    private class RoutePluginMessageListener implements MessageListener {
        public void messageReceived(Message message) {
            log.debug("A message was received");
            log.debug(message);
        }
    }

    /**
     * Creates a new <code>RoutePlugin</code> instance. Creates the
     * JPanel's, the widgets and the action listeners.
     *
     */
    public RoutePlugin() {
        super();
    }

    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID() {
        return RoutePlugin.UID;
    }

    /**
     * Getter for property pluginType.
     * @return Value of property pluginType.
     */
    public PluginType getPluginType() {
        return PluginType.UTILITIES;
    }

    /**
     * Getter for property name.
     * @return Value of property name.
     */
    public String getName() {
        return RouteResourceBundle.getInstance().getValue(RouteConstants.PLUGIN_NAME);
    }

    /**
     * Getter for property icon.
     * @return Value of property icon.
     */
    public Image getIcon(int type) {
        return (new ImageIcon()).getImage();
    }

    /**
     * Getter for property jPanelView.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView() {
        return this.mainPanel;
    }

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m) {
        m.setMessageListener(new RoutePlugin.RoutePluginMessageListener(), RoutePlugin.UID);
        this.messageOutputQueue = m.getMessageOutputQueue();
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new RoutePlugin.RoutePluginActionListener(), RoutePlugin.UID, ActionListener.PLUGIN);
        this.actionEventQueue = a.getActionEventQueue();
    }

    /**
     * Notifies the core that a plugin model is changed
     */
    public void pluginModelChangedAction() {
        actionEventQueue.put(new Action(Action.UTILITIES_PLUGIN_MODEL_CHANGED,
        RoutePlugin.UID, Action.CORE, new int[0]));
    }
}
