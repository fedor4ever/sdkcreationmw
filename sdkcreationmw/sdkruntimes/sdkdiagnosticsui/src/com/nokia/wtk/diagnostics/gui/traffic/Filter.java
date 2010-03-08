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

import java.beans.PropertyChangeListener;

import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.views.support.ContentNode;

/**
 *
 * A class implementing a model for a Filter for the traffic tree
 * */
class Filter {
    final static String REQUESTS_VISIBLE = "http-requests";
    final static String RESPONSES_VISIBLE = "http-responses";
    final static String PUSH_VISIBLE = "push-messages";
    final static String SMS_SENT_VISIBLE = "sms-sent";
    final static String SMS_RECEIVED_VISIBLE = "sms-received";
    final static String BLUETOOTH_SENT_VISIBLE = "bluetooth-sent";
    final static String BLUETOOTH_RECEIVED_VISIBLE = "bluetooth-received";
    final static String TCPUDP_SENT_VISIBLE = "tcpudp-sent";
    final static String TCPUDP_RECEIVED_VISIBLE = "tcpudp-received";

    static {
        AppEnvironment.addResources("resources/Filter.properties", Filter.class);
    }


    final Property requests;
    final Property responses;
    final Property pushed;
    final Property smsSent;
    final Property smsReceived;
    final Property bluetoothSent;
    final Property bluetoothReceived;
    final Property tcpudpReceived;
    final Property tcpudpSent;

    final PropertySet filterProps_ = new PropertySet();

    Filter() {

        requests =
            filterProps_.addBooleanProperty(
                REQUESTS_VISIBLE,
                AppEnvironment.getBoolean(REQUESTS_VISIBLE, true),
                true);
        responses =
            filterProps_.addBooleanProperty(
                RESPONSES_VISIBLE,
                AppEnvironment.getBoolean(RESPONSES_VISIBLE, true),
                true);
        pushed =
            filterProps_.addBooleanProperty(
                PUSH_VISIBLE,
                AppEnvironment.getBoolean(PUSH_VISIBLE, true),
                true);
        smsSent =
            filterProps_.addBooleanProperty(
                SMS_SENT_VISIBLE,
                AppEnvironment.getBoolean(SMS_SENT_VISIBLE, true),
                true);
        smsReceived =
            filterProps_.addBooleanProperty(
                SMS_RECEIVED_VISIBLE,
                AppEnvironment.getBoolean(SMS_RECEIVED_VISIBLE, true),
                true);
        bluetoothSent =
            filterProps_.addBooleanProperty(
                BLUETOOTH_SENT_VISIBLE,
                AppEnvironment.getBoolean(BLUETOOTH_SENT_VISIBLE, true),
                true);
        bluetoothReceived =
            filterProps_.addBooleanProperty(
                BLUETOOTH_RECEIVED_VISIBLE,
                AppEnvironment.getBoolean(BLUETOOTH_RECEIVED_VISIBLE, true),
                true);
        tcpudpReceived =
            filterProps_.addBooleanProperty(
                TCPUDP_RECEIVED_VISIBLE,
                AppEnvironment.getBoolean(TCPUDP_RECEIVED_VISIBLE, false),
                true);
        tcpudpSent =
            filterProps_.addBooleanProperty(
                TCPUDP_SENT_VISIBLE,
                AppEnvironment.getBoolean(TCPUDP_SENT_VISIBLE, false),
                true);
    }

    PropertySet getProperties() {
        return filterProps_;
    }

    /**
     * Add a property change listener for the REQUESTS_VISIBLE,
     * RESPONSES_VISIBLE, and PUSH_VISIBLE properties
     * @param p the PropertyChangeListener to add
     */
    public void addPropertyChangeListener(PropertyChangeListener p) {
        filterProps_.addPropertyChangeListener(p);
    }

    /**
     * Remove a property change listener for the REQUESTS_VISIBLE,
     * RESPONSES_VISIBLE, and PUSH_VISIBLE properties
     * @param p the PropertyChangeListener to remove
     */
    public void removePropertyChangeListener(PropertyChangeListener p) {
        filterProps_.removePropertyChangeListener(p);
    }

    /**
     * Requests whether a ContentNode should be visible or not in the tree,
     * according to this filter
     * @param node the node to test
     * @return false if the node passes the filter, true if it should not
     * be visible
     */
    public boolean isFilteredOut(ContentNode node) {
        if (node instanceof TopLevelContentNode) {
            return ((TopLevelContentNode) node).isFiltered(this);
        }
        return true;
    }

    /**
     * Set this filter to allow requests to be visible in the tree
     * @param visible whether to set requests visible or not
     */
    public void setRequestsVisible(boolean visible) {
        filterProps_.setBooleanValue(REQUESTS_VISIBLE, visible);
    }

