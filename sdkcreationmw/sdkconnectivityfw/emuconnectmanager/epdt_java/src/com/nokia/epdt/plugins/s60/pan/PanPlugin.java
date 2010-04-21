/*
 * Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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

import java.awt.Image;
import java.io.IOException;

import javax.swing.ImageIcon;
import javax.swing.JPanel;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.CoreUtils;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;

/**
 * PanPlugin
 *
 * @version 1.0
 */
public class PanPlugin extends CustomPlugin {
	private static Logger log = Logger.getLogger(PanPlugin.class);
	private static final Integer UID = Integer.valueOf(PanResourceBundle
			.getInstance().getValue(PanConstants.PLUGIN_UID), 16);
	private static final Integer TARGET_UID = Integer.valueOf(PanResourceBundle
			.getInstance().getValue(PanConstants.TARGET_UID), 16);
	private PanModel panModel;
	private MessageQueue messageOutputQueue;
	private ActionEventQueue actionEventQueue;
	private CoreController coreController;
	private JPanel jPanel;

	/**
	 * A ActionListener object is used to receive synchronously delivered actions.
	 */
	private class PanPluginActionListener implements ActionListener {
		public void actionPerformed(Action e) {
			if (e.getName() == Action.PREFERENCES_OK) {
				actionEventQueue.put(new Action(
						Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED,
						PanPlugin.UID, Action.CORE, new int[0]));
				panModel.save();
				actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN,
						PanPlugin.UID, Action.CORE, new int[0]));
			} else if (e.getName() == Action.PREFERENCES_APPLY) {
				actionEventQueue.put(new Action(
						Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED,
						PanPlugin.UID, Action.CORE, new int[0]));
				panModel.save();
			} else if (e.getName() == Action.PREFERENCES_CANCEL) {
			}
		}
	}

	/**
	 * Creates a new instance of PanPlugin
	 */
	public PanPlugin() {
		super();

	}

	/**
	 * Getter for property pluginUID.
	 * @return Value of property pluginUID.
	 */
	public Integer getPluginUID() {
		return PanPlugin.UID;
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
		return PanResourceBundle.getInstance().getValue(
				PanConstants.PLUGIN_NAME);
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
		if (jPanel == null) {
			jPanel = new PanPluginJPanel(this);
		}
		return jPanel;
	}

	/**
	 * Bind the plugin's with the epdt core.
	 * @param c instance of CoreController that is assosiated with this plugin
	 */
	public void bindCore(CoreController c) {
		try {
			panModel = new PanModel(CoreUtils.EpocRoot());
		} catch (IOException ex) { //TODO: do something to prevent view from loading
			log.error("Error loading BT ESK files: " + ex.getMessage());
		}
		coreController = c;
	}

	/**
	 * Bind the plugin's with the message interface.
	 * @param m the message interface which is used to send and receive messages
	 */
	public void bindMessageInterface(MessageInterface m) {
	}

	/**
	 * Bind the plugin's with the action interface.
	 * @param a the action interface which is used to mediate and receive actions
	 */
	public void bindActionInterface(ActionInterface a) {
		a.setActionListener(new PanPlugin.PanPluginActionListener(),
				PanPlugin.UID, ActionListener.PLUGIN);
		this.actionEventQueue = a.getActionEventQueue();
	}

	/**
	 * Notifies the core that a plugin model is changed
	 */
	public void pluginModelChangedAction() {
		if (actionEventQueue != null) {
			actionEventQueue.put(new Action(
					Action.PREFERENCES_PLUGIN_MODEL_CHANGED, PanPlugin.UID,
					Action.CORE, new int[0]));
		}
	}

	/**
	 * Returns the data model for this plugin
	 * @return the data model
	 */
	PanModel getModel() {
		return panModel;
	}
}
