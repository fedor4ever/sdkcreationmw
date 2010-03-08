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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Frame;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.List;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BoxLayout;
import javax.swing.Box;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.border.BevelBorder;
import javax.swing.border.EmptyBorder;

import com.nokia.wtk.util.StringUtils;

/**
 * A factory for application dialog windows.
 *
 *
 * Note: For callers of invoke{Warning,Error,Information}MessageDialog():
 *
 * When passing in the Action arrays, to have each action cause disposal of 
 * the dialog, you need to have the following code in each Action:
 *
 * public void actionPerformed(ActionEvent ae) {
 *   <..do real work..>
 *   Action dispose = (Action)getValue(ApplicationDialogs.Attribute_DisposeAction);
 *   if (dispose != null) {
 *      dispose.actionPerformed(null);
 *   }
 * }
 *
 * @author BAB
 */
public abstract class ApplicationDialogs {

    // use Vector because it's synchronized
    private static final List dialogs = new Vector();
    private static final JDialog[] NO_DIALOGS = new JDialog[0];
    private static JFrame defaultDialogParent = null;
    private static int defaultDialogParentInvocations = 0;

    static {
        AppEnvironment.addResources(
            "resources/ApplicationDialogs.properties",
            ApplicationDialogs.class);
    }

    private static final String cancel_label =
        AppEnvironment.getString(Resource.DIALOG_CANCEL);
    private static final String apply_label =
        AppEnvironment.getString(Resource.DIALOG_APPLY);
    private static final String ok_label =
        AppEnvironment.getString(Resource.DIALOG_OK);
    private static final String help_label =
        AppEnvironment.getString(Resource.DIALOG_HELP);
    private static final String no_label =
        AppEnvironment.getString(Resource.DIALOG_NO);
    private static final String yes_label =
        AppEnvironment.getString(Resource.DIALOG_YES);
    private static final String open_label =
        AppEnvironment.getString(Resource.DIALOG_OPEN);
    private static final String close_label =
        AppEnvironment.getString(Resource.DIALOG_CLOSE);


    // disallow instantiation, for simApp specific purpose, instantiate it for now.
    public ApplicationDialogs() {}

    /**
     * Creates a special parent window explicitly for the dialogs that don't
     * have a parent window.
     * @return a default dialog parent which allows platform access
     * to an otherwise inaccessible dialog.  In Windows, the dialog
     * will be accessible both by using the <ALT>+<TAB> key accelerator
     * as well as from the task bar.
     */
    private static Window createDefaultDialogParent() {

        if (defaultDialogParent == null) {
            // Create a dummy frame with normal app title and icon:
            defaultDialogParent = new JFrame(AppEnvironment.getTitle());
            defaultDialogParent.setIconImage(
                AppEnvironment.getIcon(AppEnvironment.appIcon).getImage());

            // Center it:
            Dimension screen =
                java.awt.Toolkit.getDefaultToolkit().getScreenSize();
            Dimension size = defaultDialogParent.getSize();
            defaultDialogParent.setLocation(
                (screen.width + size.width) / 2,
                (screen.height + size.height) / 2);
        }

        // Iconify it before making it visible
        defaultDialogParent.setState(Frame.ICONIFIED);

        // Make it visible so icon will be active:
        defaultDialogParent.setVisible(true);

        // A modal dialog will be shown on top of the frame
        // and the frame will be disposed when the dialog is disposed,
        // but it is important that the user never sees it, si it must be minimized.

        defaultDialogParentInvocations++;
        return defaultDialogParent;
    }

    private static void closeDefaultDialogParent() {
        if (defaultDialogParentInvocations > 1) {
            defaultDialogParentInvocations--;
            return;
        }
        if (defaultDialogParent != null) {
            defaultDialogParent.dispose();
            defaultDialogParent = null;
            defaultDialogParentInvocations = 0;
        }
    }
    /**
     * Returns the window that will be a parent for a dialog. If no main window
     * is available, a small window will be created for these purposes.
     * @see createDefaultDialogParent
     */
    private static Window getDialogParent() {
        Window window = null;

        ApplicationWindow lastWin =
            AppEnvironment.getAppEnvironment().getActiveWindow();
        if (lastWin != null) {
            window = lastWin.getFrame();
        }
        if ((window == null) || !window.isVisible()) {
            ApplicationWindow appWin =
                AppEnvironment.getAppEnvironment().getMainWindow();
            if (appWin != null) {
                window = appWin.getFrame();
            }
        }
        //  return (((window == null) || !window.isVisible()) ? null : window);
        return (
            ((window == null) || !window.isVisible())
                ? createDefaultDialogParent()
                : window);
    }

