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

import java.io.IOException;
import java.util.Vector;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.midlet.MIDletStateChangeException;

import java.io.OutputStream;
import java.io.InputStreamReader;



/**
 * Class QueueProcessor
 *
 * Reads data written by RedirectorReader and writes it into
 * a) socket (opened by ECMT socket forwarder)
 * b) log file (optionally)
 * 
 */
public class QueueProcessor extends Thread {
   
    private boolean iRunning = true;
    
    private RedMIDlet parentMIDlet;
    
    private Vector dataQueue;

    //Default port used 
    //a) when we couldn't find the appropriate port to connect to from shared file
    //b) for test stub to listen on
    static String DEFAULT_PORT = "1001";
    
    private String portToWriteTo;
    
    //@@@ Test mode: iWriteToLogFile = true
	private boolean iWriteToLogFile = false;
	private boolean iWriteToSocket = false;

	private StreamConnection iOutputConnection;
	private OutputStream iOutputStream;

	/**
	 * Constructs a new QueueProcessor object with a given Queue and a list 
	 * of outputs. A GCFWriter is created for each of the outputs.
	 * 
	 * @param aConfigList  the list of outputs
	 * @param aQueue  the queue of strings
	 */
	public QueueProcessor(RedMIDlet aParentMIDlet)
	{
	    parentMIDlet= aParentMIDlet;
	    
	    dataQueue = new Vector();
	    
	    portToWriteTo = readPortFromFile();
	    
	}
	
	/**
	 * Adds an array of bytes into vector of arrays to be written to output socket later on
	 * 
	 */
	public void enqueueBytes(byte[] bytesArrayToAdd)
	    {
	        synchronized(dataQueue)
	        {
	            dataQueue.addElement(bytesArrayToAdd);
	            dataQueue.notifyAll();
	        }
	    }
 
	/**
	 * Creates an OutputStream and writes
	 * the strings stored in the Queue to it.
	 */
    public void run()
		{	

        	try
        	{
			        FileLogger.writeln("QueueProcessor.QueueProcessor(): opening connection, port: "+portToWriteTo);
				    iOutputConnection = (StreamConnection)Connector.open("socket://127.0.0.1:"+portToWriteTo, 
				            Connector.WRITE, true);
				    
				    FileLogger.writeln("QueueProcessor.QueueProcessor(): creating output stream");
				    iOutputStream = iOutputConnection.openOutputStream();
					
					iWriteToSocket=true;

        	}
        	catch(IOException e)
        	{
        	    FileLogger.writeln("QueueProcessor.run(): IO Exception: "+e.getMessage());
        	    
        	    //Exit a MIDlet, if we could neither connect to the specified port nor write out to log file
        	    if(!iWriteToLogFile)
        	    {
        	           try{
        	                parentMIDlet.destroyApp(true);
        	            }
        	            catch(MIDletStateChangeException MIDletException)
        	            {
        	                FileLogger.writeln("RedirectorReader.RedirectorReader(): MIDletStateChange ex.:"
        	                        + MIDletException.getMessage());
        	            }
        	    }
        	    
        	    
        	}
        
        	//Test string for status reporting in diagnostics view:
        	write( ("Redirector MIDlet: redirection is started.\n").getBytes());

        	try{
		        synchronized(dataQueue)
		        {
		            while(iRunning)
		            {
		                //wait until data is available
		                if(dataQueue.size() == 0)
		                {
		                    dataQueue.wait();
		                }

		                write( (byte[])dataQueue.firstElement() );
		                dataQueue.removeElementAt(0);
		            }
		        }
	        }
	        catch(InterruptedException e)
	        {
	            FileLogger.writeln("QueueProcessor.run() InterruptedException: "+ e.getMessage());
	        }
		}

   public void stopRunning()
   {
       iRunning = false;
       try
       {
           if(iOutputStream != null)
           {
               iOutputStream.close();
           }
           
       }
       catch(IOException e)
       {
           FileLogger.writeln("QueueProcessor.StopRunning() IOEx: "+e.getMessage());
       }
   }
    

	/**
	 * Writes bytes into output socket & log
	 * 
	 * 
	 */
	private void write(byte[] bytesToWrite)
	{
        if(iWriteToLogFile)
        {
            FileLogger.writeln("QueueProcessor.write(): wrote: "+ (new String(bytesToWrite)));
            
        }
	    if(iWriteToSocket)
	    {
	        try{
	            iOutputStream.write(bytesToWrite);
	            iOutputStream.flush();
	        }
	        catch(IOException e)
	        {
	            FileLogger.writeln("QueueProcessor.write(): IOException: "+e.getMessage());
	        }
	    }
	    
	    return;
	}

	/**
	 * readPortFromFile() reads port from file, created by ECMT plugin
	 * The file contains ASCII-encoded port number ECMT plugin listens on
	 * 
	 * The function returns String (not int or byte[]) - it's anyway appended to URI,
	 * which is String.
	 */
	
	private String readPortFromFile()
	{		
	    	String result="";
	    	InputStreamReader isReader=null;
	    	
	       try{
	           	//We couldn't find any better folder, accessible both to the MIDlet and
	            //to ECMT plugin (native SOS app)
	            String writableFolder = System.getProperty("fileconn.dir.photos");

	            FileConnection fileConnection = (FileConnection)Connector.open(writableFolder+"ecmt.dat");
	            //Check if the file exists
	            // if NOT -> return default port
	            if(!fileConnection.exists())
	            {	
	                FileLogger.writeln("QueueProcessor.readPortFromFile(), file does not exist");
	                return DEFAULT_PORT;
	            }
	            
	            isReader = new InputStreamReader(fileConnection.openInputStream());
	            
	            StringBuffer resultBuffer=new StringBuffer();
	            
	            int b;
	            while( (b=isReader.read()) != -1)
	            {
	                //FileLogger.write("QueueProcessor.readPortFromFile(): read"+b);
	                
	                //The string in the file is ASCII-encoded port number
	                //we append the charachter if it is a valid 10-based digit:
	                if(b>='0' && b<='9')
	                {
	                    resultBuffer.append(b-48);
	                }
	                else
	                {
	                    resultBuffer.setLength(0);
	                    resultBuffer.append(DEFAULT_PORT);
	                    break;
	                }
	            }

	            result=resultBuffer.toString();
	        }
	        catch(IOException e)
	        {
	            FileLogger.writeln("QueueProcessor.readPortFromFile(): IOException"+e.getMessage());
	            
	            return DEFAULT_PORT;
	        }
	        finally
	        {
	            try
	            {
	            	if(isReader!=null){ isReader.close(); }
	            }
	            catch(IOException e)
	            {
	                FileLogger.writeln("QueueProcessor.readPortFromFile(): IOException"+e.getMessage());
	            }
	        }

	        FileLogger.writeln("QueueProcessor.readPortFromFile(), result:"+result);
	        return result;
	    
	        
	}


}
