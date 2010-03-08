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





package com.nokia.wtk.diagnostics.views;

/* java.util */
import com.nokia.wtk.util.ArrayUtils;

/**
 * A class responsible for passing events in between views.
 *
 */
public class InterViewCommunicator  {

    // Vector of Log View Listeners
    private LogViewListener [] logViewListeners = new LogViewListener[0];

    /**
     * Adds a LogViewListener to the list of Log View Listeners
     * @param l the LogViewListener to be added to the listeners list
     */
    public synchronized void addLogViewListener (LogViewListener l) {
        if (!ArrayUtils.contains(logViewListeners, l)) {
            logViewListeners = (LogViewListener[])ArrayUtils.add(logViewListeners,l);
        }
    }

    /**
     * Removes the specified LogViewListener from the list of Log View
     * Listeners
     * @param l the LogViewListener to be removed from the listeners list
     */
    public synchronized void removeLogViewListener (LogViewListener l) {
        logViewListeners = (LogViewListener[])ArrayUtils.remove(logViewListeners,l);
    }

    /**
     * Invoked when the log view has been cleared
     */
    public void logViewCleared() {
        LogViewListener [] listeners = logViewListeners;
        for (int i=0; i<listeners.length; i++) {
            listeners[i].logViewCleared();
        }
    }
}
