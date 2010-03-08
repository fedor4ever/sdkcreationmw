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

/* java.util */
import java.util.List;
import java.util.Vector;

/* java.io */
import java.io.IOException;
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * Represents a subprocess. Takes care of flushing output streams of the
 * subprocess. May notify listeners when process terminates.
 *
 */
public class SubProcess {

    /**
     * Listeners are notified when subprocess is finished.
     */
    public interface Listener {
        public void processDone(SubProcess process);
    }

    /** Empty array of {@link Listener}s. */
    public static final Listener [] NO_LISTENERS = new Listener[0];

    /** Empty array of {@link Listener}s. */
    public static final SubProcess [] NO_PROCESSES = new SubProcess[0];

    /** Static array of active processes */
    private static final List activeProcesses = new Vector();

    /** The actual process */
    private Process process;

    /** This flag is set to <code>false</code> when process dies. */
    private boolean alive = true;

    /** The list of listeners */
    private Listener[] listeners = NO_LISTENERS;

    /**
     * Creates the <code>SubProcess</code> representing the specified
     * {@link Process}.
     */
    public SubProcess(Process p) {
        this(p, (PrintWriter)null, (PrintWriter)null);
    }

    /**
     * Creates the <code>SubProcess</code> representing the specified
     * {@link Process}.
     */
    public SubProcess(Process p, OutputStream out, OutputStream err) {
        this(p, getPrintWriter(out), getPrintWriter(err));
    }

    /**
     * Creates the <code>SubProcess</code> representing the specified
     * {@link Process}.
     */
    public SubProcess(final Process p, PrintWriter out, PrintWriter err) {
        if (p == null)  throw new NullPointerException("process");
        process = p;
        new Pump("Stdout", p.getInputStream(), out).start();
        new Pump("Stderr", p.getErrorStream(), err).start();

        // NOTE: not only this thread notifies the listeners, it also notifies
        // waitFor() method about completion of the process.
        activeProcesses.add(this);
        new Thread("ProcessWaiter") {
            public void run() {
                while (alive) {
                    try {
                        waitFor();
                        Listener[] a;

                        // synchronize with add/removeListener()
                        synchronized (SubProcess.this) {
                            alive = false;
                            a = listeners;
                            SubProcess.this.notifyAll();
                        }

                        int n = a.length;
                        for( int i=0; i<n; i++) {
                            // null's are filtered out by addListener()
                            try { a[i].processDone(SubProcess.this); }
                            catch (Throwable x) { x.printStackTrace(); }
                        }

                    } catch (InterruptedException x) {
                        // ignore
                    }
                }
                // remove this process from the list of active processes
                activeProcesses.remove(SubProcess.this);
            }
        }.start();
    }

    /**
     * Gets the output stream of the subprocess.
     * Output to the stream is piped into the standard input stream of
     * the process represented by this <code>SubProcess</code> object.
     *
     * @return  the output stream connected to the normal input of the
     *          subprocess.
     */
    public OutputStream getOutputStream() {
        return process.getOutputStream();
    }

    /**
     * causes the current thread to wait, if necessary, until the
     * process represented by this <code>SubProcess</code> object
     * has terminated. This method returns immediately if the subprocess
     * has already terminated. If the subprocess has not yet terminated,
     * the calling thread will be blocked until the subprocess exits.
     *
     * @return     the exit value of the process. By convention,
     *             <code>0</code> indicates normal termination.
     * @exception  InterruptedException  if the current thread is
     *             {@link Thread#interrupt() interrupted} by another thread
     *             while it is waiting, then the wait is ended and an
     *             <code>InterruptedException</code> is thrown.
     */
    public int waitFor() throws InterruptedException {
        return process.waitFor();
    }

