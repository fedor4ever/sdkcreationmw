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
 * Logging facility.
 *
 */
public abstract class Trace {

    /* names of the system properties that affect debug output */
    public static final String VERBOSE_PROP = "nokia.wtk.verbose";
    public static final String DETAILS_PROP = "nokia.wtk.detailTrace";

    /* values of the above properties taken at startup */
    private static boolean verbose = Utils.getSystemProperty(VERBOSE_PROP,false);
    private static boolean detail = Utils.getSystemProperty(DETAILS_PROP,verbose);

    /* internal log redirectors to avoid exposing Log class */
    private static final Log DEBUG = new Log(new DebugLog());
    private static final Log STATUS  = new Log("STATUS: ",new StatusLog());
    private static final Log WARNING = new Log("WARNING: ",new WarningLog());
    private static final Log ERROR   = new Log("ERROR: ",new ErrorLog());
    private static final Log CONSOLE = new Log(new ConsoleLog());

    /* various types of traces */
    public static final MessageLog Debug   = DEBUG;
    public static final MessageLog Status  = STATUS;
    public static final MessageLog Warning = WARNING;
    public static final MessageLog Error   = ERROR;
    public static final MessageLog Console = CONSOLE;
    public static final MessageLog None    = new NoLog();

    /**
     * Default constructor is suppressed, ensuring non-instantiability.
     */
    private Trace() {
        throw new InternalError("Trace is a container of static methods");
    }

    /**
     * Returns <code>true</code> if verbose mode is enabled. The verbose
     * mode is on if <code>&quot;nokia.wtk.verbose&quot;</code>
     * system property is set to <code>&quot;true&quot;</code>.
     *
     * @return <code>true</code> if verbose mode is enabled
     */
    public static boolean isVerbose() {
        return verbose;
    }

    /**
     * Returns <code>true</code> if detail mode is enabled. The verbose
     * mode is on if <code>&quot;nokia.wtk.detailTrace&quot;</code>
     * system property is set to <code>&quot;true&quot;</code>.
     *
     * @return <code>true</code> if detail mode is enabled
     */
    public static boolean detailOutOn() {
        return detail;
    }

    /**
     * Redirects debug log. Note that if target message log is set to
     * <code>null</code>, the messages will still appear in standard
     * output if the application is switched to verbose mode by setting
     * <code>&quot;nokia.wtk.verbose&quot;</code> system property to
     * <code>&quot;true&quot;</code>. To completely disable debug messages
     * you can to do the following:
     * <pre>
     *  Trace.setDebugLog(Trace.None);
     * </pre>
     *
     * @param log the object to receive debug log messages.
     * @return the previously registered debug log
     */
    public static MessageLog setDebugLog(MessageLog log) {
        return DEBUG.setTarget(log);
    }

    /**
     * Redirects status log. Note that if target message log is set to
     * <code>null</code>, the messages will still appear in standard
     * output if the application is switched to verbose mode by setting
     * <code>&quot;nokia.wtk.verbose&quot;</code> system property to
     * <code>&quot;true&quot;</code>. To completely disable status messages
     * you can to do the following:
     * <pre>
     *  Trace.setStatusLog(Trace.None);
     * </pre>
     *
     * @param log the object to receive status log messages.
     * @return the previously registered status log
     */
    public static MessageLog setStatusLog(MessageLog log) {
        return STATUS.setTarget(log);
    }

    /**
     * Redirects warning log. Note that if target message log is set to
     * <code>null</code>, the messages will still appear in standard
     * output if the application is switched to verbose mode by setting
     * <code>&quot;nokia.wtk.verbose&quot;</code> system property to
     * <code>&quot;true&quot;</code>. To completely disable warning messages
     * you can to do the following:
     * <pre>
     *  Trace.setWarningLog(Trace.None);
     * </pre>
     *
     * @param log the object to receive warning log messages.
     * @return the previously registered warning log
     */
    public static MessageLog setWarningLog(MessageLog log) {
        return WARNING.setTarget(log);
    }

