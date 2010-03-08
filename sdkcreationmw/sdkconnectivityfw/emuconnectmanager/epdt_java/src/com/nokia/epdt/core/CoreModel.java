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


package com.nokia.epdt.core;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Hashtable;
import java.util.Observable;

import javax.swing.JFrame;

import com.nokia.wtk.device.Device;
import com.nokia.epdt.core.connection.Connection;
import com.nokia.epdt.core.connection.ConnectionStatus;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.plugin.PluginInstance;
import com.nokia.epdt.core.ui.UIFrameFactory;

/**
 * CoreModel
 */
public class CoreModel extends Observable implements Device {
	// A list that contains all a currently available plug-ins
    private PluginInstance [] plugins = new PluginInstance[0]; 
    private Map serviceMap = new Hashtable();
    private Connection connection = null;
    private ConnectionStatus connectionStatus = ConnectionStatus.IDLE;
    private CoreController controller;
    private boolean exitOnClose = false;
    
    // Preferences
    private PluginInstance [] preferencesPlugins = new PluginInstance[0];
    private JFrame preferencesUI = null;
    private boolean preferencesApplyJButtonEnabled = false;
    // Contains UIDs of the preferences plug-ins that model is changed
    private List changedPreferencesPluginModels = new ArrayList(); 
    // Contains UIDs of the preferences plug-ins that view is not ready to hide
    private List notReadyToHidePreferencesPluginList = new ArrayList(); 
    
    // Diagnostics
    private PluginInstance [] diagnosticsPlugins = new PluginInstance[0];
    private JFrame diagnosticsUI = null;
    // Contains UIDs of the diagnostics plug-ins that model is changed
    private List changedDiagnosticsPluginModels = new ArrayList(); 
    // Contains UIDs of the diagnostics plug-ins that view is not ready to hide
    private List notReadyToHideDiagnosticsPluginList = new ArrayList(); 
    
    // Utilities
    private PluginInstance [] utilitiesPlugins = new PluginInstance[0];
    private JFrame utilitiesUI = null;
    private boolean utilitiesApplyJButtonEnabled = false;
    // Contains UIDs of the utilities plug-ins that model is changed
    private List changedUtilitiesPluginModels = new ArrayList(); 
    // Contains UIDs of the utilities plug-ins that view is not ready to hide
    private List notReadyToHideUtilitiesPluginList = new ArrayList(); 

    /**
     * Creates a new instance of CoreModel
     * @param controller associated instance of {@link CoreController}
     */
    public CoreModel(CoreController controller) {
        this.controller = controller;
    }

    /**
     * Getter for property connectionStatus.
     * @return Value of property connectionStatus.
     */
    public ConnectionStatus getConnectionStatus() {
        return connectionStatus;
    }

    /**
     * Setter for property connectionStatus.
     * @param connectionStatus New value of property connectionStatus.
     */
    public void setConnectionStatus(ConnectionStatus connectionStatus) {
        this.connectionStatus = connectionStatus;
    }

    /**
     * Getter for property plugins.
     * @return Value of property plugins.
     */
    public PluginInstance [] getPlugins() {
        return (PluginInstance [])plugins.clone();
    }

    /**
     * Setter for property plugins.
     * @param p New value of property plugins.
     */
    public void setPlugins(PluginInstance [] p) {
        if (p == null) {
            plugins = new PluginInstance [0];
        } else {
            plugins = (PluginInstance [])p.clone();
        }

        List preferences = new ArrayList(0);
        List diagnostics = new ArrayList(0);
        List utilities = new ArrayList(0);
        for (int i=0; i<plugins.length; i++) {
            PluginType pluginType = plugins[i].getPlugin().getPluginType();
            if (pluginType == PluginType.PREFERENCES) {
                preferences.add(plugins[i]);
            } else if (pluginType == PluginType.DIAGNOSTICS) {
                diagnostics.add(plugins[i]);
            } else if (pluginType == PluginType.UTILITIES) {
                utilities.add(plugins[i]);
            }
        }
        preferencesPlugins = (PluginInstance [])preferences.toArray(
            new PluginInstance[preferences.size()]);
        diagnosticsPlugins = (PluginInstance [])diagnostics.toArray(
            new PluginInstance[diagnostics.size()]);
        utilitiesPlugins = (PluginInstance [])utilities.toArray(
            new PluginInstance[utilities.size()]);
    }

    /**
     * Getter for property connection.
     * @return Value of property connection.
     */
    public Connection getConnection() {
        return connection;
    }

    /**
     * Setter for property connection.
     * @param connection New value of property connection.
     */
    public void setConnection(Connection connection) {
        this.connection = connection;
    }

    /**
     * Getter for property diagnosticUI.
     * @return Value of property diagnosticUI.
     */
    public JFrame getDiagnosticsUI() {
        if (diagnosticsUI == null) {
            diagnosticsUI = UIFrameFactory.buildDiagnosticsFrame(controller);
        }
        return diagnosticsUI;
    }

