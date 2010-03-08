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

import java.util.EventListener;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.event.EventListenerList;

import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DevicePushInbox;
import com.nokia.wtk.device.DevicePushInboxListener;

/**
 * The push message models for the current device,
 * consisting of a pushed message log and a pushed message inbox.
 *
 */
// Just to be clear: when content comes in through the loader, it is added
// to the Push Log by ContentLog with the addLoggedContent call. These
// pushed messages contain no timestapms and no IDs and cannot be
// distinguished from each other. Then here also the notifications come in
// about the PushInbox. The messages have IDs set by the push inbox, and can
// thus be retrieved when they are removed or change state. However, the
//  changes in state cannot be linked back to the logged messages because of
// the lack of ID and state. Therefore no changes are actually fired to the
// push log, only the inbox listeners.
public class PushMessages {

    /**
     * Creates a new <code>PushMessages</code>.
     */
    PushMessages(Browser browser) {
        this.inboxIDs = new Vector();
        this.inboxMessages = new Vector();
        this.loggedMessages = new Hashtable();
        setDevicePushInboxListenerSupported(
            browser.registerHostService(
                DevicePushInboxListener.class,
                getDevicePushInboxListener()));
    }

    /**
     * Sets a new <code>DevicePushInbox</code> instance.
     * @param inbox     the <code>DevicePushInbox</code> instance
     */
    void setDevicePushInbox(DevicePushInbox inbox) {
        this.devicePushInbox = inbox;
        removeAll();

        if (this.devicePushInbox != null) {
            // Initialize push inbox model:
            int[] msgIDs = this.devicePushInbox.getMessageIDs();
            if (msgIDs != null) {
                for (int i = 0; i < msgIDs.length; i++) {
                    int msgID = msgIDs[i];
                    DeviceContentReceived c =
                        this.devicePushInbox.getMessage(msgID);
                    AttributedContent msg = new AttributedContent(c);
                    setMessageID(msg, msgID);
                    setMessageState(msg, STATE_PENDING);
                    addInboxMessage(msgID, msg);
                    addLoggedMessage(msg);
                }
            }
        }
    }

    /**
     * @return the current <code>DevicePushInbox</code> instance.
     */
    private DevicePushInbox getDevicePushInbox() {
        return this.devicePushInbox;
    }

    /**
     * @returns <code>true</code> if the current device supports
     * the current push inbox messages operation.
     */
    public boolean isClearInboxMessagesSupported() {
        return Browser.isMethodSupported(getDevicePushInbox(), "clearMessages");
    }

