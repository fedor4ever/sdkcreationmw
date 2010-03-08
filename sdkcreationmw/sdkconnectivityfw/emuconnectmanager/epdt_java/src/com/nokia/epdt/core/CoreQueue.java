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

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.command.Command;
import com.nokia.epdt.core.command.CommandQueue;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageListenerTable;
import com.nokia.epdt.core.messaging.MessageQueue;

/**
 * CoreQueue - LinkedList wrapper
 */
public class CoreQueue implements MessageQueue, ActionEventQueue, CommandQueue {
    private static Logger log = Logger.getLogger(CoreQueue.class);
    private int capacity;
    private List queue;

    static ActionEventQueue createActionQueue() {
        return new CoreQueue(50);
    }

    static CommandQueue createCommandQueue() {
        return new CoreQueue(50);
    }

    static MessageQueue createMessageQueue() {
        return new CoreQueue(1000);
    }

    public CoreQueue(int capacity) {
        this.queue = new LinkedList();
        this.capacity = capacity;
    }

    public Object get() throws InterruptedException {
        synchronized (queue) {
            while (queue.isEmpty()) {
                queue.wait();
            }
            return queue.remove(0);
        }
    }

    public boolean put(Object object) {
        synchronized (queue) {
            if (queue.size() >= capacity) {
                return false;
            } else {
                queue.add(object);
                queue.notifyAll();
                return true;
            }
        }
    }

    public int size() {
        return queue.size();
    }

    public Command getCommand() throws InterruptedException {
        return (Command)get();
    }

    public Action getAction() throws InterruptedException {
        return (Action)get();
    }

    public Message getMessage() throws InterruptedException {
        return (Message)get();
    }

    public boolean put(Command command) {
        return put((Object)command);
    }

    public boolean put(Action action) {
        return put((Object)action);
    }

    public boolean put(Message message) {
        return put((Object)message);
    }

    private static class ActionMediator extends Thread {
        private ActionListener actionListener;
        private List allActionListenerTable;
        private List coreActionListenerTable;
        private Map pluginActionListenerTable;
        private ActionEventQueue actionEventQueue;

        /**
         * Creates a new instance of ActionMediator
         */
        public ActionMediator(ActionEventQueue actionEventQueue,
                              List allActionListenerTable,
                              List coreActionListenerTable,
                              Map pluginActionListenerTable) {
            super("ActionMediator");
            this.actionEventQueue = actionEventQueue;
            this.allActionListenerTable = allActionListenerTable;
            this.coreActionListenerTable = coreActionListenerTable;
            this.pluginActionListenerTable = pluginActionListenerTable;
        }

        /**
         * Starting the thread causes the object's run method
         * to be called in separately executing thread.
         */
        public void run() {
            while ( true ) {
                // Try to take messages from message queue
                try {
                    log.debug("Try to take event from action event queue");
                    Action event = actionEventQueue.getAction();
                    // Try to mediate the event to the right component
                    log.debug("Notify action listeners");
                    notifyActionListeners(event);
                } catch ( InterruptedException ie ) {
                    log.error(ie);
                }
            }
        }

        /**
         * If this object has changed, then notify all of the listeners that have specified UID.
         * Each listener has its actionPerformed() method called with argument: ActionEvent.
         */
        public void notifyActionListeners(Action event) {
            if (event.getType() == event.CORE) {
                ActionListener [] listeners = (ActionListener[])
                    coreActionListenerTable.toArray(new ActionListener[
                    coreActionListenerTable.size()]);
                log.debug("Notify core.");
                for (int i=0; i<listeners.length; i++) {
                    listeners[i].actionPerformed(event);
                }
            } else if ( event.getType() == event.PLUGIN ) {
                Integer [] uids = (Integer[])
                    pluginActionListenerTable.keySet().toArray(new Integer[
                    pluginActionListenerTable.size()]);
                for (int j=0; j<uids.length; j++) {
                    Integer uid = uids[j];
                    ActionListener al = (ActionListener) pluginActionListenerTable.get(uid);
                    int[] destination = event.getDestination();
                    for ( int i = 0, l = destination.length; i < l; i++ ) {
                        if ( destination[i] == uid.intValue() ) {
                            log.debug("Notify plugin: " + uid.intValue());
                            al.actionPerformed(event);
                        }
                    }
                }
            } else if (event.getType() == event.ALL) {
                ActionListener [] listeners = (ActionListener[])
                    allActionListenerTable.toArray(new ActionListener[
                    allActionListenerTable.size()]);
                for (int i=0; i<listeners.length; i++) {
                    listeners[i].actionPerformed(event);
                }
            }
        }
    }

    public static Thread getActionMediator(ActionEventQueue actionEventQueue, List allActionListenerTable, List coreActionListenerTable, Map pluginActionListenerTable) {
        return new ActionMediator(actionEventQueue, allActionListenerTable, coreActionListenerTable, pluginActionListenerTable);
    }

    private static class CommandMediator extends Thread {
        private CommandQueue queue;

        CommandMediator(CommandQueue queue) {
            super("CommandMediator");
            this.queue = queue;
        }

        public void run() {
            try {
                while (true) {
                    queue.getCommand().execute();
                }
            } catch (InterruptedException ie) {
                log.error(ie);
            }
        }
    }

    public static Thread getCommandMediator(CommandQueue queue) {
        return new CommandMediator(queue);
    }

    static class MessageMediator extends Thread {
        private MessageQueue messageInputQueue;
        private MessageListenerTable messageListenerTable;

        /**
         * Creates a new instance of MessageMediator
         */
        MessageMediator(MessageQueue messageInputQueue) {
            super("MessageMediator");
            this.messageInputQueue = messageInputQueue;
            this.messageListenerTable = new MessageListenerTable();
        }

        /**
         * Starting the thread causes the object's run method
         * to be called in separately executing thread.
         */
        public void run() {
            log.debug("Message mediator thread is running");
            while (true) {
                try {
                    messageListenerTable.dispatch(messageInputQueue.getMessage());
                } catch (Throwable t) {
                    log.error(t,t);
                }
            }
        }

        /**
         * Returns the input MessageQueue
         * @return the input MessageQueue used by this mediator
         */
        MessageQueue getMessageInputQueue() {
            return messageInputQueue;
        }

        /**
         * Adds the specified message listener to listen message events from this component.
         * If l is null, no exception is thrown and no action is performed.
         * @param l the message listener.
         * @param uid unique identifier
         */
        void addMessageListener(MessageListener l, Integer uid) {
            if (log.isDebugEnabled()) {
                log.debug("Put message listener to table. uid = "+
                    ((uid == null) ? "null" :
                    ("0x" + Integer.toHexString(uid.intValue()))));
            }
            messageListenerTable.put(uid.intValue(), l);
        }
    }
}
