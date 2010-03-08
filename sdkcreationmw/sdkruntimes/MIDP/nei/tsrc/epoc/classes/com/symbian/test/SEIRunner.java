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
import java.io.*;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

public class SEIRunner extends Frame implements WindowListener, ActionListener,
    ItemListener
{

  //
  // Members
  //
  private Choice iCHSession = new Choice();
  private TextField iTFSEIPath = new TextField(SDKRuntimeProperties.SDK_HOME);
  private TextField iTFSuiteClassPath = new TextField(
      "C:\\Documents and Settings\\RoyH\\jbproject\\apduconnection\\classes");
  private TextField iTFMIDlet = new TextField("MIDlet1");
  private TextField iTFAttachPort = new TextField("6655");
  private Checkbox iPreverify = new Checkbox("Preverify suite classes", true);
  private Checkbox iDebug = new Checkbox("Debug");
  private Button iLaunch = new Button("Run");
  private Button iUtils = new Button("Utils.exe");
  private Button iAdvancePrefs = new Button("Advanced prefs");

  public static void main(String[] args)
  {
    new SEIRunner();
  }

  public SEIRunner()
  {
    super("SEI Runner");
    initSessionChoice();
    initLayout();
    setItemsStates();
    addEventsListeners();
    pack();
    setVisible(true);
  }

  private void initSessionChoice()
  {
    iCHSession.add("naive");
    iCHSession.add("win32");
    iCHSession.add("target");
    iCHSession.select("target");
  }

  private void initLayout()
  {
    Panel main = new Panel(new GridLayout(0, 1));
    main.add(new Label("SEI HOME"));
    main.add(iTFSEIPath);
    main.add(new Label("SEI session"));
    main.add(iCHSession);
    main.add(new Label("Suite classpath"));
    main.add(iTFSuiteClassPath);
    main.add(iPreverify);
    main.add(new Label("MIDlet class"));
    main.add(iTFMIDlet);
    main.add(iDebug);
    main.add(new Label("IDE attach port"));
    main.add(iTFAttachPort);
    Panel buttons = new Panel(new FlowLayout());
    buttons.add(iLaunch);
    buttons.add(iUtils);
    buttons.add(iAdvancePrefs);
    main.add(buttons);

    this.setLayout(new BorderLayout());
    this.add(main);
  }

  private void addEventsListeners()
  {
    iLaunch.addActionListener(this);
    iUtils.addActionListener(this);
    iAdvancePrefs.addActionListener(this);
    iDebug.addItemListener(this);
    this.addWindowListener(this);

  }

  private void setItemsStates()
  {
    iTFAttachPort.setEnabled(iDebug.getState());
  }

  public void preverify() throws IOException
  {
    String product = "hmidp91";
    String classFiles = getSuiteClassFiles(new File(iTFSuiteClassPath.getText()));
    String perverifierCmd = iTFSEIPath.getText() + File.separator + "bin" +
        File.separator + "preverify.exe" + " -classpath " + iTFSEIPath.getText() +
        File.separator + "lib" + File.separator + product + ".jar" +
        " -d . " + iTFSuiteClassPath.getText() + classFiles;
    executeCommandLine(perverifierCmd, new File(iTFSuiteClassPath.getText()));
  }

  private String getSuiteClassFiles(File aRoot)
  {
    String[] files = aRoot.list();
    String result = "";
    for (int i = 0; i < files.length; i++)
    {
      if (files[i].endsWith(".class"))
      {
        result += files[i].substring(0, files[i].indexOf(".")) + " ";
      }
    }
    return result;
  }

  private String getDeviceArg()
  {
    return " -Xdevice:" + iCHSession.getSelectedItem() + " ";
  }

  private String getEmulatorExe()
  {
    //%SEI_SDK%\bin\emulator.exe
    return " " + iTFSEIPath.getText() + File.separator + "bin" + File.separator +
        "emulator.exe";
  }


  private void launchAdvancedPrefs() throws Exception
  {
    new CmdLinePrefsUI();
  }

  private void launchUtilsExe() throws IOException
  {
    //%SEI_SDK%\bin\\utils.exe
    executeCommandLine
        (" " + iTFSEIPath.getText() + File.separator + "bin" + File.separator +
         "utils.exe",
         new File(iTFSEIPath.getText()));
  }

  private String getClassPathArg()
  {
    return " -classpath \"" + iTFSuiteClassPath.getText() + "\" ";
  }

  private String getDebugArgs()
  {
    //-Xdebug -Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=3322
    return " -Xdebug -Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=" +
        iTFAttachPort.getText() + " ";
  }

  public void debug() throws IOException
  {
    executeCommandLine(getEmulatorExe() + getDebugArgs() + getClassPathArg() +
                       getDeviceArg() + iTFMIDlet.getText(),
                       new File(iTFSEIPath.getText()));
  }

  public void run() throws IOException
  {
    executeCommandLine(getEmulatorExe() + getClassPathArg() +
                       getDeviceArg() + iTFMIDlet.getText(),
                       new File(iTFSEIPath.getText()));
  }

  public void launch() throws IOException
  {
    if (iPreverify.getState())
    {
      preverify();
    }
    if (iDebug.getState())
    {
      debug();
    }
    else
    {
      run();
    }
  }

  public void executeCommandLine(String aCmd, File aWorkingDir) throws
      IOException
  {
    System.out.println("* executing '" + aCmd + "' from " +
                       aWorkingDir.getAbsolutePath());
    Runtime.getRuntime().exec(aCmd, null, aWorkingDir);
  }

  public Dimension getMinimumSize()
  {
    return new Dimension(400, 400);
  }

  public Dimension getPreferredSize()
  {
    return getMinimumSize();
  }

  /**
   * windowActivated
   *
   * @param e WindowEvent
   */
  public void windowActivated(WindowEvent e)
  {
  }

  /**
   * windowClosed
   *
   * @param e WindowEvent
   */
  public void windowClosed(WindowEvent e)
  {
  }

  /**
   * windowClosing
   *
   * @param e WindowEvent
   */
  public void windowClosing(WindowEvent e)
  {
    System.exit(0);
  }

  /**
   * windowDeactivated
   *
   * @param e WindowEvent
   */
  public void windowDeactivated(WindowEvent e)
  {
  }

  /**
   * windowDeiconified
   *
   * @param e WindowEvent
   */
  public void windowDeiconified(WindowEvent e)
  {
  }

  /**
   * windowIconified
   *
   * @param e WindowEvent
   */
  public void windowIconified(WindowEvent e)
  {
  }

  /**
   * windowOpened
   *
   * @param e WindowEvent
   */
  public void windowOpened(WindowEvent e)
  {
  }

  /**
   * actionPerformed
   *
   * @param e ActionEvent
   */
  public void actionPerformed(ActionEvent e)
  {
    try
    {
      if (e.getSource() == iLaunch)
      {
        launch();
      }
      else if (e.getSource() == iUtils)
      {
        launchUtilsExe();
      }
      else if (e.getSource() == iAdvancePrefs)
      {
        launchAdvancedPrefs();
      }

   }
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
  }

  /**
   * itemStateChanged
   *
   * @param e ItemEvent
   */
  public void itemStateChanged(ItemEvent e)
  {
    setItemsStates();
  }

}
