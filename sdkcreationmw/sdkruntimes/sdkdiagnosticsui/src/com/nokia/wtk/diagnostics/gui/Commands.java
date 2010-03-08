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





package com.nokia.wtk.diagnostics.gui;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.net.URL;
import java.util.Hashtable;
import java.util.Map;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.Icon;
import javax.swing.JDialog;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.ApplicationDialogs;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.bookmarks.BookmarkLoader;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.views.support.*;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.PropSheet;

/**
 * An array of commands and objects for menus and buttons.
 * Different sets of commands can be created depending on whther they are
 * commands for an area with multple sub-windows (tabbed), or stand-alone
 * objects. As many commands as possible are shared as static objects between
 * the sets of commands to cut down on overhead.
 * <p>
 * No Commands object can be instantiated. Instead, a Commands object can be
 * acquired from any of the static factory methods in this class, and then be further
 * configured with any of the remaining other ones. This way a Commands object
 * can be easily be custom-assembled with the proper parameter objects that the
 * Actions are supposed to operate on, to then be passed on to a MenuBuilder.
 */
public class Commands implements BookmarkLoader, CommandNames {

    static abstract class CommandAction extends AbstractAction {

        public CommandAction() {
            super();
            this.putValue(
                AbstractAction.SMALL_ICON,
                AppEnvironment.getIcon(AppEnvironment.EMPTY_ICON));
        }
        public CommandAction(String name) {
            super(name, AppEnvironment.getIcon(AppEnvironment.EMPTY_ICON));
        }
        public CommandAction(String name, Icon icon) {
            super(name, icon);
        }
    }

    /**
     * Action implementation of "empty" command. Does nothing. Useful
     * for menu items that get actions assigned through some other way,
     * like the help system F1 action.
     */
    static class EmptyAction extends CommandAction {
        EmptyAction() {
            super(emptyAction);
        }

        public void actionPerformed(ActionEvent e) {}
    }

    /**
     * Action implementation of "exit" command
     */
    private static class ExitAction extends CommandAction {
        ExitAction() {
            super(exitAction);
        }

        public void actionPerformed(ActionEvent e) {
            AppEnvironment.getAppEnvironment().requestExitApplication();
        }
    }

    /**
     * Action implementation of "goForward" command
     */
    private class GoForwardAction extends CommandAction {
        private InvokeLaterQueueable checkHistory =
            new InvokeLaterQueueable() {
            protected void doRun() {
                setEnabled(
                    browser.getBrowserHistory().getForwardHistorySize() > 0);
            }
        };