    /**
     * Gets the label to use for "OK" buttons.
     * @return the label text to use.
     */
    public static String getOkayLabel() {
        return ok_label;
    }

    /**
     * Gets the label to use for "Cancel" buttons.
     * @return the label text to use.
     */
    public static String getCancelLabel() {
        return cancel_label;
    }

    /**
     * Gets the label to use for "Apply" buttons.
     * @return the label text to use.
     */
    public static String getApplyLabel() {
        return apply_label;
    }

    /**
     * Gets the label to use for "Help" buttons.
     * @return the label text to use.
     */
    public static String getHelpLabel() {
        return help_label;
    }

    /**
     * Gets the label to use for "Open" buttons.
     * @return the label text to use.
     */
    public static String getOpenLabel() {
        return open_label;
    }

    /**
     * Gets the label to use for "Close" buttons.
     * @return the label text to use.
     */
    public static String getCloseLabel() {
        return close_label;
    }

    /**
     * Gets the label to use for "Yes" buttons.
     * @return the label text to use.
     */
    public static String getYesLabel() {
        return yes_label;
    }

    /**
     * Gets the label to use for "No" buttons.
     * @return the label text to use.
     */
    public static String getNoLabel() {
        return no_label;
    }

    /**
     * Invokes a plain message dialog.
     * @param title the text to use for the dialog title.
     * @param icon an optional icon to use within the dialog.
     * @param message The message text to display.
     *                May also contain embedded HTML tags.
     * @param actions a set of actions to display as buttons at the bottom of the dialog.
     *                The first action will receive initial focus.
     */
    public static void invokePlainMessageDialog(
        String title,
        Icon icon,
        String message,
        Action[] actions) {
        if (!isWhitespace(message)) {
            if (!message.startsWith("<html>")) {
                message =
                    formatMessageAsHtml(
                        message,
                        false,
                        0,
                        "<strong>",
                        "</strong>");
            }
            invokeMessageDialog(
                title,
                icon,
                message,
                JOptionPane.PLAIN_MESSAGE,
                actions);
        }
    }

    /**
     * Invokes an information message dialog.
     * @param title the text to use for the dialog title.
     * @param icon an optional icon to use within the dialog.
     * @param message The message text to display.
     *                May also contain embedded HTML tags.
     * @param actions a set of actions to display as buttons at the bottom of the dialog.
     *                The first action will receive initial focus.
     */
    public static void invokeInformationMessageDialog(
        String title,
        Icon icon,
        String message,
        Action[] actions) {
        if (!isWhitespace(message)) {
            if (!message.startsWith("<html>")) {
                message =
                    formatMessageAsHtml(
                        message,
                        false,
                        0,
                        "<font" + ColorStyle.INFO + "><strong>",
                        "</strong></font>");
            }
            invokeMessageDialog(
                title,
                icon,
                message,
                JOptionPane.INFORMATION_MESSAGE,
                actions);
        }
    }

    /**
     * Invokes a warning message dialog.
     * @param title the text to use for the dialog title.
     * @param icon an optional icon to use within the dialog.
     * @param message The message text to display.
     *                May also contain embedded HTML tags.
     * @param actions a set of actions to display as buttons at the bottom of the dialog.
     *                The first action will receive initial focus.
     */
    public static void invokeWarningMessageDialog(
        String title,
        Icon icon,
        String message,
        Action[] actions) {
        if (!isWhitespace(message)) {
            if (!message.startsWith("<html>")) {
                message =
                    formatMessageAsHtml(
                        message,
                        false,
                        0,
                        "<font" + ColorStyle.WARNING + "><strong>",
                        "</strong></font>");
            }
            invokeMessageDialog(
                title,
                icon,
                message,
                JOptionPane.WARNING_MESSAGE,
                actions);
        }
    }

