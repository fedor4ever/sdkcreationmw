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

import javax.microedition.io.InputConnection;
import javax.microedition.io.Connector;
import java.io.InputStream;
import java.io.IOException;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * Class Redirector
 *
 * Reads traffic from REDIRECT_URI and writes to a vector in QueueProcessor,
 * running in parallel thread
 * 
 */
public class RedirectorReader extends Thread
{
    private boolean iRunning = true;
    
    private InputConnection inputConnection;
    private InputStream inputStream = null;
    private static String REDIRECT_URI = "redirect://"; 
    
    private QueueProcessor queueProcessor = null;
    private boolean connected = false; 
    
    private RedMIDlet parentMIDlet;
    
    public RedirectorReader(RedMIDlet aParent) {
        super();
        
        parentMIDlet = aParent;
        
        try
        {
            FileLogger.writeln("RedirectorReader constructor: initializing input connection " );
            
            inputConnection = (InputConnection)Connector.open(REDIRECT_URI, Connector.READ);
            inputStream = inputConnection.openInputStream();
            
            connected = true;
            
            FileLogger.writeln("RedirectorReader constructor: end connected = " + connected);
            
            //Create QueueProcessor        
            queueProcessor = new QueueProcessor(parentMIDlet);
            queueProcessor.start();
            
        }
        catch(IOException e)
        {
            FileLogger.writeln("RedirectorReader constructor: IOException" + e.getMessage());
            
            //Exit MIDlet here, since there is no point to be running, if we can't read redirector input 
            //stream 
            try{
                parentMIDlet.destroyApp(true);
            }
            catch(MIDletStateChangeException MIDletException)
            {
                FileLogger.writeln("RedirectorReader.RedirectorReader(): MIDletStateChange ex.:"
                        + MIDletException.getMessage());
            }
            
            connected = false;
            
        }
        
    }
    public void run() 
    {	
        try
        {
            FileLogger.writeln("Redirector reader run(): connected="+connected);
            
            while(iRunning)
            {
		        byte[] buffer = new byte[100];
		        int nRead=0;
		        
		        while( (nRead=inputStream.read(buffer)) != -1)
		        {
		            byte[] bytesRead = new byte[nRead];
		            
		            //create an array, which is added into Vector in QueueProcessor
		            System.arraycopy(buffer, 0, bytesRead, 0, nRead);
		            
		            queueProcessor.enqueueBytes(bytesRead);
		            
		            FileLogger.write("RedirectorReader ");
		            FileLogger.writeln(bytesRead);

		        }
		        
            }
        
        }
        catch(IOException e)
        {
            FileLogger.writeln("RedirectorReader.run(): IOException" + e.getMessage());
        }
    
    }
    
    public void stopRunning()
    {
        if (queueProcessor != null)
        {
            queueProcessor.stopRunning();
        }
        queueProcessor = null;
        
        iRunning = false;
        try
        {
            if(inputStream != null)
            {
                inputStream.close();
            }
        }
        catch(IOException e)
        {
            FileLogger.writeln("QueueProcessor.StopRunning() IOEx: "+e.getMessage());
        }
    }
}
