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
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  A simple interface to define en exit request. An ExitRequest
 * is a question that the app environment asks the objects using it: "Can I
 * exit now?" The implementor of the exit request might answer true or false,
 * and if it answers false, the AppEnvironment will abort exiting the program.
 * An object that wants to be asked that before AppEnvironment
 * exists should implement an ExitRequest and register it with the
 * call <code>AppEnvironment.getAppEnvironment().registerExitRequest() </code>
 * Company:      Nokia
 * @version 1.0
 */

public interface ExitRequest {
        /**
         * Called when the AppEnvironment responds to an application exit request.
         * @return <code>true</code> if exit has proceeded successfully,
         *         <code>false</code> if exit has been preempted.
         */
        public boolean applicationExit();

}
