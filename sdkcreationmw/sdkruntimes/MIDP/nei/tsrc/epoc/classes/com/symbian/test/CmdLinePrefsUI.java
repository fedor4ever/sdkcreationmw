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
package com.symbian.test;

import java.awt.*;
import java.awt.event.*;
import com.symbian.tools.j2me.sei.preferences.*;

public class CmdLinePrefsUI
    extends Frame
    implements WindowListener, ActionListener {

  //
  // Members
  //

  /**
   * User preferences
   */
  private UserPreferences iPrefs;

  /**
   * Bootstrap classpath
   */
  private TextField iBootStrapClasspath = new TextField();

  /**
   * KDP log level.
   */
  private Choice iKDPLogLevel = new Choice();

  /**
   * Enable KDP extra logs
   */
  private Checkbox iKDPExtraLogs = new Checkbox("Enable KDP extra logs");

  /**
   * Enable file logging indicator
   */
  private Checkbox iEnableLogFile = new Checkbox("Enable file logging");

  /**
   * Mute all logs
   */
  private Checkbox iMuteAllLogs = new Checkbox("Mute all logging");

  Button iSave = new Button("Save");

  Button iReload = new Button("Reload");

  Button iDefaults = new Button("Revert to defaults");

  //
  // Life cycle
  //

  public static void main(String[] args) {
    try {
      new CmdLinePrefsUI();
    }
    catch (Exception e) {
      e.printStackTrace();
    }
  }

  public CmdLinePrefsUI() throws Exception {
    super("SEI Advanced preferences");
    initLayout();
    initUI();
    pack();
    setVisible(true);
  }

  private void initLayout() {
    setLayout(new GridLayout(0, 1));
    add(new Label("Bootstrapclasses file name (on ROM Z: drive)"));
    add(iBootStrapClasspath);
    add(iEnableLogFile);
    add(iMuteAllLogs);
    add(new Label("KDP log level"));
    add(iKDPLogLevel);
    add(iKDPExtraLogs);
    iSave.addActionListener(this);
    iReload.addActionListener(this);
    iDefaults.addActionListener(this);
    Panel p = new Panel(new FlowLayout());
    p.add(iSave);
    p.add(iReload);
    p.add(iDefaults);
    add(p);
    addWindowListener(this);
    for (int i = 0; i <= 9; i++) {
      iKDPLogLevel.add("" + i);
    }
  }

  private void initUI() throws Exception {
    //create user preferences
    System.out.println("loading prefs");
    iPrefs = UserPreferences.load();
    CommandLinePreferences cmp = iPrefs.getCommandLinePreferences();
    iBootStrapClasspath.setText(cmp.getBootStrapClasspath());
    iEnableLogFile.setState(cmp.isLogFileEnabled());
    iMuteAllLogs.setState(cmp.isLoggingEnabled());
    iKDPLogLevel.select(cmp.getKDPLogLevel());
    iKDPExtraLogs.setState(cmp.isKDPExtraLogs());
  }

  //
  // WindowListener implementation
  //

  public void windowOpened(WindowEvent e) {
  }

  public void windowClosing(WindowEvent e) {
    setVisible(false);
    System.exit(0);
  }

  public void windowClosed(WindowEvent e) {
  }

  public void windowIconified(WindowEvent e) {
  }

  public void windowDeiconified(WindowEvent e) {
  }

  public void windowActivated(WindowEvent e) {
  }

  public void windowDeactivated(WindowEvent e) {
  }

  //
  // ActionListener implementation
  //

  public void actionPerformed(ActionEvent e) {
    try {
      if (e.getSource() == iSave) {
        CommandLinePreferences cmp = iPrefs.getCommandLinePreferences();
        final String boot = iBootStrapClasspath.getText().trim().equals("")?
            null : iBootStrapClasspath.getText().trim();
        cmp.setBootStrapClasspath(boot);
        System.out.println("boot=" + boot);
        cmp.setLogFileEnabled(iEnableLogFile.getState());
        System.out.println("iEnableLogFile.getState(): " + iEnableLogFile.getState());
        cmp.setLoggingEnabled(iMuteAllLogs.getState());
        System.out.println("iMuteAllLogs.getState(): " + iMuteAllLogs.getState());
        cmp.setKDPLogLevel(iKDPLogLevel.getSelectedIndex());
        System.out.println("iKDPLogLevel.getSelectedIndex(): " + iKDPLogLevel.getSelectedIndex());
        cmp.setKDPExtraLogs(iKDPExtraLogs.getState());
        System.out.println("iKDPExtraLogs.getState(): " + iKDPExtraLogs.getState());
        iPrefs.save();
        System.out.println("Preferences saved.");
      }
      else if (e.getSource() == iReload) {
        initUI();
      }
      else if (e.getSource() == iDefaults) {
        iPrefs.setDefaultValues();
        iPrefs.save();
        System.out.println("Preferences reverted to default values.");
      }
      initUI();
    }
    catch (Exception ex) {
      ex.printStackTrace();
    }

  }
}
