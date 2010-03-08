// KDPPreferencesView.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.io.*;

import com.symbian.tools.j2me.sei.preferences.KDPPreferences;
import com.symbian.tools.j2me.sei.preferences.SessionPreferences;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */
public class KDPPreferencesView extends PreferencesView implements ActionListener {


  //
  // Constants
  //

  /**
   * Directory under epoc32 home containing <platform>/<variant>/<product>
   */
  private static final String RELEASE_FOLDER = "release";


  /**
   * SEI-Slave win32 exe
   */
  private static final String SEI_SLAVE_APP = "DebugAgent.exe";

  /**
   * Name
   */
  private static final String NAME = UtilitiesResources.KDP_TAB_TITLE;

  /**
   * Indicates if to add Ping options to UI
   */
  private static final boolean ADD_PING_TO_UI = false;

  //
  // Members
  //

  /**
   * KDP preferences model
   */
  private KDPPreferences iModel;

  /**
   * Connection timeout ms
   */
  private JSlider iConnectionTimeout = new JSlider(0, 60);

  /**
   * Indicates unlimited timeout
   */
  private JCheckBox iUnlimitedTimeout = new JCheckBox(UtilitiesResources.UNLIMITED_TIMOUT);

  /**
   * Indicates enable attaching to VM
   */
  private JCheckBox iEnableAttaching = new JCheckBox(UtilitiesResources.ENABLE_ATTACHING_TO_VM);

  /**
   * Indicates defering VM execution
   */
  private JCheckBox iDeferVMExecution = new JCheckBox(UtilitiesResources.DEFER_VM_EXECUTION);

  /**
   * Indicates EKA2 emulator discovery method
   */
  private JCheckBox iEKA2EmulatorDiscovery = new JCheckBox(UtilitiesResources.EKA2_EMULATOR_DISCOVERY);

  /**
   * Indicates DebugAgent ping on startup
   */
  private JCheckBox iAgentPingOnLaunch = new JCheckBox("Ping IP on startup", false);

  /**
   * DebugAgent ping IP
   */
  private JTextField iPingIP = new JTextField();

  /**
   * DebugAgent ping port
   */
  private JTextField iPingPort = new JTextField();

  /**
   * Launch agent button
   */
  private JButton iLaunchAgent = new JButton(UtilitiesResources.AGENT_BUTTON);

  /**
   * Specify on port to enable attaching
   */
  private JTextField iKDPAttachPort = new JTextField();

  /**
   * Win32 SEI-Slave TCP host.
   */
  private JTextField iWin32SlaveTCPHost = new JTextField(TEN_SPACES);

  /**
   * Win32 SEI-Slave listening port.
   */
  private JTextField iWin32SlavePort = new JTextField(TEN_SPACES);

  /**
   * Win32 local listening port.
   */
  private JTextField iWin32LocalPort = new JTextField(TEN_SPACES);

  /**
   * Win32 outgoing connection radio button.
   */
  private JRadioButton iWin32Outgoing = new JRadioButton("Outgoing");

  /**
   * Win32 incoming connection radio button.
   */
  private JRadioButton iWin32Incoming = new JRadioButton("Incoming");

  /**
   * Target SEI-Slave TCP host.
   */
  private JTextField iTargetSlaveTCPHost = new JTextField(TEN_SPACES);

  /**
   * Target SEI-Slave listening port.
   */
  private JTextField iTargetSlavePort = new JTextField(TEN_SPACES);

  /**
   * Target local listening port.
   */
  private JTextField iTargetLocalPort = new JTextField(TEN_SPACES);

  /**
   * Target outgoing connection radio button.
   */
  private JRadioButton iTargetOutgoing = new JRadioButton("Outgoing");

  /**
   * Target incoming connection radio button.
   */
  private JRadioButton iTargetIncoming = new JRadioButton("Incoming");

  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aModel Connectivity preferences model
   */
  public KDPPreferencesView(KDPPreferences aModel) {
    setName(NAME);
    iModel = aModel;
    initLayout();
    initViewComponents();
  }

