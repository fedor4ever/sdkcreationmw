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




package com.nokia.epdt.core.messaging;

import com.nokia.wtk.util.IntMap;
import com.nokia.wtk.util.IntArrayMap;
import com.nokia.epdt.core.messaging.MessageListener;

/**
 * MessageListenerTable
 *
 * Contains all message listeners of the plug-in components
 *
 */
public class MessageListenerTable {

    private IntMap uidMap;

    /**
     * Creates a new instance of MessageListenerTable
     */
    public MessageListenerTable() {
        uidMap = new IntArrayMap();
    }

    /**
     * Associates the specified messageListener with the specified UID
     * in this table.
     * @param uid UID with which the specified value is to be associated.
     * @param messageListener messageListener to be associated with the
     * specified UID.
     */
    public void put(int uid, MessageListener messageListener) {
        synchronized (uidMap) {
            MessageListener[] listeners = (MessageListener[])uidMap.get(uid);
            if (listeners == null) {
                listeners = new MessageListener[] {messageListener};
            } else {
                int n = listeners.length;
                MessageListener [] newListeners = new MessageListener[n+1];
                System.arraycopy(listeners, 0, newListeners, 0, n);
                newListeners[n] = messageListener;
                listeners = newListeners;
            }
            uidMap.put(uid, listeners);
        }
    }

    /**
     * Dispatches a message, invoking all MessageListeners registered for
     * this message UID.
     *
     * @param m the message to dispatch
     * @return true if at least one listener has been found for the message
     *   UID, false otherwise.
     */
    public boolean dispatch(Message m) {
        int uid = m.getUid();
        MessageListener[] listeners;
        synchronized (uidMap) {
            listeners = (MessageListener[])uidMap.get(uid);
        }
        if (listeners != null) {
            int n = listeners.length;
            for (int i=0; i<n; i++) {
                listeners[i].messageReceived(m);
            }
            return true;
        } else {
            return false;
        }
    }

    /**
     * Removes all UID-listener pairs from this table
     */
    public void clear() {
        synchronized (uidMap) {
            uidMap.clear();
        }
    }

    /**
     * Removes all occurrences of the specified MessageListener from this
     * table.
     *
     * @param messageListener the listener to remove
     * @return true if at least one UID-listener pair has been removed from
     *   this table, false if it wasn't there.
     */
    public boolean remove(MessageListener messageListener) {
        boolean removed = false;
        synchronized (uidMap) {
            int [] uids = uidMap.keys();
            for (int i=uids.length-1; i>=0; i--) {
                if (remove(uids[i], messageListener)) removed = true;
            }
        }
        return removed;
    }

    /**
     * Removes the specified MessageListener from this table for the
     * specified UID
     * @param uid UID with which the listener was associated
     * @param listener the listener to removed from the table
     * @return true if the UID-listener pair has been removed from this
     *   table, false if it wasn't there.
     */
    public boolean remove(int uid, MessageListener listener) {
        synchronized (uidMap) {
            MessageListener[] listeners = (MessageListener[])uidMap.get(uid);
            MessageListener[] newListeners = remove(listeners,listener);
            if (newListeners != listeners) {
                // Found at least one occurrence
                if (newListeners != null) {
                    // There are still some listeners for this UID
                    uidMap.put(uid, newListeners);
                } else {
                    // No more listeners for this UID
                    uidMap.remove(uid);
                }
                return true;
            } else {
                // No such listener for this UID
                return false;
            }
        }
    }

    /**
     * Removes all references to the specified listener from the array.
     * @param listeners the array to modify
     * @param messageListener the listener to remove
     * @return the modified array of listeners if the listener has been found,
     *   null if no more listeners left in the array, or the input array if
     *   the listeners wasn't found
     */
    private static MessageListener[] remove(MessageListener[] listeners,
                                            MessageListener messageListener) {
        int removed = 0;
        if (listeners != null) {
            MessageListener[] copy = null;
            for (int i=listeners.length-1; i>=0; i--) {
                if (listeners[i] == messageListener) {
                    removed++;
                    int n = listeners.length-removed-i; // How many to copy
                    if (n > 0) {
                        if (copy == null) {
                            // Avoid race condition with dispatch()
                            // Will modify a copy
                            copy = (MessageListener[])listeners.clone();
                            listeners = copy;
                        }
                        System.arraycopy(listeners, i+1, listeners, i, n);
                    }
                }
            }

            // Trim the array to remove unused entries
            if (removed > 0) {
                if (removed < listeners.length) {
                    int n = listeners.length - removed; // New size
                    MessageListener[] newListeners = new MessageListener[n];
                    System.arraycopy(listeners, 0, newListeners, 0, n);
                    return newListeners;
                } else {
                    // Nothing left in the array
                    return null;
                }
            }
        }

        return listeners;
    }
}
