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





package com.nokia.wtk.diagnostics.gui.log;

/* java.io */
import java.io.File;
import java.io.Writer;
import java.io.PrintWriter;
import java.io.FileOutputStream;

/* java.awt */
import java.awt.Color;

/* java.awt.event */
import java.awt.event.MouseEvent;
import java.awt.event.ActionEvent;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.Action;
import javax.swing.JComponent;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JEditorPane;
import javax.swing.JFileChooser;
import javax.swing.AbstractAction;
import javax.swing.SwingUtilities;

/* javax.swing.event */
import javax.swing.event.CaretEvent;
import javax.swing.event.CaretListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

/* javax.swing.text */
import javax.swing.text.Caret;
import javax.swing.text.Element;
import javax.swing.text.Document;
import javax.swing.text.EditorKit;
import javax.swing.text.AttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledEditorKit;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.MutableAttributeSet;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Trace;
import com.nokia.wtk.util.PropUtils;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.MessageLog;

/* com.nokia.wtk.util.gui */
import com.nokia.wtk.util.gui.GUI;
import com.nokia.wtk.util.gui.MenuButton;

/* com.nokia.wtk.app */
import com.nokia.wtk.app.FindDialog;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.EditMenuSupport;
import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.RightClickMenuSupport;

/* com.nokia.wtk.diagnostics */
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;
import com.nokia.wtk.diagnostics.views.support.Misc;

