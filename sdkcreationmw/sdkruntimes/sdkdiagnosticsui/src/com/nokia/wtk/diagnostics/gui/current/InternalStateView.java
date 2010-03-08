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
import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JSplitPane;

import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.browser.BrowserVariables;

import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;

import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.MenuButton;



public class       InternalStateView
extends     JPanel
implements ViewAndControlsInterface,
           AbstractViewFactoryInterface {

    private Browser browser;
    private DeckView deckView;
    private JSplitPane splitPane;
    private VariablesView variablesView;
    private int dividerLocation = -1;
    private PropertySet properties;
    private InterViewCommunicator communicator;
    private DeckSummaryView summaryView;


    public InternalStateView(Browser b) {
        super(new BorderLayout());
        this.setBorder(LookFactory.loweredBorder());
        browser = b;
        properties = new PropertySet();
        deckView = new DeckView(b);
        variablesView = new VariablesView( browser );

        splitPane = new JSplitPane( JSplitPane.VERTICAL_SPLIT, false );
        splitPane.setOneTouchExpandable(true);
        splitPane.setDividerSize(5);
        JComponent comp = getDeckView();
        comp.setBorder(LookFactory.nullBorder());
        splitPane.setTopComponent( comp );
        splitPane.setEnabled( false );
        splitPane.setBorder(LookFactory.nullBorder());
        splitPane.setResizeWeight(0.8);

        add(splitPane, BorderLayout.CENTER);
        add(getDeckView().getDeckStatsLabel(), BorderLayout.SOUTH);



        // Use a VariableListListener to determine when to display the variables
        // view.
        browser.getBrowserVariables().addVariableListListener(
            new BrowserVariables.VariableListListener() {
                public void variableAdded( int index ) {
                    setVariablesViewVisible( true );
                }

                public void variableRemoved( int index ) {
                    setVariablesViewVisible( browser.getBrowserVariables().
                        getVariableCount() > 0 );
                }

                public void allVariablesRemoved() {
                    setVariablesViewVisible( false );
                }

                public void variableChanged( int index ) {
                }
            } );

        // Additionally use a HistoryListListener to determine when to remove
        // the variables view.
        browser.getBrowserHistory().addHistoryListListener(
            new BrowserHistory.HistoryListListener() {
                public void historyAdded( int index ) { }
                public void historyUpdated( int index ) { }
                public void historyRemoved( int index ) {
                    if ( browser.getBrowserHistory().
                        getCurrentDocument() == null ) {
                        setVariablesViewVisible( false );
                    }
                }
                public void allHistoryRemoved() {
                    setVariablesViewVisible( false );
                }
                public void currentPositionChanged() { }
            } );

        setPreferredSize(new Dimension(100, 200));
    }

    protected DeckView getDeckView() {
        return deckView;
    }

    /**
     * Sets the visible status of the variables view. Pass true for the
     * variables view to be displayed in the bottom of the split pane, false for
     * it to be removed.
     * @param flag true to show the variables view, false to hide it.
     */
    protected void setVariablesViewVisible( boolean flag ) {
        if ( flag ) {
            if ( !isVariablesViewVisible() ) {
                splitPane.setBottomComponent( getVariablesView() );
                if (dividerLocation > 0) {
                    splitPane.setDividerLocation(dividerLocation);
                }
                splitPane.setEnabled( true );
                splitPane.invalidate();
            }
        } else {
            if ( isVariablesViewVisible() ) {
                dividerLocation = splitPane.getDividerLocation();
                splitPane.setBottomComponent( null );
                // The JSplitPane is disabled when there is no bottom view
                // in order to prevent mouse overs from changing the cursor.
                splitPane.setEnabled( false );
                splitPane.invalidate();
            }
        }
    }

    /**
     * Returns true if the variables view is currently visible.
     * @return boolean true if the variables view is currently visible, false
     * otherwise.
     */
    protected boolean isVariablesViewVisible() {
        return splitPane.getBottomComponent() == getVariablesView();
    }

    /**
     * Returns the variables view Component.
     * @return the variables view Component.
     */
    protected VariablesView getVariablesView() {
        return variablesView;
    }

    // ======================================================================
    // ViewAndControlsInterface implementation
    // ======================================================================

    public MenuButton getMenuButton() {
        return getVariablesView().getMenuButton();
    }

    public JComponent getControlsPanel() {
        return getDeckView().getControlsPanel();
    }

    public JComponent getPodContent() {
        return this;
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
        return "statedeck";
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
            summaryView = new DeckSummaryView(browser);
        }
        return summaryView;
    }

    /**
     * @returns the short display name of the of the component.
     */
    public String getViewTitle() {
        return "Browser Source";
    }

    /**
     * @returns a longer description of the component
     */
    public String getViewDescription() {
        return "Current content and Variables active in the Browser";
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
}
