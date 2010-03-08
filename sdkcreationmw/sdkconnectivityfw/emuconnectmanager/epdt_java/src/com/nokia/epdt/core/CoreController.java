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

package com.nokia.epdt.core;

import java.util.Iterator;
import java.util.Properties;
import java.util.ResourceBundle;

import javax.swing.JButton;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.command.Command;
import com.nokia.epdt.core.command.CommandFactory;
import com.nokia.epdt.core.command.CommandQueue;
import com.nokia.epdt.core.config.epocini.EpocIniConfigurator;
import com.nokia.epdt.core.connection.ConnectionStatus;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.plugin.Plugin;
import com.nokia.epdt.core.plugin.PluginInstance;

/**
 * CoreController
 */
public class CoreController implements java.awt.event.ActionListener {
	private static final Integer UID = new Integer(0x10206FEF);
	private static Logger log = Logger.getLogger(CoreController.class);
	private ResourceBundle bundle;
	private ActionEventQueue actionEventQueue;
	private CommandQueue commandQueue;
	private CoreModel coreModel;
	private CoreHandler coreHandler;
	private CommandFactory commandFactory = new CommandFactory(this);
	private EpocIniConfigurator epocIniConfigurator;

	private JButton preferencesOkButton;
	private JButton preferencesApplyButton;
	private JButton preferencesCancelButton;

	private JButton utilitiesOkButton;
	private JButton utilitiesApplyButton;
	private JButton utilitiesCancelButton;

	public void actionPerformed(java.awt.event.ActionEvent e) {
		Object o = e.getSource();

		if (o == preferencesOkButton) {
			if (!coreModel.getChangedPreferencesPluginModels().isEmpty()) {
				for (Iterator i = coreModel.getChangedPreferencesPluginModels()
						.iterator(); i.hasNext();) {
					actionEventQueue.put(new Action(Action.PREFERENCES_OK,
							null, Action.PLUGIN, new int[] { ((Integer) i
									.next()).intValue() }));
				}
			} else {
				hidePreferencesUI();
			}
		} else if (o == preferencesCancelButton) {
			// Code commented to get Cancel Button effect, keep this code for future reference
			/*
			 * hidePreferencesUI(); 
			 * for ( Iterator i = coreModel.getChangedPreferencesPluginModels().iterator();i.hasNext(); ) { 
			 * 		actionEventQueue.put(new Action(Action.PREFERENCES_CANCEL, null, Action.PLUGIN, new int[]{((Integer) i.next()).intValue()})); 
			 * }
			 */
			System.exit(0);
		} else if (o == preferencesApplyButton) {
			for (Iterator i = coreModel.getChangedPreferencesPluginModels()
					.iterator(); i.hasNext();) {
				actionEventQueue.put(new Action(Action.PREFERENCES_APPLY, null,
						Action.PLUGIN, new int[] { ((Integer) i.next())
								.intValue() }));
			}
		} else if (o == utilitiesOkButton) {
			if (!coreModel.getChangedUtilitiesPluginModels().isEmpty()) {
				for (Iterator i = coreModel.getChangedUtilitiesPluginModels()
						.iterator(); i.hasNext();) {
					actionEventQueue.put(new Action(Action.UTILITIES_OK, null,
							Action.PLUGIN, new int[] { ((Integer) i.next())
									.intValue() }));
				}
			} else {
				hideUtilitiesUI();
			}
		} else if (o == utilitiesCancelButton) {
			hideUtilitiesUI();
			for (Iterator i = coreModel.getChangedUtilitiesPluginModels()
					.iterator(); i.hasNext();) {
				actionEventQueue.put(new Action(Action.UTILITIES_CANCEL, null,
						Action.PLUGIN, new int[] { ((Integer) i.next())
								.intValue() }));
			}
		} else if (o == utilitiesApplyButton) {
			for (Iterator i = coreModel.getChangedUtilitiesPluginModels()
					.iterator(); i.hasNext();) {
				actionEventQueue.put(new Action(Action.UTILITIES_APPLY, null,
						Action.PLUGIN, new int[] { ((Integer) i.next())
								.intValue() }));
			}
		}
	}

