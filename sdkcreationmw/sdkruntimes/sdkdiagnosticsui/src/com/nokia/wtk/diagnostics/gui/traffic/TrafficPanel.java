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





package com.nokia.wtk.diagnostics.gui.traffic;

import java.awt.event.ActionEvent;

import javax.swing.Action;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.AbstractAction;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;

import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertyEditors;
import com.nokia.wtk.util.gui.MenuButton;
import com.nokia.wtk.util.gui.RolloverJButton;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.support.Misc;

/**
 * A GUI for viewing content requests, responses and cache contents.
 */
class TrafficPanel extends CacheTrafficSuperView {

    // Resource Names:
    private static final String CONTENT_HELPID = "session.contentPanel.helpid";
    private static final String CONTROL_HELPID = "session.controlPanel.helpid";

    static {
        AppEnvironment.addResources(
            "resources/TrafficView.properties",
            TrafficView.class);
    }

    private MenuButton menuButton_;
    private JComponent buttons_;
    private Action resetTrafficAction;
    private CentralTrafficSynchronizer centralTrafficSynchronizer;
    private PropertySet properties = new PropertySet();
    private int filterMask;

    /**
     * Create a GUI for viewing content requests, responses.
     * @param sync the traffic model
     * @param filters the filter mask
     */
    TrafficPanel(CentralTrafficSynchronizer sync, int filters) {
        // create model from Browser model
        super(sync.getBrowser());
        filterMask = filters;
        centralTrafficSynchronizer = sync;
        getTree().setModel(
            centralTrafficSynchronizer.getTrafficModel().getTreeModel());

        // Listen for changes to session size that could affect ability to clear session:
        getContentModel()
            .getTreeModel()
            .addTreeModelListener(new TreeModelListener() {
            public void treeNodesChanged(TreeModelEvent e) {}
            public void treeNodesInserted(TreeModelEvent e) {
                resetTrafficAction.setEnabled(true);
            }
            public void treeNodesRemoved(TreeModelEvent e) {
                resetTrafficAction.setEnabled(getContentModel().getSize() > 0);
            }
            public void treeStructureChanged(TreeModelEvent e) {
                resetTrafficAction.setEnabled(getContentModel().getSize() > 0);
            }
        });

        resetTrafficAction =
            new AbstractAction(
                AppEnvironment.getString(RESET_SESSION_BUTTON_LABEL)) {
            {
                putValue(
                    Action.SHORT_DESCRIPTION,
                    AppEnvironment.getString(RESET_SESSION_BUTTON_TOOLTIP));
                setEnabled(getContentModel().getSize() > 0);
            }
            public void actionPerformed(ActionEvent e) {
                centralTrafficSynchronizer.getBrowser().getContentLog().removeAll();
            }
        };

        // Attach filter properties to our property set
        PropertySet filterProp = getTrafficModel().getFilter().getProperties();
        properties.addPropertySet("filter",filterProp,true);
    }

    public PropertySet getProperties() {
        return properties;
    }

    public ContentModel getContentModel() {
        return centralTrafficSynchronizer.getTrafficModel();
    }

    private TrafficModel getTrafficModel() {
        return centralTrafficSynchronizer.getTrafficModel();
    }

    public MenuButton getMenuButton() {
        if (menuButton_ == null) {
            menuButton_ = new MenuButton("ok");

            // create actions that can be performed by the control panel

            menuButton_.addAction(resetTrafficAction);
            Misc.setOpaque(menuButton_, false);
        }
        return menuButton_;
    }

    public JComponent getControlsPanel() {
        if (buttons_ == null) {
           if ((filterMask & TrafficView.TRAFFIC_FILTER_ALL) != 0) {
               buttons_ = createControlsPanel();
           }
        }
        return buttons_;
    }

    private static void addCheck(MenuButton mb, Property prop, String nameKey,
        String tipKey) {
        String name = AppEnvironment.getString(nameKey);
        JComponent check = PropertyEditors.createCheckBoxMenuItem(prop,name);
        check.setToolTipText(AppEnvironment.getString(tipKey));
        mb.getPopupMenu().add(check);
    }

    /**
     * Enables or disables all filters.
     * @param enabled <code>true</code> to enable all filters,
     *                <code>false</code> to disable
     */
    private void setAllFiltersEnabled(boolean enabled) {
        Filter filter = getTrafficModel().getFilter();
        filter.setSMSReceivedVisible(enabled);
        filter.setSMSSentVisible(enabled);
        filter.setBluetoothReceivedVisible(enabled);
        filter.setBluetoothSentVisible(enabled);
        filter.setPushedVisible(enabled);
        filter.setRequestsVisible(enabled);
        filter.setResponsesVisible(enabled);
        filter.setTCPUDPReceivedVisible(enabled);
        filter.setTCPUDPSentVisible(enabled);
    }

