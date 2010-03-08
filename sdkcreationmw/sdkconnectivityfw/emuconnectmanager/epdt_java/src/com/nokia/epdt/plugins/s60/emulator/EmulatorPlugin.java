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




package com.nokia.epdt.plugins.s60.emulator;

import java.awt.Image;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.JOptionPane;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.action.ActionEventQueue;

/**
 * EmulatorPlugin
 */
public class EmulatorPlugin extends CustomPlugin {
    private static final Integer UID = Integer.valueOf(EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.PLUGIN_UID), 16);
    private static final Integer targetUID = Integer.valueOf(EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.TARGET_UID), 16);
    private MessageQueue messageOutputQueue = null;
    private JPanel jPanel = new GlobalSettingsJPanel(this);
    private EmulatorModel emulatorModel = EmulatorModel.getInstance();
    private ActionEventQueue actionEventQueue = null;
    /**
     * A ActionListener object is used to receive synchronously delivered actions.
     */
    private class EmulatorConfigurationPluginActionListener implements ActionListener {
        public void actionPerformed(Action e) {
            // OK button action
            if ( e.getName() == Action.PREFERENCES_OK) {
                try {
                    messageOutputQueue.put(EmulatorMessageFactory.createMessage(EmulatorPlugin.targetUID.intValue()));
                    actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, EmulatorPlugin.UID, Action.CORE, new int[0]));
                    actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN, EmulatorPlugin.UID, Action.CORE, new int[0]));
                    emulatorModel.applyPreviousValues();
                } catch (ValidateException ve) {
                    JOptionPane.showMessageDialog(jPanel, ve.getMessage(), EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.ERROR_DIALOG_TITLE), JOptionPane.ERROR_MESSAGE);
                }
                // Apply button action
            } else if ( e.getName() == Action.PREFERENCES_APPLY) {
                try {
                    messageOutputQueue.put(EmulatorMessageFactory.createMessage(EmulatorPlugin.targetUID.intValue()));
                    actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, EmulatorPlugin.UID, Action.CORE, new int[0]));
                    emulatorModel.applyPreviousValues();
                } catch (ValidateException ve) {
                    JOptionPane.showMessageDialog(jPanel, ve.getMessage(), EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.ERROR_DIALOG_TITLE), JOptionPane.ERROR_MESSAGE);
                }
                // Cancel button action
            } else if (e.getName() == Action.PREFERENCES_CANCEL) {
                emulatorModel.setPreviousValues();
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
                emulatorModel.setJustInTime(emulatorMessage.isJustInTime());
                emulatorModel.setPanics(emulatorMessage.isPanics());
                emulatorModel.setDebugInfo(emulatorMessage.isDebugInfo());
                actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, EmulatorPlugin.UID, Action.CORE, new int[0]));
                emulatorModel.applyPreviousValues();
            }
        }
    }

    /**
     * Creates a new instance of EmulatorPlugin
     */
    public EmulatorPlugin() {
        super();
    }

    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID() {
        return EmulatorPlugin.UID;
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
        return EmulatorResourceBundle.getInstance().getValue(EmulatorConstants.PLUGIN_NAME);
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
        return jPanel;
    }

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m) {
        m.setMessageListener(new EmulatorPlugin.EmulatorConfigurationPluginMessageListener(), EmulatorPlugin.UID);
        this.messageOutputQueue = m.getMessageOutputQueue();
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new EmulatorPlugin.EmulatorConfigurationPluginActionListener(), EmulatorPlugin.UID, ActionListener.PLUGIN);
        this.actionEventQueue = a.getActionEventQueue();
    }

    /**
     * Notifies the core that a plugin model is changed
     */
    public void pluginModelChangedAction() {
        actionEventQueue.put(new Action(Action.PREFERENCES_PLUGIN_MODEL_CHANGED, EmulatorPlugin.UID, Action.CORE, new int[0]));
    }

}
