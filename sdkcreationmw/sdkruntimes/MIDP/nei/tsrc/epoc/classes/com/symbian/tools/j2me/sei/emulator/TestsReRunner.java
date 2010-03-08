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

package com.symbian.tools.j2me.sei.emulator;

import java.lang.reflect.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import junit.framework.*;
import java.util.*;
import com.symbian.tools.j2me.sei.SDKTestsProperties;

public class TestsReRunner extends JFrame implements ActionListener{
  private JComboBox iSessions;
  private JList iTestMethods;
  private JButton iRun = new JButton("run test");

  public static void main(String[] aArgs){
    new TestsReRunner();
  }

  public TestsReRunner() {
    initLayout();
    setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    iRun.addActionListener(this);
    pack();
    show();
  }

  private void initLayout() {
    getContentPane().setLayout(new GridLayout(0, 1, 10, 10));
    getContentPane().add(new JLabel("select session type"));
    Vector sessions = new Vector();
    sessions.add("win32");
    sessions.add("target");
    sessions.add("naive");
    iSessions = new JComboBox(sessions);
    iSessions.setSelectedIndex(0);
    getContentPane().add(iSessions);
    getContentPane().add(new JLabel("enter test name"));
    populateTests();
    getContentPane().add(new JScrollPane(iTestMethods));
    getContentPane().add(iRun);
  }

  private void populateTests(){
    Vector testMethods = new Vector();
    try {
      Class c = Class.forName("com.symbian.tools.j2me.sei.emulator.TestLauncher_BASE");
      Method[] allMethods = c.getMethods();
      for(int i = 0; i < allMethods.length; i++){
        if(allMethods[i].getName().startsWith("test_")){
          System.out.println("found: " + allMethods[i].getName());
          testMethods.add(allMethods[i].getName());
        }
      }
    }
    catch (Exception ex) {
      System.out.println("Could not load test methods name");
      ex.printStackTrace();
    }
    iTestMethods = new JList(testMethods);
    iTestMethods.setSelectedIndex(0);
  }


  public void actionPerformed(ActionEvent e){
    String session = (String)iSessions.getSelectedItem();
    String test = (String)iTestMethods.getSelectedValue();
    try {
      String sessionHost = "sdk.tests.host." + session;
      System.setProperty("sdk.tests.session", session);
      System.setProperty("sdk.tests.name", test);
      System.setProperty(sessionHost, SDKTestsProperties.RERUN_HOST);
      System.out.println("sdk.tests.name set to " + SDKTestsProperties.TEST_NAME);
      System.out.println(sessionHost + " set to " + System.getProperty(sessionHost));
      junit.swingui.TestRunner.main(new String[]{"com.symbian.tools.j2me.sei.emulator.InvestigatedTests"});
    }
    catch (Exception ex) {
      ex.printStackTrace();
      System.out.println("Please check your session, test name and enviroment settings");
    }
  }

  public Dimension getPreferredSize(){
    return getMinimumSize();
  }

  public Dimension getMinimumSize(){
    return new Dimension(400, 200);
  }

}
