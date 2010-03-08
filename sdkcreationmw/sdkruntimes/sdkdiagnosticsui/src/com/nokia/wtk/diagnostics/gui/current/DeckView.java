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
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BoxLayout;
import javax.swing.FocusManager;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JEditorPane;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextPane;
import javax.swing.JTree;
import javax.swing.JViewport;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.event.CaretEvent;
import javax.swing.event.CaretListener;
import javax.swing.text.DefaultEditorKit;
import javax.swing.text.JTextComponent;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.DefaultTreeSelectionModel;
import javax.swing.tree.TreeModel;

import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.EditMenuSupport;
import com.nokia.wtk.app.FindDialog;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.RightClickMenuSupport;
import com.nokia.wtk.app.Saveable;
import com.nokia.wtk.diagnostics.browser.AttributedContent;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.gui.SaveableMenuListener;
import com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer;
import com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewers;
import com.nokia.wtk.diagnostics.views.support.InvokeLaterQueueable;
import com.nokia.wtk.diagnostics.views.support.Misc;
import com.nokia.wtk.diagnostics.views.support.PopLabel;

import com.nokia.wtk.util.CharacterEncoding;
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.gui.LinesBorder;

/**
 * A GUI for viewing WML Deck Content.
 */
class DeckView
    extends JScrollPane
    implements Saveable, EditMenuSupport, Resource, RightClickMenuSupport {

    private class Refresher extends InvokeLaterQueueable {
        protected void doRun() {
            DeckView.this.refresh();
        }
    }

    final private Refresher refresher = new Refresher();

    private class CaretMove
        extends InvokeLaterQueueable
        implements CaretListener {
        private CaretEvent ce;

        protected void doRun() {
            DeckView.this.handleCaretEvent(ce);
        }
        synchronized public void caretUpdate(CaretEvent e) {
            ce = e;
            queue();
        }
    }

    // Labels for the different deck views
    final private String sourceViewLabel = getString(CNTNT_SOURCE_VIEW);
    final private String decodedViewLabel = getString(CNTNT_DECODED_VIEW);
    final private String bytecodeViewLabel = getString(CNTNT_BYTECODE_VIEW);
    final private String treeViewLabel = getString(CNTNT_TREE_VIEW);

    private class ViewMenuAction extends AbstractAction {
        private String label;
        ViewMenuAction(String label) {
            super(label);
            this.label = label;
            setEnabled(false);
        }
        public void actionPerformed(ActionEvent event) {
            DeckView.this.doRefresh(label);
        }
    }

    /** A table of hex digits */
    private static final char[] hexDigit =
        {
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            'A',
            'B',
            'C',
            'D',
            'E',
            'F' };
    private static final String TEMP_DIR = "java.io.tmpdir";

    private Action sourceViewAction = new ViewMenuAction(sourceViewLabel);
    private Action decodedViewAction = new ViewMenuAction(decodedViewLabel);
    private Action bytecodeViewAction = new ViewMenuAction(bytecodeViewLabel);
    private Action treeViewAction = new ViewMenuAction(treeViewLabel);

    private JMenuItem sourceViewMenuItem =
        new JRadioButtonMenuItem(sourceViewAction);
    private JMenuItem decodedViewMenuItem =
        new JRadioButtonMenuItem(decodedViewAction);
    private JMenuItem bytecodeViewMenuItem =
        new JRadioButtonMenuItem(bytecodeViewAction);
    private JMenuItem treeViewMenuItem =
        new JRadioButtonMenuItem(treeViewAction);

    // We want to enable copying from this EditorPane, but it also sometimes
    // needs to be disabled. However, if the AbstractAction from the getActions
    // of this kit is disabled directly, it will also disable the same action
    // in all other kits (strange but true, actions are not cloned on a per-kit
    // basis, but shared between all instances of the same kit). Therefore, we
    // need to create a copy/clone ourselves.
    private Action copyAction = new DefaultEditorKit.CopyAction();
    private Action findAction = null;
    private Action findNextAction = null;

    private ContentTree treeView;
    private JButton refreshButton;
    private JLabel urlLabel;
    private JLabel deckStatsLabel;
    private JComboBox comboBox;
    private JButton editButton;
    private Map mimeTypeToTextView = null; // maps MIME type to RCJEditorPane
    private Map textViewToFindDialog = null;
    // maps RCJEditorPane to FindDialog
    private KeyListener textViewKeyListener = null;
    private ContentViewer currentViewer = null;
    private AttributedContent currentContent = null;
    private BrowserEventListener browserEventListener = null;
    private JPanel columnHeader = null, rowHeader = null;
    private JTextArea rowHeaderText = null;

    private boolean contentAvailable;
    private boolean historyUpdates;
    private boolean copyActionEnabled = false;

    private File sourceFile = null;
    private Browser browser;

    static {
        AppEnvironment.addResources(
            "resources/DeckView.properties",
            DeckView.class);
    }

    /**
     * Create a GUI for viewing WML deck content.
     */
    public DeckView(Browser b) {
        this.getViewport().setScrollMode(JViewport.BLIT_SCROLL_MODE);
        browser = b;

        // Create the fields and labels:
        urlLabel = new JLabel(" ", SwingConstants.LEFT);

        // set up menu-items for right-click
        /*
        ButtonGroup menuItems = new ButtonGroup();
        menuItems.add(sourceViewMenuItem);
        menuItems.add(decodedViewMenuItem);
        menuItems.add(bytecodeViewMenuItem);
        menuItems.add(treeViewMenuItem);
        */

        findAction = new AbstractAction() {
            {
                setEnabled(false);
            }
            public void actionPerformed(ActionEvent e) {
                FindDialog findDialog = getFindDialog();
                if (findDialog != null) {
                    findDialog.actionPerformed(e);
                } else {
                    setEnabled(false);
                }
            }
        };
        findNextAction = new AbstractAction() {
            public void actionPerformed(ActionEvent e) {
                FindDialog findDialog = getFindDialog();
                if (findDialog != null) {
                    String searchString = findDialog.getLastSearchString();
                    if (searchString == null || searchString.length() == 0) {
                        findDialog.actionPerformed(e);
                    } else {
                        findDialog.getFindNextAction().actionPerformed(e);
                    }
                }
            }
        };

        ActionsUtilities.manageEMS(this);
        copyAction.setEnabled(false);

        // Show current WML Deck (use default view):
        populateViewOptions(getDefaultViewType());

        try {
            setHistoryUpdates(
                browser
                    .getBrowserHistory()
                    .isDeviceBrowserHistoryListenerSupported());
            setContentUpdates(
                browser
                    .getBrowserHistory()
                    .isDeviceBrowserHistoryListenerSupported());
        } catch (NullPointerException e) {
            e.printStackTrace();
        }

        // create the event listener
        browserEventListener = new BrowserEventListener();
        browser.getBrowserHistory().addHistoryListListener(
            browserEventListener);

        // this will do the initial update
        browserEventListener.historyRemoved(0);

        // if we don't have any content yet, force a refresh
        if (currentContent == null)
            refresher.queue();
        setPreferredSize(new Dimension(100, 150));
        SaveableMenuListener.register(this, this);
    }

    // The API provides us with a tree model of the deck. However, creating
    // a central tree to re-use with different models seems to introduce
    // a memory leak in that models cannot easily be switched out of trees.
    // We need a a tree that is as disposable as the model we get.
    private static class ContentTree extends JTree {
        ContentTree(TreeModel model) {
            super(model);

            this.setFont(LookFactory.getStandardCodeFont());
            // Must be monospaced font

            this.setEditable(false); // Not editable
            this.setSelectionModel(null); // No selection allowed
            this.setScrollsOnExpand(true);
            this.createNewRenderer();
        }

        /**
         * This method is called both during initialization and when L&F changes.
         * It is implemented here to preserve non-default settings.
         */
        public void updateUI() {
            super.updateUI(); // Normal initialization
            // These colors match the other views, whereas the value of
            // 'LookFactory.getCodeColorSet().backgroundNonEditableColor' does not:
            //LookAndFeel.installColors(this, "Viewport.background", "Viewport.foreground");
            invalidate();
        }

        /** Sets the background color. @param bg the background color to set */
        public void setBackground(Color bg) {
            this.bg = bg;
            if (this.dtcr != null) {
                this.dtcr.setBackgroundNonSelectionColor(bg);
            }
            super.setBackground(bg);
        }

        /** @return the background color */
        public Color getBackground() {
            return (this.bg != null) ? this.bg : super.getBackground();
        }

        private void createNewRenderer() {
            this.setCellRenderer(this.dtcr = new DefaultTreeCellRenderer() {
                    {// Disable default icon rendering:
    setOpenIcon(null);
                    setClosedIcon(null);
                    setLeafIcon(null);
                    setBackgroundNonSelectionColor(
                        ContentTree.this.getBackground());
                    setFont(ContentTree.this.getFont());
                }
            });
        }

        // This is the poor man's version of a cell renderer,
        // but the text is all that's important in this case:
        public String convertValueToText(
            Object value,
            boolean selected,
            boolean expanded,
            boolean leaf,
            int row,
            boolean hasFocus) {

            /* if (value instanceof WMLContent) {
                     //TODO: It sure would be nice to highlight space
                     // characters somehow, but there's no glyph that
                     // is guaranteed to be universal without also being
                     // obnoxious.
                 return StringUtils.getDisplayableString(((WMLContent)value).getRawContent());
             } else if (value instanceof WMLElement) {
                 return ((WMLElement)value).getAsText();
             } else { */
            return value.toString();
            //}
        }

        // Turns out the elements of a tree get put in all kinds of caches
        // and static Hashtable for faster viewing. Resetting these elements
        // in a tree releases those references and allows this tree to be
        // garbage collected.
        void dispose() {
            this.setSelectionModel(new DefaultTreeSelectionModel());
            this.setCellRenderer(new DefaultTreeCellRenderer());
            this.setModel(
                new DefaultTreeModel(new DefaultMutableTreeNode("test")));
        }

        private Color bg;
        private DefaultTreeCellRenderer dtcr;
    } // End of WMLDeckTree

    private void refresh() {
        // NOTE: populateViewOptions() may change current selection
        populateViewOptions((String) getComboBox().getSelectedItem());
        updateEnabledStates();
    }

    // switch to the proper Viewport and enable/disable UI
    private void setContentUpdates(boolean state) {
        boolean wasViewSupported = isViewSupported();
        this.contentAvailable = state;
        if (isViewSupported() != wasViewSupported) {
            updateEnabledStates();
        }
    }

    // Make sure to call from Swing thread!
    private void handleCaretEvent(CaretEvent caretEvent) {
        JEditorPane tv = (JEditorPane) caretEvent.getSource();
        boolean active = (tv.getSelectionStart() != tv.getSelectionEnd());
        if (active != copyAction.isEnabled()) {
            copyAction.setEnabled(copyActionEnabled && active);
        }
        JViewport vp = this.getViewport();
        Rectangle r = vp.getViewRect();
        Point oldPosition = r.getLocation();
        double width = r.getX();
        double startX = oldPosition.getX();
        double startY = oldPosition.getY();
        if (caretEvent.getDot() % 92 <= 15) {
            startX = 0;
        } else {
            startX = width / 6;
        }
        Point newP = new Point((int) startX, (int) startY);
        /**
         *java bug 4200690: function scrollRectToVisible fails when programaticly calling it.
         * the following is a work around solution
         */
        vp.setViewPosition(newP);
        this.repaint();
    }

    // switch to the proper Viewport and enable/disable UI
    private void setHistoryUpdates(boolean state) {
        boolean wasViewSupported = isViewSupported();
        this.historyUpdates = state;
        if (isViewSupported() != wasViewSupported) {
            updateEnabledStates();
        }
    }

    /**
     * @return <code>true if the view is supported by the current device
     */
    private boolean isViewSupported() {
        return (historyUpdates && contentAvailable);
    }

    // enable/disable UI
    private void updateEnabledStates() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                boolean enabled = isViewSupported();
                urlLabel.setEnabled(enabled);
                getComboBox().setEnabled(
                    enabled && getComboBox().getItemCount() > 0);
                getDeckStatsLabel().setEnabled(enabled);

                // Enable/disable the menu items
                if (!enabled) {
                    sourceViewAction.setEnabled(enabled);
                    decodedViewAction.setEnabled(enabled);
                    bytecodeViewAction.setEnabled(enabled);
                    treeViewAction.setEnabled(enabled);
                } else {
                    ContentViewer viewer = currentViewer;
                    if (viewer != null) {
                        boolean hasTreeView = (viewer.getTreeModel() != null);
                        boolean hasOriginalSource =
                            (viewer.getOriginalSource() != null);
                        boolean hasDecodedContent =
                            (viewer.getDecodedContent() != null);
                        sourceViewAction.setEnabled(hasOriginalSource);
                        decodedViewAction.setEnabled(hasDecodedContent);
                        bytecodeViewAction.setEnabled(true);
                        treeViewAction.setEnabled(hasTreeView);
                    }
                }

                doRefresh((String) getComboBox().getSelectedItem());
            }
        });
    }

    /**
     * Formats the bytecode of the specified page.
     */
    private static String formatByteCode(ContentViewer viewer) {
        byte[] bytecode = ((viewer != null) ? viewer.getByteCode() : null);
        if ((bytecode != null) && (bytecode.length > 0)) {
            StringWriter sw = new StringWriter();
            PrintWriter w = new PrintWriter(sw);
            int rowSize = 16; // Number of bytes to show in a row
            int addrDigits = 8;
            // Number of significant digits in a row address label

            // Format the bytes in columns and rows to match headers. This code
            // is what was being done in StringUtils.hexDump, but without the
            // headers.
            int len = bytecode.length;
            int nRows = (len + rowSize - 1) / rowSize;
            for (int row = 0; row < nRows; row++) {
                // Show the bytes plus their renderable characters:
                for (int offset = 0; offset < rowSize; offset++) {
                    int index = (row * rowSize) + offset;
                    if (index < len) {
                        // Separate bytes by a single space
                        if (offset != 0) {
                            w.print(" ");
                        }
                        int b = bytecode[index];
                        w.print(toHex(b >> 4)); // upper nibble
                        w.print(toHex(b)); // lower nibblebble
                    } else {
                        // Pad partial line with spaces
                        // so that the character version will align correctly:
                        w.print("   ");
                    }
                }
                // Add character version of row data:
                w.print("  ");
                for (int offset = 0; offset < rowSize; offset++) {
                    int index = (row * rowSize) + offset;
                    if (index < bytecode.length) {
                        char ch = (char) bytecode[index];
                        if (StringUtils.isPrintable(ch)) {
                            w.print(ch); // displayable character
                        } else {
                            w.print('.');
                        }
                    } else {
                        // Pad partial line with spaces
                        // so that all lines have equal length
                        w.print(' ');
                    }
                }
                w.println();
            }

            // The rest of the job is done by StringUtils.
            w.flush();

            return sw.toString();
        }
        return "";
    }

    /**
     * @return the message to use when there is no deck available.
     */
    private String getNoDeckMessage() {
        if (!isViewSupported()) {
            return getString(CNTNT_NO_UPDATES_TEXT);
        } else {
            return getString(CNTNT_NO_DECK_TEXT);
        }
    }

    /**
     * Returns the shared key listeners that reminds the user that view is
     * readonly. It also suggests the user to open file in editor.
     */
    private KeyListener getTextViewKeyListener() {
        if (textViewKeyListener == null) {
            textViewKeyListener = new KeyAdapter() {
                public void keyPressed(KeyEvent e) {
                    if (e.getKeyCode() == KeyEvent.VK_TAB) {
                        JComponent co = (JComponent) e.getSource();
                        if (e.isShiftDown()) {
                            FocusManager
                                .getCurrentManager()
                                .focusPreviousComponent(
                                co);
                        } else {
                            FocusManager
                                .getCurrentManager()
                                .focusNextComponent(
                                co);
                        }
                        e.consume();
                    }
                }
            };
        }
        return textViewKeyListener;
    }

    /**
     * Returns {@link JEditorPane} for the specified viewer.
     * Creates one if necessary
     */
    private JEditorPane getTextView(ContentViewer viewer) {
        JEditorPane textView = null;
        if (viewer != null) {
            if (mimeTypeToTextView == null) {
                mimeTypeToTextView = new Hashtable(3);
            }
            String mimeType = viewer.getMimeType();
            textView = (JEditorPane) mimeTypeToTextView.get(mimeType);

            if (textView == null) {
                textView = new JTextPane() {
                    /**
                     * Overridden to return false so that horizontal scrolling
                     * will be available. Default behavior of JEditorPane will
                     * force the scrollable to track viewport width.
                     */
                    public boolean getScrollableTracksViewportWidth() {
                        return false;
                    }

                };

                // Why do we have this when there's an "Edit" button???
                // To make the Toolkit friendlier. Offering in alternative
                // is nicer to the user than just offering an obnoxious beep.
                textView.addKeyListener(getTextViewKeyListener());
                KeyStroke ks = KeyStroke.getKeyStroke(KeyEvent.VK_HOME, 3);
                textView.unregisterKeyboardAction(ks);
                ks = KeyStroke.getKeyStroke(KeyEvent.VK_TAB, 0);
                textView.unregisterKeyboardAction(ks);

                // disable pane's right-click actions
                textView.addCaretListener(new CaretMove());
                textView.setEditable(false);
                mimeTypeToTextView.put(mimeType, textView);
                textView.setFont(LookFactory.getStandardCodeFont());
            }
        }
        return textView;
    }

    /**
     * Updates the current view
     *
    private void doRefresh() {
        doRefresh((String)getComboBox().getSelectedItem());
    }*/

    private void prepareTextView(JEditorPane textView, String s) {
        textView.setText(s);
        textView.setCaretPosition(0);
        this.setViewportView(textView);
    }

    /**
     * Actually updates the view. Is called with parameter specifying what kind of
     * view user wants to see. Has to also adjust and sync controllers of
     * right-click and and combo box.
     * Will also fire property changes for saveability, set the labels and
     * actions.
     * @param viewType the view to set in the window. Is one of the labels in
     * the menu.
     */
    private void doRefresh(String viewType) {
        ContentViewer viewer = currentViewer;
        boolean enableCopy = false;
        boolean enableFind = false;
        boolean enableSave = false;
        decodedViewMenuItem.setSelected(false);
        sourceViewMenuItem.setSelected(false);
        bytecodeViewMenuItem.setSelected(false);
        treeViewMenuItem.setSelected(false);

        if (!isViewSupported() || (viewer == null)) {
            JTextComponent msgView = Misc.createHTMLViewer();
            msgView.setText(getNoDeckMessage());
            removeHeaders();
            this.setViewportView(msgView);
        } else if (decodedViewLabel.equals(viewType)) {
            String s = viewer.getDecodedContent();
            if (s != null) {
                JEditorPane textView = getTextView(viewer);
                if (textView != null) {
                    prepareTextView(textView, s);
                    removeHeaders();
                    decodedViewMenuItem.setSelected(true);
                    enableFind = true;
                    enableSave = true;
                }
            }
        } else if (sourceViewLabel.equals(viewType)) {
            String s = viewer.getOriginalSource();
            if (s != null) {
                JEditorPane textView = getTextView(viewer);
                if (textView != null) {
                    prepareTextView(textView, s);
                    removeHeaders();
                    sourceViewMenuItem.setSelected(true);
                    enableFind = true;
                    enableSave = true;
                }
            }
        } else if (bytecodeViewLabel.equals(viewType)) {
            if (viewer.isBinary()) {
                JEditorPane textView = getTextView(viewer);
                if (textView != null) {
                    prepareTextView(textView, formatByteCode(viewer));
                    addHeaders(
                        textView.getMargin(),
                        viewer.getByteCode().length);
                    this.setBackground(Color.white);
                    bytecodeViewMenuItem.setSelected(true);
                    enableFind = true;
                    enableSave = true;
                }
            }
        } else if (treeViewLabel.equals(viewType)) {
            // release previous resources
            if (treeView != null) {
                treeView.dispose();
            }
            // Load tree model:
            TreeModel tm = viewer.getTreeModel();
            treeView = new ContentTree(tm);

            // Pre-expand all branches:
            if (tm != null) {
                for (int row = 0; row < treeView.getRowCount(); row++) {
                    treeView.expandRow(row);
                }
            }

            this.setViewportView(treeView);
            removeHeaders();
            treeViewMenuItem.setSelected(true);
        }

        copyAction.setEnabled(enableCopy);
        findAction.setEnabled(enableFind);

        // Actual value doesn't matter, will be evalueted by save action
        // anyway. Just let it be known that the save action hs to evaluate.
        this.firePropertyChange(
            Saveable.DIRTY_PROPERTY,
            !enableSave,
            enableSave);
        if ((viewer != null) && isViewSupported()) {
            // Update labels
            String url = viewer.getURL();
            String stats = viewer.getStats();
            urlLabel.setText((url == null) ? " " : url);
            getDeckStatsLabel().setText((stats == null) ? " " : stats);
            getComboBox().setSelectedItem(viewType);
        } else {
            urlLabel.setText(" ");
            deckStatsLabel.setText(" ");
        }
    }

    public JComboBox getComboBox() {
        if (comboBox == null) {
            comboBox = new JComboBox();
            comboBox.setFont(LookFactory.getStandardPlainFont());
            // JDK 1.4 has this nice call that will help this JComboBox get
            // the correct size even though it contains no items...
            // comboBox.setPrototypeDisplayValue( sourceViewLabel );
            comboBox.setMinimumSize(new Dimension(100, 10));
            comboBox.setEnabled(false);
            comboBox.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    JComponent c = (JComponent) e.getSource();

                    if (c == comboBox) {
                        String item = (String) comboBox.getSelectedItem();
                        if (item != null) {
                            ContentViewer viewer = currentViewer;
                            if (viewer != null) {
                                if (!item.equals(sourceViewLabel)
                                    || viewer.getOriginalSource() != null) {
                                    doRefresh(item);
                                }
                            }
                            copyActionEnabled = !item.equals(treeViewLabel);
                        }
                    }
                }
            });
        }
        return comboBox;
    }

    public JComponent getControlsPanel() {

        // Layout Control Pane:
        JPanel controlPane = new JPanel();
        controlPane.setLayout(new BoxLayout(controlPane, BoxLayout.X_AXIS));
        controlPane.add(getComboBox());
        return controlPane;

    }

    /**
     * Gets the default view type.
     */
    private String getDefaultViewType() {
        ContentViewer viewer = currentViewer;
        if (viewer != null) {
            if (viewer.getOriginalSource() != null) {
                return sourceViewLabel;
            } else if (viewer.getDecodedContent() != null) {
                return decodedViewLabel;
            } else if (viewer.isBinary()) {
                return bytecodeViewLabel;
            }
        }
        return null;
    }

    /**
     * Populate the view options combobox.
     * @param viewType the view type to select
     */
    private void populateViewOptions(String viewType) {
        if (getComboBox().getItemCount() > 0) {
            getComboBox().removeAllItems();
        }
        ContentViewer viewer = currentViewer;
        if (viewer != null) {
            boolean hasTreeView = (viewer.getTreeModel() != null);
            boolean hasBytecode = (viewer.getByteCode() != null);
            boolean hasDecodedContent = (viewer.getDecodedContent() != null);
            boolean hasOriginalSource = (viewer.getOriginalSource() != null);

            if (hasOriginalSource) {
                getComboBox().addItem(sourceViewLabel);
            } else if (hasDecodedContent) {
                getComboBox().addItem(decodedViewLabel);
            }

            if (hasTreeView) {
                getComboBox().addItem(treeViewLabel);
            }
            if (hasBytecode) {
                getComboBox().addItem(bytecodeViewLabel);
            }

            // Select current view
            if ((viewType == null)
                || (treeViewLabel.equals(viewType) && !hasTreeView)
                || (sourceViewLabel.equals(viewType) && !hasOriginalSource)
                || (decodedViewLabel.equals(viewType)
                    && (!hasDecodedContent || hasOriginalSource))) {
                // current viewer does not exist for this content.
                viewType = getDefaultViewType();
            }

            // Select view type as specified:
            if (viewType != null) {
                getComboBox().setSelectedItem(viewType);
            }
        }
    }

    /**
     * Converts a nibble to a hex character
     * @param   nibble  the nibble to convert.
     */
    private static char toHex(int nibble) {
        return hexDigit[(nibble & 0xF)];
    }

    private void addHeaders(Insets margin, int length) {
        if (columnHeader == null) {
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < hexDigit.length; i++) {
                buf.append('-');
                buf.append(hexDigit[i]);
                if (i < hexDigit.length - 1) {
                    buf.append(' ');
                }
            }
            JTextArea textArea = new JTextArea(buf.toString(), 1, 66);
            textArea.setFont(LookFactory.getStandardCodeFont());
            textArea.setEnabled(false);
            columnHeader = new JPanel(new BorderLayout());
            columnHeader.setBorder(
                new LinesBorder(
                    LookFactory.getCodeColorSet().backgroundColor,
                    margin));
            columnHeader.add(textArea, BorderLayout.CENTER);
        }
        this.setColumnHeaderView(columnHeader);

        StringWriter sw = new StringWriter();
        PrintWriter w = new PrintWriter(sw);
        final int rowSize = 16; // Number of bytes to show in a row
        //int len = viewer.getByteCode().length;
        int nRows = (length + rowSize - 1) / rowSize;
        for (int row = 0; row < nRows; row++) {
            // Create row address label:
            int addr = row * rowSize;
            for (int i = 28; i >= 0; i -= 4) {
                w.print(toHex(addr >> i));
            }
            w.println(" ");
        }

        if (rowHeader == null) {
            rowHeaderText = new JTextArea(sw.toString(), nRows, 9);
            rowHeaderText.setFont(LookFactory.getStandardCodeFont());
            rowHeaderText.setEnabled(false);
            rowHeader = new JPanel(new BorderLayout());
            rowHeader.setBorder(
                new LinesBorder(
                    LookFactory.getCodeColorSet().backgroundColor,
                    margin));
            rowHeader.add(rowHeaderText, BorderLayout.CENTER);
        } else {
            rowHeaderText.setText(sw.toString());
            rowHeaderText.setRows(nRows);
            rowHeaderText.setCaretPosition(0);
        }
        this.setRowHeaderView(rowHeader);
    }

    private void removeHeaders() {
        this.setColumnHeaderView(null);
        this.setRowHeaderView(null);
    }

    /**
     * Implements Saveable. Sets source file for class.
     */
    public void setSourceFile(File f) {
        sourceFile = f;
    }

    /**
     * Implements Saveable. Gets current source file
     */
    public File getSourceFile() {
        // prompt for file name
        return null;
    }

    /**
     * Returns the applicable MIME type
     */
    public String getContentType() {
        ContentViewer content = currentViewer;
        if (content != null) {
            Object selection = getComboBox().getSelectedItem();
            if (bytecodeViewLabel.equals(selection)) {
                return content.getBinaryMimeType();
            } else {
                return content.getMimeType();
            }
        }
        return null;
    }

    /**
     * Returns the Character Encoding (for text files).
     *  or null if unknown or not applicable.
     */
    public String getCharEncoding() {
        return null;
    }

    /**
     * Set the Character Encoding (for text files).
     */
    public void setCharEncoding(String enc) {}

    /**
     * Implements Saveable. Saves content to the current file
     */
    public void save() throws IOException {

        if (sourceFile == null) {
            throw new IOException("File name not defined.");
        }
        try {

            ContentViewer viewer = currentViewer;
            if (viewer != null) {

                FileOutputStream fos = new FileOutputStream(sourceFile);
                String enc = CharacterEncoding.getFileEncoding().getJavaName();
                PrintWriter w =
                    new PrintWriter(new OutputStreamWriter(fos, enc));

                String text = null;
                byte[] binary = null;
                if (bytecodeViewLabel
                    .equals(getComboBox().getSelectedItem())) {
                    binary = viewer.getByteCode();
                } else if (
                    decodedViewLabel.equals(getComboBox().getSelectedItem())) {
                    text = viewer.getDecodedContent();
                } else {
                    text = viewer.getOriginalSource();
                    if (text == null) {
                        text = viewer.getDecodedContent();
                    }
                }

                if (text != null) {

                    // Use platform-specific line termination
                    BufferedReader lines =
                        new BufferedReader(new StringReader(text));
                    String line;
                    while ((line = lines.readLine()) != null) {
                        w.println(line);
                    }
                }

                w.flush();
                w.close();
            }

        } catch (IOException ex) {
            throw ex;
        }
    }

  // Implements Saveable
  public void saveAsTemplate(File file) throws IOException
    {
    }

    /**
     * Should actually not expose this this way, but instead have the callers
     * of this method make their own label with their own even handlers. Then
     * again, what the hell, this is easy.
     **/
    JLabel getDeckStatsLabel() {
        if (deckStatsLabel == null) {
            deckStatsLabel = new PopLabel(" ", SwingConstants.LEFT);
            deckStatsLabel.setBackground(
                LookFactory.getCodeColorSet().backgroundColor);
            deckStatsLabel.setForeground(
                LookFactory.getCodeColorSet().foregroundColor);
            deckStatsLabel.setOpaque(true);
        }
        return deckStatsLabel;
    }

    public Action getCutAction() {
        return null;
    }
    public Action getCopyAction() {
        return copyAction;
    }
    public Action getPasteAction() {
        return null;
    }
    public Action getUndoAction() {
        return null;
    }
    public Action getRedoAction() {
        return null;
    }
    public Action getFindAction() {
        return findAction;
    }

    /**
     * returns "Find and Replace" dialog for this editor
     */
    private FindDialog getFindDialog() {
        FindDialog findDialog = null;
        JViewport vp = this.getViewport();
        if (vp != null) {
            Component view = vp.getView();
            if (view instanceof JTextComponent) {
                JTextComponent textView = (JTextComponent) view;
                if (textViewToFindDialog == null) {
                    textViewToFindDialog = new Hashtable(3);
                }
                findDialog = (FindDialog) textViewToFindDialog.get(textView);
                if (findDialog == null) {
                    findDialog = new FindDialog(textView);

                    // Hide the find dialog when this view becomes invisible
                    final FindDialog dialog = findDialog;
                    textView.addHierarchyListener(new HierarchyListener() {
                        public void hierarchyChanged(HierarchyEvent e) {
                            if (dialog.isVisible() && !isShowing()) {
                                dialog.setVisible(false);
                            }
                        }
                    });
                    textViewToFindDialog.put(textView, findDialog);
                }
            }
        }
        return findDialog;
    }

    /**
     * Implements Saveable.
     */
    public boolean isDirty() {
        return true;
    }

    /**
     * Sets the current data model
     */
    synchronized void setContent(AttributedContent content) {
        // Refresh only if the content is different. setContent gets called
        // everytime ANY browser history event occurs, this means multiple
        // calls about what is effectively the same event.
        if (currentContent == null || !currentContent.equals(content)) {
            currentContent = content;
            currentViewer = ContentViewers.getViewer(content); // handles null
            refresher.queue();
        }
    }

    /**
     * Sets the current data model and URL for it
     */
    synchronized void setContent(AttributedContent content, String url) {
        setContent(content);
        if (currentViewer != null) {
            currentViewer.setURL(url);
        }
    }

    /**
     * Get a String resource.
     */
    private static String getString(String key) {
        return AppEnvironment.getString(key);
    }

    /**
     * Get a resource icon value from the 'properties' file.
     * @param key the name of the resource
     */
    static ImageIcon getIcon(String key) {
        return AppEnvironment.getIcon(key);
    }

    public Object[] getRightClickActions(MouseEvent ev) {
        int num = getComboBox().getItemCount();
        List actions = new ArrayList(num + 2);

        for (int i = 0; i < num; i++) {
            Object s = getComboBox().getItemAt(i);
            if (sourceViewLabel.equals(s)) {
                actions.add(sourceViewMenuItem);
            } else if (treeViewLabel.equals(s)) {
                actions.add(treeViewMenuItem);
            } else if (decodedViewLabel.equals(s)) {
                actions.add(decodedViewMenuItem);
            } else if (bytecodeViewLabel.equals(s)) {
                actions.add(bytecodeViewMenuItem);
            }
        }
        if (!actions.isEmpty())
            actions.add(null); // separator
        actions.add(getCopyAction());
        return actions.toArray();
    }

    /**
     * Inner Class: implementation of {@link DeviceBrowserEventListener}
     * interface which receives notifications from the current device
     */
    private class BrowserEventListener
        implements BrowserHistory.HistoryListListener {

        private InvokeLaterQueueable newContent = new InvokeLaterQueueable() {
            protected void doRun() {
                AttributedContent entry =
                    browser.getBrowserHistory().getCurrentDocument();
                setContent(entry);
            }
        };
        public void currentPositionChanged() {
            newContent.queue();
        }
        public void historyRemoved(int index) {
            newContent.queue();
        }
        public void allHistoryRemoved() {
            newContent.queue();
        }
        public void historyAdded(int index) {
            newContent.queue();
        }
        public void historyUpdated(int index) {
            newContent.queue();
        }
    }
}
