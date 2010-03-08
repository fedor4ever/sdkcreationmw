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



package com.nokia.wtk.app;

/**
 * This interface is the counterpart of com.nokia.wtk.app.RemoteApp.
 * This interface is called on the local side from a remote request.
 * Of course, it also can be used as the argument handling routine of an application.
 *
 * @see RemoteApp, RemoteSingleton
 */
public interface LocalApp {
    /**
     * Request to perform a command in local application.
     * Invalid arguments are ignored.
     *
     * @param args the arguments (command line)
     */
    public void handleArguments(String [] args);
}

/*
 * Local Variables:
 * c-basic-offset:4
 * End:
 */
