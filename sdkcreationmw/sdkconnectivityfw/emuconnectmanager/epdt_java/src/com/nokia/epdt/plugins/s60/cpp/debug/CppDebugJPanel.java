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

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import com.nokia.epdt.core.config.epocini.EpocIniConstants;
import com.nokia.util.gui.GUI;

/**
 *
 * CppDebugJPanel
 */
class CppDebugJPanel extends JPanel implements ActionListener, Observer, EpocIniConstants {
    private static final int GAP = 6;
    private CppDebugPlugin plugin;
    private JCheckBox logToDebuggerCheckBox = new JCheckBox();
    private JCheckBox jitCheckBox = new JCheckBox();
    private JCheckBox logToFileCheckBox = new JCheckBox();
    private JCheckBox panicsCheckBox = new JCheckBox();
    
    private static String RESTART_NOTE =
    	"The emulator must be restarted in order to activate the changes.";

    /**
     * Constructing
     */
     public CppDebugJPanel(CppDebugPlugin plugin) {
         super(new BorderLayout());
         this.plugin = plugin;
         init();
         settings();
         addListeners();
     }

     /**
      * Initialize GUI
      */
     private void init() {
    	 setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));
         JPanel panel = new JPanel(new GridLayout(0,1));
         logToDebuggerCheckBox.setText("Log to debugger");
         jitCheckBox.setText("Just in time");
         logToFileCheckBox.setText(/*"Log to file"*/"Enable EPOCWIND.OUT logging");
         panicsCheckBox.setText("Extended panic code file");
         panel.setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));
         panel.add(logToDebuggerCheckBox);
         panel.add(jitCheckBox);
         panel.add(logToFileCheckBox);
         panel.add(panicsCheckBox);
         add(panel, BorderLayout.NORTH);
         // Warning panel
         JPanel warning = new JPanel(new BorderLayout(GAP,GAP));
         warning.add(GUI.createTextComponent(
        		 RESTART_NOTE),
         		BorderLayout.CENTER);
         ImageIcon icon = new ImageIcon(ClassLoader.getSystemResource("resources/info.gif"));
         if (icon != null) {
             JLabel warningIcon = new JLabel(icon);
             warningIcon.setVerticalAlignment(JLabel.TOP);
             warning.add(warningIcon, BorderLayout.WEST);
         }
         add(warning, BorderLayout.SOUTH);
         

         
     }

     /**
      * Adding listeners
      */
     private void addListeners() {
         jitCheckBox.addActionListener(this);
         logToDebuggerCheckBox.addActionListener(this);
         logToFileCheckBox.addActionListener(this);
         panicsCheckBox.addActionListener(this);
     }

     /**
      * Setting GUI component values to same as models
      */
     private void settings() {
         logToDebuggerCheckBox.setSelected("1".equals(plugin.getProperty(LOG_TO_DEBUGGER)));
         logToFileCheckBox.setSelected("1".equals(plugin.getProperty(LOG_TO_FILE)));
         jitCheckBox.setSelected("query".equals(plugin.getProperty(JUST_IN_TIME)));
         panicsCheckBox.setSelected("true".equals(plugin.getProperty(EXTENDED_PANIC_CODE_FILE)));
     }

     /**
      * Update call
      */
     public void update(Observable observable, Object o) {
         settings();
     }

     /**
      * Listening action events
      */
     public void actionPerformed(ActionEvent e) {
         Object o = e.getSource();
         String value;
         if(o == jitCheckBox) {
             value = jitCheckBox.isSelected()? "query":"none";
             plugin.setProperty(JUST_IN_TIME, value);
         } else if(o == logToDebuggerCheckBox) {
             value = logToDebuggerCheckBox.isSelected()? "1":"0";
             plugin.setProperty(LOG_TO_DEBUGGER, value);
         } else if(o == logToFileCheckBox) {
             value = logToFileCheckBox.isSelected()? "1":"0";
             plugin.setProperty(LOG_TO_FILE, value);
         } else if(o == panicsCheckBox) {
             value = panicsCheckBox.isSelected()? "true":"false";
             plugin.setProperty(EXTENDED_PANIC_CODE_FILE, value);
         }
     }
}