    /**
     * Flushes all current push inbox messages.
     */
    public synchronized void clearInboxMessages() {
        if (isClearInboxMessagesSupported()) {
            try {
                getDevicePushInbox().clearMessages();
            } catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * Called when any content has been pushed onto the device.
     * @param content   the content that was pushed.
     */
    void addLoggedMessage(DeviceContentReceived content) {
        synchronized (this) {
            AttributedContent msg = new AttributedContent(content);
            if (this.loggedMessages.size() >= LOG_BUFFER_LIMIT) {
                String itemToRemove = String.valueOf(firstItem);
                this.loggedMessages.remove(itemToRemove);
                // Event Notification:
                fireLoggedMessageRemoved(itemToRemove);
                firstItem++;

            }

            String itemToAdd = String.valueOf(currentItem);

            this.loggedMessages.put(itemToAdd, msg);
            // Event Notification:
            fireLoggedMessageAdded(itemToAdd);
            currentItem++;
        }
    }

    /**
     * Called when a new message has been added to the push inbox.
     * @param id        the ID of the message that was added
     * @param message   the message that was added.
     */
    private void addInboxMessage(int id, AttributedContent message) {
        synchronized (this) {
            Integer key = new Integer(id);
            int index = this.inboxIDs.indexOf(key);
            if (index >= 0) {
                this.inboxMessages.setElementAt(message, index);
                // Event Notification:
                fireInboxMessageChanged(index);
            } else {
                this.inboxIDs.add(key);
                this.inboxMessages.add(message);
                // Event Notification:
                fireInboxMessageAdded(getInboxMessageCount() - 1);
            }
        }
    }

    /**
     * Called when an item has been removed from the push inbox.
     * Fire first, remove later.
     * @param id        the ID of the message that was removed
     * @param stateReason   the final state of the message
     */
    private void removeInboxMessage(int id, int stateReason) {
        int index = -1;

        Integer key = new Integer(id);
        synchronized (this) {
            index = this.inboxIDs.indexOf(key);
            if (index >= 0) {
                this.inboxIDs.remove(index);
                this.inboxMessages.remove(index);
            }
        }
        if (index >= 0) {
            // Event Notification:
            fireInboxMessageRemoved(index, stateReason);
        }
    }

    /**
     * Called when all push messages have been deleted.
     */
    void removeAll() {
        synchronized (this) {
            this.inboxIDs.removeAllElements();
            this.inboxMessages.removeAllElements();
            this.loggedMessages.clear();
        }
        // Event Notifications:
        fireAllInboxMessagesRemoved();
        fireAllLoggedMessagesRemoved();
    }

    /**
     * Gets the current number of logged messages.
     * @return the number of logged messages.
     */
    public int getLoggedMessageCount() {
        return this.loggedMessages.size();
    }

    /**
     * Gets a logged messages at a specified index.
     * @param key the key associated with message.
     * @return the logged message, or <code>null</code> if not defined.
     */
    public AttributedContent getLoggedMessageAt(String key) {
        AttributedContent msg = null;
        msg = (AttributedContent) this.loggedMessages.get(key);
        return msg;
    }

    /**
     * Gets the current number of inbox messages.
     * @return the number of inbox messages.
     */
    public int getInboxMessageCount() {
        return this.inboxMessages.size();
    }

    /**
     * Gets an inbox messages at a specified index.
     * @param index the message index location.
     * @return the inbox message, or <code>null</code> if not defined.
     */
    public AttributedContent getInboxMessageAt(int index) {
        AttributedContent msg = null;
        if ((index >= 0) && (index < getInboxMessageCount())) {
            msg = (AttributedContent) this.inboxMessages.elementAt(index);
        }
        return msg;
    }

    /**
     * Gets a message from the inbox by ID.
     * @param id the message ID.
     * @return the message, or <code>null</code> if not defined.
     */
    private AttributedContent getInboxMessage(int id) {
        AttributedContent msg = null;
        int index = this.inboxIDs.indexOf(new Integer(id));
        if (index >= 0) {
            msg = (AttributedContent) this.inboxMessages.elementAt(index);
        }
        return msg;
    }

    /**
     * Sets a message ID for this unit of content.
     * @param id the message ID.
     */
    private static void setMessageID(AttributedContent message, int id) {
        if (message != null) {
            message.setAttribute(MESSAGE_ID, new Integer(id));
        }
    }

    /**
     * Gets a message ID for this unit of content.
     * @return the message ID, or <code>-1</code> if not defined.
     */
    public static int getMessageID(AttributedContent message) {
        int id = ID_UNDEFINED;
        if (message != null) {
            Object value = message.getAttribute(MESSAGE_ID);
            if (value instanceof Integer) {
                id = ((Integer) value).intValue();
            }
        }
        return id;
    }

    /**
     * Sets a message state value for a message.
     * @param message the message.
     * @param state the message state value.
     */
    private static void setMessageState(AttributedContent message, int state) {
        if (message != null) {
            message.setAttribute(MESSAGE_STATE, new Integer(state));
        }
    }

    /**
     * Gets a message state value from a message.
     * @return the message state value, or <code>-1</code> if not defined.
     */
    public static int getMessageState(AttributedContent message) {
        int state = STATE_UNKNOWN;
        if (message != null) {
            Object value = message.getAttribute(MESSAGE_STATE);
            if (value instanceof Integer) {
                state = ((Integer) value).intValue();
            }
        }
        return state;
    }

    /**
     * Gets the <code>DevicePushInboxListener</code> instance.
     * @return the <code>DevicePushInboxListener</code> instance.
     */
    private DevicePushInboxListener getDevicePushInboxListener() {
        if (this.devicePushInboxListener == null) {
            this.devicePushInboxListener = new DevicePushInboxListener() {
                public void messageAdded(
                    int msgID,
                    DeviceContentReceived msgContent) {
                    if (getDevicePushInbox() != null) {
                        AttributedContent msg =
                            new AttributedContent(msgContent);
                        if (msg != null) {
                            setMessageID(msg, msgID);
                            setMessageState(msg, STATE_PENDING);
                            addInboxMessage(msgID, msg);
                        }
                    }
                }

                public void messageRemoved(int msgID, int reason) {
                    AttributedContent msg = getInboxMessage(msgID);
                    if (msg != null) {
                        int state;

                        switch (reason) {
                            case DevicePushInboxListener.REMOVED_USER :
                            case DevicePushInboxListener.REMOVED_EXPIRED :
                            case DevicePushInboxListener.REMOVED_ACTION :
                            case DevicePushInboxListener.REMOVED_REPLACED :
                                state = STATE_REMOVED_BASE + reason;
                                setMessageState(msg, state);
                                break;
                            default :
                                state = STATE_REMOVED_UNKNOWN;
                                setMessageState(msg, state);
                                break;
                        }
                        removeInboxMessage(msgID, state);
                    }
                }

                public void messageActivated(int msgID) {
                    AttributedContent msg = getInboxMessage(msgID);
                    if (msg != null) {
                        setMessageState(msg, STATE_ACTIVATED);
                        // Event Notification:
                        int index =
                            PushMessages.this.inboxIDs.indexOf(
                                new Integer(msgID));
                        fireInboxMessageChanged(index);
                    }
                }
            };
        }

        return this.devicePushInboxListener;
    }

    /**
     * Sets the support status for the <code>DevicePushInboxListener</code> interface.
     * @param supported if <code>true</code>, then the current device supports
     *                  the <code>DevicePushInboxListener</code> interface
     */
    private void setDevicePushInboxListenerSupported(boolean supported) {
        this.devicePushInboxListenerSupported = supported;
    }

    /**
     * Gets the support status for the <code>DevicePushInboxListener</code> interface.
     * @return <code>true</code> if the current device supports
     *         the <code>DevicePushInboxListener</code> interface
     */
    public boolean isDevicePushInboxListenerSupported() {
        return this.devicePushInboxListenerSupported;
    }

    /**
     * Adds a listener to the list that's notified each time
     * a change to the push message log occurs.
     * @param l     the <code>LoggedMessageListener</code>
     */
    public void addLoggedMessageListener(LoggedMessageListener l) {
        if (this.listenerList == null) {
            this.listenerList = new EventListenerList();
        }
        this.listenerList.add(LoggedMessageListener.class, l);
    }

    /**
     * Removes a listener from the list that's notified each time
     * a change to the push message log occurs.
     * @param l     the <code>LoggedMessageListener</code>
     */
    public void removeLoggedMessageListener(LoggedMessageListener l) {
        if (this.listenerList != null) {
            this.listenerList.remove(LoggedMessageListener.class, l);
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param key the key associated with message.
     */
    private void fireLoggedMessageAdded(String key) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == LoggedMessageListener.class) {
                    ((LoggedMessageListener) listeners[i + 1]).messageAdded(
                        key);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param key the key associated with message.
     */
    private void fireLoggedMessageRemoved(String key) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == LoggedMessageListener.class) {
                    ((LoggedMessageListener) listeners[i + 1]).messageRemoved(
                        key);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     */
    private void fireAllLoggedMessagesRemoved() {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == LoggedMessageListener.class) {
                    ((LoggedMessageListener) listeners[i + 1])
                        .allMessagesRemoved();
                }
            }
        }
    }

    /**
     * Adds a listener to the list that's notified each time
     * a change to the push message inbox occurs.
     * @param l     the <code>InboxMessageListener</code>
     */
    public void addInboxMessageListener(InboxMessageListener l) {
        if (listenerList == null) {
            listenerList = new EventListenerList();
        }
        listenerList.add(InboxMessageListener.class, l);
    }

    /**
     * Removes a listener from the list that's notified each time
     * a change to the push message inbox occurs.
     * @param l     the <code>InboxMessageListener</code>
     */
    public void removeInboxMessageListener(InboxMessageListener l) {
        if (listenerList != null) {
            listenerList.remove(InboxMessageListener.class, l);
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index     the message index.
     */
    private void fireInboxMessageAdded(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == InboxMessageListener.class) {
                    ((InboxMessageListener) listeners[i + 1]).messageAdded(
                        index);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index     the message index.
     * @param stateReason   the final state of the removed message
     */
    private void fireInboxMessageRemoved(int index, int stateReason) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == InboxMessageListener.class) {
                    ((InboxMessageListener) listeners[i + 1]).messageRemoved(
                        index,
                        stateReason);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     */
    private void fireAllInboxMessagesRemoved() {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == InboxMessageListener.class) {
                    ((InboxMessageListener) listeners[i + 1])
                        .allMessagesRemoved();
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index     the message index.
     */
    private void fireInboxMessageChanged(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == InboxMessageListener.class) {
                    ((InboxMessageListener) listeners[i + 1]).messageChanged(
                        index);
                }
            }
        }
    }

    /**
     * A listener for push message log events.
     * The push message log contains all push messages received.
     */
    public interface LoggedMessageListener extends EventListener {
        /**
         * Called when a push message has been added to the log.
         *
         * @param key     the key associated with the message.
         */
        public void messageAdded(String key);

        /**
         * Called when a push message has been removed from the log. This
         * removal will only happen in case the log is getting too big to
         * be maintained by the model in this class.
         *
         * @param key     the key associated with the message.
         */
        public void messageRemoved(String key);

        /**
         * Called when all push messages have been removed from the log. This
         * will be called when there is a switch of device and a new log is
         * thus in effect, and the entries from the old device are cleared.
         */
        public void allMessagesRemoved();

        /**
         * Called when a push message state has been changed.
         *
         * @param key     the key associated with the message.
         */
        public void messageChanged(String key);
    } // End of LoggedMessageListener

    /**
     * A listener for push message inbox events.
     */
    public interface InboxMessageListener extends EventListener {
        /**
         * Called when a push message has been added to the inbox.
         *
         * @param index     the message index.
         */
        public void messageAdded(int index);

        /**
         * Called when a push message has been removed from the inbox.
         *
         * @param index     the message index.
         * @param stateReason the state of the message (a REMOVED value
         * defined in this PushMessages class).
         * The message with this index has actually already been removed
         * from the model when this is fired, and probably replaced in
         * position by another message.
         */
        public void messageRemoved(int index, int stateReason);

        /**
         * Called when all push messages have been removed from the inbox.
         */
        public void allMessagesRemoved();

        /**
         * Called when a push message has been changed.
         * This can be due to either state change or complete replacement.
         *
         * @param index     the message index.
         */
        public void messageChanged(int index);
    } // End of InboxMessageListener

    //
    // Message attributes:
    //

    /** The attribute name for a message ID. (type: Integer) */
    static final String MESSAGE_ID = "message_id";

    /** The attribute name for a message state. (type: Integer) */
    static final String MESSAGE_STATE = "message_state";

    /** An unknown message state. */
    public static final int ID_UNDEFINED = (-1);

    //
    // Message state values:
    //

    /** An unknown message state. */
    public static final int STATE_UNKNOWN = 0;
    /** A new message that has just been received. */
    public static final int STATE_NEW = 1;
    /** A message that has been added to the inbox and awaits activation. */
    public static final int STATE_PENDING = 2;
    /** A message that has just been activated. */
    public static final int STATE_ACTIVATED = 3;
    /** A message that has been removed for an unknown reason. */
    public static final int STATE_REMOVED_UNKNOWN = 100;
    /** A base state value for various message removal states. */
    private static final int STATE_REMOVED_BASE = 101;
    /** A message that has been removed by the user. */
    public static final int STATE_REMOVED_USER =
        STATE_REMOVED_BASE + DevicePushInboxListener.REMOVED_USER;
    /** A message that has been removed due to expiration. */
    public static final int STATE_REMOVED_EXPIRED =
        STATE_REMOVED_BASE + DevicePushInboxListener.REMOVED_EXPIRED;
    /** A message that has been removed as a side effect of another message's activation. */
    public static final int STATE_REMOVED_ACTION =
        STATE_REMOVED_BASE + DevicePushInboxListener.REMOVED_ACTION;
    /** A message that has been removed due to replacement with another message. */
    public static final int STATE_REMOVED_REPLACED =
        STATE_REMOVED_BASE + DevicePushInboxListener.REMOVED_REPLACED;
    /** A message that has been removed because there is no action to take. */
    public static final int STATE_REMOVED_NO_ACTION =
        STATE_REMOVED_BASE + DevicePushInboxListener.REMOVED_NO_ACTION;
    /** A message that has been removed due to memory resource constraints. */
    public static final int STATE_REMOVED_CAPACITY =
        STATE_REMOVED_BASE + DevicePushInboxListener.REMOVED_CAPACITY;

    private static final int LOG_BUFFER_LIMIT = 100;

    private Vector inboxIDs = null; // Type = Integer
    private Vector inboxMessages = null; // Type = AttributedContent
    private Hashtable loggedMessages = null; // Type = AttributedContent
    private EventListenerList listenerList;
    private DevicePushInbox devicePushInbox = null;
    private boolean devicePushInboxListenerSupported = false;
    private DevicePushInboxListener devicePushInboxListener = null;

    // the following static variables hold the key of the first and last
    // message respectively.
    private static int firstItem= 1;
    private static int currentItem = 1;

} // End of PushMessages
