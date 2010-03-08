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




package com.nokia.wtk.device;


/**
 * A listener for WML browser context events.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceWMLBrowserContextListener {

    /**
     * Called when a context variable has been set, changed or deleted.
     * @param name      the name of the context variable.
     * @param value     the value of the context variable.
     *                  (may be empty or <code>null</code> if deleted)
     */
    public void contextVariableChanged(String name, String value);

    /**
     * Called when all context variables have been deleted.
     * This will occur as part of a 'newcontext' operation.
     */
    public void contextVariablesCleared();

    /**
     * Called when the current WML timer state changes.
     * @param active    if <code>true</code> then the timer is active
     * @param value     the value of the timer in milliseconds
     *                  (i.e. amount of time remaining).
     */
    public void timerUpdate(boolean active, int value);

}   // End of DeviceWMLBrowserListener
