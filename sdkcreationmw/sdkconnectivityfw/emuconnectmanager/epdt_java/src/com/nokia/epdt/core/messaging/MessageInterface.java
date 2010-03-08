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

/**
 * MessageInterface
 */
public interface MessageInterface {
    
    /**
     * Sets the plugin's MessageListener.
     * Setting the message listener to null is the equivalent of unsetting
     * the message listener for the plugin.
     * @param l the listener to which the messages are to be delivered
     * @param uid the unique identifier
     */
    public void setMessageListener(MessageListener l, Integer uid);
    
    /**
     * Gets the plugin's MessageSender.
     * @return the sender for the plugin
     */
    public MessageQueue getMessageOutputQueue();
    
}