    /**
     * Invokes an error message dialog that has a single "Close" button.
     * @param title the text to use for the dialog title.
     * @param icon an optional icon to use within the dialog.
     * @param message The message text to display.
     *                May also contain embedded HTML tags.
     */
    public static void invokeErrorMessageDialog(
        String title,
        Icon icon,
        String message) {
        invokeErrorMessageDialog(
            title,
            icon,
            message,
            new Action[] {
                new AbstractAction(
                    getCloseLabel()) {
                        public void actionPerformed(ActionEvent e) {
                Action dispose =
                    (Action) getValue(ApplicationDialogs
                        .Attribute_DisposeAction);
                    if (dispose != null) { dispose.actionPerformed(e);
                    }
                }
            }
        });
    }

    /**
     * Invokes an error message dialog.
     * @param title the text to use for the dialog title.
     * @param icon an optional icon to use within the dialog.
     * @param message The message text to display.
     *                May also contain embedded HTML tags.
     * @param actions a set of actions to display as buttons at the bottom of the dialog.
     *                The first action will receive initial focus.
     */
    public static void invokeErrorMessageDialog(
        String title,
        Icon icon,
        String message,
        Action[] actions) {
        if (!isWhitespace(message)) {
            if (!message.startsWith("<html>")) {
                message =
                    formatMessageAsHtml(
                        message,
                        false,
                        0,
                        "<font" + ColorStyle.ERROR + "><strong>",
                        "</strong></font>");
            }
            invokeMessageDialog(
                title,
                icon,
                message,
                JOptionPane.ERROR_MESSAGE,
                actions);
        }
    }

    /**
     * Formats a message string as an HTML display string to be placed in a dialog.
     * The message text will be broken into separate blocks as delineated by
     * any embedded line breaks.  Each block will be enclosed as necessary
     * in a table cell (one per row) to conform to width limitations.
     * Text wrapping is prevented here to avoid dialog layout problems later.
     * @param message the message to format
     * @param centered if <code>true</code> then center the message text.
     *                 Otherwise, message text will be left-justified.
     * @param wrapwidth the message width that message text is wrapped against.
     *                  If zero or negative, then message text is <b>not</b> wrapped.
     * @param preMarkup any markup to apply immediately before a block of message text
     * @param postMarkup any markup to apply immediately after a block of message text
     */
    public static String formatMessageAsHtml(
        String message,
        boolean centered,
        int wrapwidth,
        String preMarkup,
        String postMarkup) {
        if (!isWhitespace(message)) {
            String msg = StringUtils.htmlEscape(message);
            // if any HTML, encode it
            StringBuffer sbuf = new StringBuffer();
            sbuf.append("<html>");
            if (centered)
                sbuf.append("<center>");
            if (wrapwidth > 0) {
                if (wrapwidth < MIN_MESSAGE_WIDTH) {
                    wrapwidth = MIN_MESSAGE_WIDTH;
                }
                if (wrapwidth > MAX_MESSAGE_WIDTH) {
                    wrapwidth = MAX_MESSAGE_WIDTH;
                }
                sbuf.append("<table width=" + wrapwidth + ">");
            } else {
                sbuf.append("<table>");
            }
            sbuf.append("<tr>");
            if (centered) {
                sbuf.append("<td align=center>");
            } else {
                sbuf.append("<td>");
            }
            sbuf.append("<font" + FontStyle.VAR_WIDTH + ">");
            if (preMarkup != null) {
                sbuf.append(preMarkup);
            }

            String[] lines = StringUtils.tokenize(msg, "\n\r");
            for (int i = 0; i < lines.length; i++) {
                if (i > 0) {
                    sbuf.append("<br>");
                }
                sbuf.append(lines[i]);
            }

            if (postMarkup != null) {
                sbuf.append(postMarkup);
            }
            sbuf.append("</font>");
            sbuf.append("</td>");
            sbuf.append("</tr>");
            sbuf.append("</table>");
            if (centered)
                sbuf.append("</center>");
            sbuf.append("</html>");
            return sbuf.toString();
        } else {
            return "";
        }
    }

