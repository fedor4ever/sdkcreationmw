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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.RCEMSTextField;
import com.nokia.wtk.diagnostics.browser.AttributedContent;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.views.support.*;
import com.nokia.wtk.util.Trace;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.SingleLineDocument;
import com.nokia.wtk.util.gui.RolloverJButton;

/**
 * This class contains independent pieces of the Browser UI like the
 * browser buttons and the location component, that can be used to
 * assemble a browser front end.
 *
 */
class BrowserComponentsFactory {

    private static final String HOME_BUTTON_KEY = "toolbar.home";
    private static final String BACK_BUTTON_KEY = "toolbar.back";
    private static final String RELOAD_BUTTON_KEY = "toolbar.reload";
    private static final String STOP_BUTTON_KEY = "toolbar.stop";
    private static final String ICON_KEY = "icon";
    private static final String ACTION_KEY = "action";
    private static final String TIP_KEY = "tooltip";
    private static final String LABEL_KEY = "label";
    private static final String TOOLBAR_KEY = "toolbar";
    private static final String SET_HOME_MENU = "set.home.menu";

    static {
        AppEnvironment.addResources(
            "resources/BrowserComponentsFactory.properties",
            BrowserComponentsFactory.class);
    }

    private Browser browser;
    private Commands commands;
    private PropertySet preferences;

    /**
     * Creates BrowserComponentsFactory
     * @param b the {@link Browser} instance
     * @param c the {@link Commands} instance
     */
    BrowserComponentsFactory(Browser b, Commands c, PropertySet p) {
        if (b == null)
            throw new NullPointerException("browser");
        if (c == null)
            throw new NullPointerException("commands");
        browser = b;
        commands = c;
        preferences = p;
    }

    /**
     * Make an action button
     **/
    private static JButton makeActionButton(
        String resource,
        Commands commands) {
        Icon icon = AppEnvironment.getIcon(resource + "." + ICON_KEY);

        RolloverJButton ret = new RolloverJButton(icon);
        ret.setAction(
            commands.getAction(
                AppEnvironment.getString(resource + "." + ACTION_KEY)));
        ret.setToolTipText(AppEnvironment.getString(resource + "." + TIP_KEY));
        ret.setIcon(icon);
        ret.setText("");
        ret.setMargin(new Insets(0, 0, 0, 0));
        return ret;
    }

    JComponent getBrowserButtons() {
        JPanel buttonPane = new JPanel();
        buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
        buttonPane.add(makeActionButton(BACK_BUTTON_KEY, commands));
        buttonPane.add(makeActionButton(STOP_BUTTON_KEY, commands));
        buttonPane.add(makeActionButton(RELOAD_BUTTON_KEY, commands));
        buttonPane.add(makeActionButton(HOME_BUTTON_KEY, commands));
        return buttonPane;
    }

    /**
     * Create the location tool bar.
     */
    JComponent getLocationEntryBar(Commands commands) {
        return createLocationEntryComponent(commands);
    }

    /** A class for JMenuItems that will always remember its original first size,
     * so it can be reset to that.
     **/
    static private class ResetableJMenuItem extends JMenuItem {
        private Dimension cache = new Dimension(-1, -1);
        private Dimension d;

        public ResetableJMenuItem(String s) {
            super(s);
        }

        public ResetableJMenuItem( Action action ) {
            super( action );
        }

