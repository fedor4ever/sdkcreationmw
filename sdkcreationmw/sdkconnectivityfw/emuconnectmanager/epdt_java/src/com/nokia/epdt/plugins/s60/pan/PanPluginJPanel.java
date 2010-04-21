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

/* java.awt */
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.CoreUtils;
import com.nokia.wtk.util.gui.BorderPanel;
import com.nokia.wtk.util.gui.GUI;

/**
 * PanPluginJPanel
 *
 * @version 1.0
 */
class PanPluginJPanel extends JPanel {
	private static Logger log = Logger.getLogger(PanPluginJPanel.class);
	private static final int GAP = 6;
	private static final String AUTO_PORT = "auto";
	private static final String[] HCI_MODULES = new String[] { "BCSP"/*, "H4", "USB"*/ };
	private static final int HCI_USB_INDEX = 2;
	private static final int MAX_COM_PORTS = 16;

	private PanModel panModel;
	private PanPlugin panPlugin;

	private boolean updateInProgress = false;
	private JComboBox bluetoothComPortJComboBox;
	private JRadioButton bluetoothDisableJRadioButton;
	private JRadioButton bluetoothEnableJRadioButton;
	private JComboBox bluetoothHciJComboBox;

	/**
	 * Creates new form PanPluginJPanel
	 * @param plugin the plugin that creates this panel
	 * @param connected <code>true</code> if we are connected to the emulator
	 */
	PanPluginJPanel(PanPlugin plugin) {
		super(new GridBagLayout());
		panPlugin = plugin;
		panModel = plugin.getModel();
		initComponents();
	}

	private static String getResString(String key) {
		return PanResourceBundle.getInstance().getValue(key);
	}

