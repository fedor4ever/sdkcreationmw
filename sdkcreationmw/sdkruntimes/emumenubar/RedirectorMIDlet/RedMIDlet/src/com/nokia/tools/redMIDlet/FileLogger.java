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


import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import java.io.OutputStream;
import java.io.IOException;

/**
 * Class FileLogger
 *
 * A class responsible for writing logs into a log file using static functions
 * 
 */
public class FileLogger {
    
    static private FileConnection iOutputConnection;
    static private OutputStream outputStream;
    static private boolean loggingEnabled = false;
    
    //Create log file:
    static
    {
        try{
            
            //ENABLE/DISABLE LOGGING:
            loggingEnabled = false;
            
            if(loggingEnabled)
            {
	            //System.out.println("FileLogger: opening new directory connection:");
	            
	            String writableFolder = System.getProperty("fileconn.dir.photos");
	            //fileconn.dir.private
	            //System.out.println("Private folder = " + writableFolder);
	            
	            iOutputConnection = (FileConnection)Connector.open(writableFolder+"java.txt");
	            //Check if the file exists
	            // if NOT -> create it
	            if(!iOutputConnection.exists())
	            {
	                iOutputConnection.create();
	            }
	            
	            outputStream = iOutputConnection.openOutputStream();
	            
	            loggingEnabled = true;
            }
        }
        catch(IOException e)
        {
            System.err.println("FileLogger, constructor: Exception "+e.getMessage());
            
            loggingEnabled = false;
        }        
    }
    
    static public void writeln(String aString)
    {	
        if(loggingEnabled)
        {
	        try
	        {
	        
	        outputStream.write( ( Long.toString( System.currentTimeMillis() )+" "+ aString + "\n").getBytes());
	        outputStream.flush();
	        }
	        catch(IOException e)
	        {
	            System.err.println("FileLogger.write(): Exception "+e.getMessage());
	        }
        }
    }
    static public void write(String aString)
    {	
        if(loggingEnabled)
        {
	        try
	        {
	        
	        outputStream.write( ( Long.toString( System.currentTimeMillis() )+" "+ aString).getBytes());
	        outputStream.flush();
	        }
	        catch(IOException e)
	        {
	            System.err.println("FileLogger.write(): Exception "+e.getMessage());
	        }
        }
    }

    static public void writeln(byte[] bytesToWrite)
    {	
        if(loggingEnabled)
        {
	        try
	        {
	        
	        outputStream.write( ( Long.toString( System.currentTimeMillis() )+" ").getBytes());
	        outputStream.write( bytesToWrite );
	        outputStream.write( "\n".getBytes() );
	        outputStream.flush();
	        }
	        catch(IOException e)
	        {
	            System.err.println("FileLogger.write(): Exception "+e.getMessage());
	        }
        }
    }
    
    static public void cancelLogging()
    {
        loggingEnabled = false;
        try
        {
            if(outputStream != null){
                outputStream.close();
            }
        }
        catch(IOException e)
        {
            System.err.println("FileLogger.cancelLogging() IOEx. "+e.getMessage());
        }
    }
    
}
