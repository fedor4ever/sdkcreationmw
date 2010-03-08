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



package com.nokia.wtk.bookmarks;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Insets;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyVetoException;
import java.beans.VetoableChangeListener;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.EventObject;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.JTree;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.ToolTipManager;
import javax.swing.border.EmptyBorder;
import javax.swing.event.CellEditorListener;
import javax.swing.event.ChangeEvent;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.UndoableEditEvent;
import javax.swing.event.UndoableEditListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellEditor;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeCellEditor;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.swing.undo.CannotRedoException;
import javax.swing.undo.StateEdit;
import javax.swing.undo.StateEditable;
import javax.swing.undo.UndoManager;
import javax.swing.undo.UndoableEditSupport;

import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationDialogs;
import com.nokia.wtk.app.EditMenuSupport;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.RCEMSTextField;
import com.nokia.wtk.app.RightClickMenuSupport;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.PropertyEditors;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.DraggableTree;
import com.nokia.wtk.util.gui.MenuButton;

/**
 * A GUI for the Browser's bookmarks.
 */

class BookmarkView
    extends JPanel
    implements
        StateEditable,
        Resource,
        EditMenuSupport,
        ClipboardOwner,
        RightClickMenuSupport,
        PropertyChangeListener,
        VetoableChangeListener,
        MessageWriters {

    private static class AskSamplesWarning {
        private JDialog diag;
        private AbstractAction noAction =
            new AbstractAction(ApplicationDialogs.getNoLabel()) {
            public void actionPerformed(ActionEvent e) {
                result = false;
                diag.setVisible(false);
            }
        };
        private boolean result = false;
        private AbstractAction yesAction =
            new AbstractAction(ApplicationDialogs.getYesLabel()) {
            public void actionPerformed(ActionEvent e) {
                result = true;
                diag.setVisible(false);
            }
        };

        AskSamplesWarning() {
            diag =
                ApplicationDialogs.createDialog(
                    getResource(SAMPLES_WIPING_WARNING_TITLE),
                    true);
            diag.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);

            JPanel panel = new JPanel(new BorderLayout());
            JPanel buttons = new JPanel();
            String s =
                ApplicationDialogs.formatMessageAsHtml(
                    getResource(SAMPLES_WIPING_WARNING_TEXT),
                    false,
                    0,
                    "<strong>"
                        + "<font color=\""
                        + ApplicationDialogs.ColorStyle.INFO
                        + "\">",
                    "</font></strong>");
            JLabel label = new JLabel(s);
            JLabel l = new JLabel();

            JCheckBox cb =
                (JCheckBox) PropertyEditors.createCheckBox(
                    getProperties().getProperty(
                        SAMPLES_CHANGE_DONT_WARN_PROPERTY),
                    getResource(SAMPLES_WIPING_WARNING_DONT_ASK));

            cb.setAlignmentX(0);
            cb.setAlignmentY(0);
            cb.setSelected(false);
            cb.setForeground(l.getForeground());
            panel.add(label, BorderLayout.CENTER);
            panel.add(cb, BorderLayout.SOUTH);

            buttons.add(new JButton(yesAction));
            buttons.add(new JButton(noAction));
            LookFactory.layoutDialog(
                diag,
                panel,
                new Action[] { yesAction, noAction },
                yesAction);

            ApplicationDialogs.invokeModalDialog(diag);
        }

        boolean getResult() {
            return result;
        }

        void show() {
            diag.setVisible(true);
        }
    }

    /**
     * Inner Class: the editing panel for a bookmark.
     * Handy for dialogs or in TreeCellEditors.
     */
    private static class BookmarkEditor extends JPanel {
        private Bookmarks.Bookmark bookmark = null;
        final private JTextField commentTxt_ = new RCEMSTextField(25);
        final private JTextField locTxt_ = new RCEMSTextField(25);

        final private JTextField nameTxt_ = new RCEMSTextField(25);

        public BookmarkEditor() {
            super(new BorderLayout());
            nameTxt_.setFont(LookFactory.getStandardBoldFont());
            commentTxt_.setFont(locTxt_.getFont());
            add(
                LookFactory.createRowPanel(
                    new Component[] {
                        new JLabel(getResource(TITLE_LABEL)),
                        nameTxt_,
                        new JLabel(getResource(LOCATION_LABEL)),
                        locTxt_,
                        new JLabel(getResource(COMMENT_LABEL)),
                        commentTxt_ }),
                BorderLayout.CENTER);

        }

        public void addNotify() {
            super.addNotify();
            nameTxt_.selectAll();
            nameTxt_.requestFocus();
        }

        /**
         * Tell the editor to cancel editing. Useful for when embedding
         * the editor in a TreeCellEditor.
         */
        void cancelEditing() {}
        /** Return the bookmark being edited **/
        Bookmarks.Bookmark getBookmark() {
            return this.bookmark;
        }

        /** Set a reference to the bookmark being edited. **/
        void setBookmark(Bookmarks.Bookmark bookmark) {
            this.bookmark = bookmark;

            String txt = this.bookmark.getComment();
            nameTxt_.setText(this.bookmark.getName());
            locTxt_.setText(this.bookmark.getLocation());
            commentTxt_.setText(txt);
        }

        /**
         * Tell the editor to stop editing. Useful for when embedding
         * the editor in a TreeCellEditor.
         */
        void stopEditing() {
            if (bookmark != null) {
                bookmark.set(
                    nameTxt_.getText(),
                    locTxt_.getText(),
                    commentTxt_.getText());
            }
        }
    }

    private class BookmarksTree extends DraggableTree {

        private final boolean isMutable_;

        BookmarksTree(TreeModel tmodel, BookmarkView b, boolean isMutable) {
            isMutable_ = isMutable;
            this.setEditable(true);
            this.setShowsRootHandles(true);
            this.setRootVisible(true);
            this.addDropVetoableChangeListener(b);
            this.addDropPropertyChangeListener(b);
            this.setFont(LookFactory.getStandardBoldFont());
            ActionsUtilities.registerAction(this, deleteAction, "deleteBmark");
            ActionsUtilities.registerAction(this, goAction, "goBmark");
            this.setModel(tmodel);
            ToolTipManager.sharedInstance().registerComponent(this);

            Bookmarks.BookmarkCellRenderer r =
                new Bookmarks.BookmarkCellRenderer();
            r.setOpenIcon(Bookmarks.getBranchIcon(true));
            r.setClosedIcon(Bookmarks.getBranchIcon(false));
            r.setLeafIcon(
                isMutable
                    ? Bookmarks.getLeafIcon()
                    : Bookmarks.getSamplesLeafIcon());
            r.setRootIcon(AppEnvironment.getIcon(ROOT_ICON_PROPERTY));
            this.setCellRenderer(r);
            this.setCellEditor(new DefaultTreeCellEditor(this, r, editor));
        }

        /**
         * Is this tree mutable, or should people be warned about changing it?
         * @return boolean true if this tree is mutable
         */
        boolean isMutable() {
            return isMutable_;
        }

    }

    class BookmarkTransferable implements Transferable {
        public final static String COMMENT = "Comment: ";
        public final static String LOCATION = "Location: ";

        public final static String TITLE = "Title: ";

        transient private BookmarkLoader bLoader = null;
        private String location = "", title = "", comment = "";
        private boolean userBookmark = true;

        public BookmarkTransferable(Bookmarks.Bookmark bi, BookmarkLoader bl) {
            if (bi != null) {
                title = bi.getName();
                location = bi.getLocation();
                comment = bi.getComment();
                userBookmark = bi.isUserBookmark();
            }
            bLoader = bl;
        }

        public Object getTransferData(DataFlavor flavor)
            throws UnsupportedFlavorException, IOException {
            try {
                if (flavor.equals(DataFlavor.plainTextFlavor)) {
                    String ret =
                        TITLE
                            + title
                            + "\n"
                            + LOCATION
                            + location
                            + "\n"
                            + COMMENT
                            + comment;
                    return ret.getBytes();
                } else if (flavor.equals(DataFlavor.stringFlavor)) {
                    return TITLE
                        + title
                        + "\n"
                        + LOCATION
                        + location
                        + "\n"
                        + COMMENT
                        + comment;

                } else if (flavor.isMimeTypeEqual(bookmarkFlavor)) {
                    return new Bookmarks.Bookmark(
                        title,
                        location,
                        comment,
                        bLoader,
                        userBookmark);
                } else {
                    throw new UnsupportedFlavorException(flavor);
                }
            } catch (NullPointerException n) { // something was null
                return null;
            }
        }

        public DataFlavor[] getTransferDataFlavors() {
            DataFlavor[] newFlavs =
                {
                    DataFlavor.plainTextFlavor,
                    bookmarkFlavor,
                    DataFlavor.stringFlavor };
            return newFlavs;
        }

        public boolean isDataFlavorSupported(DataFlavor flavor) {
            return (
                flavor.equals(DataFlavor.plainTextFlavor)
                    || flavor.equals(bookmarkFlavor)
                    || flavor.equals(DataFlavor.stringFlavor));
        }

    }

    /**
     * Inner Class: The tree cell editor implementation.
     */
    private class Editor implements TreeCellEditor {
        private BookmarkEditor bookmarkEditor =
            BookmarkView.getBookmarkEditor(null);
        final private JPanel box = new JPanel();
        final private JButton done = new JButton(getResource(OK_BUTTON_LABEL));
        private StateEdit edit_ = null;

        final private JPanel editor_ = new JPanel(new BorderLayout()) {
            public void addNotify() {
                super.addNotify();
                categoryTxt.selectAll();
                categoryTxt.requestFocus();

                getRootPane().setDefaultButton(done);
            }
        };

        final private Vector listeners_ = new Vector(2);

        private Object val_ = null;

        public Editor() {
            JButton cancel =
                new JButton(
                    new AbstractAction(getResource(CANCEL_BUTTON_LABEL)) {
                public void actionPerformed(ActionEvent e) {
                    cancelCellEditing();
                }
            });

            done.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    stopCellEditing();
                }
            });

            int gap = LookFactory.getGap();
            box.setLayout(new BoxLayout(box, BoxLayout.X_AXIS));
            box.add(Box.createHorizontalGlue());
            box.add(done);
            box.add(Box.createHorizontalStrut(LookFactory.getGap()));
            box.add(cancel);
            box.setBorder(BorderFactory.createEmptyBorder(gap, 0, 0, 0));

            editor_.setBorder(
                BorderFactory.createCompoundBorder(
                    LookFactory.loweredBorder(),
                    LookFactory.mediumBorder()));
            editor_.add(BorderLayout.SOUTH, box);
        }
        public void addCellEditorListener(CellEditorListener l) {
            listeners_.add(l);
        }
        public void cancelCellEditing() {
            for (Enumeration e = listeners_.elements(); e.hasMoreElements();)
                ((CellEditorListener) e.nextElement()).editingCanceled(
                    new ChangeEvent(val_));
            val_ = null;
            enableAppropriateActions();
        }
        public Object getCellEditorValue() {
            if (val_ == null)
                return "";
            else
                return val_;
        }

        public Component getTreeCellEditorComponent(
            JTree tree,
            Object value,
            boolean isSelected,
            boolean expanded,
            boolean leaf,
            int row) {

            editor_.remove(categoryTxt);
            editor_.remove(bookmarkEditor);
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) value;
            val_ = node.getUserObject();
            if (!node.getAllowsChildren()) {
                bookmarkEditor.setBookmark((Bookmarks.Bookmark) val_);
                editor_.add(bookmarkEditor, BorderLayout.CENTER);
            } else {
                categoryTxt.setText((String) val_);
                editor_.add(categoryTxt, BorderLayout.CENTER);
            }

            if (newBookmark == false) {
                edit_ = new StateEdit(BookmarkView.this, "Edit");
                edit_.end();
                support_.postEdit(edit_);
            } else {
                edit_ = null;
            }

            return editor_;
        }
        public boolean isCellEditable(EventObject anEvent) {
            return true;
        }
        public void removeCellEditorListener(CellEditorListener l) {
            listeners_.remove(l);
        }
        public boolean shouldSelectCell(EventObject anEvent) {
            return false;
        }
        public boolean stopCellEditing() {
            if (val_ instanceof Bookmarks.Bookmark) {
                bookmarkEditor.stopEditing();
            } else { // val_ is string
                val_ = categoryTxt.getText();
            }
            for (Enumeration e = listeners_.elements(); e.hasMoreElements();)
                ((CellEditorListener) e.nextElement()).editingStopped(
                    new ChangeEvent(val_));
            val_ = null;
            if (edit_ != null) {
                edit_.end();
                support_.postEdit(edit_);
            }
            enableAppropriateActions();
            return true;
        }
    }

    /** This inner class was created to localize all issues with making
     * a dialog for editing a new bookmark in a way that cuts down on the
     * final variables necessary in this single coherent unti, and should
     * be easier to garbage collect.
     */
    private class NewBookmarkDialog {
        private BookmarkLoader bLoader;
        private BookmarkEditor bmark;
        private Bookmarks.Bookmark bookmark;
        private JDialog dialog;

        private MutableTreeNode parent;
        private BookmarksTree tree;

        NewBookmarkDialog(
            BookmarksTree treee,
            MutableTreeNode prent,
            String name,
            String location,
            boolean userBookmark,
            BookmarkLoader bl) {
            JPanel box = new JPanel(new BorderLayout(0, 5));
            String name2 = name;
            bLoader = bl;

            box.add(BorderLayout.NORTH, new JLabel(getResource(ADD_TEXT)));
            parent = prent;
            tree = treee;
            if ((name2 == null) || name2.trim().equals("")) {
                name2 = location;
                if ((name2 == null) || name2.trim().equals("")) {
                    name2 = DEF_TITLE;
                }
            }
            bookmark =
                new Bookmarks.Bookmark(
                    name2,
                    location,
                    "",
                    bLoader,
                    userBookmark);
            dialog = ApplicationDialogs.createDialog("Add Bookmark", true);
            bmark = getBookmarkEditor(bookmark);
            box.add(BorderLayout.CENTER, bmark);
            // Initialize actions appropriately

            Action ok = new AbstractAction(ApplicationDialogs.getOkayLabel()) {
                public void actionPerformed(ActionEvent e) {
                        // Commit property values and then close dialog.
    bmark.stopEditing();
                    // this will save undo information
                    addBookmarkToTree(tree, parent, bookmark);
                    dialog.dispose();
                    bmark = null;
                    dialog = null;
                    bookmark = null;
                    parent = null;
                }
            };

            Action cancel =
                new AbstractAction(ApplicationDialogs.getCancelLabel()) {
                public void actionPerformed(ActionEvent e) {
                        // Close dialog without committing any property values.
    bmark.cancelEditing();
                    dialog.dispose();
                    bmark = null;
                    dialog = null;
                    bookmark = null;
                    parent = null;
                }
            };
            Action[] actions = { ok, cancel };
            // layout the dialog
            LookFactory.layoutDialog(dialog, box, actions, cancel);
            dialog.toFront();
        }

        JDialog getDialog() {
            return dialog;
        }

    }

    private class RedoAction extends AbstractAction {
        RedoAction() {
            super("Redo");
            putValue(AbstractAction.SHORT_DESCRIPTION, "Redo");
            putValue(AbstractAction.NAME, "Redo");
        }
        public void actionPerformed(ActionEvent event) {
            try {
                undoManager_.redo();
            } catch (Exception e) {
                if (verbose)
                    e.printStackTrace(SYSERR);
            }
        }
    }

    private class UndoAction extends AbstractAction {
        UndoAction() {
            super("Undo");
            putValue(AbstractAction.SHORT_DESCRIPTION, "Undo");
            putValue(AbstractAction.NAME, "Undo");
        }
        public void actionPerformed(ActionEvent event) {
            try {
                undoManager_.undo();
            } catch (Exception e) {
                if (verbose)
                    e.printStackTrace(SYSERR);
            }
        }
    }

    static {
        AppEnvironment.addResources(
            "resources/BookmarkView.properties",
            BookmarkView.class);
    }
    private static final DataFlavor bookmarkFlavor =
        new DataFlavor(
            com.nokia.wtk.bookmarks.Bookmarks.Bookmark.class,
            "Bookmark");
    private static JTextField categoryTxt = new RCEMSTextField(20);
    private static final Clipboard clipboard_ =
        java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
    private final static String DEF_TITLE = getResource(BOOKMARK_DEF_TITLE);

    private static final String MODEL = "USER_TREE_MODEL";

    private static final String newURL = "http://";

    // why is these static?
    private static PropertySet props;
    private static final String ROOT_ICON_PROPERTY = "bookmarks.root.icon";
    private static final String SAMPLE_MODEL = "SAMPLE_TREE_MODEL";
    private static final String SAMPLES_CHANGE_DONT_WARN_PROPERTY =
        "bookmarks.dont-warn-about-samples";
    private static final String SAMPLES_VISIBLE_PROPERTY =
        "bookmarks.samples-visible";

    public static final String SELECTION_MADE_PROPERTY =
        "bookmarkViewSelectionMade";

    private static JMenu subMenu = null;
    private static final String VIEW_TITLE =
        getResource("bookmarks.view.title");

    private static AskSamplesWarning gAskSamplesWarning = null;

    /**
     * return a BookmarkEditor object for the specific bookmark.
     * @param bookmark the bookmark to set inside the new BookmarkEditor.
     * if null, nothing gest set.
     * @return new BookmarkEditor object for happy usage.
     */
    private static BookmarkEditor getBookmarkEditor(Bookmarks.Bookmark bmark) {
        BookmarkEditor ret = new BookmarkEditor();
        if (bmark != null) {
            ret.setBookmark(bmark);
        }
        return ret;
    }

    public static PropertySet getProperties() {
        if (props == null) {
            props = new PropertySet();
            props.addProperty(
                SAMPLES_VISIBLE_PROPERTY,
                Boolean.class,
                Boolean.TRUE,
                true);
            props.addProperty(
                SAMPLES_CHANGE_DONT_WARN_PROPERTY,
                Boolean.class,
                Boolean.FALSE,
                true);

        }
        return props;
    }

    /**
     * Get a String resource.
     */
    static String getResource(String key) {
        return AppEnvironment.getString(key);
    }

    static String getTitle() {
        return VIEW_TITLE;
    }

    /**
     * Warn user that changes to samples directory may not be permanent.
     * @return true if change should go ahead and done, according to user,
     * false otherwise.
     */
    private static boolean warnUserAboutSamples() {
        //
        // If the property is true, it means the user DOESN'T want to see this
        //
        if (getProperties()
            .getBooleanValue(SAMPLES_CHANGE_DONT_WARN_PROPERTY, false)) {
            return true;
        }
        if (gAskSamplesWarning == null) {
            gAskSamplesWarning = new AskSamplesWarning();
        } else {
            gAskSamplesWarning.show();
        }
        return gAskSamplesWarning.getResult();
    }
    private Action copyAction = new AbstractAction() {

        public void actionPerformed(ActionEvent e) {
            DefaultMutableTreeNode node =
                (DefaultMutableTreeNode) tree_.getLastSelectedPathComponent();
            if (node == null) {
                if (samplesAvailable) {
                    node =
                        (DefaultMutableTreeNode) sampleTree_
                            .getLastSelectedPathComponent();
                }
            }
            addToClipboard(node);
        }
    };
    private Action cutAction = new AbstractAction() {

        public void actionPerformed(ActionEvent e) {
            DefaultMutableTreeNode node =
                (DefaultMutableTreeNode) tree_.getLastSelectedPathComponent();
            if (node == null) {
                if (samplesAvailable) {
                    node =
                        (DefaultMutableTreeNode) sampleTree_
                            .getLastSelectedPathComponent();
                }
            }
            if (doDelete(node)) {
                addToClipboard(node);
            }
        }
    };
    private Action deleteAction =
        new AbstractAction(getResource(DELETE_BUTTON_LABEL)) {

        {
            putValue(
                Action.ACCELERATOR_KEY,
                KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_DELETE, 0));
        }
        public void actionPerformed(ActionEvent event) {
            if (selectedTree != null) {
                doDelete(
                    (DefaultMutableTreeNode) selectedTree
                        .getLastSelectedPathComponent());
            }
        }
    };

    private StateEdit edit_ = null;

    private Action editAction =
        new AbstractAction(getResource(EDIT_BUTTON_LABEL)) {
        public void actionPerformed(ActionEvent event) {
            newBookmark = false;
            doEdit();
        }
    };
    private Editor editor = new Editor();
    private Action goAction =
        new AbstractAction(getResource(GO_BUTTON_LABEL)) {
        {
            putValue(
                Action.ACCELERATOR_KEY,
                KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_ENTER, 0));
        }
        public void actionPerformed(ActionEvent event) {
            if (selectedTree != null) {
                doGo(
                    (DefaultMutableTreeNode) selectedTree
                        .getLastSelectedPathComponent());
            }
        }
    };

    private BookmarkLoader loader;

    private boolean newBookmark = false;
    private Action newBookmarkAction_ =
        new AbstractAction(getResource(NEW_BOOKMARK_BUTTON_LABEL)) {
        public void actionPerformed(ActionEvent e) {
            newBookmark = true;
            if (selectedTree != null) {
                addBookmark(
                    selectedTree,
                    (MutableTreeNode) selectedTree
                        .getLastSelectedPathComponent(),
                    DEF_TITLE,
                    newURL);
            }
        }
    };
    private Action newFolderAction_ =
        new AbstractAction(getResource(NEW_FOLDER_BUTTON_LABEL)) {
        public void actionPerformed(ActionEvent e) {
            newBookmark = true;
            addFolder(
                (MutableTreeNode) selectedTree.getLastSelectedPathComponent(),
                "New Category");
        }
    };
    private Action pasteAction = new AbstractAction() {

        public void actionPerformed(java.awt.event.ActionEvent event) {
            Bookmarks.Bookmark bi = null; // get from clipboard
            Transferable t = null;
            try {
                t = clipboard_.getContents(this);
                if ((t != null) && (t.isDataFlavorSupported(bookmarkFlavor))) {
                    bi = (Bookmarks.Bookmark) t.getTransferData(bookmarkFlavor);
                }
            } catch (ClassCastException c) {
                System.err.println(
                    "Bookmark paste: wrong class returned by transferable? "
                        + t);
            } catch (IOException i) {} catch (UnsupportedFlavorException u) {
                System.err.println(
                    "Bookmark paste: wrong flavor supported by transferable? "
                        + t);
            } finally {
                if ((bi != null) && (selectedTree != null)) {
                    addBookmarkToTree(
                        selectedTree,
                        (MutableTreeNode) selectedTree
                            .getLastSelectedPathComponent(),
                        bi);
                }
            }
        }

    };
    private Action redoAction_ = new RedoAction();
    private Action undoAction_ = new UndoAction();

    private Action[] rightClickActions =
        {
            undoAction_,
            redoAction_,
            deleteAction,
            goAction,
            editAction,
            null,
            cutAction,
            copyAction,
            pasteAction,
            null,
            newBookmarkAction_,
            newFolderAction_ };
    private boolean samplesAvailable = true;
    private BookmarksTree sampleTree_;
    private BookmarksTree selectedTree;
    private JPanel split = new JPanel(new BorderLayout());
    private UndoableEditSupport support_;
    private BookmarksTree tree_;

    protected UndoManager undoManager_ = new UndoManager() {
        public void redo() throws CannotRedoException {
            super.redo();
            setUndoRedo();
            saveBookmarks(selectedTree);
        }
        public void undo() throws CannotRedoException {
            super.undo();
            setUndoRedo();
            saveBookmarks(selectedTree);
        }
        public void undoableEditHappened(UndoableEditEvent e) {
            super.undoableEditHappened(e);
            setUndoRedo();
            saveBookmarks(selectedTree);
        }
    };

    /**
     * Create a GUI for the bookmarks.
     */
    BookmarkView(BookmarkLoader c) {
        super(new BorderLayout());
        loader = c;
        Bookmarks.setBookmarkLoader(loader);
        JScrollPane sp = new JScrollPane(split);
        this.add(sp, BorderLayout.CENTER);
        support_ = new UndoableEditSupport(this);
        tree_ = new BookmarksTree(Bookmarks.getUserModel(), this, true);

        //create another tree for sample nodes
        try {
            sampleTree_ =
                new BookmarksTree(Bookmarks.getSampleModel(), this, false);
            sampleTree_.setBorder(BorderFactory.createEmptyBorder(20, 0, 0, 0));
        } catch (SamplesNotFoundException e) {
            //e.printStackTrace();
            samplesAvailable = false;
        }

        //assign a default value for bookmarkType
        selectedTree = tree_;
        TreeSelectionListener tsl = new TreeSelectionListener() {
            public void valueChanged(TreeSelectionEvent e) {
                    //detect which tree model is being selected, if nothing is selected, use tree_ as default.
    BookmarksTree bk = (BookmarksTree) e.getSource();
                if ((bk == null) || bk.isMutable()) {
                    if (samplesAvailable)
                        sampleTree_.clearSelection();
                    selectedTree = tree_;
                } else {
                    tree_.clearSelection();
                    if (samplesAvailable)
                        selectedTree = sampleTree_;
                }
                enableAppropriateActions();
            }
        };

        tree_.addTreeSelectionListener(tsl);
        if (samplesAvailable) {
            sampleTree_.addTreeSelectionListener(tsl);
        }
        addUndoableEditListener(undoManager_);
        setUndoRedo();

        updateTreesVisible();
        getProperties()
            .addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                updateTreesVisible();
            }
        });

        // Add a control panel:
        add(createControlPanel(), BorderLayout.NORTH);

        MouseListener editGoListener = new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                JTree tree = (JTree) e.getSource();
                int selRow = tree.getRowForLocation(e.getX(), e.getY());
                if ((selRow != -1) && (e.getClickCount() >= 2)) {
                    DefaultMutableTreeNode node =
                        (DefaultMutableTreeNode) tree
                            .getLastSelectedPathComponent();
                    if (node.isLeaf() == true) {
                        if (isReadyToGo(node)) {
                            doGo(node);
                        } else {
                            doEdit();
                        }
                    }
                }
            }
        };

        tree_.addMouseListener(editGoListener);
        
        if (tree_ != null) {
            ActionsUtilities.manageEMS(this, tree_);
        }
        if (samplesAvailable) {
            ActionsUtilities.manageEMS(this, sampleTree_);
            sampleTree_.addMouseListener(editGoListener);
        }
    }
    /**
     * Adds a bookmark to a specified tree.
     * This will prompt the user for more information before adding the
     * bookmark, giving the user a chance ot bail out.
     */
    private void addBookmark(
        BookmarksTree tree,
        final MutableTreeNode parent,
        String name,
        String location) {
        if (samplesAvailable
            && (tree == sampleTree_)
            && (!warnUserAboutSamples())) {
            return;
        }
        JDialog dialog =
            (new NewBookmarkDialog(tree,
                parent,
                name,
                location,
                tree.isMutable(),
                loader))
                .getDialog();
        // Show the dialog:
        dialog.setResizable(false);
        ApplicationDialogs.invokeModalDialog(dialog);
    }
    /**
     * Adds a bookmark to the public tree
     * This will prompt the user for more information before adding the
     * bookmark, giving the user a chance ot bail out.
     */
    void addBookmark(String name, String location) {
        JDialog dialog =
            (new NewBookmarkDialog(tree_, null, name, location, true, loader))
                .getDialog();
        // Show the dialog:
        dialog.setResizable(false);
        ApplicationDialogs.invokeModalDialog(dialog);
    }

    /**
     * Adds a new bookmark to the actual tree
     */
    private void addBookmarkToTree(
        BookmarksTree tree,
        MutableTreeNode parent,
        Bookmarks.Bookmark bm) {

        if ((!tree.isMutable()) && (!BookmarkView.warnUserAboutSamples())) {
            return;
        }

        StateEdit edit = new StateEdit(this, "New Bookmark");

        MutableTreeNode thing = Bookmarks.addBookmark(parent, bm);
        TreePath newSelect = new TreePath(Bookmarks.getPathToRoot(thing));
        tree.makeVisible(newSelect);
        tree.setSelectionPath(newSelect);
        tree.scrollPathToVisible(newSelect);
        edit.end();
        support_.postEdit(edit);
    }

    /**
     * Adds a folder.
     * This will prompt the user for more information before adding the
     * bookmark, giving the user a chance ot bail out.
     */
    private void addFolder(final MutableTreeNode parent, String name) {
        TreeNode root = null;
        for (TreeNode test = parent; test != null; test = test.getParent()) {
            root = test;
        }
        if (samplesAvailable
            && (sampleTree_.getModel().getRoot() == root)
            && (!warnUserAboutSamples())) {
            return;
        }

        String name2 = name;

        JPanel box = new JPanel(new BorderLayout(0, 5));
        box.add(BorderLayout.NORTH, new JLabel(getResource(ADD_FOLDER_TEXT)));

        if ((name2 == null) || name2.trim().equals("")) {
            name2 = DEF_TITLE;
        }
        final JDialog dialog =
            ApplicationDialogs.createDialog("Add Folder", true);
        box.add(BorderLayout.CENTER, categoryTxt);
        // Initialize actions appropriately

        Action ok = new AbstractAction(ApplicationDialogs.getOkayLabel()) {
            public void actionPerformed(ActionEvent e) {
                    // this will save undo information
    addFolderToTree(parent, categoryTxt.getText());
                dialog.dispose();
            }
        };

        Action cancel =
            new AbstractAction(ApplicationDialogs.getCancelLabel()) {
            public void actionPerformed(ActionEvent e) {
                    // Close dialog without committing any property values.
    dialog.dispose();
            }
        };
        Action[] actions = { ok, cancel };
        // layout the dialog
        LookFactory.layoutDialog(dialog, box, actions, cancel);

        // Show the dialog:
        dialog.setResizable(false);
        ApplicationDialogs.invokeModalDialog(dialog);
    }

    /**
     * Adds a new bookmark
     */
    private void addFolderToTree(MutableTreeNode parent, String title) {
        StateEdit edit = new StateEdit(this, "New Folder");
        MutableTreeNode thing = Bookmarks.addFolder(parent, title);
        TreePath newSelect = new TreePath(Bookmarks.getPathToRoot(thing));
        if (selectedTree != null) {
            selectedTree.makeVisible(newSelect);
            selectedTree.setSelectionPath(newSelect);
        }
        edit.end();
        support_.postEdit(edit);
    }

    // Will refuse to add folders to the clipboard, so only call with tree nodes
    // with bookmarks attached
    private void addToClipboard(DefaultMutableTreeNode selected) {
        try {
            Bookmarks.Bookmark bk =
                (Bookmarks.Bookmark) selected.getUserObject();
            if (bk != null) {
                Transferable i = new BookmarkTransferable(bk, loader);
                clipboard_.setContents(i, this);
                pasteAction.setEnabled(true);
            }
        } catch (ClassCastException c) {} catch (NullPointerException np) {}

    }

    public void addUndoableEditListener(UndoableEditListener l) {
        support_.addUndoableEditListener(l);
    }

    public void buildMenu(JPopupMenu jMenu) {
        buildMenu(jMenu, (TreeNode) Bookmarks.getUserModel().getRoot());
        if (samplesAvailable) {
            try {
                TreeNode tn = (TreeNode) Bookmarks.getSampleModel().getRoot();
                buildMenu(jMenu, tn);
            } catch (SamplesNotFoundException s) {
                //s.printStackTrace();
                // Samples not found, so no menu added to popup
                samplesAvailable = false;
            }
        }
    }

    private void buildMenu(JPopupMenu menu, TreeNode node) {
        int n = node.getChildCount();
        if (n == 0) {
            JMenuItem empty = new JMenuItem("(Empty)");
            empty.setEnabled(false);
            menu.add(empty);
        } else {
            //if the node is root node, then append it in the menu
            JMenu rootMenu = null;
            if (node.getParent() == null) {
                rootMenu =
                    new JMenu(
                        (String) ((DefaultMutableTreeNode) node)
                            .getUserObject());
                menu.add(rootMenu);
            }
            for (int i = 0; i < node.getChildCount(); i++) {
                TreeNode child = (TreeNode) node.getChildAt(i);
                if (child instanceof DefaultMutableTreeNode) {
                    DefaultMutableTreeNode o = (DefaultMutableTreeNode) child;
                    Object obj = o.getUserObject();
                    if (obj instanceof Bookmarks.Bookmark) {
                        Bookmarks.Bookmark bm = (Bookmarks.Bookmark) obj;
                        if (rootMenu != null) {
                            rootMenu.add(bm.getAction());
                        } else {
                            menu.add(bm.getAction());
                        }
                        //otherwise it is a bookmark folder
                    } else {
                        subMenu = new JMenu((String) o.getUserObject());
                        if (rootMenu != null) {
                            rootMenu.add(subMenu);
                        } else {
                            menu.add(subMenu);
                        }
                        buildMenu(subMenu.getPopupMenu(), child);
                    }
                }
            }
        }
    }

    /**
     * Create a control panel for the Bookmarks view.
     * @return the control panel
     */
    private JPanel createControlPanel() {
        Bookmarks
            .BookmarkCellRenderer
            .getExpandedProperty()
            .addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                SwingUtilities.updateComponentTreeUI(tree_);
                if (samplesAvailable) {
                    SwingUtilities.updateComponentTreeUI(sampleTree_);
                }
            }
        });

        JPanel controlPane = new JPanel();
        controlPane.setBorder(LookFactory.mediumBorder());
        MenuButton newRollover = new MenuButton(getResource(NEW_BUTTON_LABEL));
        JPopupMenu newMenu = newRollover.getPopupMenu();
        newMenu.add(newBookmarkAction_);
        newMenu.add(newFolderAction_);

        controlPane.setLayout(new BoxLayout(controlPane, BoxLayout.X_AXIS));

        editAction.setEnabled(false);
        deleteAction.setEnabled(false);
        goAction.setEnabled(false);

        controlPane.add(createEditPanel());
        controlPane.add(Box.createHorizontalStrut(LookFactory.getGap()));
        controlPane.add(new JButton(goAction));
        controlPane.add(new JButton(editAction));
        controlPane.add(Box.createHorizontalStrut(LookFactory.getGap()));
        controlPane.add(newRollover);
        controlPane.add(Box.createHorizontalStrut(LookFactory.getGap()));
        controlPane.add(new JButton(deleteAction));
        controlPane.add(Box.createHorizontalGlue());
        MenuButton mb = new MenuButton(getResource(OPTIONS_BUTTON_LABEL));
        JCheckBox bk =
            (JCheckBox) PropertyEditors.createCheckBox(
                Bookmarks.BookmarkCellRenderer.getExpandedProperty(),
                getResource(VIEW_LOCATIONS_LABEL));
        mb.getPopupMenu().add(bk);
        if (samplesAvailable) {
            bk =
                (JCheckBox) PropertyEditors.createCheckBox(
                    getProperties().getProperty(SAMPLES_VISIBLE_PROPERTY),
                    getResource(VIEW_SAMPLES_LABEL));
            mb.getPopupMenu().add(bk);
        }
        controlPane.add(mb);

        return controlPane;
    }

    /**
     * Creates the toolbar edit panel.
     * @return the toolbar
     */
    private JComponent createEditPanel() {
        final JToolBar editPanel = new JToolBar();
        editPanel.setBorder(new EmptyBorder(new Insets(0, 0, 0, 0)));
        editPanel.setFloatable(false);

        /**
         * Button with no text
         */
        class NullTextButton extends JButton {
            NullTextButton(Action a) {
                super(a);
                super.setText(null);
                setMargin(new Insets(0, 0, 0, 0));
                setBorderPainted(false);
            }
            /**
             * Overides JButton setText so that changes in Action does not affect
             * the toolbar buttons text
             */
            public void setText(String s) {
                // ignore the text
            }
        }

        /* Add toolbar buttons */
        editPanel.addSeparator();
        JButton b = new NullTextButton(undoAction_);
        editPanel.add(b);
        b = new NullTextButton(redoAction_);
        editPanel.add(b);

        editPanel.addSeparator();
        b = new NullTextButton(cutAction);
        editPanel.add(b);
        b = new NullTextButton(copyAction);
        editPanel.add(b);
        b = new NullTextButton(pasteAction);
        editPanel.add(b);
        editPanel.addSeparator();
        return editPanel;
    }

    /**
     * Delete the selected bookmark
     */
    private synchronized boolean doDelete(DefaultMutableTreeNode selected) {
        if (samplesAvailable
            && (selectedTree == sampleTree_)
            && (!warnUserAboutSamples())) {
            return false;
        }
        if (selected != null) {
            StateEdit edit = new StateEdit(this, "Delete");
            Bookmarks.remove(selected);
            edit.end();
            support_.postEdit(edit);
            enableAppropriateActions();
            return true;
        }
        enableAppropriateActions();
        return false;
    }

    /**
     * Start editing the selected bookmark.
     */
    private void doEdit() {
        if (selectedTree != null) {
            TreePath path = selectedTree.getLeadSelectionPath();
            if (path != null) {
                selectedTree.startEditingAtPath(path);
            }
        }
    }

    /**
     * Go to the selected bookmark
     */
    private synchronized void doGo(DefaultMutableTreeNode node) {
        // allowGo might have been set to false from another thread before
        // Swing had a chance to catch up with it
        if (!isLoadSupported()) {
            goAction.setEnabled(false);
            return;
        }

        Object obj = node.getUserObject();
        if (obj instanceof Bookmarks.Bookmark) {
            Bookmarks.Bookmark bk = (Bookmarks.Bookmark) obj;
            bk.getAction().actionPerformed(null);
        }
    }

    /** Enabled or disables the appropriate actions and buttosn based on the
     * state of the selected node in the tree.
     */
    private void enableAppropriateActions() {
        DefaultMutableTreeNode o = null;
        if (selectedTree != null) {
            o =
                (DefaultMutableTreeNode) selectedTree
                    .getLastSelectedPathComponent();
        }
        if (o == null) {
            goAction.setEnabled(false);
            deleteAction.setEnabled(false);
        } else {
            goAction.setEnabled(
                isReadyToGo(o) && isLoadSupported() && !o.getAllowsChildren());
            deleteAction.setEnabled(!o.isRoot());
        }
        pasteAction.setEnabled(o != null);
        editAction.setEnabled((o != null) && (!o.isRoot()));
        copyAction.setEnabled((o != null) && (!o.getAllowsChildren()));
        cutAction.setEnabled((o != null) && (!o.getAllowsChildren()));
    }

    BookmarkLoader getBookmarkLoader() {
        return loader;
    }
    public Action getCopyAction() {
        return copyAction;
    }
    public Action getCutAction() {
        return cutAction;
    }
    public Action getFindAction() {
        return null;
    }

    public JPopupMenu getJPopupMenu() {
        JPopupMenu menu = new JPopupMenu();
        buildMenu(menu);
        return menu;
    }
    public Action getPasteAction() {
        return pasteAction;
    }
    public Action getRedoAction() {
        return redoAction_;
    }

    /** Required implementation for RightClickMenuSystem.RightClickMenuSUpport **/
    public Object[] getRightClickActions(MouseEvent e) {

        // select what was clicked on, if anything. Right-click will not
        // have selected an item, eventhough user expects it.
        // selection is cleared because none of the menu-items work for
        // multiple nodes anyway.
        tree_.clearSelection();
        if (samplesAvailable) {
            sampleTree_.clearSelection();
        }
        // select path
        BookmarksTree c =
            (BookmarksTree) SwingUtilities.getDeepestComponentAt(
                (Component) e.getSource(),
                e.getX(),
                e.getY());

        TreePath z = c.getPathForLocation(e.getX(), e.getY());

        if (z != null) {
            c.addSelectionPath(z);
        }
        return rightClickActions;
    }

    public Action getUndoAction() {
        return undoAction_;
    }

    private boolean isLoadSupported() {
        return loader.isLoadSupported();
    }

    /**
     * Twhether the URL in the last node bookmark is valid.
     */
    boolean isReadyToGo(DefaultMutableTreeNode selected) {
        try {
            if (selected != null) {
                Object obj = selected.getUserObject();
                if (obj instanceof Bookmarks.Bookmark) {
                    Bookmarks.Bookmark bk = (Bookmarks.Bookmark) obj;
                    return bk.getAction().isEnabled();
                }
            }
        } catch (Exception x) {
            // ignore
        }
        return false;
    }

    // For ClipboardOwner interface
    public void lostOwnership(Clipboard clipboard, Transferable contents) {
        // We did? Darn.
    }

    private final DefaultMutableTreeNode makeNode(Object o) {
        DefaultMutableTreeNode ret = new DefaultMutableTreeNode();
        if (o instanceof Hashtable) {
            Hashtable h = (Hashtable) o;
            Enumeration e = h.keys();
            if (e.hasMoreElements()) {
                // better be true and better have only one single key
                Object key = e.nextElement();
                Vector v = (Vector) h.get(key);
                ret.setUserObject(key);
                ret.setAllowsChildren(true);
                for (e = v.elements(); e.hasMoreElements();) {
                    DefaultMutableTreeNode child = makeNode(e.nextElement());
                    ret.add(child);
                }
            }
        } else {
            ret.setUserObject(o);
            ret.setAllowsChildren(false);
        }
        return ret;
    }

    // Will be called when a drop has been done by the tree
    public void propertyChange(PropertyChangeEvent evt) {
        if (edit_ != null) {
            edit_.end();
            support_.postEdit(edit_);
        }
    }
    public void removeUndoableEditListener(UndoableEditListener l) {
        support_.removeUndoableEditListener(l);
    }

    /**
     * THIS WILL REPLACE THE MODEL THUS ALL LISTENERS ON THE MODEL
     * WILL PROBABLY BE LOST
     */
    public void restoreState(Hashtable state) {
        try {
            byte[] s = (byte[]) state.get(MODEL);
            ByteArrayInputStream istream = new ByteArrayInputStream(s);
            tree_.setModel(Bookmarks.loadUserBookmarks(istream));
            istream.close();
            if (samplesAvailable) {
                s = (byte[]) state.get(SAMPLE_MODEL);
                istream = new ByteArrayInputStream(s);
                sampleTree_.setModel(Bookmarks.loadSampleBookmarks(istream));
                istream.close();
            }
        } catch (Exception e) {
            SYSERR.println("Could not restore undone state: " + e);
            if (verbose)
                e.printStackTrace(SYSERR);
        }
    }

    private void saveBookmarks(BookmarksTree tree) {
        try {
            Bookmarks.saveBookmarks(tree.getModel());
        } catch (RuntimeException e) {
            System.err.println(
                "Could not save bookmarks. Message: " + e.getMessage());
        }
    }

    private final void setUndoRedo() {
        redoAction_.putValue(
            Action.NAME,
            undoManager_.getRedoPresentationName());
        redoAction_.setEnabled(undoManager_.canRedo());
        undoAction_.putValue(
            Action.NAME,
            undoManager_.getUndoPresentationName());
        undoAction_.setEnabled(undoManager_.canUndo());
    }

    public void storeState(Hashtable state) {
        try {
            ByteArrayOutputStream ostream = new ByteArrayOutputStream();
            Bookmarks.saveBookmarks(ostream, tree_.getModel());
            ostream.close();
            state.put(MODEL, ostream.toByteArray());
            if (samplesAvailable) {
                ostream = new ByteArrayOutputStream();
                Bookmarks.saveBookmarks(ostream, sampleTree_.getModel());
                ostream.close();
                state.put(SAMPLE_MODEL, ostream.toByteArray());
            }
        } catch (Exception e) {
            SYSERR.println("Could not create undoable state: " + e);
            if (verbose)
                e.printStackTrace(SYSERR);
        }
    }

    private void updateTreesVisible() {
        split.remove(tree_);
        if (samplesAvailable) {
            split.remove(sampleTree_);
        }
        if ((samplesAvailable)
            && getProperties().getBooleanValue(SAMPLES_VISIBLE_PROPERTY, false)) {
            split.add(tree_, BorderLayout.NORTH);
            split.add(sampleTree_, BorderLayout.CENTER);
        } else {
            split.add(tree_, BorderLayout.CENTER);
        }
        this.revalidate();
    }

    // Will be called when a drop is being proposed by the tree
    public void vetoableChange(PropertyChangeEvent evt)
        throws PropertyVetoException {
        edit_ = new StateEdit(this, "Move");
    }
}
