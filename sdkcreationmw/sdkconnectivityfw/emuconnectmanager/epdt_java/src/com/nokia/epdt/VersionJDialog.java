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

package com.nokia.epdt;

import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.ResourceBundle;

import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import com.nokia.epdt.core.Constants;

/**
 * VersionJFrame
 */
public class VersionJDialog extends JDialog implements Constants {
	private ResourceBundle bundle;

	private final WindowAdapter versionJDialogWindowAdapter = new WindowAdapter() {
		public void windowClosing(WindowEvent e) {
			System.exit(0);
		}
	};

	/**
	 * Creates a new instance of VersionJDialog
	 */
	public VersionJDialog(ResourceBundle bundle) {
		super();
		this.bundle = bundle;
		this.init();
	}

	/**
	 * Initializes a new instance of VersionJDialog
	 */
	private void init() {
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		this.setSize(d.width / 2, d.height / 4);
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		this.addWindowListener(this.versionJDialogWindowAdapter);
		this.setTitle(bundle.getString(Constants.VERSION_JDIALOG_TITLE));
		// Tabbed pane
		JTabbedPane jTabbedPane = new JTabbedPane(JTabbedPane.BOTTOM);
		this.getContentPane().add(jTabbedPane);
		// Requirements
		jTabbedPane.add(bundle.getString(MESSAGE_TAB_TITLE), new MessageJPanel(
				bundle));
		// Detail       
		jTabbedPane.add(bundle.getString(DETAILS_TAB_TITLE), new DetailJPanel(
				bundle));
	}

	class MessageJPanel extends JPanel implements Constants {
		private ResourceBundle bundle;

		/**
		 * Creates a new instance of MessageJPanel
		 */
		public MessageJPanel(ResourceBundle bundle) {
			super();
			this.init();
			this.bundle = bundle;
		}

		/**
		 * Initializes a new instance of MessageJPanel
		 */
		private void init() {
			this.setLayout(new FlowLayout());
		}

		public void paint(Graphics g) {
			super.paint(g);
			g.drawString(bundle.getString(MESSAGE_JPANEL_SENTENCE_0), 30, 30);
			g.drawString(bundle.getString(MESSAGE_JPANEL_SENTENCE_1), 30, 60);
		}
	}

	class DetailJPanel extends JPanel implements Constants {
		private ResourceBundle bundle;

		/**
		 * Creates a new instance of DetailJPanel
		 */
		public DetailJPanel(ResourceBundle bundle) {
			super();
			this.init();
			this.bundle = bundle;
		}

		/**
		 * Initializes a new instance of DetailJPanel
		 */
		private void init() {
			this.setLayout(new FlowLayout());
		}

		public void paint(Graphics g) {
			super.paint(g);
			g.drawString(bundle.getString(DETAILS_JPANEL_JAVA_VERSION), 30, 30);
			g.drawString(System.getProperty("java.version"), 180, 30);
			g.drawString(bundle.getString(DETAILS_JPANEL_JAVA_RUNTIME_VERSION),
					30, 60);
			g.drawString(System.getProperty("java.runtime.version"), 180, 60);
			g.drawString(bundle.getString(DETAILS_JPANEL_JAVA_VM_VERSION), 30,
					90);
			g.drawString(System.getProperty("java.vm.version"), 180, 90);
		}
	}
}