        public void setPreferredSize(Dimension set) {
            d = this.getPreferredSize();
            if (cache.width == -1) {
                cache.width = d.width;
            }
            if (cache.height == -1) {
                cache.height = d.height;
            }
            super.setPreferredSize(set);
        }
        /**
         * Set back to the cached size.
         */
        public void resetSize() {
            d = this.getPreferredSize();
            if (cache.width != -1) {
                d.width = cache.width;
            }
            if (cache.height != -1) {
                d.height = cache.height;
            }
            super.setPreferredSize(d);
        }
    }
    /**
     * Create the URL location entry component.
     */
    private JComponent createLocationEntryComponent(Commands commands) {
        final JPopupMenu popup = new JPopupMenu();
        final RCEMSTextField te = new RCEMSTextField();
        final JButton popupButton =
            new RolloverJButton(
                AppEnvironment.getIcon(AppEnvironment.ARROW_DOWN));

        // prevent text area from accepting multi-line paste
        te.setDocument(new SingleLineDocument(te.getDocument()));
        ComponentListener teResizeListener = new ComponentAdapter() {
            public void componentResized(ComponentEvent e) {
                Component[] comps = popup.getComponents();
                ResetableJMenuItem c = null;
                for (int i = comps.length; i-- > 0;) {
                    if ( !(comps[i] instanceof JSeparator ) ) {
                        c = (ResetableJMenuItem) comps[i];
                        c.resetSize(); // make all lose size;
                    }
                }
                // make the last component at least be the preferres size for the menu.
                if (c != null) {
                    int minWidth = te.getWidth() + popupButton.getWidth();
                    Dimension d = c.getPreferredSize();
                    minWidth = (minWidth > d.width ? minWidth : d.width);
                    c.setPreferredSize(new Dimension(minWidth, d.height));
                }
            }
        };

        BrowserHistory history = browser.getBrowserHistory();

        te.addComponentListener(teResizeListener);
        popup.setLightWeightPopupEnabled(true);
        popup.setBackground(te.getBackground());
        popup.setBorderPainted(false);
        popup.add(new JSeparator());
        ResetableJMenuItem item = new ResetableJMenuItem(
            new SetHomeAction( te ) );
        item.setBackground( te.getBackground() );
        popup.add( item );
        te.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 0));
        te.setMargin(new Insets(0, 0, 0, 0));

        String locLabel =
            AppEnvironment.getString(TOOLBAR_KEY + ".location." + LABEL_KEY);
        final Action gotoURL = commands.getAction(Commands.gotoURLAction);
        final Action off = new AbstractAction() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                Trace.Error.println(
                    AppEnvironment.getString("dialog.nogorequest.text"));
            }
        };
        // Register for notifications of additions or deletions:
        final Action addToMenu = new AbstractAction() {
            ActionListener doMenuSelect = new ActionListener() {
                public void actionPerformed(ActionEvent ae) {
                    if (ae.getSource() instanceof JMenuItem) {
                        te.setText(((JMenuItem) ae.getSource()).getText());
                        SwingUtilities.invokeLater(new Runnable() {
                            public void run() {
                                te.postActionEvent();
                            }
                        });
                    }
                }
            };
            public void actionPerformed(java.awt.event.ActionEvent e) {

                String newVal = te.getText();

                if ((newVal == null) || (newVal.length() == 0)) {
                    return;
                }
                if (DocViewer.createURLFromUserInput(newVal, browser)
                    == null) {
                    //TODO: filter out typed-in nonsense or 404 returns
                    return;
                }
                boolean match = false;
                Component[] comps = popup.getComponents();
                JMenuItem mii = null;

                for (int i = comps.length;(i-- > 0) && !match;) {
                    Component c = comps[i];
                    if (c instanceof JMenuItem) {
                        String s = ((JMenuItem) c).getText();
                        if (newVal.equals(s)) {
                            match = true;
                            mii = (JMenuItem) c;
                            popup.remove(c);
                        }
                    }
                }

                // Create a new item if necessary
                if (mii == null) {
                    mii = new ResetableJMenuItem(newVal);
                    mii.setBackground(te.getBackground());
                    mii.addActionListener(doMenuSelect);

                    int minWidth = te.getWidth() + popupButton.getWidth();
                    Dimension prefSize = mii.getPreferredSize();
                    prefSize.width = Math.max(prefSize.width, minWidth);
                    mii.setPreferredSize(prefSize);
                }

                // Add the item to the top
                popup.insert(mii, 0);
            }
        };

        // Listener so the latest visible URL is in entry box
        history
            .addHistoryListListener(new BrowserHistory.HistoryListListener() {

            final private InvokeLaterQueueable setTexter =
                new InvokeLaterQueueable() {
                protected void doRun() {
                    AttributedContent curr =
                        browser.getBrowserHistory().getCurrentDocument();

                    //Browser.getBrowser().getCurrentDocument();
                    String location = null;
                    if (curr != null) {
                        location = BrowserHistory.getDocumentURL(curr);
                    }
                    if (location != null) {
                        te.setText(location);
                    }
                }
            };

            public void historyAdded(int index) {
                setTexter.queue();
            }
            public void historyUpdated(int index) {
                setTexter.queue();
            }
            public void historyRemoved(int index) {
                setTexter.queue();
            }
            public void allHistoryRemoved() {}
            public void currentPositionChanged() {
                setTexter.queue();
            }
        });

        PropertyChangeListener prop = new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent evt) {
                if (gotoURL.isEnabled()) {
                    SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            te.setEditable(true);
                            te.removeActionListener(off);
                            te.addActionListener(gotoURL);
                            te.addActionListener(addToMenu);
                        }
                    });
                } else {
                    SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            te.setEditable(false);
                            te.removeActionListener(gotoURL);
                            te.removeActionListener(addToMenu);
                            te.addActionListener(off);
                        }
                    });
                }
            }
        };

        gotoURL.addPropertyChangeListener(prop);

        te.setFont(LookFactory.getStandardPlainFont());
        popupButton.addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                popup.show(
                    popupButton,
                    -te.getWidth(),
                    popupButton.getHeight());
            }
        });
        //popupButton.setFocusPainted(false);
        popup.addPopupMenuListener(new PopupMenuListener() {
            public void popupMenuCanceled(PopupMenuEvent e) {
                popupButton.setSelected(false);
            }
            public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {
                popupButton.setSelected(false);
            }
            public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
                popupButton.setSelected(true);
            }
        });
        int gap = LookFactory.getGap();
        JPanel p = new JPanel();
        JPanel comboBoxThing = new JPanel(new BorderLayout());
        p.setLayout(new BoxLayout(p, BoxLayout.X_AXIS));
        p.add(Box.createHorizontalStrut(gap));
        JLabel label = new JLabel(locLabel, SwingConstants.RIGHT);
        label.setFont(LookFactory.getStandardPlainFont());
        p.add(label);
        //p.add(Box.createHorizontalStrut(gap));
        comboBoxThing.add(te, BorderLayout.CENTER);
        comboBoxThing.add(popupButton, BorderLayout.EAST);
        comboBoxThing.setBorder(BorderFactory.createLineBorder(Color.gray));
        p.add(comboBoxThing);
        p.add(Box.createHorizontalStrut(gap));

        // set initial state
        Boolean now = new Boolean(gotoURL.isEnabled());
        Boolean notNow = new Boolean(!gotoURL.isEnabled());
        prop.propertyChange(new PropertyChangeEvent(prop, "", notNow, now));

        return p;
    }

    private class SetHomeAction extends AbstractAction {
        SetHomeAction( RCEMSTextField goField ) {
            super( AppEnvironment.getString( SET_HOME_MENU ) );
            this.goField = goField;
        }

        public void actionPerformed( ActionEvent event ) {
            if ( goField != null ) {
                SwingUtilities.invokeLater( new Runnable() {
                    public void run() {
                        preferences.
                            getProperty( ToolkitPreferences.HOME_PAGE_PROPERTY ).
                            setValue( goField.getText() );
                    }
                } );
            }
        }

        private RCEMSTextField goField = null;
    }
}
