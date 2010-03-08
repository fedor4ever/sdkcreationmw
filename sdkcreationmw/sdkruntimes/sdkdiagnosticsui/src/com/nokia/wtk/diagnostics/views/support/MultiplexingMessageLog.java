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





package com.nokia.wtk.diagnostics.views.support;

import com.nokia.wtk.util.Trace;
import com.nokia.wtk.util.ArrayUtils;
import com.nokia.wtk.util.MessageLog;

/**
 * A @{link MessageLog} that forward messages to a bunch of others MessageLogs
 */
public class MultiplexingMessageLog implements MessageLog {

    /**
     * The MultiplexingMessageLog for warnings. Will be automatically
     * hooked up to Trace.warning.
     */
    final public static MultiplexingMessageLog WarningLog =
        new MultiplexingMessageLog();
    /**
     * The MultiplexingMessageLog for status. Will be automatically
     * hooked up to Trace.status.
     */
    final public static MultiplexingMessageLog StatusLog =
        new MultiplexingMessageLog();
    /**
     * The MultiplexingMessageLog for debug. Will be automatically
     * hooked up to Trace.debug.
     */
    final public static MultiplexingMessageLog DebugLog =
        new MultiplexingMessageLog();
    /**
     * The MultiplexingMessageLog for errors. Will be automatically
     * hooked up to Trace.error.
     */
    final public static MultiplexingMessageLog ErrorLog =
        new MultiplexingMessageLog();

    /**
     * The MultiplexingMessageLog for console. Will be automatically
     * hooked up to Trace.console.
     */
    final public static MultiplexingMessageLog ConsoleLog =
        new MultiplexingMessageLog();

    static {
        MessageLog defaultWarningLog = Trace.setWarningLog(WarningLog);
        MessageLog defaultStatusLog = Trace.setStatusLog(StatusLog);
        MessageLog defaultDebugLog = Trace.setDebugLog(DebugLog);
        MessageLog defaultErrorLog = Trace.setErrorLog(ErrorLog);
        MessageLog defaultConsoleLog = Trace.setConsoleLog(ConsoleLog);

        // redirect trace to the default logs
        WarningLog.addMessageLog(defaultWarningLog);
        StatusLog.addMessageLog(defaultStatusLog);
        DebugLog.addMessageLog(defaultDebugLog);
        ErrorLog.addMessageLog(defaultErrorLog);
        ConsoleLog.addMessageLog(defaultConsoleLog);
    }

    private MessageLog [] subscribers = new MessageLog[0];

    /**
     * Will return true if it has any subscribers willing to receive
     * the messages
     * @return true if has any enabled subscriber
     * @see com.nokia.wtk.util.MessageLog#isEnabled()
     */
    public boolean isEnabled() {
        MessageLog [] logs = subscribers;
        for (int i=0; i<logs.length; i++) {
            if (logs[i].isEnabled()) {
                return true;
            }
        }
        return false;
    }

    /* (non-Javadoc)
     * @see com.nokia.wtk.util.MessageLog#println(java.lang.String)
     */
    public void println(String msg) {
        MessageLog [] logs = subscribers;
        for (int i=0; i<logs.length; i++) {
            logs[i].println(msg);
        }
    }

    /* (non-Javadoc)
     * @see com.nokia.wtk.util.MessageLog#printStackTrace(java.lang.Throwable)
     */
    public void printStackTrace(Throwable x) {
        MessageLog [] logs = subscribers;
        for (int i=0; i<logs.length; i++) {
            logs[i].printStackTrace(x);
        }
    }

    /**
     * Adds a MessageLog ot this multiplexer.
     * @param log the log to add
     */
    public void addMessageLog(MessageLog log) {
        if (log == null) {
            return;
        }
        if (log.equals(this)) {
            throw new Error("Cannot add MultiplexingLog to itself");
        }
        subscribers = (MessageLog[])ArrayUtils.add(subscribers,log);
    }

    /**
     * Remove MessageLog
     * @param log the MessageLog to remove. If hadn't been added, no change will
     * happen.
     */
    public void removeMessageLog(MessageLog log) {
        subscribers = (MessageLog[])ArrayUtils.remove(subscribers,log);
    }
}