    /**
     * causes the current thread to wait, if necessary, until the
     * process represented by this <code>SubProcess</code> object
     * has terminated or until the specified timeout expires.
     * {@link InterruptedException}s are ignored.
     *
     * @return     <code>true</code> if process has terminated,
     *             <code>false</code> if timeout expired.
     */
    public boolean waitFor(long millis) {
        if (millis < 0) {
            throw new IllegalArgumentException("negative timeout");
        } else if (millis == 0) {

            while (isAlive()) {
                try { waitFor(); }
                catch (InterruptedException x) { /* ignore */ }
            }
            return true;

        } else {

            if (isAlive()) {
                synchronized (this) {
                    long deadline = System.currentTimeMillis() + millis;
                    long timeout = millis;
                    while (isAlive() && timeout >= 0) {
                        // The "ProcessWaiter" thread will notify us when the
                        // proces terminates
                        try { wait(timeout); }
                        catch (InterruptedException x) { /* ignore */ }
                        timeout = deadline - System.currentTimeMillis();
                    }
                }
            }

            return (!isAlive());
        }
    }

    /**
     * Returns the exit value for the subprocess.
     *
     * @return  the exit value of the subprocess represented by this
     *          <code>Process</code> object. by convention, the value
     *          <code>0</code> indicates normal termination.
     * @exception  IllegalThreadStateException  if the subprocess represented
     *             by this <code>Process</code> object has not yet terminated.
     */
    public int exitValue() {
        return process.exitValue();
    }

    /**
     * Checks if subprocess is still alive.
     */
    public boolean isAlive() {
        return alive;
    }

    /**
     * Kills the subprocess.
     */
    public void kill() {
        process.destroy();
    }

    /**
     * Returns Java Process object
     */
    public Process getProcess() {
        return process;
    }

    /**
     * Adds a listener. If you need a guarantee that you won't miss
     * the notification, you want to synchronize with the notification
     * thread like this:
     *
     * <blockquote><pre>
     * SubProcess sp = new SubProcess(process);
     * boolean alreadyDead = true;
     * synchronized (sp) {
     *     if (sp.isAlive()) {
     *         sp.addListener(this);
     *         alreadyDead = false;
     *     }
     * }
     * if (alreadyDead) {
     *     processDone(sp);
     * }
     * </pre></blockquote>
     *
     * @param listener the listener to add. Must not be <code>null</code>.
     */
    public void addListener(Listener listener) {
        if (listener == null) {  // no null's, please
            throw new NullPointerException("listener");
        } else {
            synchronized (this) {
                int n = listeners.length;
                Listener[] newListeners = new Listener[n+1];
                if (n > 0) System.arraycopy(listeners, 0, newListeners, 0, n);
                newListeners[n] = listener;
                listeners = newListeners;
            }
        }
    }

    /**
     * Removes a listener.
     * @param listener the listener to remove. <code>null</code> is ignored.
     * @return <code>true</code> if listener has been removed,
     *         <code>false</code> if it wasn't there
     */
    public boolean removeListener(Listener listener) {
        if (listener != null) {   // null's are filtered out by addListener()
            synchronized (this) {
                int i = ArrayUtils.indexOf(listeners, listener);
                if (i >= 0) {
                    if (listeners.length == 1) {
                        listeners = NO_LISTENERS;
                    } else {
                        listeners = (Listener[])ArrayUtils.remove(listeners,i);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Given instance of {@link OutputStream}, returns {@link PrintWriter}
     * or <code>null</code> if <code>out</code> is <code>null</code>.
     */
    private static PrintWriter getPrintWriter(OutputStream out) {
        return ((out != null) ? (new PrintWriter(out)) : null);
    }

    /**
     * Returns array of active processes
     */
    public static SubProcess [] getActiveProcesses() {
        return (SubProcess[])activeProcesses.toArray(NO_PROCESSES);
    }

    //=======================================================================
    //          I N N E R   C L A S S E S
    //=======================================================================

    /**
     * This thread pumps output from the subprocess to the specified writer.
     */
    private class Pump extends Thread {

        /** The input stream. */
        private BufferedReader in;

        /** The output stream.*/
        private PrintWriter out;

        /** Creates a <code>Pump</code> thread and starts it. */
        public Pump( String name, InputStream in, PrintWriter out) {
            super(name);
            this.in = new BufferedReader(new InputStreamReader(in));
            this.out = out;
        }

        /** Pumps the data */
        public void run() {
            try {
                String s;
                while ((s = in.readLine()) != null) {
                    if (out != null) {
                        out.println(s);
                    }
                }
            } catch ( IOException x ) { /* ignore */ }
        }
    }
}