	/**
	 * A ActionListener object is used to receive synchronously delivered
	 * actions.
	 */
	private class CoreControllerActionListener implements ActionListener {
		public void actionPerformed(Action e) {
			// Preferences
			if (e.getName() == Action.PREFERENCES_PLUGIN_MODEL_CHANGED) {
				// check if this model is already set changed
				if (coreModel.getChangedPreferencesPluginModels().indexOf(
						e.getUid()) == -1) {
					log.debug("Add plugins uid = " + e.getUid().intValue()
							+ " to preferences model changed list.");
					coreModel.getChangedPreferencesPluginModels().add(
							e.getUid());
					coreModel.setPreferencesApplyJButtonEnabled(true);
					coreModel.getNotReadyToHidePreferencesPluginList().add(
							e.getUid());
				}
			} else if (e.getName() == Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED) {
				log.debug("Remove plugins uid = " + e.getUid().intValue()
						+ " to preferences model changed list.");
				coreModel.getChangedPreferencesPluginModels()
						.remove(e.getUid());
				if (coreModel.getChangedPreferencesPluginModels().isEmpty()) {
					coreModel.setPreferencesApplyJButtonEnabled(false);
				}
				coreModel.getNotReadyToHidePreferencesPluginList().remove(
						e.getUid());
			} else if (e.getName() == Action.PREFERENCES_HIDE_PLUGIN) {
				coreModel.getNotReadyToHidePreferencesPluginList().remove(
						e.getUid());
				if (coreModel.getNotReadyToHidePreferencesPluginList()
						.isEmpty()) {
					hidePreferencesUI();
				}
			}
			// Diagnostics
			else if (e.getName() == Action.DIAGNOSTICS_PLUGIN_MODEL_CHANGED) {
				// check if this model is already set changed
				if (coreModel.getChangedDiagnosticsPluginModels().indexOf(
						e.getUid()) == -1) {
					log.debug("Add plugins uid = " + e.getUid().intValue()
							+ " to diagnostics model changed list.");
					coreModel.getChangedDiagnosticsPluginModels().add(
							e.getUid());
					// Diagnostics does not have Apply button
					coreModel.getNotReadyToHideDiagnosticsPluginList().add(
							e.getUid());
				}
			} else if (e.getName() == Action.DIAGNOSTICS_REMOVE_PLUGIN_MODEL_CHANGED) {
				log.debug("Remove plugins uid = " + e.getUid().intValue()
						+ " to diagnostics model changed list.");
				coreModel.getChangedDiagnosticsPluginModels()
						.remove(e.getUid());
				if (coreModel.getChangedDiagnosticsPluginModels().isEmpty()) {
					// Diagnostics does not have Apply button
				}
				coreModel.getNotReadyToHideDiagnosticsPluginList().remove(
						e.getUid());
			} else if (e.getName() == Action.DIAGNOSTICS_HIDE_PLUGIN) {
				coreModel.getNotReadyToHideDiagnosticsPluginList().remove(
						e.getUid());
				if (coreModel.getNotReadyToHideDiagnosticsPluginList()
						.isEmpty()) {
					hideDiagnosticsUI();
				}
			}
			// Utilities
			else if (e.getName() == Action.UTILITIES_PLUGIN_MODEL_CHANGED) {
				// check if this model is already set changed
				if (coreModel.getChangedUtilitiesPluginModels().indexOf(
						e.getUid()) == -1) {
					log.debug("Add plugins uid = " + e.getUid().intValue()
							+ " to utilities model changed list.");
					coreModel.getChangedUtilitiesPluginModels().add(e.getUid());
					coreModel.setUtilitiesApplyJButtonEnabled(true);
					coreModel.getNotReadyToHideUtilitiesPluginList().add(
							e.getUid());
				}
			} else if (e.getName() == Action.UTILITIES_REMOVE_PLUGIN_MODEL_CHANGED) {
				log.debug("Remove plugins uid = " + e.getUid().intValue()
						+ " to utilities model changed list.");
				coreModel.getChangedUtilitiesPluginModels().remove(e.getUid());
				if (coreModel.getChangedUtilitiesPluginModels().isEmpty()) {
					coreModel.setUtilitiesApplyJButtonEnabled(false);
				}
				coreModel.getNotReadyToHideUtilitiesPluginList().remove(
						e.getUid());
			} else if (e.getName() == Action.UTILITIES_HIDE_PLUGIN) {
				coreModel.getNotReadyToHideUtilitiesPluginList().remove(
						e.getUid());
				if (coreModel.getNotReadyToHideUtilitiesPluginList().isEmpty()) {
					hideUtilitiesUI();
				}
			}
		}
	}

