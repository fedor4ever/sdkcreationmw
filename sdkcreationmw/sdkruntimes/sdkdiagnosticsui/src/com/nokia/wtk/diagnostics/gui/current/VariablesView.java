/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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





package com.nokia.wtk.diagnostics.gui.current;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.DefaultCellEditor;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.LookAndFeel;
import javax.swing.SwingUtilities;
import javax.swing.border.Border;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableModel;
import javax.swing.text.DefaultEditorKit;
import javax.swing.text.JTextComponent;

import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationDialogs;
import com.nokia.wtk.app.EditMenuSupport;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.RightClickMenuSupport;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserVariables;
import com.nokia.wtk.diagnostics.views.support.Misc;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.SingleLineDocument;
import com.nokia.wtk.util.gui.MenuButton;
import com.nokia.wtk.util.gui.RolloverJButton;

/**
 * A GUI for WML variables.
 */
class       VariablesView
extends     JPanel
implements  MessageWriters, Resource {

    private Browser browser;
    private BrowserVariables variables;
    private VariablesTable table;

    static {
            AppEnvironment.addResources("resources/VariablesView.properties",
                                VariablesView.class);
    }

    private final Action doClearAllAction =
        new AbstractAction(getString(VAR_CLEARALL_BUTTON_LABEL)) {
            { setEnabled(false);}
            public void actionPerformed(ActionEvent e) {
                doClearAll();
            }
        };
    private final Action doNewAction =
        new AbstractAction(getString(VAR_ADD_VARIABLE_LABEL)) {
            { setEnabled(false);}
            public void actionPerformed(ActionEvent e) {
                doNewVariable();
            }
        };
    private final Action doClearAction =
        new AbstractAction(getString(VAR_CLEAR_BUTTON_LABEL)) {
            { setEnabled(false);}
            public void actionPerformed(ActionEvent e) {
                String delName = table.getSelectedVariableName();
                if (delName != null) {
                    doClear(delName);
                }
            }
            public void setEnabled(boolean enable) {
                String name = getString(VAR_CLEAR_BUTTON_LABEL);
                if (enable) {
                    String nm = table.getSelectedVariableName();
                    if (nm != null && nm.length() > 0) {
                        name += " \"" + nm  + "\"";
                    }
                }
                putValue(Action.NAME, name);
                super.setEnabled(enable);
            }
        };
    private JScrollPane tableScroller;
    private VariablesTableModel tableModel;
    private boolean allowSettingVariables = true;

    private final JTextArea actionTA = new JTextArea();
    // this is the textArea that contains the text of the currently selected
    // cell. It's value is set by the cell renderer of the table. It is used
    // to implement the cutAction of the table
    private final AbstractAction copyAction = new AbstractAction() {
        public void actionPerformed(ActionEvent ev) {
            actionTA.selectAll();
            actionTA.copy();
        }
    };

    private final Object[] rightClickActions = {
        copyAction, null, doNewAction, doClearAction, doClearAllAction };


    /**
     * Create a GUI for WML variables.
     */
    VariablesView(Browser b) {
        super(new BorderLayout());
        browser = b;
        /*
        AppEnvironment.addResources("resources/VariablesView.class",
                                    this.getClass());
        */
        // Add a scrolling panel for the table:
        tableScroller = new JScrollPane();
        add(tableScroller, BorderLayout.CENTER);

        // set the model to display
        setModel(browser.getBrowserVariables());
        setToolTipText(getString(VAR_TOOLTIP));

        setVarsSettable(testDeviceIsVarsSettable());
        setVarsGettable(testDeviceIsVarsGettable());
    }

    /**
     * Create the MenuButton for the variables view.
     * @return MenuButton
     */
    MenuButton getMenuButton() {
        JMenuItem mi;

        MenuButton variablesButton = new MenuButton(getString(VAR_BUTTON_LABEL)) ;
        JPopupMenu popup = variablesButton.getPopupMenu();

        variablesButton.setToolTipText(getString(VAR_BUTTON_TOOLTIP));
        RolloverJButton.transformToRollover(variablesButton);

        //HelpSystem hs = AppEnvironment.getAppEnvironment().getHelpSystem();

        mi = popup.add(doNewAction);
        //if ( hs != null ) {
        //    hs.setHelpIDResource( mi, VAR_HELP_ID );
        //}
        mi.setToolTipText(getString(VAR_ADD_VARIABLE_TOOLTIP));
        mi = popup.add(doClearAction);
        //if ( hs != null ) {
        //    hs.setHelpIDResource( mi, VAR_HELP_ID );
        //}
        mi.setToolTipText(getString(VAR_CLEAR_BUTTON_TOOLTIP));
        mi = popup.add(doClearAllAction);
        //if ( hs != null ) {
        //    hs.setHelpIDResource( mi, VAR_HELP_ID );
        //}
        mi.setToolTipText(getString(VAR_CLEARALL_BUTTON_TOOLTIP));

        // Enable or disable "Clear" menu items when menu is about to
        // become visible
        popup.addPopupMenuListener(new PopupMenuListener() {
            public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {}
            public void popupMenuCanceled(PopupMenuEvent e) {}
            public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
                updateClearActions();
            }
        });

        return variablesButton;
    }

    private void updateClearActions() {
        doClearAllAction.setEnabled(
            (variables !=  null) && (variables.getVariableCount() > 0) &&
            allowSettingVariables);
        doClearAction.setEnabled(
            (table != null) && !(table.getSelectionModel().isSelectionEmpty()) &&
            allowSettingVariables);
    }

    private
    class       VariablesTable
    extends     JTable
    implements  EditMenuSupport, RightClickMenuSupport  {

        VariablesTable(AbstractTableModel tm) {
            super(tm);
            ActionsUtilities.copyActionStyler(copyAction);
            copyAction.setEnabled(false);
            setDefaultRenderer(String.class, new VariablesTableCellRenderer());
        }
        public Action getUndoAction() { return null; }
        public Action getRedoAction() { return null; }
        public Action getCutAction() { return null; }
        public Action getCopyAction() { return copyAction; }
        public Action getPasteAction() { return null; }
        public Action getFindAction() { return null; }

        public Object[] getRightClickActions(MouseEvent e) {
            if (this.isEditing()) {
                // if is editing the editor-component will shows its own
                // cut/copy/paste
                return null;
            }
            updateClearActions();
            return rightClickActions;
        }

        String getSelectedVariableName() {
            TableModel model = this.getModel();
            return model.getValueAt(this.getSelectedRow(), 0).toString();
        }
    }

    /**
     * Sets the WML variables model to display.
     * @param vars the WML variables
     */
    private void setModel(BrowserVariables vars) {

        // this is a private methos which is only invoked once in the view's
        // lifetime. Therefore, there's no need for any cleanup because this
        // is the first and the last time we get here

        variables = vars;
        variables.addVariableListListener(
            new BrowserVariables.VariableListListener() {

                public void variableAdded(int index) {
                    getTableModel().addEntry(index);
                }
                public void variableRemoved(int index) {
                    getTableModel().removeEntry(index);
                }
                public void allVariablesRemoved() {
                    getTableModel().removeAll();
                }
                public void variableChanged(int index) {
                    getTableModel().changeEntry(index);
                }

            });

        table = new VariablesTable(getTableModel());
        table.setShowVerticalLines(false);
        table.setGridColor(Color.lightGray);
        table.setRowSelectionAllowed(true);
        table.setColumnSelectionAllowed(false);
        table.getTableHeader().setReorderingAllowed(false);
        table.setCellSelectionEnabled(true);
        table.setFont(LookFactory.getStandardPlainFont());
        table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        table.setOpaque(false);
        table.setDefaultEditor(String.class,
                               new DefaultCellEditor(
                                    new VariablesTableCellEditorComponent()));

        // Add the table to the scrolling panel:
        setVarsGettable(testDeviceIsVarsGettable());
    }

    /**
     * Clears single variable.
     * @param name Name of the variable to clear. Do not pass null, make sure
     * the variable name exists.
     */
    private void doClear(String name) {
        try {
            variables.setBrowserVariable(name, "");
        } catch (BrowserVariables.InvalidVariableNameException in) {
            MessageWriters.DebugMsg.println(
                "Error in variables view while clearing: name was illegal? Name = " +
                in.getMessage());
        } catch (BrowserVariables.InvalidVariableValueException iv) {
            DebugMsg.println(
                "Error in variables view while clearing: value null was illegal? Name = " +
                name);
        }
    }

    /**
     * Clears all browser variables.
     * Variables are cleared along with history.
     */
    private void doClearAll() {
        int count = variables.getVariableCount();
        int i = 0;
        String[] names = new String[count];

        for (i = 0; i < count; i++) {
            names[i] = variables.getVariableNameAt(i);
        }
        for (i = 0; i < count; i++) {
            doClear(names[i]);
        }
    }

    /**
     * Update the WML variables context
     */
    private void doSet(String n, String v) {

        if (testDeviceIsVarsSettable()) {
            if ((n != null) && (n.length() > 0)) {
                try{
                    variables.setBrowserVariable(n, v);
                    StatusMsg.println("Ready");
                } catch (BrowserVariables.InvalidVariableValueException iv)  {
                    StatusMsg.println("Illegal variable value");
                } catch (BrowserVariables.InvalidVariableNameException in)  {
                    StatusMsg.println("Illegal variable name");
                }
            }
        }
    }

    /**
     * Opens dialog to add new VariablesTableModel
     */
    private void doNewVariable() {
        if (!testDeviceIsVarsSettable()) {
            return;
        }

        // Set dialog title
        String title = "Create and Set a new variable"; /* getString() */

        // create dialog
        final JDialog dialog = ApplicationDialogs.createDialog(title, true);

        // Create the content
        final JTextField name = new JTextField(20);
        final JTextField value = new JTextField(20);

        JPanel addVarComponent = LookFactory.createRowPanel( new Component [] {
            new JLabel(getString(VAR_DIALOG_NAME_LABEL)), name,
            new JLabel(getString(VAR_DIALOG_VALUE_LABEL)), value
        });

        // Initialize actions appropriately
        final Action ok =
            new AbstractAction(ApplicationDialogs.getOkayLabel()) {
                { setEnabled(false); }
                public void actionPerformed(ActionEvent e) {
                    // Commit property values and then close dialog.
                    VariablesView.this.doSet(name.getText(), value.getText());
                    dialog.dispose();
                }
            };

        final Action apply =
            new AbstractAction(ApplicationDialogs.getApplyLabel()) {
                { setEnabled(false); }
                public void actionPerformed(ActionEvent e) {
                    // Commit variable.
                   VariablesView.this.doSet(name.getText(), value.getText());
                   this.setEnabled(false);
                }
            };

        Action cancel =
            new AbstractAction(ApplicationDialogs.getCancelLabel()) {
                { setEnabled(true); }
                public void actionPerformed(ActionEvent e) {
                    // Close dialog without committing any property values.
                    dialog.dispose();
                }
            };

        // Detect changes:
        DocumentListener listen = new DocumentListener() {
            public void insertUpdate(DocumentEvent e) { doChange(); }
            public void removeUpdate(DocumentEvent e)  { doChange(); }
            public void changedUpdate(DocumentEvent e)  { doChange(); }
            private void doChange() {
                // Disable Ok/Apply actions if value is empty because
                // even if we let the user to click Apply, it won't have
                // any effect anyway
                boolean enable = (value.getDocument().getLength() > 0);
                ok.setEnabled(enable);
                apply.setEnabled(enable);
            }
        };

        name.setDocument(new SingleLineDocument(name.getDocument()));
        value.setDocument(new SingleLineDocument(value.getDocument()));
        name.getDocument().addDocumentListener(listen);
        value.getDocument().addDocumentListener(listen);

        // Layout the dialog
        Action [] actions = new Action[] { ok, cancel, apply };
        LookFactory.layoutDialog(dialog, addVarComponent, actions, cancel);

        // Show the dialog:
        dialog.setResizable(false);
        ApplicationDialogs.invokeModalDialog(dialog);

        // Uninstall listeners:
        name.getDocument().removeDocumentListener(listen);
        value.getDocument().removeDocumentListener(listen);
    }

    /**
     * Can vars be set? Checks the device for whether device will accept vars
     * to be editable.
     */
    private boolean testDeviceIsVarsSettable() {
        try {
          return browser.getBrowserVariables().isSetBrowserVariableSupported();
        }catch (NullPointerException e) {}

        return false;
    }
    /**
     * Can be vars be read? Checks the device for whether device vars can be read
     */
    private boolean testDeviceIsVarsGettable() {
        try {
          return browser.isDeviceWMLContextListenerSupported();
        }catch (NullPointerException e) {}

        return false;
    }

    // might be called from non-swing thread
    private void setVarsSettable(boolean allow) {
        allowSettingVariables = allow;
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                doNewAction.setEnabled(allowSettingVariables);
                updateClearActions();
            }
        });
    }

    private void setVarsGettable(boolean allowGettingVariables) {
        final boolean allow = allowGettingVariables;
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                if (allow) {
                    tableScroller.setViewportView(table);
                } else {
                    JTextComponent msgView = Misc.createHTMLViewer();
                    msgView.setText(getString(VAR_NO_VARS_TEXT));
                    tableScroller.setViewportView(msgView);
                    doNewAction.setEnabled(false);
                    doClearAction.setEnabled(false);
                    doClearAllAction.setEnabled(false);
                    copyAction.setEnabled(false);
                }
            }
        });
    }

    /**
     * Get the TableModel reference.
     */
    private VariablesTableModel getTableModel() {
        if (tableModel == null) {
            tableModel = new VariablesTableModel();
        }
        return tableModel;
    }

    /**
     * A private inner class to set the editing,
     * pre-setting the copy-action.
     */
    static private
    class       VariablesTableCellEditorComponent
    extends     JTextField
    implements  EditMenuSupport, RightClickMenuSupport {

        final private Action []  tfRightClickActions;

        public VariablesTableCellEditorComponent() {
            super();
            tfRightClickActions = new Action[3];

            tfRightClickActions[1] = new DefaultEditorKit.CopyAction();
            tfRightClickActions[0] = new DefaultEditorKit.CutAction();
            tfRightClickActions[2] = new DefaultEditorKit.PasteAction();
            ActionsUtilities.manageEMS(this);
        }

        public Object[] getRightClickActions(MouseEvent e) {
            return tfRightClickActions;
        }
        public Action getUndoAction() { return null; }
        public Action getRedoAction() { return null; }
        public Action getCutAction() { return tfRightClickActions[0]; }
        public Action getCopyAction() { return tfRightClickActions[1]; }
        public Action getPasteAction() { return tfRightClickActions[2]; }
        public Action getFindAction() { return null; }

    }

    /**
     * A private inner class to set the rendering and listen to selections,
     * pre-setting the copy-action.
     */
    private
    class       VariablesTableCellRenderer
    extends     DefaultTableCellRenderer {

        final private Border borderDown = BorderFactory.createCompoundBorder(
            BorderFactory.createLoweredBevelBorder(),
            BorderFactory.createEmptyBorder(0, LookFactory.getGap(), 0, 0));
        final private Border borderEmpty = BorderFactory.createEmptyBorder(
                                            0, LookFactory.getGap(), 0, 0);
        private int row = -1, column = -1;

        private void selectionAdjust(int rowN, int columnN) {
            if ((rowN == row) && (columnN == column)) { return; }
            actionTA.setText(getText());
            row = rowN;
            column = columnN;
            updateClearActions();
        }

        public Component getTableCellRendererComponent(JTable table,
                                               Object value,
                                               boolean isSelected,
                                               boolean hasFocus,
                                               int row,
                                               int column) {
            super.getTableCellRendererComponent(table, value, isSelected,
                hasFocus, row, column);
            if (column == 0) {
                setBorder(borderEmpty);
                setFont(LookFactory.getStandardBoldFont());
                if (isSelected) {
                    selectionAdjust(row, column);
                    LookAndFeel.installColors(this, "Label.foreground",
                                                       "Label.background");
                } else {
                    LookAndFeel.installColors(this, "Label.background",
                                                       "Label.foreground");
                }
                setToolTipText(getString(VAR_NAME_TOOLTIP));
            } else {
                setBorder(borderEmpty);
                setFont(LookFactory.getStandardCodeFont());
                if (isSelected) {
                    selectionAdjust(row, column);
                    // TextArea.foreground is a bit harsh as a background color
                    LookAndFeel.installColors(this, "Label.foreground",
                                                       "Label.background");
                } else {
                    LookAndFeel.installColors(this, "Text.background",
                                                       "Text.foreground");
                }
                setToolTipText(getString(VALUE_TOOLTIP));
            }
            return this;
        }
    }


    /**
     * Inner Class: A TableModel to support variablesView.
     */
    private
    class       VariablesTableModel
    extends     AbstractTableModel {

        // we need to know how many rows are available *before* a notification
        // of a row removal comes in. This variable performs that function.
        int rows = 0;

        public String getColumnName(int column) {
            if (column == 0)        return getString(VAR_NAME_LABEL);
            else if (column == 1)   return getString(VAR_VALUE_LABEL);
            else                    return null;
        }

        public Class getColumnClass(int columnIndex) {
            return String.class;
        }

        public int getRowCount() {
            rows = variables.getVariableCount();
            return rows;
        }

        public int getColumnCount() {
            return 2;
        }

        public Object getValueAt(int row, int column) {
            Object value = null;

            if ((row >= 0) && (row < variables.getVariableCount())) {
                if (column == 0) {
                    value = variables.getVariableNameAt(row);
                } else if (column == 1) {
                    value = variables.getVariableValueAt(row);
                }
            }
            return (value != null) ? value : "";
        }

        void removeEntry(int i) {
            fireTableRowsDeleted(i, i);
        }
        void removeAll() {
            fireTableRowsDeleted(0, rows);
        }
        void addEntry(int i) {
            fireTableRowsInserted(i, i);
        }
        void changeEntry(int i) {
            fireTableRowsUpdated(i, i);
        }

        public void setValueAt(Object aValue, int rowIndex, int columnIndex) {
            String name = "", value = "";
            // find out whether name or value
            if (columnIndex == 0) {
                name = aValue.toString();
                value = getValueAt(rowIndex, 1).toString();
            } else {
                name = getValueAt(rowIndex, 0).toString();
                value = aValue.toString();
            }

            doSet(name, value);
        }

        public boolean isCellEditable(int rowIndex, int columnIndex) {
            return allowSettingVariables &&  (columnIndex == 1);
        }
    }


    /**
     * Get a String resource.
     */
    private static String getString(String key) {
        return AppEnvironment.getString(key);
    }
}
