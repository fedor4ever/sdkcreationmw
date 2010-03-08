/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.epdt.plugins.s60.NEItest;

import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageQueue;

/**
 * Class NEIMessageHandler
 *
 * 
 * 
 */
public class NEIMessageHandler implements MessageListener{
    
    private static final int ecmtPluginUid = 0x1020457E;
    
    private static final byte OPEN = 1;
    private static final byte CLOSE = 2;
    private static final byte SEND = 3;
    private static final byte START_TST = 100;
    
    private final int defaultPort = 2811;
    
    private MessageQueue messageOutputQueue = null;

    /**
     * 
     */
    public NEIMessageHandler() {
        super();
        // TODO Auto-generated constructor stub
    }
    
    
    public void messageReceived(Message message) 
    {
            String messageString = new String(message.getBody());
     }
    
    public void setMessageSender(MessageQueue messageOutputQueue) {
        this.messageOutputQueue = messageOutputQueue;
    }
    
    public void startTst()
    {
 
    
        byte[] data = new byte[11]; 
        
        data[3] = 1; //Conn ID, not used
        
        data[7] = 1; //SEQ number, to be inctemented TODO 
        
        data[8] = OPEN;
        
        //data[5] =
        
        if(System.getProperty("neiPort") != null)
        {
            data[9]=(byte)(defaultPort>>8);
            data[10]=(byte)defaultPort;
            
        }
        else
        {
            data[9]=(byte)(defaultPort>>8);
            data[10]=(byte)defaultPort;
        }
        
        
        messageOutputQueue.put(new Message(ecmtPluginUid, data) );
    }
    
    public void killDebugAgent()
    {
        byte[] data = new byte[9]; 
        
        data[3] = 1; //Conn ID, not used
        
        data[7] = 2; //SEQ number, to be inctemented TODO 
        
        data[8] = CLOSE;
        
        
        messageOutputQueue.put(new Message(ecmtPluginUid, data) );      
    }

}
