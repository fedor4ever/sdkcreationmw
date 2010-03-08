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





package com.nokia.wtk.diagnostics.gui.history;

/* java.awt */
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;

import javax.swing.AbstractAction;
import javax.swing.AbstractListModel;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenuItem;
import javax.swing.JScrollPane;
import javax.swing.ListCellRenderer;
import javax.swing.SwingUtilities;
import javax.swing.border.Border;
import javax.swing.text.JTextComponent;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.app.RightClickMenuSupport;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.gui.CommandNames;
import com.nokia.wtk.diagnostics.gui.Commands;
import com.nokia.wtk.diagnostics.views.support.Misc;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;

import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.MenuButton;

/**
 * A GUI for viewing WML history.
 */
//TODO: No provision yet for history size limits
public
class       HistoryView
extends     JScrollPane
implements  Resource, ViewAndControlsInterface, RightClickMenuSupport,
            AbstractViewFactoryInterface {

    private Browser browser;
    private Commands commands;
    private BrowserHistory history;
    private Action backAction;
    private Action forwardAction;
    private HistoryListModel listModel;
    private MenuButton historyButton;
    private JMenuItem backItem = null;
    private JList historyList = null;
    private PropertySet properties;
    private InterViewCommunicator communicator;
    private HistorySummaryView summaryView;

    private Object[] actions = null;

    static {
        AppEnvironment.addResources("resources/HistoryView.properties",
                                                HistoryView.class);
    }

    /**
     * Create a GUI for viewing browser history.
     */
    public HistoryView(Browser b, Commands c) {
        browser = b;
        commands = c;
        properties = new PropertySet();

        // Initialize the history view:
        initHistoryView();
        setPreferredSize(new Dimension(100, 100));
        Misc.setOpaque(this, false);
    }

    // Can be called by property-listener from another thread **/
    private void setHistoryUpdates(boolean getsU) {
        final boolean getsUpdates = getsU;
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                if (getsUpdates) {
                    historyList = new JList(getHistoryListModel());
                    historyList.setOpaque(false);
                    historyList.setCellRenderer(new HistoryListRenderer());
                    setViewportView(historyList);
                } else {
                    JTextComponent msgView = Misc.createHTMLViewer();
                    msgView.setText(getString(HIS_NO_HISTORY_TEXT));
                    setViewportView(msgView);
                }
            }
        });
    }

    public JComponent getPodContent() { return this; }

    /**
     * Initializes history view.
     */
    private void initHistoryView() {
        history = browser.getBrowserHistory();

        // Detect browser history changes
        this.history.addHistoryListListener(
            new BrowserHistory.HistoryListListener() {
                public void historyAdded(int index) {
                    getHistoryListModel().addHistory(index);
                    updateBackAction();
                    scrollToCurrent();
                }
                public void historyRemoved(int index) {
                    getHistoryListModel().removedHistory(index);
                    updateBackAction();
                    scrollToCurrent();
                }
                public void allHistoryRemoved() {
                    getHistoryListModel().removedAllHistory();
                    updateBackAction();
                }
                public void currentPositionChanged() {
                    getHistoryListModel().newCurrent(
                                        history.getCurrentDocumentPosition());
                    updateBackAction();
                    scrollToCurrent();
                }
                public void historyUpdated(int index) {
                    getHistoryListModel().changeHistory(index);
                    updateBackAction();
                    scrollToCurrent();
                }
            });

        // Add the list to the scrolling panel:
        setHistoryUpdates(deviceTestIsHistoryEnabled());
        setToolTipText(getString(HIS_TOOLTIP));
    }

    /**
     * Answer conclusively whether the current device sends history notifications
     */
    private boolean deviceTestIsHistoryEnabled() {
        if(this.history != null) {
          return this.history.isDeviceBrowserHistoryListenerSupported();
        }

        return false;
    }

    /**
     * Create the actions for the history view.
     */
    private Action getBackAction() {
    if (backAction == null) {
            backAction = new AbstractAction(getString(HIS_PREV_BUTTON_LABEL)) {
                public void actionPerformed(ActionEvent e) {
                    Action a = commands.getAction(CommandNames.goPreviousAction);
                    if (a != null) a.actionPerformed(e);
                }
            };
            backAction.setEnabled( false );
        }
        return backAction;
    }
    public MenuButton getMenuButton() {
        historyButton = new MenuButton(getString(HIS_BUTTON_LABEL));
        historyButton.setToolTipText(getString(HIS_BUTTON_TOOLTIP));
        historyButton.addAction( getBackAction() ).setToolTipText(
            getString(HIS_PREV_BUTTON_TOOLTIP) );
        historyButton.setMargin(new Insets(0, 0, 0, 0));
        Misc.setOpaque(historyButton, false);
        return historyButton;
    }
    public JComponent getControlsPanel() { return null; }

    /**
     * Get the ListModel reference.
     */
    private HistoryListModel getHistoryListModel() {
        if (listModel == null) {
            listModel = new HistoryListModel();
        }
        return listModel;
    }

    /**
     * @return <code>true</code> if there is currently any previous history.
     */
    private boolean hasPrevious() {
        return history.getBackHistorySize() > 0;
    }

    /**
     * @return <code>true</code> if the current device supports history "back" requests.
     */
    private boolean isBackAllowed() {
        return browser.isBackSupported();
    }

    private void updateBackAction() {
        if ( isBackAllowed() ) {
            if ( hasPrevious() ) {
                backAction.setEnabled( true );
            } else {
                backAction.setEnabled( false );
            }
        }
    }

    // ensure that the viewport is adjusted to show the current item
    private void scrollToCurrent() {
       SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          int index = history.getCurrentDocumentPosition();
          if (index >= 0) {
            historyList.ensureIndexIsVisible(index);
          }
        }
      });
    }

    private
    class   HistoryListRenderer
    extends JLabel
    implements ListCellRenderer {
        final private Border etched = LookFactory.etchedBorder();
        final private Border noBorder = BorderFactory.createEmptyBorder();

        // Prevents JList from creating its own incorrect rendering component:
        public Component getListCellRendererComponent(JList list,
                                                      Object value,
                                                      int index,
                                                      boolean isSelected,
                                                      boolean cellHasFocus) {

            if (index == history.getCurrentDocumentPosition()) {
                this.setBorder(etched);
                this.setFont(LookFactory.getStandardBoldFont());
            } else {
                this.setFont(LookFactory.getStandardPlainFont());
                this.setBorder(noBorder);
            }
            this.setText(BrowserHistory.getDocumentURL(history.getDocumentAt(index)));
            this.setOpaque(false);
            return this;
        }


    }

    /**
     * Inner Class: A ListModel to support HistoryView.
     */
    private class HistoryListModel extends AbstractListModel {

        private int listSize = 0;
        private int prevSelected = 0;

        public synchronized int getSize() {
            listSize = history.getHistorySize();
            return listSize;
        }
        public synchronized Object getElementAt(int index) {
            return BrowserHistory.getDocumentURL(history.getDocumentAt(index));
        }
        public void addHistory(int i) {
            this.fireIntervalAdded(this, i, i);
        }
        public void changeHistory(int i) {
            this.fireContentsChanged(this, i, i);
        }
        public void newCurrent(int i) {
            int min = Math.min(i, prevSelected);
            this.fireContentsChanged(this, 0, getSize());
            prevSelected = i;
        }
        public void removedHistory(int i) {
            this.fireIntervalRemoved(this, i, i);
        }
        public void removedAllHistory() {
            this.fireIntervalRemoved(this, 0, listSize);
        }

    }   // End of HistoryListModel


        public Object[] getRightClickActions(MouseEvent e) {
            if (actions == null) {
                actions = new Action[] {
                    backAction,
                };
            }
            return actions;
        }

    /**
     * Get a String resource.
     */
    private static String getString(String key) {
        return AppEnvironment.getString(key);
    }

    // ======================================================================
    // AbstractViewFactoryInterface implementation
    // ======================================================================

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey() {
        return "hisview";
    }

    /**
     * Returns properties of the view
     * @return properties of the view
     */
    public PropertySet getViewProperties() {
        return properties;
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
            summaryView = new HistorySummaryView(browser);
        }
        return summaryView;
    }

    /**
     * @returns the short display name of the of the component.
     */
    public String getViewTitle() {
        return "Browser History";
    }

    /**
     * @returns a longer description of the component
     */
    public String getViewDescription() {
        return "List of URLs on the Browser stack";
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
        return 95;
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

}   // End of HistoryView