    /**
     * Creates the control panel component
     * @return a new instance of the control panel component
     */
    private JComponent createControlsPanel() {
        MenuButton menuButton;
        menuButton = new MenuButton(AppEnvironment.getString(VIEWTYPE_LABEL));

        Filter filter = getTrafficModel().getFilter();
        if ((filterMask & TrafficView.TRAFFIC_FILTER_HTTP_REQUESTS) != 0) {
            addCheck(menuButton, filter.requests,
                REQUESTS_CHOICE_LABEL,
                REQUESTS_CHOICE_TOOLTIP);
        }
        if ((filterMask & TrafficView.TRAFFIC_FILTER_HTTP_RESPONSES) != 0) {
            addCheck(menuButton, filter.responses,
                RESPONSES_CHOICE_LABEL,
                RESPONSES_CHOICE_TOOLTIP);
        }
        if ((filterMask & TrafficView.TRAFFIC_FILTER_PUSHED) != 0) {
            addCheck(menuButton, filter.pushed,
                PUSH_CHOICE_LABEL,
                PUSH_CHOICE_TOOLTIP);
        }

        if (centralTrafficSynchronizer.isSMSSupported()) {
            if ((filterMask & TrafficView.TRAFFIC_FILTER_SMS_SENT) != 0) {
                addCheck(menuButton, filter.smsSent,
                    SMS_SENT_CHOICE_LABEL,
                    SMS_SENT_CHOICE_TOOLTIP);
            }
            if ((filterMask & TrafficView.TRAFFIC_FILTER_SMS_RECEIVED) != 0) {
                addCheck(menuButton, filter.smsReceived,
                    SMS_RECEIVED_CHOICE_LABEL,
                    SMS_RECEIVED_CHOICE_TOOLTIP);
            }
        }

        if (centralTrafficSynchronizer.isBluetoothSupported()) {
            if ((filterMask & TrafficView.TRAFFIC_FILTER_BLUETOOTH_SENT) != 0) {
                addCheck(menuButton, filter.bluetoothSent,
                    BLUETOOTH_SENT_CHOICE_LABEL,
                    BLUETOOTH_SENT_CHOICE_TOOLTIP);
            }
            if ((filterMask & TrafficView.TRAFFIC_FILTER_BLUETOOTH_RECEIVED) != 0) {
                addCheck(menuButton, filter.bluetoothReceived,
                    BLUETOOTH_RECEIVED_CHOICE_LABEL,
                    BLUETOOTH_RECEIVED_CHOICE_TOOLTIP);
            }
        }

        if (centralTrafficSynchronizer.isTCPUDPSupported() &&
            ((filterMask & (TrafficView.TRAFFIC_FILTER_TCPUDP_SENT |
            TrafficView.TRAFFIC_FILTER_TCPUDP_RECEIVED)) != 0)) {

            menuButton.addSeparator();
            if ((filterMask & TrafficView.TRAFFIC_FILTER_TCPUDP_SENT) != 0) {
                addCheck(menuButton, filter.tcpudpSent,
                    TCPUDP_SENT_CHOICE_LABEL,
                    TCPUDP_SENT_CHOICE_TOOLTIP);
            }
            if ((filterMask & TrafficView.TRAFFIC_FILTER_TCPUDP_RECEIVED) != 0) {
                addCheck(menuButton, filter.tcpudpReceived,
                    TCPUDP_RECEIVED_CHOICE_LABEL,
                    TCPUDP_RECEIVED_CHOICE_TOOLTIP);
            }
        }

        AbstractAction all =
            new AbstractAction(
                AppEnvironment.getString(Resource.ALL_CHOICE_LABEL)) {
            {
                this.putValue(
                    Action.LONG_DESCRIPTION,
                    AppEnvironment.getString(Resource.ALL_CHOICE_TOOLTIP));
            }
            public void actionPerformed(ActionEvent arg0) {
                setAllFiltersEnabled(true);
            }
        };

        Action none =
            new AbstractAction(
                AppEnvironment.getString(Resource.NONE_CHOICE_LABEL)) {

            {
                this.putValue(
                    Action.LONG_DESCRIPTION,
                    AppEnvironment.getString(Resource.NONE_CHOICE_TOOLTIP));
            }

            public void actionPerformed(ActionEvent arg0) {
                setAllFiltersEnabled(false);
            }
        };

        menuButton.addSeparator();
        menuButton.addAction(all);
        menuButton.addAction(none);
        menuButton.setHorizontalTextPosition(MenuButton.RIGHT);

        Misc.setOpaque(menuButton, false);
        RolloverJButton.transformToRollover(menuButton);

        JPanel buttons = new JPanel();
        buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
        buttons.add(Box.createHorizontalGlue());
        buttons.add(menuButton);
        Misc.setOpaque(buttons, false);
        return buttons;
    }
}
