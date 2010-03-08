// SuitePreferencesView.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;

import java.util.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.table.*;

import com.symbian.tools.j2me.sei.preferences.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class SuitePreferencesView extends PreferencesView implements
    ActionListener, ListSelectionListener{


  //
  // Contants
  //

  /**
   * Name
   */
  private static final String NAME = UtilitiesResources.SUITE_TAB;

  //
  // Members
  //


  /**
   * Suite preferences model
   */
  private SuitePreferences iModel;

  /**
   * Jad properties table
   */
  private JTable iJadProperties = new JTable(new JadPropertiesTableModel(5, 2));

  /**
   * Jar file types
   */
  private JList iJarredFilesTypes = new JList(new DefaultListModel());

  /**
   * Remove file type button
   */
  private JButton iRemoveFileType = new JButton(UtilitiesResources.REMOVE);

  /**
   * Add file type button
   */
  private JButton iAddFileType = new JButton(UtilitiesResources.ADD_);


  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aModel Suite preferences model
   */
  public SuitePreferencesView(SuitePreferences aModel) {
    setName(NAME);
    iModel = aModel;
    initLayout();
    initViewComponents();
  }

  /**
   * Init layout
   */
  private void initLayout() {
    setLayout(new BorderLayout(GAP,GAP));

    //Jad properties
    //TODO: disable keys column
    JPanel tmp = new JPanel(new BorderLayout(GAP, GAP));
    tmp.setBorder(new CompoundBorder(
            new TitledBorder(UtilitiesResources.DEFAULT_JAD_PROPS),
            new EmptyBorder(0,GAP,GAP,GAP)));
    iJadProperties.setPreferredScrollableViewportSize(new Dimension(200,
        iJadProperties.getRowHeight()*iModel.getDefaultJadProperties().size()));
    tmp.add(new JScrollPane(iJadProperties));
    iJadProperties.addKeyListener(this);
    iJadProperties.getTableHeader().setReorderingAllowed(false);
    add(tmp, BorderLayout.NORTH);

    //jar file types
    iJarredFilesTypes.setVisibleRowCount(5); // default is 8 - too much
    iJarredFilesTypes.setSelectionMode(ListSelectionModel.
                                       MULTIPLE_INTERVAL_SELECTION);
    add(createListPanel(iJarredFilesTypes,
                        new JButton[]{iAddFileType, iRemoveFileType},
                        UtilitiesResources.JARRED_TYPES),
                        BorderLayout.CENTER);
    iJarredFilesTypes.addListSelectionListener(this);
    //init buttons
    iAddFileType.addActionListener(this);
    iRemoveFileType.addActionListener(this);
    iRemoveFileType.setEnabled(false);
  }


  //
  // Operations
  //


  /**
   * Save the changes that were done in the view
   */
  public void updateModel(){
    //jad properties
    Properties props = new Properties();
    TableModel model = iJadProperties.getModel();
    for(int i = 0; i < model.getRowCount(); i++){
      props.setProperty(
          (String)model.getValueAt(i, 0), (String)model.getValueAt(i, 1));
    }
    iModel.setDefaultJadProperties(props);

    //file types
    Set types = new TreeSet();
    for(int i = 0; i < iJarredFilesTypes.getModel().getSize(); i++){
      types.add(iJarredFilesTypes.getModel().getElementAt(i));
    }
    iModel.setFileTypes(types);
  }

  /**
   * Revert the changes that were done in the view
   */
  public void initViewComponents(){
    //jad properties
    Properties props = iModel.getDefaultJadProperties();
    Object key;
    int i = 0;
    TableModel model = new JadPropertiesTableModel(5, 2); //replace model because of refresh bug in swing
    for(Iterator iter = new TreeSet(props.keySet()).iterator(); iter.hasNext(); ){
      key = iter.next();
      model.setValueAt(key, i, 0);
      model.setValueAt(props.getProperty((String)key), i++, 1);
    }
    iJadProperties.setModel(model);

    //file types
    Set types = iModel.getFileTypes();
    ((DefaultListModel)iJarredFilesTypes.getModel()).removeAllElements();
    for(Iterator iter = types.iterator(); iter.hasNext(); ){
      ((DefaultListModel)iJarredFilesTypes.getModel()).addElement(iter.next());
    }
    if (types.size() > 0) {
      iJarredFilesTypes.setSelectedIndex(0);
      iRemoveFileType.setEnabled(true);
    }
  }


  //
  // ActionListener implementation
  //


  /**
   * Handle Add/Remove buttons action events
   *
   * @param aEvent Action event
   */
  public void actionPerformed(ActionEvent aEvent) {
    if (aEvent.getSource() == iAddFileType) {
      String type = JOptionPane.showInputDialog(this,UtilitiesResources.ENTER_NEW_TYPE);
      if(type != null){
        type = type.trim();
      }
      if(type != null &&
         !type.equals("") &&
         !((DefaultListModel)iJarredFilesTypes.getModel()).contains(type)){
        ((DefaultListModel)iJarredFilesTypes.getModel()).addElement(type);
        iRemoveFileType.setEnabled(true);
        int index = iJarredFilesTypes.getModel().getSize() - 1;
        iJarredFilesTypes.setSelectedIndex(index);
        iJarredFilesTypes.ensureIndexIsVisible(index);
        fireStateChanged();
      }
    }
    else if (aEvent.getSource() == iRemoveFileType) {
      int index = iJarredFilesTypes.getSelectedIndex();
      Object[] selected = iJarredFilesTypes.getSelectedValues();
      for (int i = 0; i < selected.length; i++) {
        ((DefaultListModel)iJarredFilesTypes.getModel()).removeElement(selected[i]);
        int size = iJarredFilesTypes.getModel().getSize();
        if (size == 0) {
          iRemoveFileType.setEnabled(false);
        }
        else
        {
          index = Math.min(index, size - 1);
          iJarredFilesTypes.setSelectedIndex(index);
          iJarredFilesTypes.ensureIndexIsVisible(index);
        }
        fireStateChanged();
      }
    }
  }


  //
  // ListSelectionListener implementation
  //

  /**
   * Called whenever the value of the selection changes.
   *
   * @param e List selection event
   */
  public void valueChanged(ListSelectionEvent e){
    iRemoveFileType.setEnabled(true);
  }

/**
 * Customized table model for the Jad properties to disable editing the keys
 *
 * <p>Title: PREQ 425</p>
 * <p>Description: Java debugging</p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
class JadPropertiesTableModel extends DefaultTableModel{

  /**
   * Constructor
   *
   * @param aRows number of rows
   * @param aCols number of columns
   */
  public JadPropertiesTableModel(int aRows, int aCols){
    super(aRows, aCols);
  }


  /**
   * Returns true if the cell at rowIndex and columnIndex is editable.
   *
   * @param aRowIndex row index
   * @param aColumnIndex column index
   *
   * @return boolean returns true if not the keys column
   */
  public boolean isCellEditable(int aRowIndex, int aColumnIndex){
    return aColumnIndex != 0;
  }

  public String getColumnName(int column) {
    return (column == 0) ? "Key" : "Value";
  }

  public void setValueAt(Object aValue, int row, int column) {
    if (aValue == null) aValue = "";
    if (!aValue.equals(getValueAt(row, column))) {
      super.setValueAt(aValue, row, column);
      fireStateChanged();
    }
  }
}

}