public class MessageLogView
    extends JScrollPane
    implements AbstractViewFactoryInterface,
               ViewAndControlsInterface,
               EditMenuSupport,
               Resource {

    static {
        AppEnvironment.addResources(
            "resources/MessageLogView.properties",
            MessageLogView.class);
    }

    /** default max number of characters */
    private static final int DEFAULT_MAX_SIZE = 100000;

    private static EditorKit editorKit;

    private PropertySet prefs;
    private Property sizeLimitProp;

    private LogSummaryView summaryView;
    private InterViewCommunicator communicator;
    private FindDialog findDialog;
    private Action clearAction;
    private Action copyAction;
    private Action findAction;
    private Action logToFileAction;
    private WriterLog defaultLog;
    private String viewTitle;
    private String viewDescription;
    private Icon logToFileIcon;

    private PrintWriter fileWriter;
    private File logFile;
    private char lastFileChar;

    private MenuButton menuButton;
    private LogToFileLabel controlsPanel;
    private JEditorPane textViewer;

    public MessageLogView(String title, String description) {
        prefs = new PropertySet();
        sizeLimitProp = prefs.addIntProperty("size-limit",DEFAULT_MAX_SIZE,true);

        viewTitle = title;
        viewDescription = description;
        logToFileIcon = AppEnvironment.getIcon(LOG_TO_FILE_ICON);

        controlsPanel = new LogToFileLabel();
        textViewer = new Viewer();
        textViewer.setFont(LookFactory.getStandardCodeFont());
        textViewer.getDocument().addDocumentListener(new TextChangeListener());
        textViewer.addCaretListener(new CaretListener() {
            public void caretUpdate(CaretEvent e) {
                updateCopyAction();
            }
        });

        setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        setViewportView(textViewer);
    }

    public MessageLogView(Browser b, String title, String description) {
        this(title, description);
        b.registerHostService(
            com.nokia.wtk.device.DeviceMessageHandler.class,
            new DeviceMessageHandler(this));
    }

    public MessageLogView(Browser b) {
        this(b, null, null);
    }

    public MessageLogView() {
        this(null, null);
    }

    private static EditorKit getEditorKit() {
        if (editorKit == null) {
            editorKit = new StyledEditorKit();
        }
        return editorKit;
    }

    /**
     * Returns the max configured number of characters in the log.
     */
    private int getMaxSize() {
        return PropUtils.getValue(sizeLimitProp, DEFAULT_MAX_SIZE);
    }

    private boolean isSizeLimited() {
        return getMaxSize() > 0;
    }

    /**
     * returns "Find and Replace" dialog for this editor
     */
    private FindDialog getFindDialog() {
        if (findDialog == null) {
            findDialog = new FindDialog(textViewer);
            findDialog.centerRelativeTo(this);
            getViewProperties().addProperties("find",findDialog.getPreferences(),true);

            // Hide the find dialog when this pane becomes invisible
            addHierarchyListener(new HierarchyListener() {
                public void hierarchyChanged(HierarchyEvent e) {
                    if (findDialog.isVisible() && !isShowing()) {
                        findDialog.setVisible(false);
                    }
                }
            });
        }
        return findDialog;
    }

    /**
     * Clear the text in the text viewer.
     */
    private synchronized void doClear() {
        summaryView.clear();

        Document doc = textViewer.getDocument();
        int len = doc.getLength();
        if (len > 0) {
            try { doc.remove(0, len); }
            catch (Exception x) {}
        }

        // notify the interview communicator that
        // the log view has been cleared
        if (communicator != null) {
          communicator.logViewCleared();
        }
    }

    private Action getClearAction() {
        if (clearAction == null) {
            Icon icon = AppEnvironment.getIcon(CLEAR_ACTION_ICON);
            String clearLabel = AppEnvironment.getString(CLEAR_ACTION_LABEL);
            String tip = AppEnvironment.getString(CLEAR_ACTION_TOOLTIP);
            clearAction = new AbstractAction(clearLabel) {
                { setEnabled(textViewer.getDocument().getLength() > 0); }
                public void actionPerformed(ActionEvent e) {
                    doClear();
                }
            };
            clearAction.putValue(Action.SHORT_DESCRIPTION, tip);
            clearAction.putValue(Action.SMALL_ICON, icon);
        }
        return clearAction;
    }

    private Action getLogToFileAction() {
        if (logToFileAction == null) {
            logToFileAction = new AbstractAction() {
                private String activeTip;
                private String activeLabel;
                private String inactiveTip;
                private String inactiveLabel;
                private JFileChooser chooser;
                {
                    activeTip = AppEnvironment.getString(LOG_TO_FILE_ACTION_ACTIVE_TOOLTIP);
                    activeLabel = AppEnvironment.getString(LOG_TO_FILE_ACTION_ACTIVE_LABEL);
                    inactiveTip = AppEnvironment.getString(LOG_TO_FILE_ACTION_INACTIVE_TOOLTIP);
                    inactiveLabel = AppEnvironment.getString(LOG_TO_FILE_ACTION_INACTIVE_LABEL);
                    putValue(Action.SMALL_ICON, AppEnvironment.getIcon(LOG_TO_FILE_ACTION_ICON));
                    putValue(Action.NAME, inactiveLabel);
                    putValue(Action.SHORT_DESCRIPTION, inactiveTip);
                }
                public void actionPerformed(ActionEvent e) {
                    try {
                        if (fileWriter == null) {
                            if (chooser == null) {
                                chooser = new JFileChooser();
                                chooser.setDialogTitle(inactiveLabel);
                            }
                            int option = chooser.showSaveDialog(getPodContent());
                            if (option == JFileChooser.APPROVE_OPTION) {
                                File f = chooser.getSelectedFile().getAbsoluteFile();
                                fileWriter = new PrintWriter(new FileOutputStream(f));
                                logFile = f;
                                lastFileChar = 0;
                                putValue(Action.NAME, activeLabel);
                                putValue(Action.SHORT_DESCRIPTION, activeTip);
                                summaryView.setIcon(logToFileIcon);
                                summaryView.setIconToolTip(logFile.getPath());
                                controlsPanel.setLogToFile(true);
                            }
                        } else {
                            try {
                                fileWriter.close();
                            } finally {
                                fileWriter = null;
                                logFile = null;
                                putValue(Action.NAME, inactiveLabel);
                                putValue(Action.SHORT_DESCRIPTION, inactiveTip);
                                summaryView.setIcon(null);
                                summaryView.setIconToolTip(null);
                                controlsPanel.setLogToFile(false);
                            }
                        }
                    } catch (Exception x) {
                        Trace.printStackTrace(x);
                    }
                }
            };
        }
        return logToFileAction;
    }

    public Action getCutAction() {
        return null;
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
        if (findAction == null) {
            findAction = new AbstractAction() {
                public void actionPerformed(ActionEvent e) {
                    getFindDialog().actionPerformed(e);
                }
            };
            ActionsUtilities.findActionStyler(findAction);
            GUI.registerKeyboardAction(this, findAction);
            findAction.setEnabled(textViewer.getDocument().getLength() > 0);
        }
        return findAction;
    }

    public Action getCopyAction() {
        if (copyAction == null) {
            copyAction = new AbstractAction() {
                public void actionPerformed(ActionEvent e) {
                    textViewer.copy();
                }
            };
            ActionsUtilities.copyActionStyler(copyAction);
            GUI.registerKeyboardAction(this, copyAction);
            updateCopyAction();
        }
        return copyAction;
    }

    private void updateCopyAction() {
        if (copyAction != null) {
            Caret c = textViewer.getCaret();
            c.setVisible(true);
            if (copyAction != null) {
                copyAction.setEnabled(c.getDot() != c.getMark());
            }
        }
    }

    public JComponent getControlsPanel() {
        return controlsPanel;
    }

    /*
     * Return the MenuButton with all controls
     * @return a MenuButton that contains the controls for this pod
     * @see ViewAndControlsInterface#getMenuButton
     */
    public MenuButton getMenuButton() {
        if (menuButton == null) {
            menuButton = new MenuButton(getViewTitle());
            Misc.setOpaque(menuButton, false);
            JPopupMenu popup = menuButton.getPopupMenu();
            popup.add(getCopyAction());
            popup.add(getFindAction());
            popup.addSeparator();
            popup.add(getClearAction());
            popup.add(getLogToFileAction());
        }
        return menuButton;

    }

    /**
     * Get the content for the pod
     * @return the pod contents, this class itself
     * @see ViewAndControlsInterface#getPodContent
     */
    public JComponent getPodContent() {
        return this;
    }

    /**
     * Return the log object for sending messages to the message view.
     *
     * @return the log object for sending messages to the message view.
     */
    public WriterLog getDefaultLog() {
        if (defaultLog == null) {
            synchronized (this) {
                if (defaultLog == null) {
                    defaultLog = new DefaultLog();
                }
            }
        }
        return defaultLog;
    }

    /**
     * Creates a styled log. The messages sent to the text viewer through
     * this log will be marked with the specified color. If the color is
     * <code>null</code>, the default color will be used, which ia defined
     * by the current look and feel.
     * <p>
     * To send unmarked messages to the message view, use the default
     * log object returned by the {@link #getDefaultLog()} method.
     *
     * @param fg the text foreground color
     * @return the log object for sending marked messages to the message view
     */
    public StyledLog createStyledLog(Color fg) {
        return new Log(fg, null);
    }

    /**
     * Creates a styled log. The messages sent to the text viewer through
     * this log will be marked with the specified background and foreground
     * colors. If any of these colors is <code>null</code>, the default
     * colors will be used, which are defined by the current look and feel.
     *
     * @param fg the text foreground color
     * @param bg the text background color
     * @return the log object for sending marked messages to the message view
     */
    public StyledLog createStyledLog(Color fg, Color bg) {
        return new Log(fg, bg);
    }

    // ======================================================================
    // Document listener
    // ======================================================================

    private class TextChangeListener implements DocumentListener {
        public void insertUpdate(DocumentEvent e)  { update(e); }
        public void removeUpdate(DocumentEvent e)  { update(e); }
        public void changedUpdate(DocumentEvent e) { update(e); }
        private void update(DocumentEvent e) {
            Document doc = e.getDocument();
            int len = doc.getLength();
            if (findAction != null) findAction.setEnabled(len > 0);
            if (clearAction != null) clearAction.setEnabled(len > 0);
        }
    }

    // ======================================================================
    // Extended Message Log
    // ======================================================================

    /**
     * Extension of {@link MessageLog} that adds a Writer interface to it.
     */
    public interface WriterLog extends MessageLog {

        /**
         * Return the {@link Writer} for receiving log messages and sending
         * them to the text viewer.
         *
         * @return the {@link Writer} that will forward messages to the
         * message view.
         */
        public PrintWriter getLogWriter();

        /**
         * Sets the {@link MessageLog} object where messages will be
         * forwarded, in addition to being displayed in the message view.
         *
         * @param target the target logger, <code>null</code> to disable
         *               forwarding.
         */
        public void setForward(MessageLog target);

        /**
         * Returns the {@link MessageLog} where messages are being
         * forwarded, in addition to being displayed in the message view.
         *
         * @return the target {@link MessageLog}, <code>null</code> if none
         */
        public MessageLog getForward();
    }

    /**
     * Extension of {@link MessageLog} for the purposes of the message log
     * view. The view can create a number of such objects, and the messages
     * submitted through these objects will be marked with different colors.
     */
    public interface StyledLog extends WriterLog {

        /**
         * Returns the text foreground color for the messages coming through
         * this logger.
         *
         * @return the text foreground color, <code>null</code> if the default
         * color is being used
         */
        public Color getForeground();

        /**
         * Returns the text background color for the messages coming through
         * this logger.
         *
         * @return the text background color, <code>null</code> if the default
         * color is being used
         */
        public Color getBackground();
    }

    private abstract class AbstractWriterLog implements WriterLog {

        private MessageLog forward;

        public boolean isEnabled() {
            return true;
        }

        public void println(String msg) {
            getLogWriter().println(msg);
            MessageLog dest = forward;
            if (dest != null) dest.println(msg);
        }

        public void printStackTrace(Throwable x) {
            x.printStackTrace(getLogWriter());
            MessageLog dest = forward;
            if (dest != null) dest.printStackTrace(x);
        }

        public void setForward(MessageLog target) {
            forward = target;
        }

        public MessageLog getForward() {
            return forward;
        }
    }

    private class DefaultLog extends AbstractWriterLog {
        private PrintWriter logWriter;

        public boolean isEnabled() {
            return isShowing();
        }

        public void println(String msg) {
            if (summaryView != null) {
                summaryView.setForeground(null);
                summaryView.setBackground(null);
                summaryView.setText(msg);
            }
            super.println(msg);
        }

        public void printStackTrace(Throwable x) {
            if (summaryView != null) {
                summaryView.setForeground(null);
                summaryView.setBackground(null);
                summaryView.setText(x.getLocalizedMessage());
            }
            super.printStackTrace(x);
        }

        public PrintWriter getLogWriter() {
            if (logWriter == null) {
                synchronized (this) {
                    if (logWriter == null) {
                        Writer writer = new LogWriter(null);
                        logWriter = new PrintWriter(writer, true);
                    }
                }
            }
            return logWriter;
        }
    }

    private class Log extends AbstractWriterLog implements StyledLog {

        private Color foreground;
        private Color background;
        private MutableAttributeSet attr;
        private PrintWriter logWriter;

        Log(Color fg, Color bg) {
            foreground = fg;
            background = bg;
            attr = new SimpleAttributeSet();
            if (fg != null) attr.addAttribute(StyleConstants.Foreground,fg);
            if (bg != null) attr.addAttribute(StyleConstants.Background,bg);
        }

        public void println(String msg) {
            if (summaryView != null) {
                summaryView.setForeground(foreground);
                summaryView.setBackground(background);
                summaryView.setText(msg);
            }
            super.println(msg);
        }

        public void printStackTrace(Throwable x) {
            if (summaryView != null) {
                summaryView.setForeground(foreground);
                summaryView.setBackground(background);
                summaryView.setText(x.getLocalizedMessage());
            }
            super.printStackTrace(x);
        }

        public Color getForeground() {
            return foreground;
        }

        public Color getBackground() {
            return background;
        }

        public PrintWriter getLogWriter() {
            if (logWriter == null) {
                synchronized (this) {
                    if (logWriter == null) {
                        Writer writer = new LogWriter(attr);
                        logWriter = new PrintWriter(writer, true);
                    }
                }
            }
            return logWriter;
        }
    }

    // ======================================================================
    // Text Viewer
    // ======================================================================

    /**
     * The text viewer
     */
    private class Viewer
    extends JEditorPane
    implements RightClickMenuSupport {

        private Action[] rightClickActions;

        Viewer() {
            setEditorKit(MessageLogView.getEditorKit());
            setFont(LookFactory.getStandardCodeFont());
            setEditable(false);
            //setLineWrap(false);
            //setTabSize(8);
            setOpaque(true);
            getCaret().setVisible(true);
            getCaret().setBlinkRate(0);
        }

        public Object[] getRightClickActions(MouseEvent e) {
            if (rightClickActions == null) {
                rightClickActions = new Action[] {
                    getCopyAction(),
                    getFindAction(),
                    null,
                    getClearAction(),
                    getLogToFileAction()
                };
            }
            return rightClickActions;
        }

        // JTextArea allows paste into a disabled and/or readonly
        // text area. Ha!
        public void paste() {
            getToolkit().beep();
        }
    }

    // ======================================================================
    // Message Writer
    // ======================================================================

    /**
     * A {@link Writer} that writes to the message log window.
     */
    private class LogWriter extends Writer {
        private StringBuffer buffer = new StringBuffer();
        private Runnable flusher;
        private boolean flusherRunning = false;
        private AttributeSet attributes;

        LogWriter(AttributeSet a) {
            attributes = a;
            flusher = new Runnable() {
                public void run() {
                    synchronized (LogWriter.this) {
                        flusherRunning = false;
                    }
                    flushImpl();
                }
            };
        }

        /**
         * Write the data. The are being bufferred and only get to the
         * screen when flush() is invoked or internal buffer gets too big.
         */
        public void write(char cbuf[], int off, int len) {
            if ((cbuf != null) && (len > 0)) {

                // Force flush if we have accumulated more than 1K of text
                boolean flushMe = false;
                synchronized (LogWriter.this) {
                    buffer.append(cbuf, off, len);
                    if (buffer.length() >= 1024) {
                        flushMe = true;
                    } else {
                        // Look for embedded EOL
                        for (int i=off+len-1; i>= off; i--) {
                            if (cbuf[i] == '\n') {
                                flushMe = true;
                                break;
                            }
                        }
                    }
                }

                if (flushMe) flush();
            }
        }

        /**
         * Flush triggers actual output to the text area.
         * This method is thread safe.
         */
        public void flush() {
            if (SwingUtilities.isEventDispatchThread()) {
                flushImpl();
            } else {
                synchronized (this) {
                    if (!flusherRunning) {
                        SwingUtilities.invokeLater(flusher);
                        flusherRunning = true;
                    }
                }
            }
        }

        /**
         * flush() implementation. Invoked on GUI thread.
         */
        private void flushImpl() {
            String s = null;

            // grab data from the buffer
            synchronized (this) {
                if (buffer.length() > 0) {
                    s = buffer.toString();
                    buffer.setLength(0);
                }
            }

            // don't do anything if buffer was empty
            if (s != null) {
                try {
                    boolean scroll = false;
                    Document d = textViewer.getDocument();
                    int docLen = d.getLength();
                    if (docLen > 0) {
                        scroll = (textViewer.getCaretPosition() >= (docLen-1));
                    }

                    // Insert new text:
                    d.insertString(d.getLength(), s, attributes);

                    // Is number of lines limited?
                    if (isSizeLimited() && docLen > getMaxSize()) {
                        Element root = d.getDefaultRootElement();
                        int i = root.getElementIndex(docLen - getMaxSize());
                        if (i >= 0) {
                            Element e = root.getElement(i);
                            int offset = e.getEndOffset();
                            d.remove(0,Math.min(offset,docLen));
                        }
                    }

                    // Scroll view to the end only if cursor was at the end:
                    if (scroll) {
                        textViewer.setCaretPosition(d.getLength());
                    }

                    // Update summary view:
                    if (summaryView != null && d.getLength() > 0) {
                        Element root = d.getDefaultRootElement();
                        int n = root.getElementCount();
                        if (n > 0) {
                            Element last = root.getElement(n-1);
                            int start = last.getStartOffset();
                            int end = last.getEndOffset();
                            if ((end-start) <= 1 && n > 1) {
                                last = root.getElement(n-2);
                                start = last.getStartOffset();
                                end = last.getEndOffset();
                            }

                            AttributeSet a = last.getAttributes();
                            Object fg = a.getAttribute(StyleConstants.Foreground);
                            Object bg = a.getAttribute(StyleConstants.Background);
                            if (!(fg instanceof Color)) {
                                fg = LookFactory.getCodeColorSet().foregroundColor;
                            }
                            if (!(bg instanceof Color)) {
                                bg = LookFactory.getCodeColorSet().backgroundColor;
                            }
                            summaryView.setForeground((Color)fg);
                            summaryView.setBackground((Color)bg);

                            String text = d.getText(start,end-start);
                            if (text.length() > 0) {
                                summaryView.setText(text);
                            } else {
                                summaryView.clear();
                            }
                        }
                    }

                    // Write the file:
                    if (fileWriter != null) {
                        char lastChar = lastFileChar;
                        int len = s.length();
                        for (int i=0; i<len; i++) {
                            char c = s.charAt(i);
                            if (c == '\n') {
                                if (lastFileChar == '\r') {
                                    fileWriter.write(c);
                                } else {
                                    // Use platform-specific end of line seq
                                    fileWriter.println();
                                }
                            } else {
                                fileWriter.write(c);
                            }
                            lastChar = c;
                        }

                        lastFileChar = lastChar;
                        fileWriter.flush();
                    }

                } catch(Exception ex) {
                    ex.printStackTrace();
                }
            }
        }

        public void close() {}
    }

    // ======================================================================
    // AbstractViewFactoryInterface
    // ======================================================================

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey() {
        return "messages";
    }

    /**
     * Returns properties of the view
     * @return properties of the view
     */
    public PropertySet getViewProperties() {
        return prefs;
    }

    /**
     * Create the elements to show when the pod is opened large to show.
     * @returns a ViewAndControlsInterface object that will be shown,
     * both the content panel and the controls, when the user requests
     * full disclosure.
     */
    public ViewAndControlsInterface getPodComponent() {
        return this;
    }

    /**
     * Create the small component to show.
     * @returns a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent() {
        if (summaryView == null) {
            summaryView = new LogSummaryView(getLogToFileAction());
        }
        return summaryView;
    }

    /**
     * @returns the short display name of the of the component.
     */
    public String getViewTitle() {
        if (viewTitle == null) {
            viewTitle = AppEnvironment.getString(VIEW_TITLE);
        }
        return viewTitle;
    }

    /**
     * @returns a longer description of the component
     */
    public String getViewDescription() {
        if (viewDescription == null) {
            viewDescription = AppEnvironment.getString(VIEW_DESCRIPTION);
        }
        return viewDescription;
    }

    /**
     * @returns true if the view is expanded by default
     */
    public boolean getDefaultViewOpen() {
        return false;
    }

    /**
     * @returns the default width of the view
     */
    public int getDefaultViewWidth() {
        return 400;
    }

    /**
     * @returns the default height of the view
     */
    public int getDefaultViewHeight() {
        return 100;
    }

    /**
     * Registers with {@link InterViewCommunicator} to receive inter-views
     * notifications. This method is invoked when view is being added to
     * diagnostics.
     *
     * @param comm the communicator to register with
     */
    public void bindToCommunicator(InterViewCommunicator comm) {
        communicator = comm;
    }

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm) {
        communicator = null;
    }

    // ======================================================================
    // "Log to File" label
    // ======================================================================

    private class LogToFileLabel extends JLabel implements RightClickMenuSupport {
        private Action[] rightClickActions;

        LogToFileLabel() {
        }

        public Object[] getRightClickActions(MouseEvent e) {
            return rightClickActions;
        }

        void setLogToFile(boolean on) {
            if (on) {
                controlsPanel.setIcon(logToFileIcon);
                controlsPanel.setToolTipText(logFile.getPath());
                rightClickActions = new Action[] {
                    getLogToFileAction()
                };
            } else {
                controlsPanel.setIcon(null);
                controlsPanel.setToolTipText(null);
                rightClickActions = null;
            }
        }
    }
}
