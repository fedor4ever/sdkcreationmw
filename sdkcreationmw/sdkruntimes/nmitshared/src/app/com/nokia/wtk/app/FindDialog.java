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



package com.nokia.wtk.app;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.lang.ref */
import java.lang.ref.Reference;
import java.lang.ref.WeakReference;

/* java.util.* */
import java.util.Vector;
import java.util.Enumeration;

/* java.awt */
import java.awt.Window;
import java.awt.Component;
import java.awt.BorderLayout;

/* java.awt.event */
import java.awt.event.KeyEvent;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentAdapter;

/* javax.swing */
import javax.swing.Action;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JDialog;
import javax.swing.JButton;
import javax.swing.UIManager;
import javax.swing.KeyStroke;
import javax.swing.JTextField;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.AbstractAction;
import javax.swing.SwingUtilities;

/* javax.swing.text */
import javax.swing.text.Document;
import javax.swing.text.JTextComponent;
import javax.swing.text.BadLocationException;

/* javax.swing.event */
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

/* javax.swing.border */
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.border.CompoundBorder;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Utils;
import com.nokia.wtk.util.PropUtils;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertyEditors;
import com.nokia.wtk.util.SingleLineDocument;

/**
 * The "Find and Replace" dialog.
 */
public class FindDialog extends JDialog implements ActionListener {

    private static final String FIND_TITLE = "Find";
    private static final String REPLACE_TITLE = "Find/Replace Text";

    // "single dialog" functionality
    private static Reference currentDialog = null;
    private static boolean forceSingleDialog = true;

    // instance variables
    private boolean loopedOnce = false;
    private int startPos = -1;
    private int startInd = 0;
    private int endInd = 0;

    private JButton fndButton;
    private JButton cancelButton;
    private JTextField findWhat;
    private JTextField chgWith;
    private JTextComponent textEditor;
    private String plaf = UIManager.getLookAndFeel().getClass().getName();
    private Vector undoManagers;
    private String lastSearch;
    private Action findNextAction;
    private Component centerComponent;

    /* property names */
    public static final String MATCH_CASE_PROPERTY = "match-case";
    public static final String FROM_TOP_PROPERTY = "from-top";
    public static final String BACKWARDS_PROPERTY = "backwards";

    /* contains above properties */
    private PropertySet prefs;
    private Property matchCaseProp;
    private Property backProp;
    private Property fromTopProp;

    /**
     * Some JTextComponents unfortunatly implement the
     * <code>replaceSelection()</code> such
     * that multiple UndoableEdits are generated, eventhough most humans
     * regard the replace as a single action. Therefore, this interafce.
     * A user of FindDialog can supply the FindDialog object with implementations
     * of ReplaceListener by calling <code>addReplaceListener</code>.
     * It is guaranteed that, whenever a <code>replaceSelection()</code> is
     * going to be performed, first <code>replaceStart()</code> will be called
     * on all supplied ReplaceListeners, and after the
     * <code>replaceSelection()</code>, <code>replaceEnd</code> will be called.
     *
     */
    public interface ReplaceListener {

        /**
         * Calling this method on an ReplaceListener should warn the ReplaceListener
         * that replacing is about to start, and that thus the following
         * UndoableEdits should be collapsed into one, as
         * they are all related to replacing a piece of text for another.
         */
        abstract public void replaceStart();

        /**
         * Tells the object that the replace operation has ended.
         */
        abstract public void replaceEnd();
    }

