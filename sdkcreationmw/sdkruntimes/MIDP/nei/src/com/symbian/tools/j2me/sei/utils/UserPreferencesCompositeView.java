// UserPreferencesCompositeView.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;

import java.awt.*;
import javax.swing.*;

import java.util.*;



/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class UserPreferencesCompositeView extends PreferencesView {

  //
  // Members
  //

  /**
   * Tabbed Pane
   */
  private JTabbedPane iTabbedPane = new JTabbedPane();


  //
  // Life cycle
  //


  /**
   * Constructor
   */
  public UserPreferencesCompositeView() {
      this.setLayout(new BorderLayout(GAP, GAP));
      this.add(iTabbedPane, BorderLayout.CENTER);
  }

  //
  // Operations
  //



  /**
   * Builder method to add a view
   *
   * @param aView new View
   */
  public void addView(PreferencesView aView){
    iTabbedPane.add(aView.getName(), aView);
  }

  /**
   * Save the changes that were done in the view
   */
  public void updateModel(){
    //delegate to all composite views
    for(int i = iTabbedPane.getComponentCount() - 1; i >= 0; i--){
      ((PreferencesView)iTabbedPane.getComponent(i)).updateModel();
    }
  }

  /**
   * Revert the changes that were done in the view
   */
  public void initViewComponents(){
    //delegate to all composite views
    for(int i = iTabbedPane.getComponentCount() - 1; i >= 0; i--){
      ((PreferencesView)iTabbedPane.getComponent(i)).initViewComponents();
    }
  }

}
