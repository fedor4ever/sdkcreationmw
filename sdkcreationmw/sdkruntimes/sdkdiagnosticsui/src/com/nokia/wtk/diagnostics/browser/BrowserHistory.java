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






package com.nokia.wtk.diagnostics.browser;


import java.util.Stack;
import java.util.EventListener;
import javax.swing.event.EventListenerList;

import com.nokia.wtk.device.DeviceBrowserHistoryListener;
import com.nokia.wtk.device.DeviceContentReceived;


/**
 * The browser history of requested documents for the current device.
 *
 */
public class BrowserHistory {

    /**
     * Creates a new <code>BrowserHistory</code>.
     * @param browser the BRowser instance
     */
    BrowserHistory(Browser browser) {
        this.backHistory = new Stack();
        this.forwardHistory = new Stack();
        setDeviceBrowserHistoryListenerSupported(browser.registerHostService(
            DeviceBrowserHistoryListener.class, getDeviceBrowserHistoryListener()));
    }

    /**
     * Sets the <code>ContentLog</code> instance.
     * @param log the <code>ContentLog</code> instance.
     */
    void setContentLog(ContentLog log) {
        this.contentLog = log;
    }

    /**
     * Gets the <code>ContentLog</code> instance.
     * @return the <code>ContentLog</code> instance.
     */
    private ContentLog getContentLog() {
        return this.contentLog;
    }

    /**
     * Gets the total number of history items.
     * @return the total number of history items.
     */
    public int getHistorySize() {
        return getBackHistorySize()
             + ((getCurrentDocument() != null) ? 1 : 0)
             + getForwardHistorySize();
    }

    /**
     * Gets the number of history items previous to the current document.
     * @return the number of previous history items.
     */
    public int getBackHistorySize() {
        return this.backHistory.size();
    }

    /**
     * Gets the number of history items forward from the current document.
     * @return the number of forward history items.
     */
    public int getForwardHistorySize() {
        return this.forwardHistory.size();
    }

    /**
     * Checks whether there is any previous document history.
     * @return <code>true</code> if there is any previous document history.
     */
    public boolean hasBackHistory() {
        return (!this.backHistory.isEmpty());
    }

    /**
     * Checks whether there is any forward document history.
     * @return <code>true</code> if there is any forward document history.
     */
    public boolean hasForwardHistory() {
        return (!this.forwardHistory.isEmpty());
    }

    /**
     * Gets the index of the current document.
     * @return the current documentposition index.
     */
    public int getCurrentDocumentPosition() {
        return getBackHistorySize();
    }

    /**
     * Gets an item of history at a specified index.
     * @param index the history location index (0 = oldest).
     * @return the history content, or <code>null</code> if not defined.
     */
    public AttributedContent getDocumentAt(int index) {
        AttributedContent item = null;
        if ((index >= 0) && (index < getHistorySize())) {
            if (index < getCurrentDocumentPosition()) {
                item = getBackHistoryDocumentAt((getBackHistorySize() - index) - 1);
            } else if (index > getCurrentDocumentPosition()) {
                item = getForwardHistoryDocumentAt((index - getCurrentDocumentPosition()) - 1);
            } else {
                item = getCurrentDocument();
            }
        }
        return item;
    }

    /**
     * Gets an item of back history at a specified index.
     * @param index the history location index (0 = closest to current document position).
     * @return the history content, or <code>null</code> if not defined.
     */
    private AttributedContent getBackHistoryDocumentAt(int index) {
        AttributedContent item = null;
        if ((index >= 0) && (index < getBackHistorySize())) {
            item = (AttributedContent)
                   this.backHistory.elementAt((getBackHistorySize() - index) - 1);
        }
        return item;
    }

    /**
     * Gets an item of forward history at a specified index.
     * @param index the history location index (0 = closest to current document position).
     * @return the history content, or <code>null</code> if not defined.
     */
    private AttributedContent getForwardHistoryDocumentAt(int index) {
        AttributedContent item = null;
        if ((index >= 0) && (index < getForwardHistorySize())) {
            item = (AttributedContent)
                   this.forwardHistory.elementAt((getForwardHistorySize() - index) - 1);
        }
        return item;
    }

    /**
     * Sets the current document.
     * @param current   the current document or <code>null</code>.
     */
    private void setCurrentDocument(AttributedContent current) {
        if ( this.currentDocument != null ) {
            this.lastCurrentDocument = this.currentDocument;
        }

        this.currentDocument = current;
    }

    /**
     * Gets the current document.
     * @return the current document, or <code>null</code> if not defined.
     */
    public AttributedContent getCurrentDocument() {
        return this.currentDocument;
    }

