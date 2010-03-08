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


package com.nokia.j2me.sei;

/* java.io */
import java.io.File;
import java.io.IOException;

/* java.net */
import java.net.Socket;
import java.net.InetSocketAddress;

/* com.symbian.utils */
import com.symbian.utils.Debug;

/* com.symbian.tools.j2me.sei */
import com.symbian.tools.j2me.sei.preferences.UserPreferences;
import com.symbian.tools.j2me.sei.preferences.SessionPreferences;
import com.symbian.tools.j2me.sei.emulator.session.RemoteSession;

class DebugSession extends RemoteSession {

    private File seiExe;
    private UserPreferences userPrefs; 
    private Process process;

    protected DebugSession(String exe) {
        seiExe = new File(exe);
    }

    /**
     * Check whether the subprocess is alive.
     * @throw IllegalStateException if the process is dead.
     */
    private void checkProcess() {
        try {
            // throws IllegalThreadStateException if process is still alive
            process.exitValue();
            throw new IllegalStateException("subprocess exited unexpectedly");
        } catch (IllegalThreadStateException x) {
            // still running
        }
    }

    /**
     * Makes a single connection attempt
     * @return <code>true</code> on success, <code>false</code> otherwise
     * @throws Exception if anything goes terribly wrong
     */
    private boolean connect() throws Exception {
        try {
            connectToSlave();
        } catch (IOException x) {
            return false;
        }
        Debug.println(this, "DebugSession: connected to the emulator");
        doHandshake(userPrefs.getSecurityPreferences().getChallenge());
        return true;
    }

    /**
     * Initialize the session.
     * The intialization code shouldnt be in the constructor to allow shutdown
     * during initialization, coming from the user.
     *
     * @param prefs user preferences used for initializing the connection
     *
     * @throws Exception if some error occured
     */
    public void initialize(UserPreferences prefs) throws Exception
    {  	userPrefs = prefs;
    	// Configure session properties to work with the emulator
        SessionPreferences sp = prefs.getKDPPreferences().getSessionPreferences(getName());
        sp.setHost("127.0.0.1");
        
        //TODO: should be incoming, shouldnt'it?
        sp.setOutgoing(true);
        super.initialize(prefs);
    	if(!prefs.getKDPPreferences().isEKA2EmulatorDiscovery())
      	{	//If we don't want to reconnect, always take a new free port number:
	        Socket socket = new Socket();
	        socket.bind(null);
	        int port = socket.getLocalPort();
	        socket.close();
	        sp.setPort(port);
	        launchProcess(port);
    	}
    	else
    	{	Socket testSocket = new Socket();
    		try{//Check if DA is running by trying to connect to it
    			testSocket.connect(new InetSocketAddress(sp.getHost(), sp.getPort()));
    			testSocket.close();
    		}
    		catch(IOException e)
    		{	//If we're here, we couldn't connect to Debug Agent
    			//WE ASSUME emulator isn't running and launch it
    			testSocket.close();
    			launchProcess(sp.getPort());
    		}
    	}
        while (!connect()) checkProcess();
    }
/**
 * Launch the SEI-Slave win32 executable
 *
 * @param port slave port argument
 * @throws IOException in case of I/O error
 */
private void launchProcess(int port) throws IOException {
    process = DebugUtils.launchProcess(seiExe, Integer.toString(port));
}

    /**
     * Terminate the session.
     */
    public void terminate() {
        super.terminate();
        if (!userPrefs.getKDPPreferences().isEKA2EmulatorDiscovery()) {
            Debug.println(this, "DebugSession: destroying emulator process");
            process.destroy();
        }
    }
}
