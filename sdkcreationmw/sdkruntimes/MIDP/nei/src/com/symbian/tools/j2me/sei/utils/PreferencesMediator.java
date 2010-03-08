// PreferencesMediator.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;


import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import com.symbian.tools.j2me.sei.preferences.UserPreferences;
import com.symbian.utils.Debug;
import com.symbian.utils.SystemExitHandler;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class PreferencesMediator
    implements ButtonsPanelListener, ChangeListener, WindowListener{
  //
  // Members
  //



  /**
   * Top frame
   */
  private JFrame iFrame;

  /**
   * Buttons panel
   */
  private ButtonsPanel iButtonsPanel;

  /**
   * User preferences
   */
  private UserPreferences iUserPreferences;

  /**
   * Top composite view
   */
  private PreferencesView iTopView;



  //
  // Life cycle
  //



  /**
   * Constructor
   *
   * @param aFrame view frame
   * @param aPrefs user preferences
   * @param aButtonsPanel buttons panel
   */
  public PreferencesMediator(UserPreferences aPrefs,
                             JFrame aFrame,
                             ButtonsPanel aButtonsPanel) {
    iUserPreferences = aPrefs;
    iFrame = aFrame;
    iButtonsPanel = aButtonsPanel;
    iButtonsPanel.setListener(this);
    iFrame.addWindowListener(this);
  }




  //
  // Operations
  //


  /**
   * Get the frame
   *
   * @return JFrame
   */
  public JFrame getFrame(){
    return iFrame;
  }

  /**
   * Get the Buttons panel
   *
   * @return Buttons panel
   */
  public ButtonsPanel getButtonsPanel(){
    return iButtonsPanel;
  }

  /**
   * Display the view window
   */
  public void showViewWindow(){
    iFrame.pack();
    iFrame.setVisible(true);
  }

  /**
   * Handle closing the application
   */
  private void handleCloseImpl(){
    iFrame.setVisible(false);
    iFrame.dispose();
    SystemExitHandler.invokeSystemExit(0);
  }


  /**
   * Set top view
   *
   * @param aView top view
   */
  void setTopView(PreferencesView aView){
      iTopView = aView;
  }



  //
  // ButtonsPanel implementation
  //



  /**
   * Handle Close button pressed
   */
  public void handleClosePressed(){
    handleCloseImpl();
  }

  /**
   * Handle Revert button pressed
   */
  public void handleRevertPressed(){
    iTopView.revertChanges();
    iButtonsPanel.setApplyEnabled(false);
    iButtonsPanel.setRevertEnabled(false);
  }

  /**
   * Handle Apply button pressed
   */
  public void handleApplyPressed(){
    try{
      iTopView.updateModel();
      iUserPreferences.save();
      iTopView.initViewComponents();
      iButtonsPanel.setApplyEnabled(false);
      iButtonsPanel.setRevertEnabled(false);
    }
    catch(Exception e){
      Debug.printStackTrace(this, e);
      JOptionPane.showMessageDialog
          (iFrame, e.getMessage(), UtilitiesResources.FAILED_TO_SAVE, JOptionPane.ERROR_MESSAGE);
    }
  }

  /**
   * Handle set-defaults button pressed
   */
  public void handleSetDefaultsPressed(){
    if(JOptionPane.showConfirmDialog
       (iFrame,
        UtilitiesResources.CONFIRM_LOOSING_SETTINGS,
        UtilitiesResources.CONFIRMATION,
        JOptionPane.YES_NO_CANCEL_OPTION) == JOptionPane.YES_OPTION){
      try {
        iUserPreferences.setDefaultValues();
        iTopView.revertChanges();
        iUserPreferences.save();
        iButtonsPanel.setApplyEnabled(false);
        iButtonsPanel.setRevertEnabled(false);
        iButtonsPanel.setDefaultsEnabled(false);
      }
      catch (Exception e) {
        Debug.printStackTrace(this, e);
        JOptionPane.showMessageDialog
            (iFrame, e.getMessage(), UtilitiesResources.FAILED_TO_SAVE,
             JOptionPane.ERROR_MESSAGE);
      }
    }
  }

  //
  // ChangeListener implementations
  //

  /**
   * Handle state changed event
   *
   * @param aEvent changed event
   */
  public void stateChanged(ChangeEvent aEvent){
    iButtonsPanel.setApplyEnabled(true);
    iButtonsPanel.setRevertEnabled(true);
    iButtonsPanel.setDefaultsEnabled(true);
  }


  //
  // WindowListener implementation
  //


  /**
   * Handle Window closing event
   *
   * @param aEvent Window event
   */
  public void windowClosing(WindowEvent e) {
    handleCloseImpl();
  }

  //
  // No-op
  //
  public void windowOpened(WindowEvent e) {
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

}
