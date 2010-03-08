/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.emulator.view;


import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

import com.symbian.tools.j2me.sei.emulator.*;
import com.symbian.tools.j2me.sei.emulator.event.*;
import com.symbian.utils.*;

/**
 * Default view panel giving the user notification about SEI progress.
 *
 * <p>Title: PREQ 425</p>
 * <p>Description: Java debugging</p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class EmulatorLauncherView extends JFrame
    implements EmulatorLauncherListener, WindowListener, ActionListener{


  public static void main(String[] args) {
    EmulatorLauncherView view = new EmulatorLauncherView(null);
    Utilities.centerWindow(view);
    view.pack();
    view.setVisible(true);

  }


  //
  // Constants
  //



  /**
   * Insets gap
   */
  private static final int GAP = 10;



  //
  // Members
  //



  /**
   * Map of launcher states -> view components
   */
  private Map iStates2Components = new HashMap();

  /**
   * Map of launcher states -> error messeges
   */
  private Map iStates2Errors = new HashMap();

  /**
   * Status bar
   */
  private JLabel iStatusBar = new JLabel(EmulatorResources.sOnProgress);

  /**
   * Abort button
   */
  private JButton iAbort = new JButton(EmulatorResources.sAbort);

  /**
   * Hide button
   */
  private JButton iHide = new JButton(EmulatorResources.sHide);

  /**
   * Layout constraints
   */
  private GridBagConstraints iGbc = new GridBagConstraints();

  /**
   * Layout manager
   */
  private GridBagLayout iLayout = new GridBagLayout();

  /**
   * Add another MIDlet to the running VM
   */
  private JButton iAddMIDlet = new JButton(EmulatorResources.sAdd__);

  /**
   * Suites MIDlets
   */
  private JComboBox iMIDlets;

  /**
   * Launcher associated with this view
   */
  private EmulatorLauncher iLauncher;




  //
  // Life cycle
  //



  /**
   * Deafault constructor
   *
   * @param aLauncher launcher
   */
  public EmulatorLauncherView(EmulatorLauncher aLauncher){
    super(EmulatorResources.sViewTitle);
    addWindowListener(this);
    setResizable(false);
    iLauncher = aLauncher;
    initLayout();

    //states with no visible indicator
    iStates2Errors.put(new Integer(EmulatorLauncher.CREATE_SEI_COMMAND),
                          EmulatorResources.sParseErrorMessage);
    iStates2Errors.put(new Integer(EmulatorLauncher.ANSWER_QUERY),
                                      EmulatorResources.sQueryErrorMessage);
  }

  /**
   * Init components layout
   */
  private void initLayout(){
    //layout manager
    iGbc.gridx = GridBagConstraints.REMAINDER;
    iGbc.anchor = GridBagConstraints.SOUTH;
    iGbc.fill = GridBagConstraints.HORIZONTAL;
    getContentPane().setLayout(iLayout);

    //indicators
    addStateIndicator(EmulatorLauncher.READ_USER_PREFERENCES, EmulatorResources.sReadPrefs, EmulatorResources.sReadPrefsFailed);
    addStateIndicator(EmulatorLauncher.CREATE_VM_LAUNCH_ARGS, EmulatorResources.sCreateVMArgs, EmulatorResources.sCreateVMCommandErrorMessage);
    addStateIndicator(EmulatorLauncher.INIT_JAD, EmulatorResources.sCreateJad, EmulatorResources.sInitJadErrorMessage);
    addStateIndicator(EmulatorLauncher.RESOLVE_APPLICATION, EmulatorResources.sResolveApp, EmulatorResources.sResolveAppFailed);
    addStateIndicator(EmulatorLauncher.INIT_JAR, EmulatorResources.sCreateJar, EmulatorResources.sInitJarErrorMessage);
    addStateIndicator(EmulatorLauncher.ADD_JAR_SIGNATURE, EmulatorResources.sSignJad, EmulatorResources.sSignJadFailed);
    addStateIndicator(EmulatorLauncher.INIT_SESSION, EmulatorResources.sInitSession, EmulatorResources.sInitSessionFailed);
    addStateIndicator(EmulatorLauncher.ASSIGN_SESSION_PORTS, EmulatorResources.sInitPorts, EmulatorResources.sInitPortsFailed);
    addStateIndicator(EmulatorLauncher.SESSION_LAUNCH, EmulatorResources.sLaunchSessions, EmulatorResources.sExecErrorMessage);
    addStateIndicator(EmulatorLauncher.SESSION_DEBUG, EmulatorResources.sDebugSession, EmulatorResources.sDebugErrorMessage);
    addStateIndicator(EmulatorLauncher.FINAL, EmulatorResources.sDone, EmulatorResources.sUnknownErrorMessage);

    //buttons
    JPanel buttonsPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
    buttonsPanel.add(iHide);
    buttonsPanel.add(iAddMIDlet);
    buttonsPanel.add(iAbort);
    iAddMIDlet.setEnabled(false);

    iLayout.setConstraints(buttonsPanel, iGbc);
    getContentPane().add(buttonsPanel);
    iHide.addActionListener(this);
    iAbort.addActionListener(this);
    iAddMIDlet.addActionListener(this);

    //status bar
    iStatusBar.setBorder(BorderFactory.createLoweredBevelBorder());
    iLayout.setConstraints(iStatusBar, iGbc);
    getContentPane().add(iStatusBar);
  }





  //
  // Operations
  //




  /**
   * Get frame insets
   *
   * @return frame insets
   */
  public Insets getInsets(){
    Insets tmp = super.getInsets();
    return new Insets(tmp.top,
                      tmp.right + GAP,
                      tmp.bottom + GAP,
                      tmp.left + GAP);
  }

  /**
   * Add state indicator
   *
   * @param aState launcher state
   * @param aDescription indicator description
   * @param aErrorMessage error message if this phase fails
   */
  private void addStateIndicator(int aState,
                                 String aDescription,
                                 String aErrorMessage){
    StateJLabel tmp = new StateJLabel(aDescription);
    iLayout.setConstraints(tmp, iGbc);
    getContentPane().add(tmp);
    Integer key = new Integer(aState);
    iStates2Components.put(key, tmp);
    iStates2Errors.put(key, aErrorMessage);
  }



  //
  // EmulatorLauncherListener implementation
  //

  /**
   * Handle emulator launcher event
   *
   * @param aEvent emulator launcher event
   */
  public void handleEmulatorLauncherEvent(EmulatorLauncherEvent aEvent){
    StateView view =
        ((StateView)iStates2Components.get(new Integer(aEvent.getState())));
    switch(aEvent.getType()){
      case (EmulatorLauncherEvent.UNHANDLED_EXCEPTION): {
        if(view != null){
          view.setError();
        }
        iStatusBar.setText(EmulatorResources.sError);
        handleException(aEvent.getException(), aEvent.getState());
        break;
      }
      case(EmulatorLauncherEvent.SHOW_VIEW):{
        //The launcher dispatches the event after checking the user preferences.
        if(view != null){
          view.setActive(true);
          iStatusBar.setText(view.getText());
        }
        pack(); //pack() will also start the AWT thread.
        setVisible(true);
        break;
      }
      case(EmulatorLauncherEvent.EXIT_STATE):{
        if(view != null){
          view.setActive(false);
        }
        break;
      }
      case(EmulatorLauncherEvent.ENTER_STATE):{
        if(view != null){
          view.setActive(true);
          iStatusBar.setText(view.getText());
        }
        if(aEvent.getState() == EmulatorLauncher.FINAL
           && iLauncher.getMIDlets().size() > 1){
          iAddMIDlet.setEnabled(true);
        }
        break;
      }
      case(EmulatorLauncherEvent.SKIPPED_STATE):{
        if(view != null){
          view.setSkipped();
        }
        break;
      }
    }
  }


  /**
   * The emulator caught an unhandled exception
   *
   * @param aEx unhandled exception
   * @param aState state of the emulator when the exception was thrown
   */
  private void handleException(Throwable aEx, int aState){
    //set icon
    iStatusBar.setText("Error");
    iStatusBar.setIcon(EmulatorResources.sXImage);

    String errorMessage = (String)iStates2Errors.get(new Integer(aState));
    StringBuffer dialogMessage = errorMessage == null?
        new StringBuffer(EmulatorResources.sUnknownErrorMessage):
        new StringBuffer(errorMessage);

    //produce a stack trace
    dialogMessage.append("\n");
    dialogMessage.append(EmulatorResources.sCaughtException);
    dialogMessage.append("\n");
    ByteArrayOutputStream bos = new ByteArrayOutputStream();
    PrintStream ps = new PrintStream(bos);
    aEx.printStackTrace(ps);
    ps.flush();
    dialogMessage.append(new String(bos.toByteArray()));

    //show the dialog
    JOptionPane.showMessageDialog(this,
                                  dialogMessage.toString(),
                                  EmulatorResources.sErrorDialogTitle,
                                  JOptionPane.ERROR_MESSAGE);

  }





  /**
   * Dispose UI
   */
  public void disposeUI(){
    setVisible(false);
    super.dispose();
  }

  //
  // ActionListener implementation
  //



  /**
   * Handle buttons events
   *
   * @param aEvent buttons events
   */
  public void actionPerformed(ActionEvent aEvent){
    if(aEvent.getSource() == iAbort){
      disposeUI();
      SystemExitHandler.invokeSystemExit(0);
    }
    else if(aEvent.getSource() == iAddMIDlet){
      try{
        //pop up selection dialog
        Object[] midlets = iLauncher.getMIDlets().values().toArray();
        Object selected = JOptionPane.showInputDialog
            (this,
             EmulatorResources.sAddMIDletLabel,
             EmulatorResources.sAddMIDletTitle,
             JOptionPane.INFORMATION_MESSAGE,
             null,
             midlets,
             iLauncher.getMIDletInfo().getMIDletName());

        if(selected != null){
          iLauncher.addMIDlet((String)selected);
        }

      }
      catch(Throwable t){
        handleException(t, EmulatorLauncher.FINAL);
      }
    }
    else if(aEvent.getSource() == iHide){
      disposeUI();
    }
  }



  //
  // WindowListener implementation
  //



  /**
   * Handle user closing the window
   *
   * @param e event
   */
  public void windowClosing(WindowEvent e) {
    disposeUI();
    SystemExitHandler.invokeSystemExit(0);
  }

  public void windowClosed(WindowEvent e) {}
  public void windowOpened(WindowEvent e) {}
  public void windowIconified(WindowEvent e) {}
  public void windowDeiconified(WindowEvent e) {}
  public void windowActivated(WindowEvent e) {}
  public void windowDeactivated(WindowEvent e) {}


}
