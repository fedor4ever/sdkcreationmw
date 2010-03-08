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
 * An interface to the device WML browser.
 * This interface must be implemented by a device simulator component,
 * if it is to support WML-specific browsing operations.
 *
 */
public interface DeviceWMLBrowser extends DeviceBrowser {

    /**
     * Redisplays the current document using
     * possibly updated WML browser context.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void refresh();

    /**
     * Clears the current WML browser context.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void newContext();

    /**
     * Assigns a value to a WML browser context variable.
     * If a variable does not exist, then it will be created.
     * If the value is an empty string or <code>null</code>,
     * then the variable will be removed.
     *
     * @param name      the name of the context variable.
     * @param value     the value to assign.
     *
     * @exception       UnsupportedOperationException
     *                  if this method is not supported.
     *                  Implementations that do not support this method
     *                  should explicitly declare this exception.
     */
    public void setContextVar(String name, String value);

}   // End of DeviceWMLBrowser