	/**
	 * A simple way to perform command via ECMT connection
	 */
	private class CoreControllerMessageListener implements MessageListener {
		public void messageReceived(Message e) {
			Properties param = new Properties();
			// The body contains null separated key-value pairs of UCS2
			// strings in network (big endian, MSB first) byte order
			int state = 0; // 0 = nothing, 1 = key, 2 = value
			StringBuffer key = new StringBuffer();
			StringBuffer value = new StringBuffer();
			byte[] body = e.getBody();
			int n = body.length / 2;
			for (int i = 0; i < n; i++) {
				int msb = ((int) body[2 * i]) & 0xff;
				int lsb = ((int) body[2 * i + 1]) & 0xff;
				char c = (char) ((msb << 8) | lsb);
				if (c == 0) {
					state = (state + 1) % 3;
					if (state == 0) {
						param.put(key.toString(), value.toString());
						key.setLength(0);
						value.setLength(0);
					}
				} else {
					switch (state) {
					case 0:
						state = 1; // no break
					case 1:
						key.append(c);
						break;
					case 2:
						value.append(c);
						break;
					}
				}
			}

			if (state == 2) {
				param.put(key.toString(), value.toString());
			}

			log.debug("Command received via ecmt: "
					+ param.get(Constants.COMMAND_KEY));
			commandQueue.put(commandFactory.create(param));
		}
	}

	public CoreController(boolean exitOnClose, String unused, Properties param,
			ResourceBundle bundle, EpocIniConfigurator epocIniConfigurator) {
		this(exitOnClose, unused, bundle, epocIniConfigurator);
		log.debug("Put action to command queue");
		getCommandQueue().put(commandFactory.create(param));
	}

	/**
	 * Creates a new instance of CoreController
	 */
	public CoreController(boolean exitOnClose, String unused,
			ResourceBundle bundle, EpocIniConfigurator epocIniConfigurator) {

		this.epocIniConfigurator = epocIniConfigurator;
		this.bundle = bundle;

		log.debug("Initialize model");
		coreModel = new CoreModel(this);
		coreModel.setExitOnClose(exitOnClose);

		// Create plug-ins
		log.debug("Create plug-ins");
		PluginInstance[] plugins = PluginInstance.createPlugins(bundle
				.getString(Constants.PLUGIN_DIRECTORY), bundle
				.getString(Constants.PLUGIN_FILE_NAME_EXTENSION), bundle);
		coreModel.setPlugins(plugins);

		this.commandQueue = CoreQueue.createCommandQueue();
		this.coreHandler = new CoreHandler();

		log.debug("Bind message and action interfaces to core ");
		bindActionInterface(coreHandler);
		bindMessageInterface(coreHandler);

		log.debug("Bind message and action interfaces to plug-ins");
		for (int i = 0; i < plugins.length; i++) {
			Plugin plugin = plugins[i].getPlugin();

			// Bind messaging interface to plug-ins
			try {
				plugin.bindMessageInterface(coreHandler);
			} catch (Throwable x) {
				log.error(plugins[i].getPluginName(), x);
			}

			// Bind action interface to cplug-ins
			try {
				plugin.bindActionInterface(coreHandler);
			} catch (Throwable x) {
				log.error(plugins[i].getPluginName(), x);
			}

			// Bind core to plug-ins
			try {
				plugin.bindCore(this);
			} catch (Throwable x) {
				log.error(plugins[i].getPluginName(), x);
			}
		}

		if (Boolean.getBoolean("diagnostics.visible")) {
			coreModel.getDiagnosticsUI().setVisible(true);
		}
		if (Boolean.getBoolean("preferences.visible")) {
			coreModel.getPreferencesUI().setVisible(true);
		}
		if (Boolean.getBoolean("utilities.visible")) {
			coreModel.getUtilitiesUI().setVisible(true);
		}
		startThreads();
	}

	public void startThreads() {
		log.debug("Start action mediator");
		coreHandler.getActionMediator().start();
		log.debug("Start message mediator");
		coreHandler.getMessageMediator().start();
		CoreQueue.getCommandMediator(commandQueue).start();
	}

