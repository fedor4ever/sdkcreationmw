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
 * MessageListener
 *
 * A MessageListener object is used to receive asynchronously delivered messages.
 *
 * The listener interface for listening messages events (message received) on a component.
 * The class that is interested in processing a message event either implements this interface.
 * 
 * The listener object created from that class is then registered with a plug-in using the plug-in's
 * addMessageListener method. A message event is generated when the message is received.
 */
public interface MessageListener {
    
    /**
     * Invoked when the message has been received.
     */
    public void messageReceived(Message e);
    
}
