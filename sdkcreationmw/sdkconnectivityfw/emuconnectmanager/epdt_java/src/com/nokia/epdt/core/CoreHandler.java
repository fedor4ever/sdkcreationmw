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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageQueue;

/**
 * CoreHandler
 */
public class CoreHandler implements MessageInterface, ActionInterface {
    private static final Logger log = Logger.getLogger(CoreHandler.class);
    private MessageQueue messageOutputQueue;
    private MessageQueue messageInputQueue;
    private CoreQueue.MessageMediator messageMediator;
    private ActionEventQueue actionEventQueue = CoreQueue.createActionQueue();
    private List allActionListenerTable = new ArrayList();
    private List coreActionListenerTable = new ArrayList();
    private Map pluginActionListenerTable = new HashMap();
    private Thread actionMediator = CoreQueue.getActionMediator(actionEventQueue, allActionListenerTable, coreActionListenerTable, pluginActionListenerTable);

    /**
     * Creates a new instance of MessageHandler
     * @param mediator the associated MessageMediator object
     */
    public CoreHandler() {
        messageOutputQueue = CoreQueue.createMessageQueue();
        messageInputQueue = CoreQueue.createMessageQueue();
        messageMediator = new CoreQueue.MessageMediator(messageInputQueue);
    }

    public MessageQueue getMessageInputQueue() {
        return messageInputQueue;
    }

    /**
     * Gets the MessageSender.
     * @return the message sender
     */
    public MessageQueue getMessageOutputQueue() {
        return messageOutputQueue;
    }

    /**
     * Gets the MessageMediator.
     * @return the MessageMediator
     */
    public Thread getMessageMediator() {
        return messageMediator;
    }

    /**
     * Sets the plugin's MessageListener.
     * Setting the message listener to null is the equivalent of unsetting
     * the message listener for the plugin.
     * @param l the listener to which the messages are to be delivered
     * @param uid the unique identifier
     */
    public void setMessageListener(MessageListener l, Integer uid) {
        messageMediator.addMessageListener(l, uid);
    }

    public Thread getActionMediator() {
        return actionMediator;
    }

    public List getAllActionListenerTable() {
        return allActionListenerTable;
    }

    /**
     * Gets the plugin's ActionDispatcher.
     * @return the dispatcher for the plugin
     */
    public ActionEventQueue getActionEventQueue() {
        return actionEventQueue;
    }

    /**
     * Sets the plugin's ActionListener.
     * Setting the action listener to null is the equivalent of unsetting
     * the action listener for the plugin.
     * @param l the listener to which the actions are to be delivered
     * @param uid the unique identifier
     */
    public void setActionListener(ActionListener l, Integer uid, int type) {
        allActionListenerTable.add(l);

        if(type == Action.CORE)
            coreActionListenerTable.add(l);
        else if(type == Action.PLUGIN)
            pluginActionListenerTable.put(uid, l);

        log.debug("Put action listener to table. uid = " + uid);
    }
}
