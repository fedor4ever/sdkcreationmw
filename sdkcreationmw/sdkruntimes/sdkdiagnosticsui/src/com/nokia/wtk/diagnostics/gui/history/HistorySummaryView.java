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

import java.text.MessageFormat;
import java.util.Enumeration;
import java.util.Vector;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.views.support.PopLabel;
import com.nokia.wtk.diagnostics.views.support.ContentDelayedHeartbeat;

/**
 * Shows a summary for the history stack
 *
 */
public
class   HistorySummaryView
extends PopLabel
implements ContentDelayedHeartbeat.HeartbeatListener, Resource {

    private final Browser browser_;

    private int forward = 0;
    private int backward = 0;

    private boolean updated = false;
    private boolean cleared = false;
    private Vector removed = new Vector(3);

    static {
        AppEnvironment.addResources(
            "resources/HistorySummaryView.properties",
            HistorySummaryView.class);
    }

    public HistorySummaryView(Browser b) {
        browser_ = b;
        browser_.getBrowserHistory().addHistoryListListener(
            new BrowserHistory.HistoryListListener() {
                /**
                 * Called when a history item has been added to the list.
                 *
                 * @param index     the history list index.
                 */
                public void historyAdded(int index) {}

                /**
                 * Called when a history item has been updated within the list.
                 *
                 * @param index     the history list index.
                 */
                public void historyUpdated(int index) { updated = true; }

                /**
                 * Called when a variable has been removed from the list.
                 *
                 * @param index     the history list index.
                 */
                public void historyRemoved(int index) {
                    removed.add(new Integer(index));
                }

                /**
                 * Called when all history items have been removed from the
                 * list.
                 */
                public void allHistoryRemoved() { cleared = true; }

                /**
                 * Called when the current document position within the
                 * history item has been changed.
                 */
                public void currentPositionChanged() {}

            });

        new ContentDelayedHeartbeat(b).addContentHeartbeatListener(this);
        beat();
    }

    private void makeText() {
        int newForward = browser_.getBrowserHistory().
                                                    getForwardHistorySize();
        int steps;
        MessageFormat msg;
        boolean position = false;

        String text = "";
        if (newForward > forward) {
                // stepped back
                steps = newForward - forward;
                msg = new MessageFormat(AppEnvironment.getString(
                                                            HIST_SUMMARY_BACK_TXT));
                text += msg.format(new String[] { "" + steps });
        } else if (forward > newForward) {
            // stepped forward
            boolean forwardPathRemoved = false;
            for (Enumeration e = removed.elements(); e.hasMoreElements(); ) {
                Integer j = (Integer) e.nextElement();
                if (j.intValue() >= newForward) {
                    forwardPathRemoved = true;
                    break;
                }
            }
            if (forwardPathRemoved) {
                // what happened is that user moved forward on a new
                // path and removed forward history for a new history
                text += AppEnvironment.getString(HIST_SUMMARY_NP_TXT);
            } else {
                // some browser put there actually implements 'step forward'??
                steps =  forward - newForward;
                msg = new MessageFormat(AppEnvironment.getString(
                                                            HIST_SUMMARY_FORW_TXT));
                text += msg.format(new String[] { "" + steps });
            }
        }
        if (text.length() > 0) {
            msg = new MessageFormat(AppEnvironment.getString(
                                                            HIST_SUMMARY_POS_TXT));
            text += msg.format(new String[] {
                                    "" + (browser_.getBrowserHistory().
                                                        getBackHistorySize() + 1),
                                    "" + browser_.getBrowserHistory().
                                                            getHistorySize() });
            position = true;
        }

        if (cleared) {
            text += AppEnvironment.getString(HIST_SUMMARY_CLEARED_TXT);
        }
        if (removed.size() > 0) {
            msg = new MessageFormat(AppEnvironment.
                                        getString(HIST_SUMMARY_REM_TXT));
            text +=  msg.format(new String[] {"" + removed.size() } );
        }
        if (!position) {
            int size = browser_.getBrowserHistory().getHistorySize();
            msg = new MessageFormat(AppEnvironment.
                                        getString(HIST_SUMMARY_SIZE_TXT));
            text += msg.format(new String[] {"" + size });
        }
        if (text.length() > 0) {
            setText(text.trim());
        }
    }

    /**
     * Implement HeartbeatListener
     */
    public void beat() {
        makeText();
        forward = browser_.getBrowserHistory().getForwardHistorySize();
        backward = browser_.getBrowserHistory().getBackHistorySize();
        cleared = false;
        updated = false;
        removed.clear();
    }
}
