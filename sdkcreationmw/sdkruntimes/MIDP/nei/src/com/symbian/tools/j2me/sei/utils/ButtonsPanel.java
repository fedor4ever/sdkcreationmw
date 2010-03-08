// ButtonsPanel.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.ResourceBundle;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class ButtonsPanel extends JPanel implements ActionListener{

  //
  // Members
  //

  /**
   * Close button
   */
  private JButton iClose = new JButton(UtilitiesResources.CLOSE);

  /**
   * Revert button
   */
  private JButton iRevert = new JButton(UtilitiesResources.REVERT);

  /**
   * Apply button
   */
  private JButton iApply = new JButton(UtilitiesResources.APPLY);

  /**
   * Reset defaults
   */
  private JButton iSetDefaults = new JButton(UtilitiesResources.RESET_DEFAULTS);

  /**
   * Buttons panel listener
   */
  private ButtonsPanelListener iListener;


  //
  // Life cycle
  //


  /**
   * Default constructor
   */
  public ButtonsPanel() {
    initLayout();
  }

  /**
   * Init layout
   */
  private void initLayout() {
    //layout
    this.setLayout(new FlowLayout(FlowLayout.CENTER, 10, 10));
    this.add(iApply);
    this.add(iRevert);
    this.add(iSetDefaults);
    this.add(iClose);

    //listeners
    iApply.addActionListener(this);
    iRevert.addActionListener(this);
    iSetDefaults.addActionListener(this);
    iClose.addActionListener(this);

    //init state
    iApply.setEnabled(false);
    iRevert.setEnabled(false);
    iSetDefaults.setEnabled(false);
    iClose.setEnabled(true);
  }

  //
  // Operations
  //


  /**
   * Set buttons panel listener
   * @param aListener buttons panel listener
   */
  public void setListener(ButtonsPanelListener aListener){
    iListener = aListener;
  }

  /**
   * Set Apply button enabled
   *
   * @param aEnabled indicates if enabled or disabled
   */
  public void setApplyEnabled(boolean aEnabled){
    iApply.setEnabled(aEnabled);
  }



  /**
   * Set Revert button enabled
   *
   * @param aEnabled indicates if enabled or disabled
   */
  public void setRevertEnabled(boolean aEnabled){
    iRevert.setEnabled(aEnabled);
  }

  /**
   * Set Set-Defaults button enabled
   *
   * @param aEnabled indicates if enabled or disabled
   */
  public void setDefaultsEnabled(boolean aEnabled){
    iSetDefaults.setEnabled(aEnabled);
  }

  //
  // ActionListener implementation
  //



  /**
   * Handle Buttons action event
   *
   * @param aEvent action event
   */
  public void actionPerformed(ActionEvent aEvent){
    if (aEvent.getSource() == iClose) {
      iListener.handleClosePressed();
    }
    else if (aEvent.getSource() == iRevert) {
      iListener.handleRevertPressed();
    }
    else if (aEvent.getSource() == iApply) {
      iListener.handleApplyPressed();
    }
    else if (aEvent.getSource() == iSetDefaults) {
      iListener.handleSetDefaultsPressed();
    }


  }

}