    /**
     * Returns <code>true</code> if a string is empty or contains only whitespace.
     * @param s the string to test
     */
    private static boolean isWhitespace(String s) {
        boolean isWhite = true;
        if ((s != null) && (s.length() > 0)) {
            for (int index = 0; index < s.length(); index++) {
                if (!Character.isWhitespace(s.charAt(index))) {
                    isWhite = false;
                    break;
                }
            }
        }
        return isWhite;
    }

    /**
     * Invokes the Toolkit "about" dialog.
     * @param appTitle the application title
     * @param icon the icon to appear in the dialog
     * @param appVersionInfo application version information text
     * @param bodyMessage Content body text
     * @param copyrightNotice Copyright notice text
     * @param actions additional actions beyond default action
     */
    public static void invokeAboutDialog(
        String title,
        Icon appIcon,
        String version,
        String aboutText,
        String copyrightNotice,   // now ignored; should be part of message
        Action[] actions) {

        final JDialog aboutDialog = createDialog(title + ", " + version, true);
        aboutDialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
        String text = aboutText;	// formatMessageAsHtml(aboutText, false, 0, null, null);

        ImageIcon aboutIconTop =
                 AppEnvironment.getIcon(Resource.ABOUT_LEFT_TOP_ICON);
        ImageIcon aboutIconBottom =
                 AppEnvironment.getIcon(Resource.ABOUT_LEFT_BOTTOM_ICON);

        // the panel on the left
        JLabel leftTop = new JLabel(aboutIconTop);
        leftTop.setBackground(Color.white);
        leftTop.setHorizontalAlignment(JLabel.RIGHT);
        leftTop.setVerticalAlignment(JLabel.NORTH);

        JLabel leftBottom = new JLabel(aboutIconBottom);
        leftBottom.setBackground(Color.white);
        leftBottom.setHorizontalAlignment(JLabel.RIGHT);
        leftBottom.setVerticalAlignment(JLabel.NORTH);

        JPanel leftPanel = new JPanel(new BorderLayout());
        leftPanel.setBackground(Color.white);
        leftPanel.add(leftTop, BorderLayout.CENTER);
        leftPanel.add(leftBottom, BorderLayout.SOUTH);
        leftPanel.setBorder(new BevelBorder(BevelBorder.LOWERED));

        // the image at the top
        JLabel topImage = new JLabel(appIcon);
        topImage.setHorizontalAlignment(JLabel.CENTER);
        topImage.setVerticalAlignment(JLabel.CENTER);

        // the content
        JPanel panel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();

        gbc.insets.right = 10;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.BOTH;
        panel.add(leftPanel, gbc);

        gbc.gridx = 1;
        gbc.weightx = 1;
        gbc.gridheight = 1;
        gbc.insets.right = 0;
        gbc.insets.bottom = 4;    // add gap
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        panel.add(topImage, gbc);

        gbc.insets.left = 30;
        gbc.insets.right = 30;
        gbc.anchor = GridBagConstraints.NORTH;
        gbc.fill = GridBagConstraints.VERTICAL;
        panel.add(new JLabel(text), gbc);

        // Create a dialog "disposer" action:
        final Action disposeAction = new AbstractAction(getCloseLabel()) {
            public void actionPerformed(ActionEvent e) {
                if (!Boolean.TRUE.equals(getValue(Attribute_Disposed))) {
                    putValue(Attribute_Disposed, Boolean.TRUE);
                    aboutDialog.dispose();
                }
            }
        };

        JButton okButton = new JButton(disposeAction);
        gbc.insets.right = 0;
        gbc.insets.bottom = 0;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.NONE;
        gbc.anchor = GridBagConstraints.SOUTHEAST;
        panel.add(okButton, gbc);
        panel.setBorder(new EmptyBorder(10,10,10,10));

        aboutDialog.setContentPane(panel);
        aboutDialog.getRootPane().setDefaultButton(okButton);
        aboutDialog.pack();
        aboutDialog.setLocationRelativeTo(getDialogParent());
        aboutDialog.setVisible(true);
    }