    /**
     * Gets the last current document.
     * Same as the "current document" except that this one will survive a purge.
     * @return the last current document, or <code>null</code> if not defined.
     */
    AttributedContent getLastCurrentDocument() {
        return this.lastCurrentDocument;
    }

    /**
     * Called when a new document URL has been pushed onto the history stack.
     * @param content   the content that was pushed onto the history stack.
     */
    private void push(AttributedContent content) {
        synchronized (this) {
            if (getCurrentDocument() != null) {
                this.backHistory.push(getCurrentDocument());
            }
            setCurrentDocument(content);
            if (hasForwardHistory()) {
                this.forwardHistory.removeAllElements();
            }
        }
        // Event Notification:
        fireHistoryAdded(getHistorySize() - 1);
        fireCurrentPositionChanged();
    }

    /**
     * Called when the top of the history stack has been popped.
     * The history stack position index is also decremented.
     */
    private void pop() {
        synchronized (this) {
            if (hasForwardHistory()) {
                this.forwardHistory.pop();
            } else if (hasBackHistory()) {
                setCurrentDocument((AttributedContent)this.backHistory.pop());
            }
        }
        // Event Notification:
        fireHistoryRemoved(getHistorySize());
        fireCurrentPositionChanged();
    }

    /**
     * Called when history stack position index has been decremented without a 'pop'.
     */
    private void moveBack() {
        synchronized (this) {
            if (hasBackHistory()) {
                this.forwardHistory.push(getCurrentDocument());
                setCurrentDocument((AttributedContent)this.backHistory.pop());
            }
        }
        // Event Notification:
        fireCurrentPositionChanged();
    }

    /**
     * Called when history stack position index has been incremented without a 'push'.
     */
    private void moveForward() {
        synchronized (this) {
            if (hasForwardHistory()) {
                this.backHistory.push(getCurrentDocument());
                setCurrentDocument((AttributedContent)this.forwardHistory.pop());
            }
        }
        // Event Notification:
        fireCurrentPositionChanged();
    }

    /**
     * Called when the history stack has been cleared.
     */
    void removeAll() {
        synchronized (this) {
            this.backHistory.removeAllElements();
            this.forwardHistory.removeAllElements();
            setCurrentDocument(null);
        }
        // Event Notification:
        fireAllHistoryRemoved();
        fireCurrentPositionChanged();
    }

    /**
     * Called when the bottom-most(deepest) item in the history stack has been removed.
     */
    private void removeFromBottom() {
        if (hasBackHistory()) {
            this.backHistory.removeElementAt(0);
            // Event Notification:
            fireHistoryRemoved(0);
        }
    }

    /**
     * Sets the document URL value for a history entry.
     * @param doc the history entry
     * @param url the document URL value.
     */
    private static void setDocumentURL(AttributedContent doc, String url) {
        if (doc != null) {
            doc.setAttribute(DOCUMENT_URL, url);
        }
    }

    /**
     * Gets the document URL value from a history entry.
     * @return the document URL value, or <code>null</code> if not defined.
     */
    public static String getDocumentURL(AttributedContent doc) {
        String url = null;
        if (doc != null) {
            Object value = doc.getAttribute(DOCUMENT_URL);
            if (value instanceof String) {
                url = (String)value;
            }
        }
        return url;
    }

    /**
     * Gets the DeviceBrowserHistoryListener instance.
     * @return the DeviceBrowserHistoryListener instance.
     */
    private DeviceBrowserHistoryListener getDeviceBrowserHistoryListener() {
        if (this.deviceHistoryListener == null) {
            this.deviceHistoryListener = new DeviceBrowserHistoryListener() {
                public void historyPushed(String url) {
                    if (getContentLog() != null) {
                        DeviceContentReceived c = getContentLog().findMostRecentContentReference(url);
                        AttributedContent item = new AttributedContent(c);
                        setDocumentURL(item, url);
                        push(item);
                        if (c == null) {
                            // Internal error: Cannot match URL to content
                            // This can be the result of URL format mismatch
                            // or from the Content Log having been cleared
                            // just prior to this event (a rare race condition).
                            // The first case can be worked around via flexible
                            // URL comparison.  The second case (if important)
                            // could be handled by preventing Content Log entries
                            // that correspond to history entries from being flushed.
                        }
                    } else {
                        // Internal error: Missing content log
                        //TODO: Should not occur
                    }
                }

                public void historyRefresh(String url) {
                    if (getContentLog() != null) {
                        DeviceContentReceived c = getContentLog().findMostRecentContentReference(url);
                        AttributedContent item = new AttributedContent(c);
                        setDocumentURL(item, url);
                        setCurrentDocument(item);
                        fireHistoryUpdated(getCurrentDocumentPosition());
                        if (c == null) {
                            // Internal error: Cannot match URL to content
                            // This can be the result of URL format mismatch
                            // or from the Content Log having been cleared
                            // just prior to this event (a rare race condition).
                            // The first case can be worked around via flexible
                            // URL comparison.  The second case (if important)
                            // could be handled by preventing Content Log entries
                            // that correspond to history entries from being flushed.
                        }
                    } else {
                        // Internal error: Missing content log
                        //TODO: Should not occur
                    }
                }

                public void historyPopped() {
                    pop();
                }

                public void historyBack() {
                    moveBack();
                }

                public void historyForward() {
                    moveForward();
                }

                public void historyCleared() {
                    removeAll();
                }

                public void historyTrimmed() {
                    removeFromBottom();
                }
            };
        }
        return this.deviceHistoryListener;
    }

