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




package com.nokia.wtk.diagnostics.midpView;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.Icon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTree;
import javax.swing.SwingConstants;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreeSelectionModel;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;
import com.nokia.wtk.diagnostics.views.support.Misc;

import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.MenuButton;

/**
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class MidpView extends JPanel
                      implements ViewAndControlsInterface,
                                 AbstractViewFactoryInterface {


    static {
        AppEnvironment.addResources(
            "resources/MidpView.properties",
            MidpView.class);
    }

    private PropertySet properties;
    private InterViewCommunicator communicator;
    private MidpSummaryView summaryView;
    private Browser browser;

    public MidpView(Browser br) {

        browser = br;
        properties = new PropertySet();
        setLayout(new BorderLayout());
        // make fuel guage panel
        Box guagePanel = Box.createHorizontalBox();
        Box guagePanel2 = Box.createVerticalBox();
        Box guagePanelLabel = Box.createVerticalBox();

        add(guagePanel, BorderLayout.CENTER);

        KVMMemoryModel guageModel = KVMMemoryModel.getKVMMemoryModel(br);
        JProgressBar guage =
            new JProgressBar(guageModel.getBoundedRangeModel());

        JTable valuesTable = new JTable(guageModel.getTableModel());
        valuesTable.setGridColor(Color.lightGray);
        valuesTable.setShowHorizontalLines(true);
        valuesTable.setShowVerticalLines(false);

        JLabel lab =
            new JLabel(
                " " + AppEnvironment.getString(Resource.MIDP_MEMORY_TITLE));
        lab.setVerticalTextPosition(SwingConstants.TOP);
        guagePanelLabel.add(lab);
        guagePanelLabel.add(Box.createVerticalGlue());
        guagePanel.add(guagePanelLabel);

        guagePanel2.add(guage);
        guagePanel2.add(valuesTable);
        guagePanel2.add(Box.createVerticalStrut(LookFactory.getGap()));
        guagePanel.add(guagePanel2);

        Misc.setOpaque(this, false);
        guage.setForeground(LookFactory.getCodeColorSet().selectionColor);
        guage.setOpaque(true);
    }

    public MenuButton getMenuButton() {
        // TODO Auto-generated method stub
        return null;
    }

    public JComponent getControlsPanel() {
        // TODO Auto-generated method stub
        return null;
    }

    public JComponent getPodContent() {
        // TODO Auto-generated method stub
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
        return "midpview";
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
            summaryView = new MidpSummaryView(browser);
        }
        return summaryView;
    }

    /**
     * @returns the short display name of the of the component.
     */
    public String getViewTitle() {
        return "MIDP";
    }

    /**
     * @returns a longer description of the component
     */
    public String getViewDescription() {
        return "KVM usage information";
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
        return 85;
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
