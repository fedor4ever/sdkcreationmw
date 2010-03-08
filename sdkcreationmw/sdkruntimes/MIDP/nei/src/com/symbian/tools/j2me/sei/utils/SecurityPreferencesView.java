// SecurityPreferencesView.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.filechooser.FileFilter;

import java.security.*;
import java.io.*;
import java.util.*;


import com.symbian.tools.j2me.sei.preferences.SecurityPreferences;

import com.symbian.utils.Debug;



/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class SecurityPreferencesView extends PreferencesView
    implements ActionListener{


  //
  // Constants
  //


  /**
   * Name
   */
  private static final String NAME = UtilitiesResources.SECURITY_TAB_TITLE;

  //
  // Members
  //


  /**
   * Security preferences model
   */
  private SecurityPreferences iModel;

  /**
   * Keystore file
   */
  private JTextField iKeystoreFile = new JTextField();

  /**
   * Select keystore file button
   */
  private JButton iSelectKeystoreFile = new JButton(UtilitiesResources.SELECT_);

  /**
   * Keystore password
   */
  private JPasswordField iKeystorePassword = new JPasswordField();

  /**
   * Key password
   */
  private JPasswordField iKeyPassword = new JPasswordField();

  /**
   * Key alias
   */
  private JComboBox iKeyAlias = new JComboBox();

  /**
   * Key alias select button
   */
  private JButton iSelectKeyAlias = new JButton(UtilitiesResources.SELECT_);

  /**
   * Indicates always to sign Jad
   */
  private JCheckBox iAlwaysAddSignature = new JCheckBox(UtilitiesResources.ALWAYS_SIGN);

  /**
   * Remove permission button
   */
  private JButton iRemovePermission = new JButton(UtilitiesResources.REMOVE);

  /**
   * Add permission button
   */
  private JButton iAddPermission = new JButton(UtilitiesResources.ADD_);

  /**
   * Required permissions list
   */
  private JList iRequiredPermissionsList = new JList(new DefaultListModel());

  /**
   * Trust mode indicator
   */
  private JCheckBox iTrusted = new JCheckBox(UtilitiesResources.TRUSTED);

  /**
   * Target challenge
   */
  private JTextField iChallenge = new JTextField();


  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aModel Security preferences model
   */
  public SecurityPreferencesView(SecurityPreferences aModel) {
    setName(NAME);
    iModel = aModel;
    initLayout();
    initViewComponents();
  }

  /**
   * Init layout
   */
  private void initLayout(){
    this.setLayout(new BorderLayout(GAP, GAP));

    Box northPanel = new Box(BoxLayout.Y_AXIS);

    //challenge panel
    JPanel challengePanel = new JPanel(new GridLayout(0, 1));
    challengePanel.setBorder(new TitledBorder(UtilitiesResources.CHALLENGE));
    challengePanel.add(iChallenge);
    iChallenge.addKeyListener(this);
    northPanel.add(challengePanel);

    //trusted mode panel
    JPanel trustedPanel = new JPanel(new GridLayout(0, 1));
    trustedPanel.setBorder(new TitledBorder(UtilitiesResources.TRUST_MODE));
    trustedPanel.add(iTrusted);
    iTrusted.addActionListener(this);
    northPanel.add(trustedPanel);


    //key panel
    JPanel keyPanel = new JPanel();
    keyPanel.setLayout(new BoxLayout(keyPanel, BoxLayout.Y_AXIS));
    keyPanel.setBorder(new TitledBorder(UtilitiesResources.SIGNING));
    JPanel tmp = new JPanel(new BorderLayout());
    tmp.add(iAlwaysAddSignature, BorderLayout.WEST);
    iAlwaysAddSignature.addActionListener(this);
    keyPanel.add(tmp);
    keyPanel.add(createComboPanel(iKeystoreFile, iSelectKeystoreFile, UtilitiesResources.KEYSTORE_FILE));
    keyPanel.add(createComboPanel(iKeystorePassword, new JLabel(TEN_SPACES), UtilitiesResources.KEYSTORE_PASSWORD));
    keyPanel.add(createComboPanel(iKeyAlias, iSelectKeyAlias, UtilitiesResources.KEY_ALIAS));
    iKeyAlias.addActionListener(this);
    iKeyAlias.addKeyListener(this);
    keyPanel.add(createComboPanel(iKeyPassword, new JLabel(""), UtilitiesResources.KEY_PASSWORD));
    iKeyPassword.addActionListener(this);
    iKeystoreFile.setEditable(false);
    iSelectKeystoreFile.addActionListener(this);
    iKeystorePassword.addKeyListener(this);
    northPanel.add(keyPanel);
    this.add(northPanel, BorderLayout.NORTH);

    //permissions
    this.add(createListPanel(iRequiredPermissionsList,
                             new JButton[] {iAddPermission, iRemovePermission},
                             UtilitiesResources.MIDLET_PERMISSIONS),
             BorderLayout.CENTER);
    iAddPermission.addActionListener(this);
    iRemovePermission.addActionListener(this);

    iAlwaysAddSignature.addActionListener(this);
    iKeyPassword.addKeyListener(this);
  }


  //
  // Operations
  //





  /**
   * Save the changes that were done in the view
   */
  public void updateModel(){
    iModel.setChallenge(iChallenge.getText());
    iModel.setKeystoreFile(iKeystoreFile.getText());
    iModel.setKeystorePassword(iKeystorePassword.getPassword());
    iModel.setKeyPassword(iKeyPassword.getPassword());
    iModel.setKeyAlias((String)iKeyAlias.getSelectedItem());
    iModel.setAlwaysAddSignature(iAlwaysAddSignature.isSelected());

    Object[] srcPerms =
        ((DefaultListModel)iRequiredPermissionsList.getModel()).toArray();
    String[] destPerms = new String[srcPerms.length];
    System.arraycopy(srcPerms, 0, destPerms, 0, srcPerms.length);
    iModel.setRequiredPermissions(destPerms);
    iModel.setTrusted(iTrusted.isSelected());
  }

  /**
   * Revert the changes that were done in the view
   */
  public void initViewComponents(){
    iChallenge.setText(iModel.getChallenge());
    iAlwaysAddSignature.setSelected(iModel.isAlwaysAddSignature());
    iTrusted.setSelected(iModel.isTrusted());
    iKeystoreFile.setText(iModel.getKeystoreFile());
    iKeystorePassword.setText(new String(iModel.getKeystorePassword()));
    iKeyPassword.setText(new String(iModel.getKeyPassword()));
    if (iModel.isAlwaysAddSignature()) {
      initAliases();
    }
    iKeyAlias.setSelectedItem(iModel.getKeyAlias());
    DefaultListModel listModel = new DefaultListModel();
    String[] perms = iModel.getRequiredPermissions();
    for(int i = 0; i < perms.length; i++){
      listModel.addElement(perms[i]);
    }
    iRequiredPermissionsList.setModel(listModel);

    enableKeyPanelComponents(iModel.isAlwaysAddSignature());
  }


  /**
   * Enable the signing components
   *
   * @param aEnabled enable mode
   */
  private void enableKeyPanelComponents(boolean aEnabled){
    iKeystoreFile.setEnabled(aEnabled);
    iKeystorePassword.setEnabled(aEnabled);
    iKeyPassword.setEnabled(aEnabled);
    iKeyAlias.setEnabled(aEnabled);
    iSelectKeyAlias.setEnabled(aEnabled);
    iSelectKeystoreFile.setEnabled(aEnabled);

  }


  /**
   * Init aliases from keystore
   */
  private void initAliases() {
    FileInputStream fis = null;
    try{
      fis = new FileInputStream(iKeystoreFile.getText());
      KeyStore store = KeyStore.getInstance(KeystoreFileFilter.KEYSTORE_TYPE);
      char[] a = iKeystorePassword.getPassword();
      store.load(fis, a);
      iKeyAlias.removeAllItems();
      for (Enumeration e = store.aliases(); e.hasMoreElements(); ) {
        iKeyAlias.addItem(e.nextElement());
      }
      iKeyAlias.setSelectedItem(iModel.getKeyAlias());
    }
    catch(Exception e){
      Debug.printStackTrace(this, e);
      JOptionPane.showMessageDialog
          (this,
           UtilitiesResources.CAUGHT_EXCEPTION + e.getMessage(),
           UtilitiesResources.FAILED_LOAD_ALIASES,
           JOptionPane.ERROR_MESSAGE);
    }
    finally{
      if(fis != null){
        try {
          fis.close();
        }
        catch(Exception e) {}
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
  public void actionPerformed(ActionEvent aEvent){
    Object source = aEvent.getSource();
    if(source == iAlwaysAddSignature){
      enableKeyPanelComponents(iAlwaysAddSignature.isSelected());
    }
    else if(source == iSelectKeystoreFile){
      //show a file selection dialog
      JFileChooser chooser = new JFileChooser();
      chooser.setFileFilter(new KeystoreFileFilter());
      if(chooser.showOpenDialog(new JFrame()) == JFileChooser.APPROVE_OPTION) {
        iKeystoreFile.setText(chooser.getSelectedFile().getAbsolutePath());
        initAliases();
        fireStateChanged();
      }
    }
    else if (source == iAddPermission){
      //display selection dialog
      String[] perms = iModel.getJ2MEPermissions();
      PermissionsDialog dialog = new PermissionsDialog(null, perms);
      dialog.pack();
      dialog.setVisible(true);
      int[] selected = dialog.getSelectedPermissions();
      if(selected.length > 0){
        DefaultListModel model = (DefaultListModel) iRequiredPermissionsList.
            getModel();
        for (int i = 0; i < selected.length; i++) {
          if (!model.contains(perms[selected[i]])) {
            model.addElement(perms[selected[i]]);
          }
        }
        iRequiredPermissionsList.setSelectedIndex
            (model.indexOf(perms[selected.length - 1]));
        iRemovePermission.setEnabled(true);
      }
    }
    else if (source == iRemovePermission){
      //remove selected items in required
      DefaultListModel model = (DefaultListModel)iRequiredPermissionsList.getModel();
      Object[] selected = iRequiredPermissionsList.getSelectedValues();
      for(int i = 0; i < selected.length; i++){
          model.removeElement(selected[i]);
      }
      if(!model.isEmpty()){
        iRequiredPermissionsList.setSelectedIndex(model.size() - 1);
      }
      iRemovePermission.setEnabled(!model.isEmpty());
    }
    //notify
    fireStateChanged();
  }



}

class KeystoreFileFilter extends FileFilter {

  //
  // Constants
  //

  /**
   * Keystore file extension
   */
  static final String KEYSTORE_TYPE = KeyStore.getDefaultType();

  /**
   * File chooser dialog description
   */
  private static final String DESCRIPTION = UtilitiesResources.WILDCARD + KEYSTORE_TYPE;


  //
  // FileFilter implementation
  //

  /**
   * Whether the given file is accepted by this filter.
   *
   * @param f file
   *
   * @return Whether the given file is accepted by this filter.
   */
  public boolean accept(File f) {
    return f.isDirectory() || f.getName().endsWith("." + KEYSTORE_TYPE) ;
  }

  /**
   * Get the description of this filter.
   *
   * @return description of this filter.
   */
  public String getDescription() {
    return DESCRIPTION;
  }

}
