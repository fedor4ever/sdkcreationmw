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

/* java.io */
import java.io.PrintWriter;

/**
 * A set of specialized PrintWriters for message routing. These writers will
 * use the MessageLog as returned by AppEnvironment, and thus implementing
 * AppEnvironment as desired will automatically make all these routers route
 * correctly.
 * @see MessageLog
 * @see AppEnvironment.
 *
 */
public interface MessageWriters {

    public static final String VERBOSE_SYSPROP = "nokia.wtk.verbose";
    public static boolean verbose = Utils.getSystemProperty(VERBOSE_SYSPROP,false);

    public final static PrintWriter SYSOUT = new PrintWriter(System.out, true);
    public final static PrintWriter SYSERR = new PrintWriter(System.err, true);

    public final static MessageLog DetailMsg = Trace.Debug;
    public final static MessageLog DebugMsg = Trace.Debug;
    public final static MessageLog PlainMsg = Trace.Status;
    public final static MessageLog WarnMsg = Trace.Warning;
    public final static MessageLog ErrorMsg = Trace.Error;
    public final static MessageLog FatalMsg = Trace.Error;
    public final static MessageLog StatusMsg = Trace.Status;
    public final static MessageLog NullMsg = Trace.None;
}