  /**
   * Init layout
   */
  private void initLayout() {
    this.setLayout(new BorderLayout());
    Box mainPanel = new Box(BoxLayout.Y_AXIS);

    //init connection timeout
    JPanel debugPanel = new JPanel(new GridLayout(2, 1, GAP, GAP));
    debugPanel.setBorder(new TitledBorder(UtilitiesResources.DEBUG_CONNECTION_TIP));
    iConnectionTimeout.addChangeListener(this);
    iConnectionTimeout.setPaintTicks(true);
    iConnectionTimeout.setToolTipText(UtilitiesResources.MAX_DEBUG_TIMEOUT_TIP);
    iConnectionTimeout.setMajorTickSpacing(5);
    iConnectionTimeout.setMinorTickSpacing(1);
    iConnectionTimeout.setPaintLabels(true);
    iConnectionTimeout.setSnapToTicks(true);
    iUnlimitedTimeout.setSelected(false);
    iUnlimitedTimeout.addActionListener(this);
    iEnableAttaching.setToolTipText(UtilitiesResources.ENABLE_ATTACHING);
    iDeferVMExecution.addActionListener(this);
    iDeferVMExecution.setToolTipText(UtilitiesResources.DEFER_VM_EXECUTION_TIP);
    debugPanel.add(iConnectionTimeout);
    debugPanel.add(iUnlimitedTimeout);
    mainPanel.add(debugPanel);

    //init attaching
    JPanel attachPanel = new JPanel(new BorderLayout());
    attachPanel.setBorder(new TitledBorder(UtilitiesResources.ATTACH_OPTIONS));
    iEnableAttaching.setSelected(iModel.isAttachingToVMEnabled());
    iEnableAttaching.addActionListener(this);
    attachPanel.add(iEnableAttaching, BorderLayout.NORTH);
    iKDPAttachPort.setText(Integer.toString(iModel.getAttachedVMKDPPort()));
    iKDPAttachPort.addKeyListener(this);
    iKDPAttachPort.setEnabled(iModel.isAttachingToVMEnabled());
    attachPanel.add(createComboPanel
            (iKDPAttachPort, new JLabel(TEN_SPACES), UtilitiesResources.KDP_ATTACH_PORT)
            , BorderLayout.CENTER);
    mainPanel.add(attachPanel);

    //init deferred launch
    JPanel vmPanel = new JPanel(new GridLayout(0, 1));
    vmPanel.setBorder(new TitledBorder(UtilitiesResources.VM_LAUNCH_OPTIONS));
    vmPanel.add(iDeferVMExecution);
    mainPanel.add(vmPanel);

    //Sessions panel
    JPanel agentBox = new JPanel();
    agentBox.setBorder(new TitledBorder(UtilitiesResources.AGENT_TAB));
    agentBox.setLayout(new BoxLayout(agentBox, BoxLayout.Y_AXIS));
    agentBox.add(getWin32SessionPanel());
    agentBox.add(getTargetSessionPanel());
    mainPanel.add(agentBox);

    this.add(mainPanel, BorderLayout.NORTH);
  }

  /**
   * Create Win32 session panel
   *
   * @return JPanel
   */
  private JPanel getWin32SessionPanel()
  {
  	JPanel sessionPanel = getSessionPanel("emulator",
  			iWin32Outgoing, iWin32SlaveTCPHost, iWin32SlavePort,
			iWin32Incoming, iWin32LocalPort);
    //Emulator (discovery) panel
    JPanel discoveryPanel = new JPanel(new GridLayout(1, 2));
    discoveryPanel.add(iEKA2EmulatorDiscovery);
    discoveryPanel.add(iLaunchAgent);
    sessionPanel.add(discoveryPanel);
    if(ADD_PING_TO_UI)
    {
      JPanel pingPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
      pingPanel.add(iAgentPingOnLaunch);
      pingPanel.add(new Label("IP"));
      pingPanel.add(iPingIP);
      pingPanel.add(new Label("Port"));
      pingPanel.add(iPingPort);
      sessionPanel.add(pingPanel);
    }
    iEKA2EmulatorDiscovery.addActionListener(this);
    iEKA2EmulatorDiscovery.setToolTipText(UtilitiesResources.EKA2_EMULATOR_DISCOVERY_TIP);
    iLaunchAgent.addActionListener(this);

  	return sessionPanel;
  }

