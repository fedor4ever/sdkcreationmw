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

package com.nokia.epdt.core.action;

/**
 * Action
 *
 */
public final class Action {
    // name
	public final static String DEFAULT = "Default";
    public final static String DIAGNOSTICS_HIDE_PLUGIN = "DiagnosticsHidePlugin";
    public final static String DIAGNOSTICS_PLUGIN_MODEL_CHANGED = "DiagnosticsPluginModelChanged";
    public final static String DIAGNOSTICS_REMOVE_PLUGIN_MODEL_CHANGED = "DiagnosticsRemovePluginModelChanged";
    public final static String PREFERENCES_APPLY = "PreferencesApply";
    public final static String PREFERENCES_CANCEL = "PreferencesCancel";
    public final static String PREFERENCES_HIDE_PLUGIN = "PreferencesHidePlugin";
    public final static String PREFERENCES_OK = "PreferencesOk";
    public final static String PREFERENCES_PLUGIN_MODEL_CHANGED = "PreferencesPluginModelChanged";
    public final static String PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED = "PreferencesRemovePluginModelChanged";
    public final static String UTILITIES_APPLY = "UtilitiesApply";
    public final static String UTILITIES_CANCEL = "UtilitiesCancel";
    public final static String UTILITIES_HIDE_PLUGIN = "UtilitiesHidePlugin";
    public final static String UTILITIES_OK = "UtilitiesOk";
    public final static String UTILITIES_PLUGIN_MODEL_CHANGED = "UtilitiesPluginModelChanged";
    public final static String UTILITIES_REMOVE_PLUGIN_MODEL_CHANGED = "UtilitiesRemovePluginModelChanged";
    public final static String PROGRAM_CLOSING = "ProgramClosing";

    // Type
    // Event is mediated to core only
    public static final int CORE = 0; 
    // Event is mediated to all plugins that are same uid as included in destination array
    public static final int PLUGIN = 1; 
    // Event is mediated to all
    public static final int ALL = 2; 

    private static final int[] DUMMY_DESTINATION = new int[0];
    private String name;
    private Integer uid; // plug-in UID
    private int type;
    private int[] destination;

    /**
     * Creates a new instance of ActionEvent
     * @param action an action instance
     * @param uid Action event unique identifier
     */
    public Action(String name, Integer uid, int type) {
        this(name, uid, type, DUMMY_DESTINATION);
    }

    /**
     * Creates a new instance of ActionEvent
     * @param action an action instance
     * @param uid Action event unique identifier
     */
    public Action(String name, Integer uid, int type, int[] destination) {
        super();
        this.name = (name == null) ? Action.DEFAULT : name;
        this.uid = uid;
        this.type = type;
        this.destination = (destination == null) ? new int[0] : destination;
    }

    /**
     * Getter for property action. Returns the action associated with this
     * action. This action allows a "modal" component to specify one of
     * several actions, depending on its state. For example, a single button
     * might toggle between "show details" and "hide details". The source
     * object and the event would be the same in each case, but the action
     * instance would specify and identify the intended action.
     * @return the action identifying the action for this event
     */
    public String getName() {
        return this.name;
    }

    public String toString() {
        return getName();
    }

    /**
     * Getter for property uid.
     * @return Value of property uid.
     */
    public Integer getUid() {
        return this.uid;
    }

    /** Getter for property type.
     * @return Value of property type.
     *
     */
    public int getType() {
        return type;
    }

    /** Getter for property destination.
     * @return Value of property destination.
     *
     */
    public int[] getDestination() {
        return destination;
    }
}
