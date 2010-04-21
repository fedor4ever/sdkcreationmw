/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

package com.nokia.epdt.plugins.s60.securityconf;

/* java.awt */
import java.awt.Color;
import java.awt.SystemColor;
import java.awt.BorderLayout;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/* javax.swing */
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.JPanel;
import javax.swing.JComboBox;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.ButtonGroup;
import javax.swing.DefaultCellEditor;
import javax.swing.border.EmptyBorder;
import javax.swing.table.TableColumn;

/* com.nokia.wtk.util.gui */
import com.nokia.wtk.util.gui.BorderPanel;

/**
 * Security configuration UI class, main panel in the view.
 */
class SecConfMainPanel extends JPanel {

    private static final int GAP = 6;

    // Domain Selection combo
    private JComboBox jDomainCombo;

    // Permission bindings (settings) table
    private JTable jBindingsTable;

    // Data model for jBindingsTable
    private PermTableModel permTableModel;

    private SecurityConfPlugin parentPlugin;
    private SecConfModel secConfModel;
    private ProtectionDomain[] domains;

    private static Color TABLE_ACTIVE_BGND = SystemColor.window;
    private static Color TABLE_INACTIVE_BGND = SystemColor.control;

    /**
     * This is a constructor. In case if you haven't noticed :)
     */
    SecConfMainPanel(SecurityConfPlugin aParentPlugin) {
        super(new GridBagLayout());
        parentPlugin = aParentPlugin;
        secConfModel = parentPlugin.getSecConfModel();
        domains = secConfModel.getProtectionDomains(false);
        permTableModel = new PermTableModel(parentPlugin);

        // Domain combo box initialization
        jDomainCombo = new JComboBox(domains);
        jDomainCombo.setSelectedItem(secConfModel.getSelectedDomain());

       // JRadioButton realLifeRadio = new JRadioButton(
       //     "Run MIDlets within security domain specified by their certificates");
       // JRadioButton emulatedRadio = new JRadioButton(
       //     "Run MIDlets within the following domain:");
        JLabel obj_JLabel = new JLabel("Run MIDlets within the following domain:");
        

        ButtonGroup bg = new ButtonGroup();
      //  bg.add(realLifeRadio);
      //  bg.add(emulatedRadio);
      //  bg.add(obj_JLabel);
    /*    if (secConfModel.isEmulatedSecurityMode()) {
            realLifeRadio.setSelected(false);
            emulatedRadio.setSelected(true);
            jDomainCombo.setEnabled(true);
        } else {
            realLifeRadio.setSelected(true);
            emulatedRadio.setSelected(false);
            jDomainCombo.setEnabled(false);
        }*/

        jDomainCombo.addActionListener(new DomainSelectionListener());
       // realLifeRadio.addActionListener(new RealLifeRadioListener());
       // emulatedRadio.addActionListener(new EmulatedRadioListener());

       // BorderPanel bp = new BorderPanel(emulatedRadio);
        BorderPanel bp = new BorderPanel(obj_JLabel);
        bp.getContentPanel().add(createConfigurationsPanel());

        ProtectionDomain d = secConfModel.getSelectedDomain();
        if (d != null && d.isUserDefined() && secConfModel.isEmulatedSecurityMode()) {
            jBindingsTable.setEnabled(true);
            jBindingsTable.setBackground(TABLE_ACTIVE_BGND);
        } else {
            jBindingsTable.setEnabled(false);
            jBindingsTable.setBackground(TABLE_INACTIVE_BGND);
        }

        // We could use BorderLayout and this:
        //
        //   add(getJRealLifeRadio(), BorderLayout.NORTH);
        //   add(bp, BorderLayout.CENTER);
        //
        // would be ALMOST perfect, except that radio buttons wouldn't be
        // aligned horizontally. With GridBagLayout we can manipulate insets
        // of each cell, which allows us to aligh the radio buttons. A little
        // bit more complicated, but looks better.

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.weightx = 1.0;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.NORTH;
        gbc.insets.left = bp.getBorder().getBorderInsets(bp).left +
            BorderPanel.COMPONENT_INSET_H;
        //add(realLifeRadio, gbc);

        gbc.weighty = 1.0;
        gbc.insets.left = 0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        add(bp, gbc);

        // Final touches
        setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));
        jBindingsTable.setPreferredScrollableViewportSize(
        jBindingsTable.getPreferredSize());
    }

    private JPanel createConfigurationsPanel() {
        jBindingsTable = new JTable(permTableModel);
        jBindingsTable.setShowGrid(true);
        jBindingsTable.getTableHeader().setReorderingAllowed(false);

        JComboBox permCombo = new JComboBox(new Permission [] {
            Permission.Allowed,
            Permission.Denied,
            Permission.Session,
            Permission.OneShot
        });

        TableColumn permColumn = jBindingsTable.getColumnModel().getColumn(1);
        permColumn.setCellEditor(new DefaultCellEditor(permCombo));
        jBindingsTable.setBackground(TABLE_ACTIVE_BGND);
        jBindingsTable.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);

        JPanel p = new JPanel(new BorderLayout(GAP, GAP));
        p.add(jDomainCombo, BorderLayout.NORTH);
        p.add(new JScrollPane(jBindingsTable), BorderLayout.CENTER);
        p.setBorder(new EmptyBorder(0,GAP,GAP,GAP));
        return p;
    }

    private class DomainSelectionListener implements ActionListener {
        public void actionPerformed(ActionEvent aEvent) {
            ProtectionDomain d = (ProtectionDomain)jDomainCombo.getSelectedItem();
            if (secConfModel.getSelectedDomain() != d) {

                // Selection has changed
                secConfModel.setSelectedDomain(d);
                parentPlugin.pluginModelChangedAction();

                // Notify table about the update
                permTableModel.fireTableDataChanged();
                jBindingsTable.getColumnModel().getColumn(1).
                    getCellEditor().cancelCellEditing();

                // If selected domain is editable and security is emulated
                if (d.isUserDefined() &&
                    secConfModel.isEmulatedSecurityMode()) {
                    jBindingsTable.setEnabled(true);
                    jBindingsTable.setBackground(TABLE_ACTIVE_BGND);
                } else {
                    jBindingsTable.setEnabled(false);
                    jBindingsTable.setBackground(TABLE_INACTIVE_BGND);
                }
            }
        }
    }

    private class RealLifeRadioListener implements ActionListener {
        public void actionPerformed(ActionEvent aEvent) {
            if (secConfModel.isEmulatedSecurityMode()) {
                // Radio button's state has changed
                secConfModel.setEmulatedSecurityMode(false);
                jDomainCombo.setEnabled(false);
                jBindingsTable.setEnabled(false);
                jBindingsTable.clearSelection();
                jBindingsTable.setBackground(TABLE_INACTIVE_BGND);
                jBindingsTable.getColumnModel().getColumn(1).
                    getCellEditor().cancelCellEditing();
                parentPlugin.pluginModelChangedAction();
            }
        }
    }

    private class EmulatedRadioListener implements ActionListener {
        public void actionPerformed(ActionEvent aEvent) {
            if (!secConfModel.isEmulatedSecurityMode()) {
                // Radio button's state has changed
                secConfModel.setEmulatedSecurityMode(true);
                jDomainCombo.setEnabled(true);
                ProtectionDomain d = secConfModel.getSelectedDomain();
                if (d != null && d.isUserDefined() &&
                    secConfModel.isEmulatedSecurityMode()) {
                    jBindingsTable.setEnabled(true);
                    jBindingsTable.setBackground(TABLE_ACTIVE_BGND);
                }
                parentPlugin.pluginModelChangedAction();
            }
        }
    }
}
