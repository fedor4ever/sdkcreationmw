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

package com.symbian.tools.j2me.sei.utils;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

class PermissionsDialog extends JDialog
    implements ActionListener, ListSelectionListener{

  //
  // Constants
  //

  private static final int INSETS = 10;

  //
  // Members
  //

  /**
   * Permissions list
   */
  private JList iPermissions = new JList(new DefaultListModel());

  /**
   * Apply button
   */
  private JButton iOK = new JButton(UtilitiesResources.OK);

  /**
   * Cancel button
   */
  private JButton iCancel = new JButton(UtilitiesResources.CANCEL);

  /**
   * Indicates if the user wanted to apply his selection
   */
  private boolean iSelectionApplies = false;


  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aParent parent frame
   */
  public PermissionsDialog(Frame aParent, String[] aPermissions){
    super(aParent, "PERMISSIONS_DIALOG_TITLE", true);
    for(int i = 0; i < aPermissions.length; i++){
      ((DefaultListModel)iPermissions.getModel()).addElement(aPermissions[i]);
    }
    initLayout();
    super.setDefaultCloseOperation(WindowConstants.HIDE_ON_CLOSE);
  }


  /**
   * Layout components
   */
  private void initLayout(){
    this.getContentPane().setLayout(new BorderLayout());
    //list
    this.getContentPane().add(iPermissions, BorderLayout.CENTER);

    //buttons
    JPanel buttonsPanel = new JPanel(new FlowLayout());
    buttonsPanel.add(iOK);
    buttonsPanel.add(iCancel);
    this.getContentPane().add(buttonsPanel, BorderLayout.SOUTH);
    iOK.setEnabled(false);

    //listeners
    iOK.addActionListener(this);
    iCancel.addActionListener(this);
    iPermissions.addListSelectionListener(this);
  }

  //
  // Operations
  //

  /**
   * Get selected permissions indices
   *
   * @return selected permissions indices
   */
  public int[] getSelectedPermissions(){
    if(!iSelectionApplies){
      return new int[0];
    }
    else{
      return iPermissions.getSelectedIndices();
    }
  }


  /**
   * Get Insets
   *
   * @return Insets
   */
  public Insets getInsets(){
    Insets tmp = super.getInsets();
    return new Insets(tmp.top + INSETS,
                      tmp.left + INSETS,
                      tmp.bottom + INSETS,
                      tmp.right + INSETS);
  }


  //
  // ActionListener implementation
  //

  /**
   * Handle buttons action event
   *
   * @param aEvent action event
   */
  public void actionPerformed(ActionEvent aEvent){
    if(aEvent.getSource() == iOK){
      iSelectionApplies = true;
    }
    setVisible(false);
  }

  //
  // ListSelectionListener implementation
  //


  /**
   * Called whenever the value of the selection changes.
   *
   * @param aEvent selection event
   */
  public void valueChanged(ListSelectionEvent aEvent) {
    iOK.setEnabled(!iPermissions.isSelectionEmpty());
  }


}
