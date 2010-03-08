// UIPreferencesView.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.util.ResourceBundle;

import com.symbian.utils.Debug;


import com.symbian.tools.j2me.sei.preferences.UIPreferences;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class UIPreferencesView extends PreferencesView implements ActionListener{


  //
  // Contants
  //

  /**
   * Name
   */
  private static final String NAME = UtilitiesResources.UI_TAB;

  //
  // Members
  //



  /**
   * Indicates if to show the SEI dialog
   */
  private JCheckBox iShowSEIDialog = new JCheckBox(UtilitiesResources.SHOW_PROGRESS);

  /**
   * Indicates if to show the SEI in the task bar
   */
  private JCheckBox iShowTaskBar = new JCheckBox(UtilitiesResources.SHOW_IN_TASK_BAR);

  /**
   * Indicates if to show the SEI dialog
   */
  private JComboBox iPlafs = new JComboBox();

  /**
   * UI preferences model
   */
  private UIPreferences iModel;

  //
  // Life cycle
  //


  /**
   * Constructor
   *
   * @param aModel UI preferences model
   */
  public UIPreferencesView(UIPreferences aModel) {
    setName(NAME);
    iModel = aModel;
    initLayout();
    initViewComponents();
    iPlafs.addActionListener(this);
  }


  /**
   * Init layout
   */
  private void initLayout() {
    this.setLayout(new BorderLayout());
    JPanel tmp = new JPanel(new GridLayout(3, 1, GAP, GAP));
    iShowSEIDialog.setBorder(new TitledBorder(UtilitiesResources.SHOW_PROGRESS));
    iShowSEIDialog.setToolTipText(UtilitiesResources.SHOW_PROGRESS_TIP);
    iShowSEIDialog.setBorderPainted(true);
    tmp.add(iShowSEIDialog);

    iShowTaskBar.setBorder(new TitledBorder(UtilitiesResources.SHOW_IN_TASK_BAR));
    iShowTaskBar.setBorderPainted(true);
    //If we add support for Window task bar - uncomment the next line
    //tmp.add(iShowTaskBar);

    iPlafs.setBorder(new TitledBorder(UtilitiesResources.PLAF));
    iPlafs.setToolTipText(UtilitiesResources.SELECT_PLAF_TIP);
    tmp.add(iPlafs);
    this.add(tmp, BorderLayout.NORTH);

    iShowSEIDialog.addChangeListener(this);
    iShowTaskBar.addChangeListener(this);
  }


  //
  // Operations
  //


  /**
   * Save the changes that were done in the view
   */
  public void updateModel(){
    iModel.setShowLauncherView(iShowSEIDialog.isSelected());
    iModel.setShowInTaskBar(iShowTaskBar.isSelected());
    UIManager.LookAndFeelInfo[] plafInfos = UIManager.getInstalledLookAndFeels();
    for(int i = 0; i < plafInfos.length; i++){
      if(plafInfos[i].getName().equals(iPlafs.getSelectedItem())){
        try{
          UIManager.setLookAndFeel(plafInfos[i].getClassName());
          iModel.setPlafClassName(plafInfos[i].getClassName());
        }
        catch(Exception e){
          Debug.printStackTrace(this, e);
        }
      }
    }
  }

  /**
   * Revert the changes that were done in the view
   */
  public void initViewComponents(){
    iShowSEIDialog.setSelected(iModel.isShowLauncherView());
    iShowTaskBar.setSelected(iModel.isShowInTaskBar());

    //set LAFs values
    UIManager.LookAndFeelInfo[] plafInfos = UIManager.getInstalledLookAndFeels();
    iPlafs.removeAllItems();
    for(int i = 0; i < plafInfos.length; i++){
      iPlafs.addItem(plafInfos[i].getName());
      if(plafInfos[i].getClassName().equals(iModel.getPlafClassName())){
        iPlafs.setSelectedItem(plafInfos[i].getName());
      }
    }

    //set actual LAF
    for(int i = 0; i < plafInfos.length; i++){
      if(plafInfos[i].getName().equals(iPlafs.getSelectedItem())){
        try{
          UIManager.setLookAndFeel(plafInfos[i].getClassName());
        }
        catch(Exception e){
          Debug.printStackTrace(this, e);
        }
      }
    }

    //init listeners
    iShowTaskBar.addActionListener(this);
    iShowSEIDialog.addActionListener(this);
  }


}