    /**
     * Invokes a generic message dialog.
     * @param title the text to use for the dialog title.
     * @param icon an optional icon to use within the dialog.
     * @param message The formatted HTML message text to display.
     * @param type the type of dialog:
     *             <ul>
     *             <li><code>JOptionPane.PLAIN_MESSAGE</code></li>
     *             <li><code>JOptionPane.INFORMATION_MESSAGE</code></li>
     *             <li><code>JOptionPane.WARNING_MESSAGE</code></li>
     *             <li><code>JOptionPane.ERROR_MESSAGE</code></li>
     *             </ul>
     * @param actions a set of actions to display as buttons at the bottom of the dialog.
     *                The first action will receive initial focus.
     */
    private static void invokeMessageDialog(
        String title,
        Icon icon,
        String message,
        int type,
        Action[] actions) {
        //System.out.println("invokeMessageDialog(\"" + title + "\", \"" + message + "\")");
        // Control Button(s):
        if ((actions == null) || (actions.length == 0)) {
            // Create default "OK" action:
            actions =
                new Action[] { new AbstractAction(getOkayLabel()) { {
                    putValue(Attribute_InitialFocus, Boolean.TRUE);
                    }
                    {
                        putValue(Attribute_OnExit, Boolean.TRUE);
                    }
                    public void actionPerformed(ActionEvent e) {
                        Action dispose =
                            (Action) getValue(ApplicationDialogs
                                .Attribute_DisposeAction);
                        if (dispose != null) {
                            dispose.actionPerformed(null);
                        }
                    }
                }
            };
        }
        JButton[] options = new JButton[actions.length];
        int firstFocusIndex = 0;
        int onExitIndex = (-1);
        for (int i = 0; i < actions.length; i++) {
            options[i] = new JButton(actions[i]);
            if (Boolean
                .TRUE
                .equals(actions[i].getValue(Attribute_InitialFocus))) {
                firstFocusIndex = i;
            }
            if (Boolean.TRUE.equals(actions[i].getValue(Attribute_OnExit))) {
                onExitIndex = i;
            }
        }
        final Object initialOption = options[firstFocusIndex];
        final Action exitAction =
            (onExitIndex >= 0) ? actions[onExitIndex] : null;

        // Create the message content component:
        Object[] content = new Object[] { new JLabel(message)};

        JOptionPane p = new JOptionPane(content,                    // message
                                        type,                       // message type
                                        JOptionPane.DEFAULT_OPTION, // option type
                                        null,                       // icon
                                        options,                    // options
                                        initialOption);             // initial focus

        if (icon != null) {
            p.setIcon(icon);
        }

        final JDialog dialog = createDialog(title, p);
        dialog.toFront();
        dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);

        // Create a dialog "disposer" action:
        final Action disposeAction = new AbstractAction("Dispose") {
            public void actionPerformed(ActionEvent e) {
                if (!Boolean.TRUE.equals(getValue(Attribute_Disposed))) {
                    putValue(Attribute_Disposed, Boolean.TRUE);
                    // dialog.dispose does not notify the window listener that
                    // windowClosing is happening and dispose the default
                    // frame in SimApp case.
                    //dialog.getOwner().dispose();
                    // USING THE ABOVE CODE SCREWS UP MODAL ERROR MESSAGES IN
                    // THE TOOLKIT
                    dialog.dispose();
                }
            }
        };

        // Any option can dispose the dialog, if it chooses to:
        for (int i = 0; i < options.length; i++) {
            actions[i].putValue(Attribute_DisposeAction, disposeAction);
        }

        // Initial focus determined after dialog is made visible:
        dialog.addWindowListener(new WindowAdapter() {
            public void windowOpened(WindowEvent e) {
                // Dialog now visible; time to request default focus:
                if (initialOption instanceof Component) {
                    ((Component) initialOption).requestFocus();
                }
            }
            public void windowClosed(WindowEvent e) {
                if (exitAction != null) {
                    disposeAction.putValue(Attribute_Disposed, Boolean.TRUE);
                    exitAction.actionPerformed(null);
                }
            }
        });

