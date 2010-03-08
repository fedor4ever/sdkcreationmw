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



package com.nokia.tools.redMIDlet;

import javax.microedition.io.StreamConnectionNotifier;
import java.io.InputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import java.io.IOException;

/**
 * Test class (stub): listens on the DEFAULT_PORT for connections from 
 * QueueProcessor, reads data upon connection and writes it into a log file
 * 
 */
public class TestStubSocketListener extends Thread
{
    
    
    private StreamConnectionNotifier connNotifier;
    private StreamConnection connection;

    public void run()
    {

        try
        {
            FileLogger.writeln("TestStubSocketListener.run(): creating notifier: ");
        	//synchronized(QueueProcessor.testSocketListenerLock)
        	//{
		       	
        	    connNotifier = (StreamConnectionNotifier)Connector.open("socket://:"+QueueProcessor.DEFAULT_PORT);
		        
		        FileLogger.writeln("TestStubSocketListener.run(): CREATED notifier: ");
		            
		        FileLogger.writeln("TestStubSocketListener.run(): accepting connections:");
        	//}
	    	connection = connNotifier.acceptAndOpen();
	        
	    	FileLogger.writeln("TestStubSocketListener.run(): received conn. request:");
	    	
	        InputStream is = connection.openInputStream();
	        
	        int r;
	        while((r = is.read()) != -1)
	        {
	            FileLogger.writeln("TestStubSocketListener.run(): read: "+r);
	        }
            
        }
        catch(IOException e)
        {
            FileLogger.writeln("TestStubSocketListener.TestStubSocketListener(): IO Ex: "+e.getMessage());
        }
        
    }

}