    /**
     * Checks if Diagnostics UI is visible.
     * @return <code>true</code> if Diagnostics UI is visible,
     *    <code>false</code> otherwise.
     */
    public boolean isDiagnosticsUIVisible() {
        if (diagnosticsUI == null) {
            // if it doesn't exist, it's obviously not visible
            return false;
        } else {
            return diagnosticsUI.isVisible();
        }
    }

    /**
     * Getter for property preferencesUI.
     * @return Value of property preferencesUI.
     */
    public JFrame getPreferencesUI() {
        if (preferencesUI == null) {
            preferencesUI = UIFrameFactory.buildPreferencesFrame(controller);
        }
        return preferencesUI;
    }

    /**
     * Checks if Preferences UI is visible.
     * @return <code>true</code> if Preferences UI is visible,
     *    <code>false</code> otherwise.
     */
    public boolean isPreferencesUIVisible() {
        if (preferencesUI == null) {
            // if it doesn't exist, it's obviously not visible
            return false;
        } else {
            return preferencesUI.isVisible();
        }
    }

    /** Getter for property utilitiesUI.
     * @return Value of property utilitiesUI.
     *
     */
    public JFrame getUtilitiesUI() {
        if (utilitiesUI == null) {
            utilitiesUI = UIFrameFactory.buildUtilitiesFrame(controller);
        }
        return utilitiesUI;
    }

    /**
     * Checks if Utilities UI is visible.
     * @return <code>true</code> if Utilities UI is visible,
     *    <code>false</code> otherwise.
     */
    public boolean isUtilitiesUIVisible() {
        if (utilitiesUI == null) {
            // if it doesn't exist, it's obviously not visible
            return false;
        } else {
            return utilitiesUI.isVisible();
        }
    }

    /**
     * Hides the diagnostics UI
     */
    public void hideDiagnosticsUI() {
        if (diagnosticsUI != null) {
            diagnosticsUI.setVisible(false);
        }
    }

    /**
     * Hides the preferences UI
     */
    public void hidePreferencesUI() {
        if (preferencesUI != null) {
            preferencesUI.setVisible(false);
        }
    }

    /**
     * Hides the utilities UI
     */
    public void hideUtilitiesUI() {
        if (utilitiesUI != null) {
            utilitiesUI.setVisible(false);
        }
    }

    /** Getter for property preferencesApplyJButtonEnabled.
     * @return Value of property preferencesApplyJButtonEnabled.
     *
     */
    public boolean isPreferencesApplyJButtonEnabled() {
        return preferencesApplyJButtonEnabled;
    }

    /** Setter for property preferencesApplyJButtonEnabled.
     * @param applyButtonEnabled New value of property preferencesApplyJButtonEnabled.
     *
     */
    public void setPreferencesApplyJButtonEnabled(boolean applyButtonEnabled) {
        if (preferencesApplyJButtonEnabled != applyButtonEnabled) {
            preferencesApplyJButtonEnabled = applyButtonEnabled;
            if (!applyButtonEnabled) changedPreferencesPluginModels.clear();
            setChanged();
            notifyObservers();
        }
    }

    /** Getter for property changedPreferencesPluginModels.
     * @return Value of property changedPreferencesPluginModels.
     *
     */
    public List getChangedPreferencesPluginModels() {
        return changedPreferencesPluginModels;
    }

    /** Setter for property changedPreferencesPluginModels.
     * @param changedPreferencesPluginModels New value of property changedPreferencesPluginModels.
     *
     */
    public void setChangedPreferencesPluginModels(List changedPreferencesPluginModels) {
        this.changedPreferencesPluginModels = changedPreferencesPluginModels;
    }

    /** Getter for property notReadyToHidePreferencesPluginList.
     * @return Value of property notReadyToHidePreferencesPluginList.
     *
     */
    public List getNotReadyToHidePreferencesPluginList() {
        return notReadyToHidePreferencesPluginList;
    }

    /** Setter for property notReadyToHidePreferencesPluginList.
     * @param notReadyToHidePreferencesPluginList New value of property notReadyToHidePreferencesPluginList.
     *
     */
    public void setNotReadyToHidePreferencesPluginList(List notReadyToHidePreferencesPluginList) {
        this.notReadyToHidePreferencesPluginList = notReadyToHidePreferencesPluginList;
    }

    /** Getter for property changedDiagnosticsPluginModels.
     * @return Value of property changedDiagnosticsPluginModels.
     *
     */
    public List getChangedDiagnosticsPluginModels() {
        return changedDiagnosticsPluginModels;
    }

    /** Setter for property changedDiagnosticsPluginModels.
     * @param changedDiagnosticsPluginModels New value of property changedDiagnosticsPluginModels.
     *
     */
    public void setChangedDiagnosticsPluginModels(List changedDiagnosticsPluginModels) {
        this.changedDiagnosticsPluginModels = changedDiagnosticsPluginModels;
    }