  /**
   * Create Target session panel
   *
   * @return JPanel
   */
  private JPanel getTargetSessionPanel()
  {
  	return getSessionPanel("target",
  			iTargetOutgoing, iTargetSlaveTCPHost, iTargetSlavePort,
			iTargetIncoming, iTargetLocalPort);
  }


  /**
   * Create session panel
   *
   * @return JPanel
   */
  private JPanel getSessionPanel(String aSessionName,
  		JRadioButton aOut, JTextField aRemoteHost, JTextField aRemotePort,
		JRadioButton aIn, JTextField aLocalPort)
  {
  	JPanel sessionPanel = new JPanel(new GridLayout(0, 1));
    sessionPanel.setBorder(new TitledBorder(aSessionName));
    ButtonGroup group = new ButtonGroup();
  	aOut.setToolTipText("Connect to Agent");
  	aIn.setToolTipText("Wait for incoming Agent connection");
  	group.add(aOut);
  	group.add(aIn);
  	aOut.addActionListener(this);
	aIn.addActionListener(this);
	JPanel radioButtons = new JPanel(new GridLayout(1, 4));
	radioButtons.add(new JLabel("Connection"));
	radioButtons.add(aOut);
	radioButtons.add(aIn);
	radioButtons.add(new JLabel());
	sessionPanel.add(radioButtons);
	JPanel outgoing = new JPanel(new GridLayout(1, 4));
  	outgoing.add(new JLabel("Agent host"));
  	outgoing.add(aRemoteHost);
  	aRemoteHost.addKeyListener(this);
  	outgoing.add(new JLabel("Agent port"));
  	outgoing.add(aRemotePort);
  	aRemotePort.addKeyListener(this);
  	sessionPanel.add(outgoing);
  	JPanel incoming = new JPanel(new GridLayout(1, 4));
 	incoming.add(new JLabel("Local port"));
 	incoming.add(aLocalPort);
 	aLocalPort.addKeyListener(this);
 	incoming.add(new JLabel());
 	incoming.add(new JLabel());
 	sessionPanel.add(incoming);
  	return sessionPanel;
  }

  //
  // Operations
  //

  /**
   * Save the changes that were done in the view
   */
  public void updateModel() {
    iModel.setConnectionTimeout
        (iUnlimitedTimeout.isSelected() ?
         Integer.MAX_VALUE : iConnectionTimeout.getValue());
    iModel.setAttachingToVMEnabled(iEnableAttaching.isSelected());
    iModel.setVMLaunchingDeferred(iDeferVMExecution.isSelected());
    iModel.setAttachedVMKDPPort(Integer.parseInt(iKDPAttachPort.getText()));
    iModel.setEKA2EmulatorDiscovery(iEKA2EmulatorDiscovery.isSelected());
    SessionPreferences win32Prefs = iModel.getSessionPreferences("win32");
    win32Prefs.setHost(iWin32SlaveTCPHost.getText());
    win32Prefs.setPort(Integer.parseInt(iWin32SlavePort.getText()));
    win32Prefs.setLocalPort(Integer.parseInt(iWin32LocalPort.getText()));
    win32Prefs.setOutgoing(iWin32Outgoing.isSelected());
    SessionPreferences targetPrefs = iModel.getSessionPreferences("target");
    targetPrefs.setHost(iTargetSlaveTCPHost.getText());
    targetPrefs.setPort(Integer.parseInt(iTargetSlavePort.getText()));
    targetPrefs.setLocalPort(Integer.parseInt(iTargetLocalPort.getText()));
    targetPrefs.setOutgoing(iTargetOutgoing.isSelected());
  }

