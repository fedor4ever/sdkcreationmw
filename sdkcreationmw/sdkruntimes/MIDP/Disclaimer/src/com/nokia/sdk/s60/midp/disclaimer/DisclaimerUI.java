/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.sdk.s60.midp.disclaimer;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.SoftBevelBorder;
import javax.swing.border.TitledBorder;

/**
 * Show the disclaimer at the startup of the emulator. User will be able to
 * choose the options weather to display or not to display the disclaimer
 * window.
 * 
 * @author Pranav Gothadiy
 * 
 */
public class DisclaimerUI/* extends JFrame */{

	/**
	 * Disclaimer properties file path
	 */
	private static String DISCLAIMER_PROPERTIES_PATH;
	
	/**
	 * Dialog box title
	 */
	private static final String DISCLAIMER_TITLE = "Licensee Agreement";
	
	/**
	 * Check box title
	 */
	private static final String CHECKBOX_TITLE = "Don't show disclaimer again";
	
	/**
	 * Error message on console - unable to find the properties file
	 */
	private static final String ERROR_IN_SETTING_NATIVE_LOOK_AND_FEEL = "Error in setting native Look And Feel: ";
	
	/**
	 * Ok button
	 */
	protected JButton btnExit = null;
	
	/**
	 * Check Box - to show of not to show disclaimer
	 */
	protected JCheckBox isDisclaimerRequire = null;

	/**
	 * Boder is being used by the discliamer
	 */
	private Border loweredBorder = new CompoundBorder(new SoftBevelBorder(
			SoftBevelBorder.LOWERED), new EmptyBorder(5, 5, 5, 5));

	/**
	 * Construct the Disclaimer pane
	 */
	public DisclaimerUI(Disclaimer frame) {

		DISCLAIMER_PROPERTIES_PATH = Disclaimer.ROOT_PATH
				+ "disclaimer.properties";
		// Read the property file and check weather disclaimer is required to
		// display
		if (!readPropertiesFile().equals("true")) {

			// No need to display disclaimer, exit silently
			System.exit(0);
		}

		// Disclaimer require, display it
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {
			System.out.println(ERROR_IN_SETTING_NATIVE_LOOK_AND_FEEL + e);
		}

		frame.setTitle(DISCLAIMER_TITLE);
		frame.setSize(300, 250);
		frame.setResizable(false);
		frame.setLocation(200, 200);

		// Set to ignore the button
		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		frame.setAlwaysOnTop(true);

		Container container = frame.getContentPane();
		JPanel p2 = createUserDefinedPanel(true);
		container.add(p2);
		p2.setBorder(new CompoundBorder(new TitledBorder(null, "",
				TitledBorder.LEFT, TitledBorder.TOP), new EmptyBorder(5, 5, 5,
				5)));

		JPanel innerPan1 = new JPanel();
		p2.add(innerPan1);

		innerPan1.add(new JLabel("This instance of the S60 5th Edition SDK"));
		innerPan1.add(Box.createRigidArea(new Dimension(0, 15)));
		innerPan1.add(new JLabel("is for internal Nokia use only. However,"));
		innerPan1.add(Box.createRigidArea(new Dimension(0, 15)));
		innerPan1.add(new JLabel("it may be provided to subcontractors,"));
		innerPan1.add(Box.createRigidArea(new Dimension(0, 15)));
		innerPan1.add(new JLabel("partners, and others as long as the use"));
		innerPan1.add(Box.createRigidArea(new Dimension(0, 15)));
		innerPan1.add(new JLabel(" is governed by NDA and LLA contracts."));
		innerPan1.add(Box.createRigidArea(new Dimension(0, 15)));
		innerPan1.add(new JLabel("Any other use is prohibited."));

		isDisclaimerRequire = new JCheckBox(CHECKBOX_TITLE, false);
		innerPan1.add(isDisclaimerRequire);
		innerPan1.add(Box.createRigidArea(new Dimension(0, 50)));

		btnExit = new JButton("OK");
		innerPan1.add(btnExit);

		// Register listeners for buttons
		btnExit.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				System.exit(0);
			}
		});

		// Register Listener for check box
		isDisclaimerRequire.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				boolean enabled = isDisclaimerRequire.isSelected();
				String propValue = "";
				// If choiceBox is enabled, means do not show disclaimer, and
				// that why we need to make it false
				if (enabled) {
					propValue = "false";
				} else {
					propValue = "true";
				}
				try {

					File file = new File(DISCLAIMER_PROPERTIES_PATH);
					FileOutputStream out = new FileOutputStream(file);
					byte[] bits = ("disclaimer=" + propValue).getBytes();
					out.write(bits);
					out.flush();

					System.out.println("done!!");
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});

		frame.setVisible(true);

	}

	/**
	 * Creates user defined panel
	 * @param threeD
	 * @return
	 */
	public JPanel createUserDefinedPanel(boolean threeD) {
		JPanel p = new JPanel();
		p.setLayout(new BoxLayout(p, BoxLayout.PAGE_AXIS));
		p.setAlignmentY(JFrame.TOP_ALIGNMENT);
		p.setAlignmentX(JFrame.CENTER_ALIGNMENT);
		if (threeD) {
			p.setBorder(loweredBorder);
		}
		return p;
	}

	/**
	 * Read disclaimer.properties file
	 * 
	 * @return
	 */
	public String readPropertiesFile() {
		try {
			File file = new File(DISCLAIMER_PROPERTIES_PATH);
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(file)));
			String data = br.readLine();
			data = data.substring(data.indexOf('=') + 1);

			return data;
		} catch (Exception e) {
			System.out.println(e);
			return "false";
		}
	}

}