    /** Getter for property notReadyToHideDiagnosticsPluginList.
     * @return Value of property notReadyToHideDiagnosticsPluginList.
     *
     */
    public List getNotReadyToHideDiagnosticsPluginList() {
        return notReadyToHideDiagnosticsPluginList;
    }

    /** Setter for property notReadyToHideDiagnosticsPluginList.
     * @param notReadyToHideDiagnosticsPluginList New value of property notReadyToHideDiagnosticsPluginList.
     *
     */
    public void setNotReadyToHideDiagnosticsPluginList(List notReadyToHideDiagnosticsPluginList) {
        this.notReadyToHideDiagnosticsPluginList = notReadyToHideDiagnosticsPluginList;
    }

    /** Getter for property diagnosticsPlugins.
     * @return Value of property diagnosticsPlugins.
     *
     */
    public PluginInstance [] getDiagnosticsPlugins() {
        return (PluginInstance[])diagnosticsPlugins.clone();
    }

    /** Getter for property preferencesPlugins.
     * @return Value of property preferencesPlugins.
     *
     */
    public PluginInstance [] getPreferencesPlugins() {
        return (PluginInstance[])preferencesPlugins;
    }

    /** Getter for property exitOnClose.
     * @return Value of property exitOnClose.
     *
     */
    public boolean isExitOnClose() {
        return exitOnClose;
    }

    /** Setter for property exitOnClose.
     * @param exitOnClose New value of property exitOnClose.
     *
     */
    public void setExitOnClose(boolean exitOnClose) {
        this.exitOnClose = exitOnClose;
    }

    /** Getter for property utilitiesPlugins.
     * @return Value of property utilitiesPlugins.
     *
     */
    public PluginInstance [] getUtilitiesPlugins() {
        return (PluginInstance[])utilitiesPlugins.clone();
    }

    /** Getter for property utilitiesApplyJButtonEnabled.
     * @return Value of property utilitiesApplyJButtonEnabled.
     *
     */
    public boolean isUtilitiesApplyJButtonEnabled() {
        return utilitiesApplyJButtonEnabled;
    }

    /** Setter for property utilitiesApplyJButtonEnabled.
     * @param applyButtonEnabled New value of property utilitiesApplyJButtonEnabled.
     *
     */
    public void setUtilitiesApplyJButtonEnabled(boolean applyButtonEnabled) {
        if (utilitiesApplyJButtonEnabled != applyButtonEnabled) {
            utilitiesApplyJButtonEnabled = applyButtonEnabled;
            if (!applyButtonEnabled) changedUtilitiesPluginModels.clear();
            setChanged();
            notifyObservers();
        }
    }

    /** Getter for property notReadyToHideUtilitiesPluginList.
     * @return Value of property notReadyToHideUtilitiesPluginList.
     *
     */
    public List getNotReadyToHideUtilitiesPluginList() {
        return notReadyToHideUtilitiesPluginList;
    }

    /** Setter for property notReadyToHideUtilitiesPluginList.
     * @param notReadyToHideUtilitiesPluginList New value of property notReadyToHideUtilitiesPluginList.
     *
     */
    public void setNotReadyToHideUtilitiesPluginList(List notReadyToHideUtilitiesPluginList) {
        this.notReadyToHideUtilitiesPluginList = notReadyToHideUtilitiesPluginList;
    }

    /** Getter for property changedUtilitiesPluginModels.
     * @return Value of property changedUtilitiesPluginModels.
     *
     */
    public List getChangedUtilitiesPluginModels() {
        return changedUtilitiesPluginModels;
    }

    /** Setter for property changedUtilitiesPluginModels.
     * @param changedUtilitiesPluginModels New value of property changedUtilitiesPluginModels.
     *
     */
    public void setChangedUtilitiesPluginModels(List changedUtilitiesPluginModels) {
        this.changedUtilitiesPluginModels = changedUtilitiesPluginModels;
    }

    // com.nokia.wtk.device.Device implementation

    /**
     * Called to begin a session with a new device.
     */
    public void openDevice() {}

    /**
     * Called to end a session with a device.
     */
    public void closeDevice() {
        serviceMap.clear();
    }

    /**
     * Returns an instance of a service support object, given an interface.
     * @param type the Class of the device service requested.
     * @return an instance of the service requested or null if not supported.
     */
    public Object getInstanceOf(Class type) {
        return serviceMap.get(type);
    }

    /**
     * Handles registration of host services (listeners)
     * @param type the Class of the service (listener interface)
     * @param instance the instance of the listener or null to unset.
     * @return true if the listener is supported, false if not or unset.
     */
    public boolean setInstanceOf(Class type, Object instance) {
        if (instance != null) {
            serviceMap.put(type, instance);
        } else {
            serviceMap.remove(type);
        }
        // We support everything!
        return true;
    }
}
