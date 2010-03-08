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



package com.nokia.wtk.util;

/**
 * Represents a message log channel. Provides methods for logging a single
 * line of text or an exception stack trace.
 *
 */
public interface MessageLog {

    /**
     * Returns <code>true</code> if log is enabled for this channel.
     * The users of this class are encouraged to check if log is enabled
     * prior to constructing expensive log messages.
     *
     * @return <code>true</code> if log is enabled for this channel
     */
    public boolean isEnabled();

    /**
     * Logs a messages and terminates the line
     * @param msg the message to print
     */
    public void println(String msg);

    /**
     * Logs an exception stack trace.
     * @param x the exception to log
     */
    public void printStackTrace(Throwable x);
}