	JPanel buildOnePanel(String t, JRadioButton b1, JRadioButton b2, JPanel c) {
		BorderPanel bp = new BorderPanel(b2);
		bp.getContentPanel().add(c);

		JPanel p = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();

		gbc.weightx = 1.0;
		gbc.gridwidth = GridBagConstraints.REMAINDER;
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.anchor = GridBagConstraints.NORTH;
		gbc.insets.left = bp.getBorder().getBorderInsets(bp).left
				+ BorderPanel.COMPONENT_INSET_H;
		p.add(b1, gbc);

		gbc.weighty = 1.0;
		gbc.insets.left = 0;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.gridheight = GridBagConstraints.REMAINDER;
		p.add(bp, gbc);

		int left = BorderPanel.COMPONENT_INSET_H + GAP;
		c.setBorder(new EmptyBorder(0, left, GAP, GAP));
		p.setBorder(new TitledBorder(t));
		return p;
	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 */
	private void initComponents() {
		Object[] btComPorts = new Object[MAX_COM_PORTS];
		for (int i = 0; i < MAX_COM_PORTS; i++) {
			String label = Integer.toString(i + 1);
			btComPorts[i] = new ComPortItem(label);
		}

		bluetoothDisableJRadioButton = new JRadioButton(
				getResString(PanConstants.BLUETOOTH_DISABLE_LABEL));
		bluetoothEnableJRadioButton = new JRadioButton(
				getResString(PanConstants.BLUETOOTH_ENABLE_LABEL));
		bluetoothComPortJComboBox = new JComboBox(btComPorts);
		bluetoothHciJComboBox = new JComboBox(HCI_MODULES);

		// Bluetooth
		JPanel bluetoothEnableJPanel = GUI
				.createRowPanel(new Component[] {
						new JLabel(
								getResString(PanConstants.BLUETOOTH_COM_PORT_LABEL)),
						bluetoothComPortJComboBox,
						new JLabel(
								getResString(PanConstants.BLUETOOTH_COM_HCI_LABEL)),
						bluetoothHciJComboBox });

		JPanel bluetoothJPanel = buildOnePanel(
				getResString(PanConstants.BLUETOOTH_BORDER_TITLE),
				bluetoothDisableJRadioButton, bluetoothEnableJRadioButton,
				bluetoothEnableJPanel);

		ButtonGroup bluetoothButtonGroup = new ButtonGroup();
		bluetoothButtonGroup.add(bluetoothDisableJRadioButton);
		bluetoothDisableJRadioButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				bluetoothDisableJRadioButtonActionPerformed(evt);
			}
		});

		bluetoothButtonGroup.add(bluetoothEnableJRadioButton);
		bluetoothEnableJRadioButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				bluetoothEnableJRadioButtonActionPerformed(evt);
			}
		});

		bluetoothComPortJComboBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				bluetoothComPortJComboBoxActionPerformed(evt);
			}
		});

		bluetoothHciJComboBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				bluetoothHciJComboBoxActionPerformed(evt);
			}
		});

		JPanel row = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridheight = GridBagConstraints.REMAINDER;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.insets.right = GAP;
		gbc.weightx = 1.0;
		gbc.weighty = 1.0;
		row.add(bluetoothJPanel, gbc);

		gbc.insets.right = 0;
		gbc.gridwidth = GridBagConstraints.REMAINDER;

		gbc.gridheight = 1;
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.anchor = GridBagConstraints.NORTH;
		add(row, gbc);

		gbc.gridheight = GridBagConstraints.REMAINDER;
		gbc.anchor = GridBagConstraints.SOUTH;

		setBorder(new EmptyBorder(GAP, GAP, GAP, GAP));

		//Set the initial values, obtained from .esk files:
		update();
		log.debug("Done initComponents");
	}

	/**
	 * Update UI components from Model:
	 */
	public void update() {
		log.debug("update() updateInProgress: " + updateInProgress);
		// Update Bluetooth panel
		try {
			if (!updateInProgress)
				log.debug("Inside if loop of update");
			{//If the function is not called as the result of components updates,
				//i.e. there is no update ongiong already
				updateInProgress = true;
				boolean bluetoothEnabled = panModel.isBluetoothEnabled();
				log.debug("bluetooth Enable: " + bluetoothEnabled);
				int hci = panModel.getBluetoothHci();
				boolean usb = (hci == HCI_USB_INDEX);
				log.debug("hci USB index: " + hci);
				bluetoothDisableJRadioButton.setSelected(!bluetoothEnabled);
				bluetoothEnableJRadioButton.setSelected(bluetoothEnabled);
				bluetoothHciJComboBox.setSelectedIndex(bluetoothEnabled ? hci
						: -1);
				bluetoothHciJComboBox.setEnabled(bluetoothEnabled);
				bluetoothComPortJComboBox.setEnabled(bluetoothEnabled);
				log.debug("bluetooth COM port: " + panModel.getBluetoothComPort());
				bluetoothComPortJComboBox.setSelectedIndex(panModel
						.getBluetoothComPort());
				bluetoothComPortJComboBox.setEnabled(bluetoothEnabled && !usb);

				updateInProgress = false;
				log.debug("If loop over");
			}
		} catch (RuntimeException e) {
			log.debug("Runtime Exception: " + e.getMessage());
			e.printStackTrace();
		}
	}

	private void bluetoothEnableJRadioButtonActionPerformed(ActionEvent evt) {
		if (!updateInProgress) {//I.e. if update came from UI

			log.debug(CoreUtils.EpocRoot() + FindAvailablePort.EXECUTABLE_FILE_NAME);

			FindAvailablePort availPort = FindAvailablePort.findAvailablePort();
			String port = null;
			try {
				port = availPort.getAvailablePortForBT(CoreUtils.EpocRoot()
						+ FindAvailablePort.EXECUTABLE_FILE_NAME);

				if (port == null) {
					log.debug(FindAvailablePort.NO_AVAILABLE_PORT_MESSAGE);
					// We set the default values of com ports (COM 1) when
					// switching from disabled mode
					panModel.setBluetoothComPort(0);
				} else {
					log.debug(FindAvailablePort.AVAILABLE_PORT_MESSAGE + port);
					// Set the port
					int setTempPort = Integer.parseInt(port.trim())-1;
					panModel.setBluetoothComPort(setTempPort);
					System.out.println("Output: " + port);
				}
			} catch (Exception e) {
				log.debug("Error reading COM port. BT enabled. Port set to 1.");
				// Set the default port 1 as a BT port and enable the BT
				panModel.setBluetoothComPort(0);
				e.printStackTrace();
			}

			panPlugin.pluginModelChangedAction();
			update();
		}
	}

	private void bluetoothDisableJRadioButtonActionPerformed(ActionEvent evt) {
		if (!updateInProgress) {//I.e. if update came from UI
			//Setting com port to -1 means disabling it
			panModel.setBluetoothComPort(-1);
			panPlugin.pluginModelChangedAction();
			update();
		}
	}

	private void bluetoothHciJComboBoxActionPerformed(ActionEvent evt) {
		if (!updateInProgress) {//I.e. if update came from UI
			int index = bluetoothHciJComboBox.getSelectedIndex();
			if (index >= 0) {
				panModel.setBluetoothHci(index);
				panPlugin.pluginModelChangedAction();
				update();
			}
		}
	}

	private void bluetoothComPortJComboBoxActionPerformed(ActionEvent evt) {
		if (!updateInProgress) {//I.e. if update came from UI

			panModel.setBluetoothComPort(bluetoothComPortJComboBox
					.getSelectedIndex());
			panPlugin.pluginModelChangedAction();
		}
	}

	private class ComPortItem {
		private String comPortNumber;

		ComPortItem(String label) {
			comPortNumber = label;
		}

		public String toString() {
			
			return panModel.getBluetoothHci() == HCI_USB_INDEX ? AUTO_PORT
					: comPortNumber;
		}
	}

}
