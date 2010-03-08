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


package com.nokia.epdt.plugins.s60.winsock;

/* java.util */
import java.util.Properties;

/* java.awt */
import java.awt.Image;

/* javax.swing */
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.ImageIcon;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertySetAdapter;

/* org.apache.log4j */
import org.apache.log4j.Logger;

/* com.nokia.epdt.core.plugin */
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.plugin.CustomPlugin;

/* com.nokia.epdt.core.messaging */
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageQueue;

/* com.nokia.epdt.core.action */
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.connection.ConnectionStatus;
import com.nokia.epdt.plugins.s60.emulator.EmulatorConstants;
import com.nokia.epdt.plugins.s60.emulator.EmulatorResourceBundle;


/**
 * Provides Winsock configuration GUI.
 */
public class WinsockPlugin extends CustomPlugin implements WinsockConstants {
    private static final Integer PLUGIN_UID = new Integer(WINSOCK_PREFS_UID);
    private static final Logger log = Logger.getLogger(WinsockPlugin.class);
    private MessageQueue messageOutputQueue;
    private PropertySet currentSettings;
    private PropertySet backupSettings;
    private JPanel pluginPanel;
    private ActionEventQueue actionEventQueue;
    private PropertySetAdapter modelChangeListener;
    private CoreController coreController;

    /**
     * A ActionListener object is used to receive synchronously delivered actions.
     */
    private class WinsockActionListener implements ActionListener {

        private void apply(boolean ok) {
        	((WinsockPluginJPanel)pluginPanel).showRestartWarning();
        	
            backupSettings.applyValues(currentSettings);
            messageOutputQueue.put(WinsockMessageFactory.createConfigMessage(currentSettings));
            fireActionEvent(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED);
            if (ok) fireActionEvent(Action.PREFERENCES_HIDE_PLUGIN);

        }

        public void actionPerformed(Action e) {
            if ( e.getName() == Action.PREFERENCES_OK ||
                 e.getName() == Action.PREFERENCES_APPLY) {
                apply(e.getName() == Action.PREFERENCES_OK);
            } else if (e.getName() == Action.PREFERENCES_CANCEL) {
                currentSettings.removePropertySetListener(modelChangeListener);
                currentSettings.applyValues(backupSettings);
                currentSettings.addPropertySetListener(modelChangeListener);
            }
        }
    }

    /**
     * Creates a new instance of EthernetPlugin
     */
    public WinsockPlugin() {
        currentSettings =  new PropertySet();
        modelChangeListener = new ModelChangeListener();

        // Winsock specific properties
        currentSettings.addBooleanProperty(USE_WINSOCK_PROP, true);
        currentSettings.addBooleanProperty(WINSOCK_USE_PROXY_PROP, false);
        currentSettings.addStringProperty(WINSOCK_PROXY_SERVER_PROP, "");
        currentSettings.addIntProperty(WINSOCK_PROXY_PORT_PROP, 80);
        currentSettings.addBooleanProperty(WINSOCK_UI_ENABLED_PROP, false);

        // Clone the values
        backupSettings = currentSettings.getCopy();
        currentSettings.addPropertySetListener(modelChangeListener);
    }

    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID() {
        return PLUGIN_UID;
    }

    /**
     * Getter for property pluginType.
     * @return Value of property pluginType.
     */
    public PluginType getPluginType() {
        return PluginType.PREFERENCES;
    }

    /**
     * Getter for property name.
     * @return Value of property name.
     */
    public String getName() {
        return "Network";
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
		currentSettings.setBooleanValue(WINSOCK_UI_ENABLED_PROP, 
				coreController.getModel().getConnectionStatus()  == 
				com.nokia.epdt.core.connection.ConnectionStatus.ESTABLISHED);
		if (pluginPanel == null) {
            pluginPanel = new WinsockPluginJPanel(currentSettings);
        }
        return pluginPanel;
    }

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m) {
        messageOutputQueue = m.getMessageOutputQueue();
        m.setMessageListener(new WinsockMessageListener(),PLUGIN_UID);
        // request the current config. Proxy settings are only returned in
        // response to this message if WSOCK is enabled. It's done to avoid
        // any possibility of WSOCK code interfering with anything in the
        // emulator (that is, avoid reading CommDb).
        messageOutputQueue.put(WinsockMessageFactory.createConfigRequestMessage());
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new WinsockActionListener(), PLUGIN_UID, ActionListener.PLUGIN);
        this.actionEventQueue = a.getActionEventQueue();
    }
    
	public void bindCore(CoreController c){
		coreController = c;
	}

    /**
     * Sends action action event to core
     * @param action the action to perform
     */
    private void fireActionEvent(String action) {
        if (actionEventQueue != null) {
            actionEventQueue.put(new Action(action,
                                      PLUGIN_UID,
                                      Action.CORE,
                                      new int[0]));
        }
    }

    /**
     * Notifies  the core that the model has changed
     */
    private class ModelChangeListener extends PropertySetAdapter {
        public void propertySetChanged(PropertySet ps) {
            fireActionEvent(Action.PREFERENCES_PLUGIN_MODEL_CHANGED);
        }
    }

    /**
     * Listener for winsock messages. The message starts with a single byte
     * that identifies the message type. The rest depends on the message
     * type. In case of the configuration message, the message type (zero)
     * is followed by the null separated key-value pairs of UCS2 strings in
     * network (big endian, MSB first) byte order.
     *
     * The configuration request message is sent to the emulator plugin to
     * request the configration message to be sent back.
     */
    private class WinsockMessageListener implements MessageListener {

        /**
         * Handles incoming ECMT messages
         * @param e the message event
         */
        public void messageReceived(Message e) {
            byte[] body = e.getBody();
            if (body.length >= 1) { // need at least one byte
                int type = ((int)body[0]) % 0xff;
                if (log.isDebugEnabled()) {
                    log.debug("received Winsock message of type "+type);
                }
                switch(type) {
                case MESSAGE_TYPE_CONFIG:
                    Properties p = WinsockMessageParser.parseConfigMessage(body,1);
                    currentSettings.applyTextValues(p);
                    backupSettings.applyTextValues(p);
                    break;
                default:
                    if (log.isDebugEnabled()) {
                        log.error("unexpected message type "+type);
                    }
                    break;
                }
            }
        }
    }
}