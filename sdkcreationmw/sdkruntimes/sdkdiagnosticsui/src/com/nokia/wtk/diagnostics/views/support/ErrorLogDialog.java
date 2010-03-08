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

import java.util.LinkedList;
import java.util.List;

import javax.swing.JLabel;

import com.nokia.wtk.app.ApplicationDialogs;
import com.nokia.wtk.util.MessageLog;
import com.nokia.wtk.util.MessageWriters;

/**
 * An Error Dialog for message viewing.
 */
public class ErrorLogDialog implements MessageWriters {

    /**
     * Thread that shows error dialogs. Exits if idle for more than 30 seconds.
     */
    private static class ErrorThread extends Thread {

        private long timeout = 30000;
        ErrorThread() {
            super("ErrorDialogs");
            start();
        }

        public void run() {
            try {
                if (verbose)
                    DebugMsg.println(getName() + " thread started.");
                long start = System.currentTimeMillis();
                while (true) {
                    Runnable workItem = null;
                    synchronized (queue) {
                        if (queue.isEmpty()) {
                            try {
                                queue.wait(timeout);
                            } catch (InterruptedException x) {}
                        }
                        if (queue.isEmpty()) {

                            // if waiting for too long, stop the thread
                            long now = System.currentTimeMillis();
                            if (now >= (start + timeout)) {
                                if (errorThread == this) {
                                    errorThread = null;
                                }
                                break;
                            } else {
                                continue;
                            }
                        } else {
                            workItem = (Runnable) queue.remove(0);
                        }
                    }

                    workItem.run();
                    start = System.currentTimeMillis(); // restart the timer
                }
            } finally {
                synchronized (queue) {
                    if (errorThread == this) {
                        errorThread = null;
                    }
                }
                if (verbose)
                    DebugMsg.println(getName() + " thread exiting.");
            }
        }
    }
    private static String currentMessage = null;
    private static Thread errorThread = null;

    //======================================================================
    //  Use separate thread to display dialogs to avoid blocking the
    //  calling thread. Otherwise, a deadlock may occur
    //======================================================================

    private static List queue = new LinkedList();

    /**
     * Formats an error message string as an HTML display string to be placed
     * in a dialog. The message text will be broken into separate blocks as
     * delineated by any embedded line breaks.  Each block will be enclosed
     * as necessary in a table cell (one per row) to conform to width
     * limitations.
     *
     * @param msg the message to format
     * @param center if <code>true</code> then center the message text.
     *               Otherwise, message text will be left-justified.
     * @param wrap   the message width that message text is wrapped against.
     *                If zero or negative, then message text is <b>not</b>
     *                wrapped.
     */
    private static String formatMessageAsHtml(
        String msg,
        boolean center,
        int wrap) {
        return ApplicationDialogs.formatMessageAsHtml(
            msg,
            center,
            wrap,
            "<font color=#000000><strong>",
            "</strong></font>");
    }

    static void queueDialog(final String title, final String text) {

        // don't queue a dialog if it's the same message as the one
        // currently being displayed
        String current = currentMessage;
        if (current != null && current.equals(text)) {
            if (verbose)
                DebugMsg.println(
                    "Not showing duplicate error message: " + text);
            return;
        }

        // don't show useless messages
        String trim = text.trim();
        if (trim.equals("null") || trim.length() == 0) {
            if (verbose)
                DebugMsg.println("Not showing useless error message: " + text);
            return;
        }

        Runnable request = new Runnable() {
            public void run() {
                String s;

                // don't format formatted text
                if (text.startsWith("<html>")) {
                    s = text;
                } else {

                    // Let the message to be as wide as it wants to be
                    s = formatMessageAsHtml(text, false, 0);

                    // Test if it's gotten too wide
                    JLabel test = new JLabel(s);
                    if (test.getPreferredSize().width
                        >= ApplicationDialogs.MAX_MESSAGE_WIDTH) {

                        // Re-format the message, this time limiting its
                        // horizontal size
                        s =
                            formatMessageAsHtml(
                                text,
                                false,
                                ApplicationDialogs.MAX_MESSAGE_WIDTH);
                    }
                }

                currentMessage = s;
                try {
                    ApplicationDialogs.invokeErrorMessageDialog(
                        title,
                        null,
                        s,
                        null);
                } finally {
                    currentMessage = null;
                }
            }
        };

        // add it to the queue and wake up (or start) the error dialog thread
        synchronized (queue) {
            queue.add(request);
            if (errorThread == null || !errorThread.isAlive()) {
                errorThread = new ErrorThread();
            }
            queue.notify();
        }
    }

    /**
     * Adds a set of dialogs to consume error messages.
     */
    public static void startErrorDialogs() {

        MultiplexingMessageLog.ErrorLog.addMessageLog(new ErrorMessageLog());
    }
}

class ErrorMessageLog implements MessageLog {

    ErrorMessageLog() {}

    /**
        * Returns <code>true</code> if log is enabled for this channel.
        * The users of this class are encouraged to check if log is enabled
        * prior to constructing expensive log messages.
        *
        * @return <code>true</code> if log is enabled for this channel
        */
    public boolean isEnabled() {
        return true;
    }

    /**
     * Logs a messages and terminates the line
     * @param msg the message to print
     */
    public void println(String msg) {
        ErrorLogDialog.queueDialog("Error", msg);
    }

    /**
     * Logs an exception stack trace.
     * @param x the exception to log
     */
    public void printStackTrace(Throwable x) {
        ErrorLogDialog.queueDialog("Error", x.toString());
    }
}