  /**
   * Revert the changes that were done in the view
   */
  public void initViewComponents() {
    iUnlimitedTimeout.setSelected(iModel.getConnectionTimeout() == Integer.MAX_VALUE);
    iConnectionTimeout.setEnabled(iModel.getConnectionTimeout() != Integer.MAX_VALUE);
    iConnectionTimeout.setValue((int)iModel.getConnectionTimeout());
    iDeferVMExecution.setSelected(iModel.isVMLaunchingDeferred());
    iEnableAttaching.setSelected(iModel.isAttachingToVMEnabled());
    iKDPAttachPort.setEnabled(iModel.isAttachingToVMEnabled());
    iEKA2EmulatorDiscovery.setSelected(iModel.isEKA2EmulatorDiscovery());
    iLaunchAgent.setEnabled(iEKA2EmulatorDiscovery.isSelected());
    SessionPreferences win32Prefs = iModel.getSessionPreferences("win32");
    iWin32SlaveTCPHost.setText(win32Prefs.getHost());
    iWin32SlavePort.setText("" + win32Prefs.getPort());
    iWin32LocalPort.setText("" + win32Prefs.getLocalPort());
    iWin32Outgoing.setSelected(win32Prefs.isOutgoing());
    iWin32Incoming.setSelected(!win32Prefs.isOutgoing());
    SessionPreferences targetPrefs = iModel.getSessionPreferences("target");
    iTargetSlaveTCPHost.setText(targetPrefs.getHost());
    iTargetSlavePort.setText("" + targetPrefs.getPort());
    iTargetLocalPort.setText("" + targetPrefs.getLocalPort());
    iTargetOutgoing.setSelected(targetPrefs.isOutgoing());
    iTargetIncoming.setSelected(!targetPrefs.isOutgoing());

    //Set default to same host
    iPingIP.setText(win32Prefs.getHost());
    iPingPort.setText("" + (win32Prefs.getPort() + 1));
  }

  /**
   * Launch the SEI-Slave win32 executable
   *
   * @param aPort slave port argument
   * @throws FileNotFoundException if could not find the exe file
   */
  private void launchDebugAgent() {
    final int agentPort = iModel.getSessionPreferences("win32").getPort();
    if(agentPort != Integer.parseInt(iWin32SlavePort.getText())){
      JOptionPane.showMessageDialog
          (this,
           UtilitiesResources.LAUNCH_AGENT_REMINDER_MESSAGE,
           UtilitiesResources.REMINDER_TITLE,
           JOptionPane.INFORMATION_MESSAGE);
    }
    else{
      try
      {
        //find exe
        File slaveDir = new File(SDKRuntimeProperties.EPOC_HOME +
                                 File.separator +
                                 RELEASE_FOLDER + File.separator +
                                 SDKRuntimeProperties.PLATFORM + File.separator +
                                 SDKRuntimeProperties.VARIANT);
        File app = new File(slaveDir, SEI_SLAVE_APP);
        if (!app.exists() || !app.isFile())
        {
          throw new FileNotFoundException
              ("cannot find debug agent win32 exe: " + app.getAbsolutePath());
        }

        //create process
        String args = " " + agentPort;
        if(iAgentPingOnLaunch.isSelected())
        {
          args += " " + iPingIP.getText() + " " + iPingPort.getText();

        }
        Runtime.getRuntime().exec(app.getAbsolutePath() + args, null,
                                  slaveDir);
      }
      catch (Exception e)
      {
        e.printStackTrace();
      }
    }
  }


  //
  // ActionListener implementation
  //

  /**
   * Handle action events
   *
   * @param aEvent action event
   */
  public void actionPerformed(ActionEvent aEvent) {
    iConnectionTimeout.setEnabled(!iUnlimitedTimeout.isSelected());
    iKDPAttachPort.setEnabled(iEnableAttaching.isSelected());
    super.fireStateChanged();
    if(aEvent.getSource() == iEKA2EmulatorDiscovery){
      iLaunchAgent.setEnabled(iEKA2EmulatorDiscovery.isSelected());
    }
    if(aEvent.getSource() == iLaunchAgent){
      launchDebugAgent();
    }
    else if(aEvent.getSource() == iDeferVMExecution && iDeferVMExecution.isSelected()){
      JOptionPane.showMessageDialog
          (this,
           UtilitiesResources.REMINDER_MESSAGE,
           UtilitiesResources.REMINDER_TITLE,
           JOptionPane.INFORMATION_MESSAGE);
    }
  }


}
