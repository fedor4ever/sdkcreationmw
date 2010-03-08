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

import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import javax.swing.*;
import javax.swing.border.*;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public abstract class PreferencesView extends JPanel
    implements KeyListener, ActionListener, ChangeListener {

  //
  // Constants
  //

  /**
   * Gap between components
   */
  protected static final int GAP = 6;

  /**
   * 10 spaces for text fields creation.
   */
  protected static final String TEN_SPACES = "          ";


  //
  // Members
  //



  /**
   * Change listener
   */
  private ChangeListener iChangeListener;


  //
  // Operations
  //



  /**
   * Factory method to create a JPanel with label, text field and side
   *  component (i.e. button).
   *
   * (Unlike all other UI components, JTextField doesnt have a label which
   * can give the user a description of what value it holds)
   *
   * @param aTextField text field
   * @param aButton button
   * @param aLabel label
   *
   * @return panel
   */
  static JPanel createComboPanel(JComponent aComponent, Component aButton, String aLabel){
    JPanel textFieldPanel = new JPanel(new GridLayout(2, 1));
    JPanel tmp = new JPanel(new BorderLayout());
    tmp.add(new JLabel(aLabel), BorderLayout.WEST);
    textFieldPanel.add(tmp);
    GridBagLayout layout = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    gbc.anchor = GridBagConstraints.NORTHWEST;
    tmp = new JPanel(layout);
    gbc.weightx = 4;
    gbc.fill = GridBagConstraints.HORIZONTAL;
    layout.setConstraints(aComponent, gbc);
    tmp.add(aComponent);
    gbc.weightx = 1;
    gbc.fill = GridBagConstraints.NONE;
    gbc.anchor = GridBagConstraints.CENTER;
    layout.setConstraints(aButton, gbc);
    tmp.add(aButton);
    textFieldPanel.add(tmp);
    return textFieldPanel;
  }



  /**
   * Factory method to create list panel
   *
   * @param aList list
   * @param aButton button
   * @param aTitle panel border title
   *
   * @return List panel
   */
  static JPanel createListPanel(JComponent aList, JButton[] aButtons, String aTitle){
    JPanel listPanel = new JPanel(new BorderLayout(GAP,GAP));
    listPanel.setBorder(new CompoundBorder(new TitledBorder(aTitle),new EmptyBorder(0,GAP,GAP,GAP)));
    listPanel.add(new JScrollPane(aList), BorderLayout.CENTER);
    Box buttonPanel = Box.createHorizontalBox();
    for(int i = 0; i < aButtons.length; i++){
      if (i > 0) buttonPanel.add(Box.createHorizontalStrut(GAP));
      buttonPanel.add(aButtons[i]);
    }
    JPanel tmp = new JPanel(new BorderLayout());
    tmp.add(buttonPanel, BorderLayout.EAST);
    listPanel.add(tmp, BorderLayout.SOUTH);
    return listPanel;
  }


  /**
   * Get Insets
   *
   * @return Insets
   */
  public Insets getInsets(){
    return new Insets(GAP, GAP, GAP, GAP);
  }

  /**
   * Set change listener
   *
   * @param aChangeListener change listener
   */
  public void setChangeListener(ChangeListener aChangeListener){
    iChangeListener = aChangeListener;
  }

  /**
   * Notify listener state changed
   */
  protected void fireStateChanged(){
    if(iChangeListener != null){
      iChangeListener.stateChanged(new ChangeEvent(this));
    }
  }

  /**
   * Initialize view components values
   */
  public abstract void initViewComponents();

  /**
   * Update the model with the values from the view
   */
  public abstract void updateModel();

  /**
   * Revert the changes that were done in the view
   */
  public void revertChanges(){
    initViewComponents();
  }



  //
  // KeyListener implementation
  //

  /**
   * Invoked when a key has been pressed.
   */
  public void keyPressed(KeyEvent e){

  }

  /**
   * Invoked when a key has been released.
   */
  public void keyReleased(KeyEvent e){

  }

  /**
   * Invoked when a key has been typed.
   */
  public void keyTyped(KeyEvent e){
    fireStateChanged();
  }



  //
  // Action listener implementation
  //


  /**
   * Handle action performed
   *
   * @param aEvent action event
   */
  public void actionPerformed(ActionEvent aEvent){
    fireStateChanged();
  }


  //
  // ChangeListener implementation
  //

  /**
   * Invoked when some UI component has changed its state.
   * Notify the inner change listener
   *
   * @param aEvent ChangeEvent object
   */
  public void stateChanged(ChangeEvent aEvent){
    fireStateChanged();
  }


}