        // Turns out that that bug where menus were partly visible on screen after
        // a dialog is shown is actually a really tough bug SUN has been dealing
        // with for 2 years and 3 releases. It appears only on Windows before 2K,
        // and is related to the double-buffering mechanism and backing store,
        // specifically how the backing store retains the pixels of the open menu
        // while the menu is closing itself underneat the dialog, and blasts those
        // pixels to screen when the dialog leaves.
        // ANyway, the fix is to make sure the menu pops down before the dialog
        // appears. This can be achieved by making the showing of the dialog happen
        // affter all Swing events have been processed, i.e. use invokeLater as
        // below. THis is one of the SUN approved work-arounds, but clumsy.
        // TODO: REMOVE FIX ASAP - probably when we migrate to JDK 1.4

        //SwingUtilities.invokeLater(
        //    new Runnable() {
        //        public void run() {
        // Show the dialog:
        dialog.pack();
        dialog.setResizable(false);
        invokeModalDialog(dialog);
        //        }
        //    });

    }

    /**
     * Displays a modal dialog. While dialog is open, monitors look and feel
     * changes and updates all open dialog accordingly
     */
    public static void invokeModalDialog(JDialog dialog) {

        // this method does not make sense for modeless dialogs because
        // their setVisible() method returns immediately
        if (!dialog.isModal()) {
            throw new IllegalArgumentException("modeless dialog");
        }

        dialogs.add(dialog);
        try {
            dialog.setVisible(true);
        } finally {
            dialogs.remove(dialog);
            closeDefaultDialogParent();
        }
    }

    /**
     * Create a JDialog with parent the last window made active, which is then
     * set for the lifetime of the dialog as the parent.
     */
    public static JDialog createDialog(String title, boolean modal) {
        final JFrame f = (JFrame) getDialogParent();
        JDialog dialog = new JDialog(f, title, modal);
        return dialog;
    }

    /**
     * Creates a JDialog from a JOptionPane.
     * @param title the dialog title
     * @param optionPane the JOptionPane
     * @return the JDialog
     */
    private static JDialog createDialog(String title, JOptionPane optionPane) {
        /*JFrame appFrame = (Toolkit.getApplicationWindow() != null)
                        ? Toolkit.getApplicationWindow().getFrame()
                        : null;*/
        JDialog dialog = optionPane.createDialog(getDialogParent(), title);
        dialog.setResizable(false);
        return dialog;
    }

    /**
     * Returns all currently active dialogs. Is no dialogs are active ,
     * returns empty array.
     */
    public static JDialog[] getDialogs() {
        return (JDialog[]) dialogs.toArray(NO_DIALOGS);
    }

    //
    // Inner Class: A set of HTML Font Style Definitions.
    //
    private interface FontStyle {
        static final String VAR_WIDTH = " size=\"-1\" face=sansserif";
        static final String FIXED_WIDTH = " size=\"-1\" face=monospaced";
    } // End of FontStyle

    //
    // Inner Class: A set of HTML Font Color Style Definitions.
    //
    public interface ColorStyle {
        //*/
        static final String PLAIN = " color=#000000";
        static final String INFO = " color=#202060";
        static final String WARNING = " color=#206020";
        static final String ERROR = " color=#602020";
        /*/
                static final String PLAIN =     " color=#000000";
                static final String INFO =      " color=#000000";
                static final String WARNING =   " color=#000000";
                static final String ERROR =     " color=#000000";
        //*/
    } // End of ColorStyle

    public static final int MIN_MESSAGE_WIDTH = 100;
    public static final int MAX_MESSAGE_WIDTH = 600;

    private static final String Attribute_Disposed = "Disposed";
    public static final String Attribute_DisposeAction = "DisposeAction";
    public static final String Action_Dispose = "Dispose";
    public static final String Attribute_InitialFocus = "InitialFocus";
    public static final String Attribute_OnExit = "OnExit";
} // End of ApplicationDialogs
