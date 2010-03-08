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

import java.util.*;

import com.nokia.wtk.diagnostics.browser.*;



public class ContentDelayedHeartbeat {

    private Vector registrants_ = new Vector(3);
    private NotifyAfterDelay notifyAfterDelay_ = new NotifyAfterDelay();
    private BrowserHistory.HistoryListListener historyListener_;
    private PushMessages.LoggedMessageListener inboxListener_;
    private Browser browser_;

    public ContentDelayedHeartbeat(Browser b) {
        b.getBrowserHistory().addHistoryListListener(
            historyListener_ = new BrowserHistory.HistoryListListener() {
                public void historyAdded(int index) {}
                public void historyUpdated(int index) {}
                public void historyRemoved(int index) {}
                public void allHistoryRemoved() {}
                public void currentPositionChanged() {
                    notifyAfterDelay_.queue();
                }
            });
        b.getPushMessages().addLoggedMessageListener(
            inboxListener_ = new PushMessages.LoggedMessageListener() {
                public void messageChanged(String key) {}
                public void messageRemoved(String key) {}
                public void messageAdded(String key) {
                    notifyAfterDelay_.queue();
                }
                public void allMessagesRemoved() {}
            });
        browser_ = b;
    }

    public synchronized void addContentHeartbeatListener(
                                                        HeartbeatListener n) {
        if (!registrants_.contains(n)) {
            registrants_.add(n);
        }
    }
    public synchronized void removeContentHeartbeatListener(
                                                        HeartbeatListener n) {
        registrants_.remove(n);
    }

    public void dispose() {
        registrants_.clear();
        browser_.getBrowserHistory().removeHistoryListListener(
                                                            historyListener_);
        browser_.getPushMessages().removeLoggedMessageListener(inboxListener_);
    }

    /**
     * <p>Title: NotifyAfterDelay</p>
     * <p>Description: This is the Runnable that will issue the heartbeat
     * with a 250ms delay to all listeners. It is built specially so that
     * this Runnable can be asked to run multiple times with the queue
     * call while it asleep those 250 ms, but will execute only once
     * per 250 ms cycle.</p>
     */
    private
    class NotifyAfterDelay
    implements Runnable {
        private boolean go_ = false;

        NotifyAfterDelay() {}

        private synchronized final void queue() {
            if (!go_) {   // first available, so start queue
                Thread z = new Thread(this);
                go_ = true;
                z.start();
            }
        }

        public final void run() {
            try {
                Thread.sleep(250);
            } catch (InterruptedException i) {}
            synchronized (this) {
                for (Enumeration e = registrants_.elements();
                                                    e.hasMoreElements(); ) {
                    ((HeartbeatListener) e.nextElement()).beat();
                }
                go_ = false;
            }
        }
    }

    /**
     * <p>Title: HeartbeatListener</p>
     * <p>Description: This is the interface to actually implement if you
     * want to be informed of the latest content/push updates with a
     * delay to have all other Browser events be processed.</p>
     */
    static public interface HeartbeatListener {
        /**
         * Will be called 250ms after a pus/content received update.
         */
        public void beat();
    }
}