	public void setPreferencesButtons(JButton ok, JButton cancel, JButton apply) {
		preferencesOkButton = ok;
		preferencesApplyButton = apply;
		preferencesCancelButton = cancel;

		preferencesOkButton.addActionListener(this);
		preferencesApplyButton.addActionListener(this);
		preferencesCancelButton.addActionListener(this);
	}

	public void setUtilitiesButtons(JButton ok, JButton cancel, JButton apply) {
		utilitiesOkButton = ok;
		utilitiesApplyButton = apply;
		utilitiesCancelButton = cancel;

		utilitiesOkButton.addActionListener(this);
		utilitiesApplyButton.addActionListener(this);
		utilitiesCancelButton.addActionListener(this);
	}

	public CoreModel getModel() {
		return coreModel;
	}

	public ResourceBundle getResourceBundle() {
		return bundle;
	}

	public CommandQueue getCommandQueue() {
		return commandQueue;
	}

	public CoreHandler getMessageHandler() {
		return coreHandler;
	}

	public ActionInterface getActionInterface() {
		return coreHandler;
	}

	public EpocIniConfigurator getEpocIniConfigurator() {
		return epocIniConfigurator;
	}

	/**
	 * Bind the controller with the action interface.
	 * 
	 * @param a
	 *            the action interface which is used to mediate and receive
	 *            actions
	 */
	private void bindActionInterface(ActionInterface a) {
		a.setActionListener(new CoreControllerActionListener(), null,
				ActionListener.CORE);
		this.actionEventQueue = a.getActionEventQueue();
	}

	/**
	 * Bind the controller with the message interface.
	 * 
	 * @param m
	 *            the message interface which is used to send and receive
	 *            messages
	 */
	private void bindMessageInterface(MessageInterface m) {
		m.setMessageListener(new CoreControllerMessageListener(), UID);
	}

	/**
	 * Hide preferences UserInterface, its subcomponents, and all of its owned
	 * children. The UserInterface and its subcomponents can be made visible
	 * again with a call to show.
	 */
	private void hidePreferencesUI() {
		if (coreModel.isExitOnClose()) {
			exitProgramCall();
		} else {
			coreModel.getPreferencesUI().setVisible(false);
			systemExitCheck();
		}
	}

	/**
	 * Hide diagnostics UserInterface, its subcomponents, and all of its owned
	 * children. The UserInterface and its subcomponents can be made visible
	 * again with a call to show.
	 */
	private void hideDiagnosticsUI() {
		if (coreModel.isExitOnClose()) {
			exitProgramCall();
		} else {
			coreModel.getDiagnosticsUI().setVisible(false);
			systemExitCheck();
		}
	}

	/**
	 * Hide utilities UserInterface, its subcomponents, and all of its owned
	 * children. The UserInterface and its subcomponents can be made visible
	 * again with a call to show.
	 */
	private void hideUtilitiesUI() {
		if (coreModel.isExitOnClose()) {
			exitProgramCall();
		} else {
			coreModel.getUtilitiesUI().setVisible(false);
			systemExitCheck();
		}
	}

	/**
	 * System exits if there is no connection and user interfaces are not
	 * visible
	 */
	private void systemExitCheck() {
		if (coreModel.getConnectionStatus() == ConnectionStatus.IDLE
				&& !coreModel.isPreferencesUIVisible()
				&& !coreModel.isDiagnosticsUIVisible()
				&& !coreModel.isUtilitiesUIVisible()) {
			exitProgramCall();
		}
	}

	private class End implements Command {
		public void execute() {
			log
					.debug("CoreController.End.execute() HAS BEEN CALLED! \n******* EXITING ***** ");
			Action event = new Action(Action.PROGRAM_CLOSING, null, Action.ALL);
			Iterator i = coreHandler.getAllActionListenerTable().iterator();
			while (i.hasNext()) {
				ActionListener l = (ActionListener) i.next();
				l.actionPerformed(event);
			}
			System.exit(0);
		}
	}

	/**
	 * Program closing execution call. Iterating through ActionListeners and
	 * ending program with System.exit call.
	 */
	public void exitProgramCall() {
		log
				.debug("CoreController.exitProgramCall() HAS BEEN CALLED! \n******* EXITING ***** ");
		commandQueue.put(new End());
	}
}