    /**
     * Redirects error log. Note that if target message log is set to
     * <code>null</code>, the messages will still appear in standard
     * output if the application is switched to verbose mode by setting
     * <code>&quot;nokia.wtk.verbose&quot;</code> system property to
     * <code>&quot;true&quot;</code>. To completely disable error messages
     * you can to do the following:
     * <pre>
     * Trace.setErrorLog(Trace.None);
     * </pre>
     *
     * @param log the object to receive error log messages.
     * @return the previously registered error log
     */
    public static MessageLog setErrorLog(MessageLog log) {
        return ERROR.setTarget(log);
    }

    /**
     * Redirects console log. Note that if target message log is set to
     * <code>null</code>, the messages will still appear in standard
     * output if the application is switched to verbose mode by setting
     * <code>&quot;nokia.wtk.verbose&quot;</code> system property to
     * <code>&quot;true&quot;</code>. To completely disable console messages
     * you can to do the following:
     * <pre>
     *  Trace.setConsoleLog(Trace.None);
     * </pre>
     *
     * @param log the object to receive console log messages.
     * @return the previously registered console log
     */
    public static MessageLog setConsoleLog(MessageLog log) {
        return CONSOLE.setTarget(log);
    }

    /**
     * Equivalent to <code>Trace.Debug.printStackTrace(x)</code>
     * @param x the exception to dump
     */
    public static void printStackTrace(Throwable x) {
        Debug.printStackTrace(x);
    }

    //=======================================================================
    //          I N N E R    C L A S S E S
    //=======================================================================

    private static class DebugLog implements MessageLog {
        public boolean isEnabled() { return isVerbose(); }
        public void println(String msg) {
            if (isEnabled()) System.out.println(msg);
        }
        public void printStackTrace(Throwable x) {
            if (isEnabled()) x.printStackTrace(System.out);
        }
    }

    private static class StatusLog implements MessageLog {
        public boolean isEnabled() { return true; }
        public void println(String msg) {
            System.out.println(msg);
        }
        public void printStackTrace(Throwable x) {
            x.printStackTrace(System.out);
        }
    }

    private static class ErrorLog implements MessageLog {
        public boolean isEnabled() { return true; }
        public void println(String msg) {
            System.out.println(msg);
        }
        public void printStackTrace(Throwable x) {
            x.printStackTrace(System.out);
        }
    }

    private static class WarningLog implements MessageLog {
        public boolean isEnabled() { return true; }
        public void println(String msg) {
            System.out.println(msg);
        }
        public void printStackTrace(Throwable x) {
            x.printStackTrace(System.out);
        }
    }

    private static class ConsoleLog implements MessageLog {
        public boolean isEnabled() { return true; }
        public void println(String msg) {
            System.out.println(msg);
        }
        public void printStackTrace(Throwable x) {
            x.printStackTrace(System.out);
        }
    }

    private static class NoLog implements MessageLog {
        public boolean isEnabled() { return false; }
        public void println(String msg) {}
        public void printStackTrace(Throwable x) {}
    }

    private static class Log implements MessageLog {
        private String prefix;
        private MessageLog target;
        Log(MessageLog log) {
            this(null, log);
        }
        Log(String pref, MessageLog log) {
            prefix = pref;
            target = log;
        }
        MessageLog setTarget(MessageLog log) {
            MessageLog oldTarget = target;
            target = log;
            return oldTarget;
        }
        public boolean isEnabled() {
            MessageLog log = target;
            if (log != null) {
                return log.isEnabled();
            } else {
                return verbose;
            }
        }
        public void println(String msg) {
            MessageLog log = target;
            if (log != null) {
                log.println(msg);
            } else if (verbose) {
                if (prefix != null) {
                    System.out.println(prefix  + msg);
                } else {
                    System.out.println(msg);
                }
            }
        }
        public void printStackTrace(Throwable x) {
            MessageLog log = target;
            if (log != null) {
                log.printStackTrace(x);
            } else if (verbose) {
                x.printStackTrace(System.out);
            }
        }
    }
}
