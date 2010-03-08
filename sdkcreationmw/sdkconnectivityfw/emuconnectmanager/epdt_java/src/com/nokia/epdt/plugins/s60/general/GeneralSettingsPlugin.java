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


package com.nokia.epdt.plugins.s60.general;

import java.awt.Image;
import java.util.ResourceBundle;

import javax.swing.ImageIcon;
import javax.swing.JPanel;
import org.apache.log4j.Logger;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.config.epocini.EpocIniConfigurator;
import com.nokia.epdt.core.config.epocini.EpocIniConstants;

/**
 * GeneralSettingsPlugin
 *
 * @version 1.0
 */
public class GeneralSettingsPlugin extends CustomPlugin
		implements
			EpocIniConstants
{
	private static Logger log = Logger.getLogger(GeneralSettingsPlugin.class);
	private static final Integer UID = new Integer(0x10204576);
	private static final Integer targetUID = new Integer(0x1020456D);
	private GeneralSettingsJPanel mainPanel;
	private ActionEventQueue actionEventQueue = null;
	private EpocIniConfigurator configurator;
	private static ResourceBundle resourceBundle = ResourceBundle
			.getBundle("com.nokia.epdt.plugins.s60.general.generalsettings");

	/**
	 * A ActionListener object is used to receive synchronously delivered actions.
	 */
	private class GeneralSettingsConfigurationPluginActionListener
			implements
				ActionListener,
				EpocIniConstants
	{
		private void apply(Action e)
		{
			mainPanel.applyChanges();

			actionEventQueue.put(new Action(
					Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED,
					GeneralSettingsPlugin.UID, Action.CORE, new int[0]));

			if (e.getName() == Action.PREFERENCES_OK)
				actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN,
						GeneralSettingsPlugin.UID, Action.CORE, new int[0]));

			/*JOptionPane.showMessageDialog(mainPanel,
			 resourceBundle.getString("remainder.dialog.message"),
			 resourceBundle.getString("remainder.dialog.title"),
			 JOptionPane.INFORMATION_MESSAGE);*/
		}

		public void actionPerformed(Action e)
		{
			String a = e.getName();
			if (log.isDebugEnabled())
			{
				log.debug("Action listener");
			}

			// OK button action
			if (a == Action.PREFERENCES_OK)
			{
				if (log.isDebugEnabled())
				{
					log.debug("PreferencesOkAction");
				}
				apply(e);
				actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN,
						GeneralSettingsPlugin.UID, Action.CORE, new int[0]));
				// Apply button action
			} else if (a == Action.PREFERENCES_APPLY)
			{
				if (log.isDebugEnabled())
				{
					log.debug("PreferencesApplyAction");
				}
				apply(e);
				// Cancel button action
			} else if (a == Action.PREFERENCES_CANCEL)
			{
				actionEventQueue.put(new Action(
						Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED,
						GeneralSettingsPlugin.UID, Action.CORE, new int[0]));
				if (log.isDebugEnabled())
				{
					log.debug("PreferencesCancelAction");
				}
			}
		}
	}

	/**
	 * Creates a new instance of GeneralSettingsPlugin
	 */
	public GeneralSettingsPlugin()
	{
		super();
		if (log.isDebugEnabled())
		{
			log.debug("Constructor");
		}
	}

	/**
	 * Getter for property pluginUID.
	 * @return Value of property pluginUID.
	 */
	public Integer getPluginUID()
	{
		return GeneralSettingsPlugin.UID;
	}

	/**
	 * Getter for property pluginType.
	 * @return Value of property pluginType.
	 */
	public PluginType getPluginType()
	{
		return PluginType.PREFERENCES;
	}

	/**
	 * Getter for property name.
	 * @return Value of property name.
	 */
	public String getName()
	{
		return resourceBundle.getString("plugin.name");
	}

	/**
	 * Getter for property icon.
	 * @return Value of property icon.
	 */
	public Image getIcon(int type)
	{
		return (new ImageIcon()).getImage();
	}

	/**
	 * Getter for property jPanelView.
	 * @return plug-in module user interface
	 */
	public JPanel getJPanelView()
	{
		return mainPanel;
	}

	/**
	 * Bind the plugin's with the action interface.
	 * @param a the action interface which is used to mediate and receive actions
	 */
	public void bindActionInterface(ActionInterface a)
	{
		a
				.setActionListener(
						new GeneralSettingsPlugin.GeneralSettingsConfigurationPluginActionListener(),
						GeneralSettingsPlugin.UID, ActionListener.PLUGIN);
		this.actionEventQueue = a.getActionEventQueue();
	}

	public void bindCore(CoreController controller)
	{
		configurator = controller.getEpocIniConfigurator();
		mainPanel = new GeneralSettingsJPanel(this);
	}

	/**
	 * Notifies the core that a plugin model is changed
	 */
	public void pluginModelChangedAction()
	{
		if (log.isDebugEnabled())
		{
			log.debug("ModelChangedAction");
		}
		actionEventQueue.put(new Action(
				Action.PREFERENCES_PLUGIN_MODEL_CHANGED,
				GeneralSettingsPlugin.UID, Action.CORE, new int[0]));
	}

	public EpocIniConfigurator getConfigurator()
	{
		return configurator;
	}

}