    /**
     * creates the Find and Replace window
     * @param tc The JTextComponent;
     */
    public FindDialog(JTextComponent tc) {
        super(JOptionPane.getFrameForComponent(tc), tc.isEditable() ? REPLACE_TITLE : FIND_TITLE, true);

        textEditor = tc;
        findWhat = new RCEMSTextField(15);
        findWhat.setDocument(new SingleLineDocument(findWhat.getDocument()));

        findNextAction = new AbstractAction("Find Next") {
            public void actionPerformed(ActionEvent e) {
                findNext();
            }
        };

        fndButton = new JButton(findNextAction);

        JButton chgButton = null;
        if (tc.isEditable()) {
            chgWith  = new RCEMSTextField(15);
            chgWith.setDocument(new SingleLineDocument(chgWith.getDocument()));
            chgButton = new JButton(new AbstractAction("Replace") {
                public void actionPerformed(ActionEvent e) {
                    String searchFor = findWhat.getText();
                    if (searchFor == null || searchFor.length() == 0) {
                        message("Cannot search for a blank string");
                    } else {
                        String s = textEditor.getSelectedText();
                        if (startInd != endInd && searchFor.equalsIgnoreCase(s)) {
                            textEditor.select(startInd, endInd);
                            textEditor.getCaret().setSelectionVisible(true);
                            FindDialog.this.warnReplaceListenersStart();
                            textEditor.replaceSelection(chgWith.getText());
                            FindDialog.this.warnReplaceListenersEnd();
                            endInd = textEditor.getCaretPosition();
                            textEditor.select(startInd, endInd);
                            findNext();
                        }
                    }
                }
            });
        }

        cancelButton = new JButton(new AbstractAction("Done") {
            public void actionPerformed(ActionEvent e) {
                startInd = endInd = 0;
                setVisible(false);
            }
        });

        // reset search context when user changes search options
        PropertyChangeListener resetListener = new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                if (isVisible()) {
                    getRootPane().setDefaultButton(fndButton);
                    reset();
                }
            }
        };

        prefs = new PropertySet();
        matchCaseProp = prefs.addBooleanProperty(MATCH_CASE_PROPERTY,false,true);
        fromTopProp = prefs.addBooleanProperty(FROM_TOP_PROPERTY,false,true);
        backProp = prefs.addBooleanProperty(BACKWARDS_PROPERTY,false,true);
        prefs.addPropertyChangeListener(resetListener);

        // also reset context when text in text editor changes
        textEditor.getDocument().addDocumentListener( new DocumentListener() {
            public void insertUpdate(DocumentEvent e) { reset(); }
            public void removeUpdate(DocumentEvent e)  { reset(); }
            public void changedUpdate(DocumentEvent e)  { reset(); }
        });

        // reset context and make "Find Next" a default button if text in
        // one of the edit fields changes
        DocumentListener findTextChangeListener = new DocumentListener() {
            public void insertUpdate(DocumentEvent e) { update(); }
            public void removeUpdate(DocumentEvent e)  { update(); }
            public void changedUpdate(DocumentEvent e)  { update(); }
            private void update() {
                if (isVisible()) {
                    getRootPane().setDefaultButton(fndButton);
                    reset();
                }
            }
        };

        findWhat.getDocument().addDocumentListener(findTextChangeListener);
        if (chgWith != null) {
            chgWith.getDocument().addDocumentListener(findTextChangeListener);
        }

        // initialize layout
        JPanel optPane = LookFactory.createRowPanel(new Component[] {
            PropertyEditors.createCheckBox(matchCaseProp,"Match case"), null,
            PropertyEditors.createCheckBox(backProp,"Search backwards"), null,
            PropertyEditors.createCheckBox(fromTopProp,"Search from start of file"), null
        },0);

        int gap = LookFactory.getGap();
        optPane.setBorder(new CompoundBorder(
                          new TitledBorder("Options"),
                          new EmptyBorder(0,gap,gap,gap)));

        JPanel findPane;
        JPanel btnPane;

        JLabel findLabel = new JLabel(" Find what: ", JLabel.RIGHT);
        if (tc.isEditable()) {
            findPane = LookFactory.createRowPanel(new Component[] {
                findLabel, findWhat,
                new JLabel(" Replace with: ", JLabel.RIGHT), chgWith,
                optPane, null
            });
            btnPane = LookFactory.createRowPanel(new Component[] {
                fndButton, null,
                chgButton, null,
                cancelButton, null
            });
        } else {
            findPane = LookFactory.createRowPanel(new Component[] {
                findLabel, findWhat,
                optPane, null
            });
            btnPane = LookFactory.createRowPanel(new Component[] {
                fndButton, null,
                cancelButton, null
            });
        }

        JPanel content = new JPanel(new BorderLayout(gap,gap));
        content.setBorder(LookFactory.mediumBorder());
        content.add(findPane, BorderLayout.CENTER);
        content.add(btnPane, BorderLayout.EAST);

        // Escape key closes the dialog
        String key = "cancel";
        content.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
        KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE,0),key);
        content.getActionMap().put(key,
            new AbstractAction(cancelButton.getText()) {
                public void actionPerformed(ActionEvent e) {
                    if (cancelButton.isEnabled()) {
                        cancelButton.doClick();
                    }
                }
            }
        );

        getContentPane().add(content);
        pack();
        setResizable(false);

        // when find dialog is visible, this listener listens for UI changes
        final PropertyChangeListener plafListener = new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                if ("lookAndFeel".equals(e.getPropertyName())) {
                    updateLookAndFeel();
                }
            }
        };

        // deregister look and feel listener when window is closed.
        // update look and feel and register the listener when window is opened
        addComponentListener(new ComponentAdapter() {
            public void componentShown(ComponentEvent e) {
                if (!plaf.equals(UIManager.getLookAndFeel().getClass().getName())) {
                    updateLookAndFeel();
                }
                UIManager.addPropertyChangeListener(plafListener);

                // if there's another Find dialog on the screen, close it
                // multiple Find dialogs look extremely confusing to me. SM
                if (forceSingleDialog) {
                    boolean createRef = true;
                    Reference ref = currentDialog;
                    if (ref != null) {
                        FindDialog findDialog = (FindDialog)ref.get();

                        // if the same dialog is being shown again after being
                        // hidden, there's no need to create a new reference
                        createRef = (findDialog != FindDialog.this);
                        if (findDialog != null &&
                            findDialog != FindDialog.this &&
                            findDialog.isVisible()) {

                            // hide the other dialog
                            findDialog.setVisible(false);
                        }
                    }
                    if (createRef) {
                        currentDialog = new WeakReference(FindDialog.this);
                    }
                }
            }
            public void componentHidden(ComponentEvent e) {
                UIManager.removePropertyChangeListener(plafListener);
            }
        });
    }

    /**
     * Sets the component to center against when the dialog becomes visible.
     * By default, the dialog centers itself relative to the parent frame
     * window or dialog.
     * @param c the component to center against.
     */
    public void centerRelativeTo(Component c) {
        centerComponent = c;
    }

    /**
     * Returns the "Find Next" action
     */
    public Action getFindNextAction() {
        return findNextAction;
    }

    /**
     * Returns the last search string. This can be used by the editors to
     * determine what the "Search Again" action should do. When search string
     * is <code>null</code> or empty, it should open the "Find" dialog.
     * Otherwise it should perform the "Find Next" action returned by
     * the {@link #getFindNextAction()} method.
     */
    public String getLastSearchString() {
        return lastSearch;
    }

    /**
     * Returns property set containing preferences for the find dialog
     * @return the dialog preferences
     */
    public PropertySet getPreferences() {
        return prefs;
    }

    private void warnReplaceListenersStart() {
        if (undoManagers == null) {
            return;
        }
        for (Enumeration e = undoManagers.elements(); e.hasMoreElements(); ) {
            ReplaceListener u = (ReplaceListener) e.nextElement();
            u.replaceStart();
        }
    }

    private void warnReplaceListenersEnd() {
        if (undoManagers == null) {
            return;
        }
        for (Enumeration e = undoManagers.elements(); e.hasMoreElements(); ) {
            ReplaceListener u = (ReplaceListener) e.nextElement();
            u.replaceEnd();
        }
    }

    /**
     * Add an ReplaceListener to listen when replacing starts and ends.
     * @param man An ReplaceListener that will be told when a term will be
     * replaced in the text editor.
     */
    public void addReplaceListener(ReplaceListener man) {
        if (undoManagers == null) {
            undoManagers = new Vector(1);
        }
        undoManagers.add(man);
    }

    /**
     * Remove an ReplaceListener supplied with addReplaceListener.
     * @param man the undoManager to remove. Do not call with null.
     */
    public void removeReplaceListener(ReplaceListener man) {
        if (undoManagers == null) {
            return;
        }
        undoManagers.remove(man);
    }

    /**
     * Updates L&F
     */
    private void updateLookAndFeel() {
        SwingUtilities.updateComponentTreeUI(this);
        plaf = UIManager.getLookAndFeel().getClass().getName();
        pack();
    }

    /**
     * Invalidates cached data
     */
    public void reset() {
        loopedOnce = false;
        startPos = -1;
    }

    public boolean isCaseSensitive() {
        return PropUtils.getValue(matchCaseProp,false);
    }

    public void setCaseSensitive(boolean sensitive) {
        matchCaseProp.setValue(Utils.getBoolean(sensitive));
    }

    public boolean isSearchBack() {
        return PropUtils.getValue(backProp,false);
    }

    public void setSearchBack(boolean back) {
        backProp.setValue(Utils.getBoolean(back));
    }

    public boolean isFromTop() {
        return PropUtils.getValue(fromTopProp,false);
    }

    public void setFromTop(boolean back) {
        fromTopProp.setValue(Utils.getBoolean(back));
    }

    /**
     * Makes the dialog visible
     */
    public void actionPerformed(ActionEvent event)  {
        if (isVisible()) {
            toFront();
        } else {

            // grab current selection and cursor position from the text editor
            endInd = textEditor.getCaretPosition();
            String selectedText = textEditor.getSelectedText();
            if (selectedText != null && selectedText.length() > 0) {
                findWhat.setText(selectedText);
                findWhat.selectAll();
            }

            // position ourselves relative to the parent frame if we are
            // not given the component to center against
            if (centerComponent != null) {
                setLocationRelativeTo(centerComponent);
            } else {
                Window frame = SwingUtilities.getWindowAncestor(textEditor);
                setLocationRelativeTo(frame);
            }
            setVisible(true);

            // set up initial focus and default button
            getRootPane().setDefaultButton(fndButton);
            findWhat.requestFocus();
            reset();

            // highlight selected text
            textEditor.getCaret().setSelectionVisible(true);
            startInd = textEditor.getSelectionStart();
            endInd = textEditor.getSelectionEnd();
        }
    }

    /**
     * Finds the text
     */
    public void findNext() {

        // NOTE: this method may be invoked when dialog is invisible.
        // In such case it has to behave slightly differently
        String searchFor;
        if (isVisible()) {
            searchFor = findWhat.getText();
            lastSearch = searchFor;
        } else {
            searchFor = lastSearch;
        }
        if (searchFor == null || searchFor.length() == 0) {
            if (isVisible()) {
                message("Cannot search for a blank string");
            } else {
                getToolkit().beep();
            }
        } else {

            // to highlight selected text
            textEditor.getCaret().setSelectionVisible(true);
            endInd = textEditor.getCaretPosition();

            String selectedText = textEditor.getSelectedText();
            if (selectedText != null) {
                startInd = textEditor.getSelectionStart();
                endInd = textEditor.getSelectionEnd();
            } else {
                startInd = endInd;
            }

            if (startPos < 0) {
                if (isFromTop()) {
                    startInd = endInd = 0;
                }
                startPos = startInd;
            }

            // find() assumes that startInd, endInd and startPos have been
            // set up and it can call itself
            find(searchFor);
        }
    }

    /**
     * searches for the next occurence of the specified string
     * @return <code>true</code> if text has been found
     */
    private boolean find(String searchFor) {

        Document doc = textEditor.getDocument();
        String originalSearchFor = searchFor;
        String searchIn;
        String allText;

        // NOTE: we must take the text directly from the document rather than
        // from the text component. The text we get from the text component
        // may have \n replaced with \r\n in which case a position in the
        // string won't match the document position. And it's faster, too.
        try { allText = doc.getText(0,doc.getLength()); }
        catch (BadLocationException x) {
            x.printStackTrace();
            return false;
        }

        if (isCaseSensitive()) {
            searchIn = allText;
        } else {
            searchIn = allText.toUpperCase();
            searchFor = searchFor.toUpperCase();
        }

        int foundAt;
        if (isSearchBack()) {
            foundAt = searchIn.lastIndexOf(searchFor, startInd-1);
            if (loopedOnce && foundAt <= startPos) foundAt = -1;
        } else {
            foundAt = searchIn.indexOf(searchFor, endInd);
            if (loopedOnce && foundAt >= startPos) foundAt = -1;
        }

        if (foundAt > -1)  {
            startInd = foundAt;
            endInd = startInd + searchFor.length();
            textEditor.select(startInd, endInd);
            return true;
        } else {
            if (loopedOnce) {
                message("String '" + originalSearchFor + "' not found");
                findWhat.requestFocus();
                getRootPane().setDefaultButton(cancelButton);
                reset();
                return false;
            } else {
                loopedOnce = true;
                if (isSearchBack()) {
                    startInd = endInd = allText.length();
                } else {
                    startInd = endInd = 0;
                }
                return find(originalSearchFor);
            }
        }
    }

    /**
     * Displays a message box
     */
    private void message(String msg) {
        Window w;
        if (isVisible()) {
            w = this;
        } else {
            w = SwingUtilities.getWindowAncestor(textEditor);
        }
        JOptionPane.showMessageDialog(w,msg,getTitle(),JOptionPane.INFORMATION_MESSAGE);
    }
}