        GoForwardAction() {
            super(goForwardAction);
            setEnabled(browser.isForwardSupported());

            // Detect WML history changes:
            browser
                .getBrowserHistory()
                .addHistoryListListener(
                    new BrowserHistory
                    .HistoryListListener() {
                public void allHistoryRemoved() {
                    SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            GoForwardAction.this.setEnabled(false);
                        }
                    });
                }
                public void currentPositionChanged() {
                    checkHistory.queue();
                }
                public void historyAdded(int index) {
                    checkHistory.queue();
                }
                public void historyRemoved(int index) {
                    checkHistory.queue();
                }
                public void historyUpdated(int i) {}
            });
            checkHistory.queue();
        }

        public void actionPerformed(ActionEvent e) {
            browser.forward();
        }
    }

    /**
     * Action implementation of "goHome" command
     */
    private class GoHomeAction extends CommandAction {
        GoHomeAction() {
            super(goHomeAction);
            Property prop = preferences.
                getProperty( ToolkitPreferences.HOME_PAGE_PROPERTY );
            prop.addPropertyChangeListener(
                new PropertyChangeListener() {
                    public void propertyChange( PropertyChangeEvent event ) {
                        GoHomeAction.this.setEnabled(
                            browser.isLoadSupported() &&
                            GoHomeAction.this.isNonEmptyString(
                                event.getNewValue() ) );
                    }
                }
            );
            setEnabled(browser.isLoadSupported() && isNonEmptyString(
                prop.getValue() ) );
        }

        public void actionPerformed(ActionEvent e) {
            String url =
                (String) preferences
                    .getProperty(ToolkitPreferences.HOME_PAGE_PROPERTY)
                    .getValue();
            url = loadUrlCheck(url);
            if (url != null && url.length() > 0) {
                browser.load(url);
            }
        }

        private boolean isNonEmptyString( Object obj ) {
            return obj != null && obj instanceof String &&
                ( (String)obj ).length() > 0;
        }
    }

    /**
     * Action implementation of "goPrevious" command
     */
    private class GoPreviousAction extends CommandAction {
        private InvokeLaterQueueable checkHistory =
            new InvokeLaterQueueable() {
            protected void doRun() {
                setEnabled(
                    browser.isBackSupported()
                        && browser.getBrowserHistory().getBackHistorySize() > 0);
            }
        };
        GoPreviousAction() {
            super(goPreviousAction);
            setEnabled(
                browser.isBackSupported()
                    && browser.getBrowserHistory().getBackHistorySize() > 0);

            // Detect WML history changes:
            browser
                .getBrowserHistory()
                .addHistoryListListener(
                    new BrowserHistory
                    .HistoryListListener() {
                public void allHistoryRemoved() {
                    SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            GoPreviousAction.this.setEnabled(false);
                        }
                    });
                }
                public void currentPositionChanged() {
                    checkHistory.queue();
                }
                public void historyAdded(int index) {
                    checkHistory.queue();
                }
                public void historyRemoved(int index) {
                    checkHistory.queue();
                }
                public void historyUpdated(int index) {}
            });
            checkHistory.queue();
        }

        public void actionPerformed(ActionEvent e) {
            browser.back();
        }
    }

    /**
     * Action implementation of "gotoURLAction" command
     */
    private class GotoURLAction extends CommandAction {
        GotoURLAction() {
            super(gotoURLAction);
            setEnabled(browser.isLoadSupported());
        }

        public void actionPerformed(ActionEvent e) {
            //System.out.println("BrowserUI.GotoURLAction: " + e);
            String urlString = null;
            if (e.getSource() instanceof JTextField) {
                urlString = ((JTextField) e.getSource()).getText().trim();
            }
            openLocation(urlString);
        }
    }

    /**
     * Action implementation of "newContext" command
     */
    private class NewContextAction extends CommandAction {
        NewContextAction() {
            super(newContextAction);
            //TODO: Wire up newcontext capability to enable state
        }

        public void actionPerformed(ActionEvent e) {
            browser.newContext();
        }
    }


    /**
     * Action implementation of "preferences" command
     */

    static private class PreferencesAction extends CommandAction {
        PreferencesAction() {
            super(preferencesAction);
        }

        public void actionPerformed(ActionEvent e) {

            String title =
                AppEnvironment.getString(Resource.DIALOG_PREFERENCES_TITLE);
            AppEnvironment
                .getAppEnvironment()
                .getMainWindow()
                .setWindowVisible(
                true);
            final JDialog dialog = ApplicationDialogs.createDialog(title, true);
            dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);

            final PropertySet ps = AppEnvironment.getUserProperties();
            final PropertySet clonedToolkitProperties = ps.getCopy(false);

            // Create the content
            Component content =
                new PropSheet(
                    ToolkitPreferences.createPages(clonedToolkitProperties));

            // Initialize actions appropriately
            final boolean[] dirtyBit = new boolean[] { false };

            Action ok = new AbstractAction(ApplicationDialogs.getOkayLabel()) {
                public void actionPerformed(ActionEvent e) {
                        // Commit property values and then close dialog.
    if (dirtyBit[0]) {
                        // Commit property values:
                        ps.applyValues(clonedToolkitProperties);
                    }
                    //reset configuration for server simulator.
                    dialog.dispose();
                }
            };

            final Action cancel =
                new AbstractAction(ApplicationDialogs.getCancelLabel()) {
                public void actionPerformed(ActionEvent e) {
                        // Close dialog without committing any property values.
    dialog.dispose();
                }
            };

            final Action apply =
                new AbstractAction(ApplicationDialogs.getApplyLabel()) {
                public void actionPerformed(ActionEvent e) {
                        // Commit property values:
    ps.applyValues(clonedToolkitProperties);
                    // Refresh cloned property values in case of side-effects:
                    //reconfigure the server simulator settings.
                    clonedToolkitProperties.applyValues(ps);

                    dirtyBit[0] = false;
                    setEnabled(false);
                    //cancel.setEnabled(false);
                }
            };

            // Detect property changes:
            //cancel.setEnabled(false);
            apply.setEnabled(false);
            clonedToolkitProperties
                .addPropertyChangeListener(new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent evt) {
                    dirtyBit[0] = true;
                    apply.setEnabled(true);
                    //cancel.setEnabled(true);
                }
            });

            Action[] actions = new Action[] { ok, cancel, apply };

            // layout the dialog
            LookFactory.layoutDialog(dialog, content, actions, cancel);

            // Watch for L&F changes
            PropertyChangeListener plafListener =
                new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent e) {
                    dialog.pack();
                }
            };

            // Show the dialog:
            dialog.setResizable(false);
            ApplicationDialogs.invokeModalDialog(dialog);
            dialog.dispose();
        }

    }

    /**
     * Action implementation of "refresh" command
     */
    private class RefreshAction extends CommandAction {
        private InvokeLaterQueueable checkHistory =
            new InvokeLaterQueueable() {
            protected void doRun() {
                DeviceContentReceived curr =
                    browser.getBrowserHistory().getCurrentDocument();
                setEnabled((curr != null) && isWmlMime(curr.getType()));
            }
        };

        RefreshAction() {
            super(refreshAction);
            setEnabled(browser.isRefreshSupported());
            browser
                .getBrowserHistory()
                .addHistoryListListener(
                    new BrowserHistory
                    .HistoryListListener() {
                public void allHistoryRemoved() {
                    checkHistory.queue();
                }
                public void currentPositionChanged() {
                    checkHistory.queue();
                }
                public void historyAdded(int index) {
                    checkHistory.queue();
                }
                public void historyRemoved(int index) {
                    checkHistory.queue();
                }
                public void historyUpdated(int index) {
                    checkHistory.queue();
                }
            });
            checkHistory.queue();
        }

        public void actionPerformed(ActionEvent e) {
            browser.refresh();
        }

        private boolean isWmlMime(String type) {
            return (
                MimeType.WML_BINARY.equals(type)
                    || MimeType.WML_SOURCE.equals(type));
        }
    }

    /**
     * Action implementation of "reloadDeck" command
     */
    private class ReloadDeckAction extends CommandAction {
        ReloadDeckAction() {
            super(reloadDeckAction);
            setEnabled(browser.isReloadSupported());
        }

        public void actionPerformed(ActionEvent e) {
            browser.reload();
        }
    }

    /**
     * Action implementation of "stopLoading" command
     */
    private class StopAction extends CommandAction {

        StopAction() {
            super(stopAction);
            setEnabled(browser.isStopSupported());
        }

        public void actionPerformed(ActionEvent e) {
            browser.stop();
        }
    }

    /* keystrokes */
    final static KeyStroke CLOSE_KEY =
        KeyStroke.getKeyStroke(KeyEvent.VK_F4, ActionEvent.CTRL_MASK);
    final static AbstractAction emptyActionC = new EmptyAction();
    final static AbstractAction exitActionC = new ExitAction();
    private static final String FILE_URL_PREFIX = "file:";

    private static final String HTTP_URL_PREFIX = "http:";
    final static AbstractAction preferencesActionC = new PreferencesAction();

    /**
     * Checks a document url prior to a load.
     * For "file:" URLs, there are side-effects.
     *
     * @param url       the document location.
     * @return          the massaged document location URL
     */
    public static String loadUrlCheck(String url) {
        if (url != null && url.length() > 0) {
            url = url.trim();
            if (url.substring(0, 5).equalsIgnoreCase(HTTP_URL_PREFIX)) {
                return url;
            } else if (url.substring(0, 5).equalsIgnoreCase(FILE_URL_PREFIX)) {
                if (url.charAt(6) != '/') {
                    return FILE_URL_PREFIX + "//" + url.substring(5);
                }
                return url;
            } else if (url.charAt(2) == ':') {
                // This case never seems to occur, and this code handles it
                // inadequately
                String formatUrl = FILE_URL_PREFIX + url;
                return formatUrl;
            } else if (url.indexOf(':') < 0) {
                //when user not typing any schema, add http in front
                String formatUrl = HTTP_URL_PREFIX + "//" + url;
                return formatUrl;
            }
            return url;
        } else {
            System.out.println(
                "the url being passed into check vaild url function is null");
            return null;
        }
    }
    private Browser browser;

    // Where the actions are stored, keyed by name
    private Map commands;
    private PropertySet preferences;

    Commands(String[] names, Browser b, PropertySet prefs) {
        browser = b;
        commands = new Hashtable();
        preferences = prefs;
        addNamedCommands(names);
    }

    void add(String commandName, Action action) {
        this.commands.put(commandName, action);
    }

    /**
     * Adds the bookmark action to the loader.
     * @param cmd  Name of the action.
     * @param action the bookmark action.
     */
    public void addAction(String name, Action action) {
        add(name, action);
    }

    /**
     * Add actions according to a list of supplied names.
     * @param commandNames a Vector of strings taken from CommandNames to
     *        set which commands to include.
     */
    private void addNamedCommands(String[] names) {
        Action a = null;

        for (int i = 0; i < names.length; i++) {
            if (commands.containsKey(names[i])) {
                // do nothing;
            } else if (CommandNames.emptyAction.equals(names[i])) {
                a = emptyActionC;
            } else if (CommandNames.exitAction.equals(names[i])) {
                a = exitActionC;
            } else if (CommandNames.goHomeAction.equals(names[i])) {
                a = new GoHomeAction();
            } else if (CommandNames.goPreviousAction.equals(names[i])) {
                a = new GoPreviousAction();
            } else if (CommandNames.goForwardAction.equals(names[i])) {
                a = new GoForwardAction();
            } else if (CommandNames.gotoURLAction.equals(names[i])) {
                a = new GotoURLAction();
            } else if (CommandNames.newContextAction.equals(names[i])) {
                a = new NewContextAction();
            } else if (CommandNames.preferencesAction.equals(names[i])) {
                a = preferencesActionC;
            } else if (CommandNames.refreshAction.equals(names[i])) {
                a = new RefreshAction();
            } else if (CommandNames.reloadDeckAction.equals(names[i])) {
                a = new ReloadDeckAction();
            } else if (CommandNames.stopAction.equals(names[i])) {
                a = new StopAction();
            }
            if (a != null) {
                commands.put(a.getValue(Action.NAME), a);
                a = null;
            }
        }

    }

    /**
     * Releases all resources of this object. Do not use again.
     */
    public void dispose() {
        this.commands.clear();
    }

    /**
     * Fetch the action that was created for the given command.
     * @param cmd  Name of the action.
     * @returns action created for the given command or null
     *  if one wasn't created.
     */
    public Action getAction(String cmd) {
        return (commands != null) ? (Action) commands.get(cmd) : null;
    }

    /**
     * Returns a map that amaps keys into actions
     */
    public Map getActionMap() {
        return commands; // should be unmodifiable?
    }

    /**
     * Gets entire actions of the loader.
     * @return the <code>Map</code> of actions with references to its names as
     * keys.
     */
    public Map getActions() {
        return getActionMap();
    }

    Browser getBrowser() {
        return browser;
    }

    /**
     * Gets the set of available commands, appropriate for the editor supplied.
     * @return an array of command actions. Created anew locally for likelier
     * release of memory.
     */
    public Action[] getCommands() {
        Action[] actions = new Action[commands.size()];
        return (Action[]) commands.values().toArray(actions);
    }

    /**
      * Gets the current URI location of the current load operation.
      * @returns the URI String of the current load operation; returns
      * <code>null</code> if no URI present.
      */
    public String getLocation() {
        String loc = null;
        try {
            loc =
                browser.getBrowserHistory().getCurrentDocument().getLocation();
        } catch (NullPointerException e) {
            //return null;
        }
        return loc;
    }

    /**
     * Tests if bookmark loader supports load operation.
     * @returns <code>true</code> if loader supports load operation;
     * <code>false</code> othrewise.
     */
    public boolean isLoadSupported() {
        return browser.isLoadSupported();
    }

    /** BookmarkLoader implementation */

    /**
     * Is this request made with a full schema? Needs to be implemented by any
     * class wanting to use bookmarks.
     * @param location the location to answer the query about
     * @retunr true if param location is a URL with a schema, false otherwise
     */
    public boolean isSchemaRequest(String location) {
        return DocViewer.isSchemaRequest(location);
    }

    /**
     * Request to load the location. Needs to be implemented by any
     * class wanting to use bookmarks.
     * @param location the location to load. Will be a URL with a schema, and
     * a file, and a host
     */
    public void loadContent(String location) {
        openLocation(location);
    }

    /**
     * Opens specified location
     * @param location the location to open
     */
    public void openLocation(String location) {
        if (!browser.isLoadSupported()) {
            ApplicationDialogs.invokeErrorMessageDialog(
                "No Device",
                null,
                "There is no selected device to load URL");
            return;
        }
        if (location != null) {
            String s = location;
            if (!DocViewer.isSchemaRequest(s)) {
                URL url = DocViewer.createURLFromUserInput(location, browser);
                if (url != null) {
                    s = url.toExternalForm();
                }
            }
            s = loadUrlCheck(s);
            if (s != null && s.length() > 0) {
                browser.load(s);
            }
        }
    }

    /**
     * Opens the location specified by URL
     * @param url the URL to open
     */
    private void openLocation(URL url) {
        if (url != null) {
            openLocation(url.toExternalForm());
        }
    }

    public String toString() {
        return commands.toString();
    }
}
