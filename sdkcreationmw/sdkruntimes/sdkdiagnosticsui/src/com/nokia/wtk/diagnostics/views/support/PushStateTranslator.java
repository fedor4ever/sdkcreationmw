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





package com.nokia.wtk.diagnostics.views.support;

import java.util.Hashtable;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.browser.PushMessages;
/**
 * <p>Title: PushStateTRanslator</p>
 * <p>Description: A simple class to translate between push states and
 * human-redaable strings from a properties file. This class is by itself so
 * it can be shared by multiple other classes.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
public
class PushStateTranslator  {

    private static final Hashtable translator__ = new Hashtable();
    private static final String KEY_FOR_UNKNOWN =
                                        "pushStateTranslation.state.unknown";

    static {
        AppEnvironment.addResources(
                    "resources/PushStateTranslator.properties",
                    PushStateTranslator.class);

        translator__.put(
                    new Integer(PushMessages.STATE_PENDING),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.pending"));
        translator__.put(
                    new Integer(PushMessages.STATE_ACTIVATED),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.activated"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_USER),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.removedUser"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_UNKNOWN),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.removedUnknown"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_EXPIRED),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.removedExpired"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_REPLACED),
                    AppEnvironment.getString(
                                        "pushStateTranslation.replaced"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_ACTION),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.removedAction"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_NO_ACTION),
                    AppEnvironment.getString(
                                        "pushStateTranslation.removedNoAction"));
        translator__.put(
                    new Integer(PushMessages.STATE_REMOVED_CAPACITY),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.removedCapacity"));
        translator__.put(
                    new Integer(PushMessages.STATE_UNKNOWN),
                    AppEnvironment.getString(
                                        KEY_FOR_UNKNOWN));
        translator__.put(
                    new Integer(PushMessages.STATE_NEW),
                    AppEnvironment.getString(
                                        "pushStateTranslation.state.new"));
    }

    /**
     * Translate a PushInbox state into a printable string.
     * @see com.nokia.wtk.diagnostics.browser.PushMessages
     */
    public static String getStateString(int st) {

        String ret = (String) translator__.get(new Integer(st));
        if (ret == null) {
            ret = KEY_FOR_UNKNOWN;
            ret = AppEnvironment.getString(ret);
        }
        return ret;
    }

    /**
     * Return whether the supplied state corresponds with a REMOVED of some kind
     * @param state the state from a PushMessage.getState() call
     * @return false if the state corresponds with the message having been
     * removed in any way, true otherwise.
     */
    public static boolean isStateNotRemoved(int state) {
        return (state == PushMessages.STATE_NEW) ||
                    (state == PushMessages.STATE_ACTIVATED) ||
                    (state == PushMessages.STATE_PENDING) ||
                    (state == PushMessages.STATE_UNKNOWN);
    }

}
