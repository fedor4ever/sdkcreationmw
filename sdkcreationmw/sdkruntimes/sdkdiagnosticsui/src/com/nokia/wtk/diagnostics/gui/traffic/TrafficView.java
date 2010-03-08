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

import javax.swing.JComponent;

import com.nokia.wtk.util.PropertySet;

import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;

/**
 * A GUI for viewing content requests, responses and cache contents.
 *
 */
public class TrafficView implements AbstractViewFactoryInterface {

    // Filter flags
    public static final int TRAFFIC_FILTER_HTTP_REQUESTS      = 0x00000001;
    public static final int TRAFFIC_FILTER_HTTP_RESPONSES     = 0x00000002;
    public static final int TRAFFIC_FILTER_PUSHED             = 0x00000004;
    public static final int TRAFFIC_FILTER_SMS_SENT           = 0x00000008;
    public static final int TRAFFIC_FILTER_SMS_RECEIVED       = 0x00000010;
    public static final int TRAFFIC_FILTER_BLUETOOTH_SENT     = 0x00000020;
    public static final int TRAFFIC_FILTER_BLUETOOTH_RECEIVED = 0x00000040;
    public static final int TRAFFIC_FILTER_TCPUDP_SENT        = 0x00000080;
    public static final int TRAFFIC_FILTER_TCPUDP_RECEIVED    = 0x00000100;
    public static final int TRAFFIC_FILTER_ALL                = 0x000001ff;

    private Browser browser;
    private PropertySet properties;
    private CentralTrafficSynchronizer traffic;
    private TrafficPanel largeView;
    private JComponent summaryView;
    private int filterMask;

    /**
     * Creates TrafficView
     * @param b the data model
     */
    public TrafficView(Browser b) {
        this(b, TRAFFIC_FILTER_ALL);
    }

    /**
     * Creates TrafficView
     * @param b the data model
     * @param filters the filter mask
     */
    public TrafficView(Browser b, int filters) {
        browser = b;
        properties = new PropertySet();
        filterMask = filters;
    }

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey() {
        return "traffic-view";
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
        if (largeView == null) {
            largeView = new TrafficPanel(getTrafficSynchronizer(), filterMask);
            properties.addPropertySet("traffic",largeView.getProperties(),true);
        }
        return largeView;
    }

    /**
     * Create the small component to show.
     * @returns a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent() {
        if (summaryView == null) {
            summaryView = new TrafficSummaryView(getTrafficSynchronizer());
        }
        return summaryView;
    }

    /**
     * @returns the short display name of the of the component.
     */
    public String getViewTitle() {
        return "Traffic";
    }

    /**
     * @returns a longer description of the component
     */
    public String getViewDescription() {
        return "Details of Browser Requests, Responses from servers.";
    }

    /**
     * @returns true if the view is expanded by default
     */
    public boolean getDefaultViewOpen() {
        return true;
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
        getTrafficSynchronizer().getTrafficModel().bindToCommunicator(comm);
    }

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm) {
        getTrafficSynchronizer().getTrafficModel().unbindFromCommunicator(comm);
    }

    private CentralTrafficSynchronizer getTrafficSynchronizer() {
        if (traffic == null) {
            traffic = new CentralTrafficSynchronizer(browser);
        }
        return traffic;
    }
}
