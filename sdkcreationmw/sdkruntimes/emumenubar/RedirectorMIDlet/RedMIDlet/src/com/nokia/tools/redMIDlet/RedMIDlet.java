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

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * Class RedMIDlet
 *
 * Redirector MIDlet: reads data from REDIRECT_URI, where output from 
 * System.out and System.err is written, and puts it into a socket.
 * The socket is opened by ECMT plugin ("socket forwarder") which 
 * writes received data to PC side.
 * 
 */
public class RedMIDlet extends MIDlet  implements CommandListener{
    
    Command exitCommand = null;
	RedirectorReader redirectorReader;

    public RedMIDlet()
    {
        
        //@@@ TEST CODE:
        //	TestStubSocketListener testSocketListener = new TestStubSocketListener();
        //	testSocketListener.start();
        //END TEST CODE
        
        redirectorReader = new RedirectorReader(this);
        redirectorReader.start();
        

        
    }

    protected void destroyApp(boolean arg0) throws MIDletStateChangeException 
    {
        FileLogger.writeln("RedMIDlet.destroyApp(): EXITING!");
        
        redirectorReader.stopRunning();
        
        redirectorReader = null;
        
        FileLogger.cancelLogging();
        
        notifyDestroyed();
        
    }

    protected void startApp() throws MIDletStateChangeException {
        
       FileLogger.writeln("RedMIDlet.startApp()");
       
	    Form screenForm = new Form("Redirector");
	    
	    exitCommand = new Command("Exit", Command.BACK, 1);
	    screenForm.addCommand(exitCommand);
	    
	    screenForm.setCommandListener(this);
	    
	    screenForm.append("\nRedirection of System.out and System.err is started.\n\nExit Redirector to stop it.");
		
	    Display.getDisplay(this).setCurrent(screenForm);
       
    }

    protected void pauseApp() {
    }

	public void commandAction(Command c, Displayable d)
	{
	    if(c==exitCommand)
	    {
	        try
	        {
	        	destroyApp(true);
	        }
	        catch(Exception ex)
	        {}
	    }
	}
}
