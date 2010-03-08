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



package com.nokia.wtk.app;

/* java.util */
import java.util.Set;
import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;

/* javax.swing */
import javax.swing.SwingUtilities;

import com.nokia.wtk.util.WeakHashSet;
import com.nokia.wtk.util.ArrayUtils;
/**
 * Support for progress indicators. Multiple progress indicators are alowed.
 * Indicators are being notified on GUI thread, hence the UI suffix.
 *
 * <p>
 * NOTE: since this class maintains a static list of all registered indicators,
 * it's a potential source of "memory leaks". Registered indicators will never
 * be picked up by the garbage collector until they deregister. Typically,
 * it's not a problem, but developers should be aware of that. If automatic
 * garbage collection of progress indicators was required, this class could
 * store weak references to the indicators. At this point, I don't think this
 * is a requirement.
 *
 */
public class ProgressUI {

    /**
     * Progress indicator interface. Indicator is being always invoked
     * on GUI thread.
     */
    public interface Indicator {
        public void startProgress();
        public void stopProgress();
    }

    /** Empty array of indicators */
    private final Indicator[] NO_INDICATORS = new Indicator[0];

    /** Array of registered progress indicators */
    private Indicator[] indicators = NO_INDICATORS;

    /** Internal lock invisible from the outside */
    private final Object lock = new Object();

    /** A weak set of "progress operations". */
    private final Set operations = new WeakHashSet(3);

    /** The object that invokes indicators on GUI thread */
    private final Runnable updater = new Updater();

    /** No need to instantiate this class */
    ProgressUI() {}

    /**
     * Registers an indicator. Does not add the same indicator twice.
     * @param i the indicator to add
     */
    public void addIndicator(Indicator i) {
        if (i == null) {
            throw new NullPointerException("null indicator");
        }
        synchronized (lock) {
            if (!ArrayUtils.contains(indicators, i)) {
                indicators = (Indicator[])ArrayUtils.add(indicators, i);
                updateIndicators();
            }
        }
    }

    /**
     * Unregisters an indicator
     * @param i the indicator to remove
     */
    public void removeIndicator(Indicator i) {
        synchronized (lock) {
            indicators = (Indicator[])ArrayUtils.remove(indicators, i);
            updateIndicators();
        }
    }

    /**
     * Returns <code>true</code> if at least one operation is in progress
     */
    public boolean isActive() {
        synchronized (lock) {
            return (!operations.isEmpty());
        }
    }

    /**
     * Start a progress indicator. Returns the object that needs to be passed
     * to stopProgress() in order to stop progress indicator.
     */
    public Object startProgress() {
        Object operation = new Object();
        startProgress(operation);
        return operation;
    }

    /**
     * Start a progress indicator for specified operation. <code>null</code>
     * operation is ignored
     */
    public void startProgress(Object operation) {
        if (operation != null) {
            synchronized (lock) {
                operations.add(operation);
                updateIndicators();
            }
        }
    }

    /**
     * Stop a progress indicator for specified operation. <code>null</code>
     * operation is ignored
     */
    public void stopProgress(Object operation) {
        if (operation != null) {
            synchronized (lock) {
                operations.remove(operation);
                updateIndicators();
            }
        }
    }

    /**
     * Update indicators on event dispatch thread.
     */
    private void updateIndicators() {
        SwingUtilities.invokeLater(updater);
    }

    /**
     * The object that invokes indicators on GUI thread
     */
    private class Updater implements Runnable {
        private boolean active = false;
        private Indicator[] activeIndicators = NO_INDICATORS;

        Updater() {}
        public void run() {

            boolean wasActive = active;
            List newIndicators = null;

            // copy information under synchronization to ensure consistency
            synchronized (ProgressUI.this.lock) {
                wasActive = active;
                active = (!ProgressUI.this.operations.isEmpty());

                // if operation is in progress, we always notify new indicators
                // even if current state didn't change. This is done so that
                // indicators don't get confused if they get called with
                // stopProgress() without startProgress() being invoked first
                if (active && wasActive &&
                   !Arrays.equals(activeIndicators,ProgressUI.this.indicators)) {

                    // store new indicators in newIndicators list
                    for (int i=0; i<ProgressUI.this.indicators.length; i++) {
                        Indicator indicator = ProgressUI.this.indicators[i];
                        if (!ArrayUtils.contains(activeIndicators,indicator)) {
                            if (newIndicators == null) {
                                newIndicators = new ArrayList();
                            }
                            newIndicators.add(indicator);
                        }
                    }
                }

                // keep reference to the most recent array of indicators
                activeIndicators = ProgressUI.this.indicators;
            }

            // if progress state has changed, notify all indicators
            if (wasActive != active) {
                int n = activeIndicators.length;
                for (int i=0; i<n; i++) {
                    try {
                        if (active) {
                            activeIndicators[i].startProgress();
                        } else {
                            activeIndicators[i].stopProgress();
                        }
                    } catch (Throwable x) {
                        x.printStackTrace();
                    }
                }

            // if the state didn't change, we may still need to notify
            // new indicators that operation is currently in progress
            } else if (active && newIndicators != null) {
                int n = newIndicators.size();
                for (int i=0; i<n; i++) {
                    try { ((Indicator)(newIndicators.get(i))).startProgress(); }
                    catch (Throwable x) { x.printStackTrace(); }
                }
            }
        }
    }
}
