/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.j2me.sei;

/* java.io */
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.FileNotFoundException;

/**
 * Package private utilities
 */
abstract class DebugUtils {

    /**
     * Disables instantiation
     */
    private DebugUtils() {
    }

    /**
     * Starts a win32 executable
     *
     * @param exe the executable to launch
     * @return the Java process object
     *
     * @throws IOException in case of I/O error
     */
    static Process launchProcess(File exe) throws IOException {
        return launchProcess(exe, (String[])null);
    }

    /**
     * Starts a win32 executable
     *
     * @param exe the executable to launch
     * @param arg command line argument
     * @return the Java process object
     *
     * @throws IOException in case of I/O error
     */
    static Process launchProcess(File exe, String arg) throws IOException {
        String [] args = ((arg == null) ? null : new String[] { arg });
        return launchProcess(exe, args);
    }

    /**
     * Starts a win32 executable
     *
     * @param exe the executable to launch
     * @param args command line arguments
     * @return the Java process object
     *
     * @throws IOException in case of I/O error
     */
    static Process launchProcess(File exe, String [] args) throws IOException {
        if (exe.isFile()) {
            File dir = exe.getParentFile();
            int nargs = (args == null) ? 0 : args.length;
            String [] cmd = new String[nargs+1];
            cmd[0] = exe.getAbsolutePath();
            if (nargs > 0) System.arraycopy(args,0,cmd,1,nargs);
            Process process = Runtime.getRuntime().exec(cmd, null, dir);
            String s = exe.getName();
            new Pump(s+"-Stdout",process.getInputStream(),System.out).start();
            new Pump(s+"-Stderr",process.getErrorStream(),System.err).start();
            return process;
        } else {
            throw new FileNotFoundException(exe.getAbsolutePath());
        }
    }

    /**
     * This thread pumps output from the subprocess to the specified output
     * stream.
     */
    private static class Pump extends Thread {

        /** The input stream. */
        private BufferedReader in;

        /** The output stream.*/
        private PrintStream out;

        /**
         * Creates a <code>Pump</code> thread and starts it.
         *
         * @param name the thread name
         * @param in input stream
         * @param out output stream
         */
        public Pump( String name, InputStream in, PrintStream out) {
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