    /**
     * Return whether this filter says request nodes should be visible
     * @return true if requests should be visible in the tree, false
     * otherwise
     */
    public boolean isRequestsVisible() {
        return filterProps_.getBooleanValue(REQUESTS_VISIBLE, true);
    }

    /**
     * Set this filter to allow rsponses to be visible in the tree
     * @param visible whether to set responses visible or not
     */
    public void setResponsesVisible(boolean visible) {
        filterProps_.setBooleanValue(RESPONSES_VISIBLE, visible);
    }

    /**
     * Return whether this filter says response nodes should be visible
     * @return true if responses should be visible in the tree, false
     * otherwise
     */
    public boolean isResponsesVisible() {
        return filterProps_.getBooleanValue(RESPONSES_VISIBLE, true);
    }

    /**
     * Set this filter to allow pushed content to be visible in the tree
     * @param visible whether to set pushed content nodes visible or not
     */
    public void setPushedVisible(boolean visible) {
        filterProps_.setBooleanValue(PUSH_VISIBLE, visible);
    }

    /**
     * Return whether this filter says pushed nodes should be visible
     * @return true if pushed should be visible in the tree, false
     * otherwise
     */
    public boolean isPushedVisible() {
        return filterProps_.getBooleanValue(PUSH_VISIBLE, true);
    }

    /**
     * Set this filter to allow sent SMS to be visible in the tree
     * @param visible whether to set SMS content nodes visible or not
     */
    public void setSMSSentVisible(boolean visible) {
        filterProps_.setBooleanValue(SMS_SENT_VISIBLE, visible);
    }

    /**
     * Return whether this filter says sent SMS nodes should be visible
     * @return true if SMS should be visible in the tree, false
     * otherwise
     */
    public boolean isSMSSentVisible() {
        return filterProps_.getBooleanValue(SMS_SENT_VISIBLE, true);
    }

    /**
     * Set this filter to allow received SMS to be visible in the tree
     * @param visible whether to set SMS content nodes visible or not
     */
    public void setSMSReceivedVisible(boolean visible) {
        filterProps_.setBooleanValue(SMS_RECEIVED_VISIBLE, visible);
    }

    /**
     * Return whether this filter says received SMS nodes should be visible
     * @return true if SMS should be visible in the tree, false
     * otherwise
     */
    public boolean isSMSReceivedVisible() {
        return filterProps_.getBooleanValue(SMS_RECEIVED_VISIBLE, true);
    }

    /**
     * Set this filter to allow received Bluetooth to be visible in the tree
     * @param visible whether to set Bluetooth content nodes visible or not
     */
    public void setBluetoothReceivedVisible(boolean visible) {
        filterProps_.setBooleanValue(BLUETOOTH_RECEIVED_VISIBLE, visible);
    }

    /**
     * Return whether this filter says received Bluetooth nodes should be visible
     * @return true if Bluetooth should be visible in the tree, false
     * otherwise
     */
    public boolean isBluetoothReceivedVisible() {
        return filterProps_.getBooleanValue(BLUETOOTH_RECEIVED_VISIBLE, true);
    }

    /**
     * Set this filter to allow sent Bluetooth to be visible in the tree
     * @param visible whether to set Bluetooth content nodes visible or not
     */
    public void setBluetoothSentVisible(boolean visible) {
        filterProps_.setBooleanValue(BLUETOOTH_SENT_VISIBLE, visible);
    }

    /**
     * Return whether this filter says sent Bluetooth nodes should be visible
     * @return true if Bluetooth should be visible in the tree, false
     * otherwise
     */
    public boolean isBluetoothSentVisible() {
        return filterProps_.getBooleanValue(BLUETOOTH_SENT_VISIBLE, true);
    }

    /**
     * Set this filter to allow sent TCP/UDP to be visible in the tree
     * @param visible whether to set tcp/udp content nodes visible or not
     */
    public void setTCPUDPSentVisible(boolean visible) {
        filterProps_.setBooleanValue(TCPUDP_SENT_VISIBLE, visible);
    }

    /**
     * Return whether this filter says sent TCP/UDP nodes should be visible
     * @return true if tcp/udp  should be visible in the tree, false
     * otherwise
     */
    public boolean isTCPUDPSentVisible() {
        return filterProps_.getBooleanValue(TCPUDP_SENT_VISIBLE, true);
    }

    /**
     * Set this filter to allow received TCP/UDP to be visible in the tree
     * @param visible whether to set tcp/udp  content nodes visible or not
     */
    public void setTCPUDPReceivedVisible(boolean visible) {
        filterProps_.setBooleanValue(TCPUDP_RECEIVED_VISIBLE, visible);
    }

    /**
     * Return whether this filter says received TCP/uDP nodes should be visible
     * @return true if tcp/udp  should be visible in the tree, false
     * otherwise
     */
    public boolean isTCPUDPReceivedVisible() {
        return filterProps_.getBooleanValue(TCPUDP_RECEIVED_VISIBLE, true);
    }
}
