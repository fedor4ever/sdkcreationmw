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
 * An interface for posting device error messages.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceMessageHandler {

    /**
     * Checks debug message enable status.
     * @return              <code>true</code> if debug messages are enabled.
     */
    public boolean isDebugEnabled();

    /**
     * Sends a debug message.
     * @param message       the message to send.
     * @param ex            an optional exception.
     */
    public void debug(String message, Throwable ex);

    /**
     * Sends notification of a device warning condition.
     * @param message       the message to send.
     * @param ex            an optional exception.
     */
    public void warning(String message, Throwable ex);

    /**
     * Sends notification of a device error condition.
     * @param message       the message to send.
     * @param ex            an optional exception.
     */
    public void error(String message, Throwable ex);

    /**
     * Sends a a JAM console message.
     * @param message       the message to send.
     * @param ex            an optional exception.
     */
    public void console(String message, Throwable ex);

}   // End of DeviceMessageHandler

