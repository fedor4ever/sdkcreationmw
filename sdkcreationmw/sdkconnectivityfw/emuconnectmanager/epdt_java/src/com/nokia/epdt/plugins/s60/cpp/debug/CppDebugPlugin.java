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



package com.nokia.epdt.plugins.s60.cpp.debug;

import java.util.Properties;
import javax.swing.JOptionPane;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.config.epocini.EpocIniConfigurator;
import com.nokia.epdt.core.config.epocini.EpocIniConstants;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.plugins.s60.emulator.EmulatorConstants;
import com.nokia.epdt.plugins.s60.emulator.EmulatorMessage;
import com.nokia.epdt.plugins.s60.emulator.EmulatorMessageFactory;
import com.nokia.epdt.plugins.s60.emulator.EmulatorMessageParser;
import com.nokia.epdt.plugins.s60.emulator.EmulatorModel;
import com.nokia.epdt.plugins.s60.emulator.EmulatorResourceBundle;
import com.nokia.epdt.plugins.s60.emulator.ValidateException;

import org.apache.log4j.Logger;

/**
 *
 * CppDebugPlugin
 */
public class CppDebugPlugin extends CustomPlugin implements EpocIniConstants {
    private static Logger log = Logger.getLogger(CppDebugPlugin.class);
    private static final Integer UID = Integer.valueOf(EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.PLUGIN_UID), 16);
    private static final Integer targetUID = Integer.valueOf(EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.TARGET_UID), 16);
    private CppDebugJPanel jPanel = null;
    private MessageQueue messageOutputQueue = null;
    private Properties modelProperties = new Properties();
    private Properties prevModelProperties = new Properties();
    private ActionEventQueue actionEventQueue = null;
    private EmulatorModel emulatorModel = EmulatorModel.getInstance();
    private EpocIniConfigurator epocIniConfigurator;
    
    private class CppDebugActionListener implements ActionListener {
        private void apply() {
            prevModelProperties = new Properties(modelProperties);
            writeData(modelProperties);

            try {
                messageOutputQueue.put(EmulatorMessageFactory.createMessage(CppDebugPlugin.targetUID.intValue()));
                emulatorModel.applyPreviousValues();
            } catch (ValidateException ve) {
                JOptionPane.showMessageDialog(jPanel, ve.getMessage(), EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.ERROR_DIALOG_TITLE), JOptionPane.ERROR_MESSAGE);
            }

            actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, CppDebugPlugin.UID, Action.CORE, new int[0]));
        }

        public void actionPerformed(Action e) {
            setEmulatorModelData();
            // OK button action
            if ( e.getName() == Action.PREFERENCES_OK) {
                apply();
                actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN, CppDebugPlugin.UID, Action.CORE, new int[0]));
                // Apply button action
            } else if ( e.getName() == Action.PREFERENCES_APPLY) {
                apply();
                // cancel button action
            } else if ( e.getName() == Action.PREFERENCES_CANCEL) {
                modelProperties = new Properties(prevModelProperties);
                emulatorModel.setPreviousValues();
                actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, CppDebugPlugin.UID, Action.CORE, new int[0]));
                actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN, CppDebugPlugin.UID, Action.CORE, new int[0]));

            }
        }
    }

    /**
     * A MessageListener object is used to receive asynchronously delivered messages.
     */
    private class EmulatorConfigurationPluginMessageListener implements MessageListener {
        public void messageReceived(Message message) {
            EmulatorMessage emulatorMessage = EmulatorMessageParser.parseMessage(message);

            if ( emulatorMessage.getType().equals(EmulatorConstants.ERROR_TYPE_EMULATOR_MESSAGE) ) {
                JOptionPane.showMessageDialog(jPanel, emulatorMessage.getErrorMessage(), EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.ERROR_DIALOG_TITLE), JOptionPane.ERROR_MESSAGE);
            } else if ( emulatorMessage.getType().equals(EmulatorConstants.OK_TYPE_EMULATOR_MESSAGE) ) {
                // DO NOTHING
                //JOptionPane.showMessageDialog(jPanel, EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.INFORMATION_DIALOG_MESSAGE), EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.INFORMATION_DIALOG_TITLE), JOptionPane.INFORMATION_MESSAGE);
            } else if ( emulatorMessage.getType().equals(EmulatorConstants.VALUE_TYPE_EMULATOR_MESSAGE)) {
                // set model
		//                modelProperties.setProperty(JUST_IN_TIME, emulatorMessage.isJustInTime() == true ? "1": "0");
                modelProperties.setProperty(EXTENDED_PANIC_CODE_FILE, emulatorMessage.isPanics() == true ? "true": "false");
		//                modelProperties.setProperty(LOG_TO_FILE, emulatorMessage.isDebugInfo() == true ? "1": "0");
		setEmulatorModelData();
            }
        }
    };

    /**
     * Set emulator model data
     */
    private void setEmulatorModelData() {
        emulatorModel.setJustInTime(modelProperties.getProperty(JUST_IN_TIME).equals("query") ? true: false);
        emulatorModel.setPanics(modelProperties.getProperty(EXTENDED_PANIC_CODE_FILE).equals("true") ? true: false);
        emulatorModel.setDebugInfo(modelProperties.getProperty(LOG_TO_FILE).equals("1") ? true: false);
    }

    /**
     * Trying to write properties to file
     * @param p model properties
     */
    private void writeData(Properties p) {
    	
        EpocIniConfigurator configurator = epocIniConfigurator;
        configurator.setProperties(modelProperties);
        configurator.saveSettings();

	if (log.isDebugEnabled()) {
	    log.debug("writeData:" +
		      " " + LOG_TO_FILE + " = " + modelProperties.getProperty(LOG_TO_FILE) +
		      " " + LOG_TO_DEBUGGER + " = " + modelProperties.getProperty(LOG_TO_DEBUGGER) +
		      " " + JUST_IN_TIME + " = " + modelProperties.getProperty(LOG_TO_FILE) +
		      " " + EXTENDED_PANIC_CODE_FILE + " = " + modelProperties.getProperty(LOG_TO_FILE));
	}
    }


    /**
     * Constructing
     */
    public CppDebugPlugin() {
        super(UID, EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.PLUGIN_NAME), PluginType.PREFERENCES);
    }

    /**
     * Properties setting from ini file
     */
    private void loadSettings(Properties p) {
        EpocIniConfigurator c = epocIniConfigurator;
        p.setProperty(LOG_TO_FILE, c.getProperty(LOG_TO_FILE));
        p.setProperty(LOG_TO_DEBUGGER, c.getProperty(LOG_TO_DEBUGGER));
        p.setProperty(JUST_IN_TIME, c.getProperty(JUST_IN_TIME));
        p.setProperty(EXTENDED_PANIC_CODE_FILE, c.getProperty(EXTENDED_PANIC_CODE_FILE));

	if (log.isDebugEnabled()) {
	    log.debug("loadSettings:" +
		      " " + LOG_TO_FILE + " = " + c.getProperty(LOG_TO_FILE) +
		      " " + LOG_TO_DEBUGGER + " = " + c.getProperty(LOG_TO_DEBUGGER) +
		      " " + JUST_IN_TIME + " = " + c.getProperty(LOG_TO_FILE) +
		      " " + EXTENDED_PANIC_CODE_FILE + " = " + c.getProperty(LOG_TO_FILE));
	}
    }

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m) {
        m.setMessageListener(new CppDebugPlugin.EmulatorConfigurationPluginMessageListener(), CppDebugPlugin.UID);
        this.messageOutputQueue = m.getMessageOutputQueue();
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new CppDebugPlugin.CppDebugActionListener(), CppDebugPlugin.UID, ActionListener.PLUGIN);
        this.actionEventQueue = a.getActionEventQueue();
    }

    public void bindCore(CoreController controller) {
    	epocIniConfigurator = controller.getEpocIniConfigurator();
        loadSettings(modelProperties);
        loadSettings(prevModelProperties);
        jPanel = new CppDebugJPanel(this);
        //this.emulatorModel.addObserver(jPanel);
        this.setJPanelView(jPanel);
    }
    
    /**
     * Getter for plugin property
     * @param key
     * @param value
     */
    public void setProperty(String key, String value) {
	if (log.isDebugEnabled()) {
	    log.debug("setProperty: key = " + key + ", value = " + value);
	}

        modelProperties.setProperty(key, value);
        actionEventQueue.put(new Action(Action.PREFERENCES_PLUGIN_MODEL_CHANGED, CppDebugPlugin.UID, Action.CORE, new int[0]));
    }

    /**
     * Setter for plugin property
     * @param key
     * @return value or ""
     */
    public String getProperty(String key) {
        return modelProperties.getProperty(key, "");
    }
}
