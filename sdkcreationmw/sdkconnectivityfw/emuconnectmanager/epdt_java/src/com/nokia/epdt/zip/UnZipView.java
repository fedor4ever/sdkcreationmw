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




package com.nokia.epdt.zip;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.Toolkit;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;
import javax.swing.UIManager;
import javax.swing.WindowConstants;

/**
 * UnzipView
 *
 * A view for Unzip
 *
 * @version 1.0
 */
public class UnZipView extends JFrame {
    private final JProgressBar progressBar;
    private final JLabel       label;
	
    public UnZipView() 
    {
		super();
		setTitle("Changing language...");
		setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		setResizable(false);
		this.setDefaultLookAndFeelDecorated(true);
        try {
            UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
            ImageIcon image = new ImageIcon(ClassLoader.getSystemResource("resources/icon16x16.gif"));
        	setIconImage(image.getImage());
        } catch (Exception e) {
        	e.printStackTrace();
        }
		Container pane = this.getContentPane();
		pane.setLayout(new BoxLayout(pane, BoxLayout.PAGE_AXIS));
		pane.add(Box.createRigidArea(new Dimension(10, 10)));
		label = new JLabel(" ");
		label.setPreferredSize(new Dimension(500, 20));
		pane.add(label);
		pane.add(Box.createRigidArea(new Dimension(10, 10)));
		progressBar = new JProgressBar();
		progressBar.setStringPainted(true);
		progressBar.setMaximum(100);
		pane.add(progressBar);
		pack();
	
		Toolkit tk = Toolkit.getDefaultToolkit ();
		Dimension screen = tk.getScreenSize ();
		// And place it in center of screen.
		int lx = (int) (screen.getWidth () - getWidth()) / 2;
		int ly = (int) (screen.getHeight () - getHeight()) / 2;
		setLocation (lx,ly);
    }
	
    public void setProgressPercent(int value) {
    	progressBar.setValue(value);
    }
	
    public void setLabelText(String text) {
    	label.setText(text);
    }
	
    public void showInformationMessage(String message) {
    	JOptionPane.showMessageDialog(this, message, "Information", JOptionPane.INFORMATION_MESSAGE);
    }
	
    public void showErrorMessage(String message) {
    	JOptionPane.showMessageDialog(this, message, "Error", JOptionPane.ERROR_MESSAGE);
    }
}
