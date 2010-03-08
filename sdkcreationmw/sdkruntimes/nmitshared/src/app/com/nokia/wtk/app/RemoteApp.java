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

/* java.rmi */
import java.rmi.Remote;
import java.rmi.RemoteException;

/**
 * This remote interface is used for making single instance of an
 * application perform actions on behalf of other processes.
 *
 */
public interface RemoteApp extends Remote {

    /**
     * Remote request to perform command
     *
     * @param args the arguments (command line)
     */
    public void performCommand(String [] args) throws RemoteException;
}

/*
 * Local Variables:
 * c-basic-offset:4
 * End:
 */