    /**
     * Sets the support status for the <code>DeviceBrowserHistoryListener</code> interface.
     * @param supported if <code>true</code>, then the current device supports
     *                  the <code>DeviceBrowserHistoryListener</code> interface
     */
    private void setDeviceBrowserHistoryListenerSupported(boolean supported) {
        this.deviceHistoryListenerSupported = supported;
    }

    /**
     * Gets the support status for the <code>DeviceBrowserHistoryListener</code> interface.
     * @return <code>true</code> if the current device supports
     *         the <code>DeviceBrowserHistoryListener</code> interface
     */
    public boolean isDeviceBrowserHistoryListenerSupported() {
        return this.deviceHistoryListenerSupported;
    }

    /**
     * Adds a listener to the list that's notified each time
     * a change to the history list occurs.
     * @param l     the <code>HistoryListListener</code>
     */
    public void addHistoryListListener(HistoryListListener l) {
        if (this.listenerList == null) {
            this.listenerList = new EventListenerList();
        }
        this.listenerList.add(HistoryListListener.class, l);
    }

    /**
     * Removes a listener from the list that's notified each time
     * a change to the history list occurs.
     * @param l     the <code>HistoryListListener</code>
     */
    public void removeHistoryListListener(HistoryListListener l) {
        if (this.listenerList != null) {
            this.listenerList.remove(HistoryListListener.class, l);
        }
    }

    /**
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index     the history list index.
     */
    private void fireHistoryAdded(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == HistoryListListener.class) {
                    ((HistoryListListener)listeners[i+1]).historyAdded(index);
                }
            }
        }
    }

    /**
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index     the history list index.
     */
    void fireHistoryUpdated(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == HistoryListListener.class) {
                    ((HistoryListListener)listeners[i+1]).historyUpdated(index);
                }
            }
        }
    }

    /**
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index     the history list index.
     */
    private void fireHistoryRemoved(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == HistoryListListener.class) {
                    ((HistoryListListener)listeners[i+1]).historyRemoved(index);
                }
            }
        }
    }

    /**
     * Notify all listeners that have registered interest for
     * notification on this event type.
     */
    private void fireAllHistoryRemoved() {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == HistoryListListener.class) {
                    ((HistoryListListener)listeners[i+1]).allHistoryRemoved();
                }
            }
        }
    }

    /**
     * Notify all listeners that have registered interest for
     * notification on this event type.
     */
    private void fireCurrentPositionChanged() {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == HistoryListListener.class) {
                    ((HistoryListListener)listeners[i+1]).currentPositionChanged();
                }
            }
        }
    }


    /**
     * A listener for browser history list events.
     */
    public interface HistoryListListener extends EventListener {
        /**
         * Called when a history item has been added to the list.
         *
         * @param index     the history list index.
         */
        public void historyAdded(int index);

        /**
         * Called when a history item has been updated within the list.
         *
         * @param index     the history list index.
         */
        public void historyUpdated(int index);

        /**
         * Called when a variable has been removed from the list.
         *
         * @param index     the history list index.
         */
        public void historyRemoved(int index);

        /**
         * Called when all history items have been removed from the list.
         */
        public void allHistoryRemoved();

        /**
         * Called when the current document position within the history item has been changed.
         */
        public void currentPositionChanged();
    }   // End of HistoryListListener


    //
    // History entry attributes (added to an AttributedContent instance):
    //

    /** The attribute name for a document URL. (type: String) */
    // NB: This is needed in case device does not supply AttributedContent
    // instances as part of a loader event.
    static final String DOCUMENT_URL = "document_url";


    private boolean deviceHistoryListenerSupported = false;
    private DeviceBrowserHistoryListener deviceHistoryListener = null;
    private ContentLog contentLog = null;
    private AttributedContent currentDocument = null;
    private AttributedContent lastCurrentDocument = null;
    private Stack backHistory = null;           // Type = AttributedContent
    private Stack forwardHistory = null;        // Type = AttributedContent
    private EventListenerList listenerList;
}   // End of BrowserHistory
