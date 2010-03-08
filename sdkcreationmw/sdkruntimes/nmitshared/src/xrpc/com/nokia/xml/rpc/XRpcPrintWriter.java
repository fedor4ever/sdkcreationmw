/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * $Id: XRpcPrintWriter.java,v 1.4 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcPrintWriter.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.xml.rpc;

/* java.io */
import java.io.Writer;
import java.io.PrintWriter;
import java.io.OutputStream;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;

/**
 * A writer interface for producing nicely formatted XML.
 *
 */
public class XRpcPrintWriter extends PrintWriter implements XRpcWriter {

    /**
     * Default indent string
     */
    private static final String DEFAULT_INDENT_STRING = "  ";

    /**
     * Current indent level
     */
    private int indentLevel;

    /**
     * Current indent string
     */
    private String indentString = DEFAULT_INDENT_STRING;

    /**
     * <code>true</code> if <code>println()</code> has been invoked but
     * no character has been written to the new line
     */
    private boolean blank = true;

    /**
     * Create a new <code>XRpcPrintWriter</code> with an initial indent
     * level of 0.
     *
     * @param w a {@link Writer}
     */
    public XRpcPrintWriter(Writer w) {
        this(w, 0);
    }

    /**
     * Create a new <code>XRpcPrintWriter</code>.
     *
     * @param w a {@link Writer}
     * @param level the initial indent level (number of spaces)
     */
    public XRpcPrintWriter(Writer w, int level) {
        super(w, true);
        indentLevel = level;
    }

    /**
     * Create a new <code>XRpcPrintWriter</code> with an initial indent
     * level of 0.
     *
     * @param out an output stream
     */
    public XRpcPrintWriter(OutputStream out)
    throws UnsupportedEncodingException {
        this(out, 0);
    }

    /**
     * Create a new <code>XRpcPrintWriter</code>.
     *
     * @param out an output stream
     * @param level the initial indent level (number of spaces)
     */
    public XRpcPrintWriter(OutputStream out, int level)
    throws UnsupportedEncodingException {
        this(out, XRpcConstants.XRPC_ENCODING, level);
    }

    /**
     * Create a new <code>XRpcPrintWriter</code>.
     *
     * @param out an output stream
     * @param encoding an output stream
     * @param level the initial indent level (number of spaces)
     */
    private XRpcPrintWriter(OutputStream out, String encoding, int level)
    throws UnsupportedEncodingException {
        this(new BufferedWriter(new OutputStreamWriter(out,encoding)),level);
    }

    /**
     * Sets indent string
     */
    public void setIndentString(String s) {
        indentString = ((s == null) ? DEFAULT_INDENT_STRING : s);
    }

    /**
     * Checks if the string is empty and if it is, prints the indentation
     * prefix.
     */
    private void checkIndent() {
        if (blank) {
            synchronized (lock) {
                if (blank) {
                    blank = false;
                    for (int i=0; i<indentLevel; i++) {
                        super.print(indentString);
                    }
                }
            }
        }
    }

    //=======================================================================
    //      I N T E R F A C E
    //=======================================================================

    public Object getLock() {
        return lock;
    }

    public PrintWriter getPrintWriter() {
        return this;
    }

    public void print(char c) {
        checkIndent();
        super.print(c);
    }

    public void print(String s) {
        checkIndent();
        super.print(s);
    }

    public void println() {
        synchronized (lock) {
            super.print("\n"); // don't do the "\r\n" thing
            flush();
            blank = true;
        }
    }

    public void indent() {
        synchronized (lock) {
            if (!blank) println();
            indentLevel++;
        }
    }

    public void unindent() {
        synchronized (lock) {
            if (!blank) println();
            indentLevel--;
        }
    }

    //=======================================================================
    //  These methods make sure that we keep indentation even if someone is
    //  using this class as a PrintWriter. We make an assumtion that people
    //  terminate the line by calling println(). We don't handle end-of-line
    //  characters embedded in strings
    //=======================================================================

    /**
     * Print a boolean and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(boolean)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>boolean</code> to be printed
     */
    public synchronized void println(boolean x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print a character and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(char)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>char</code> to be printed.
     */
    public synchronized void println(char x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print an integer and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(int)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>int</code> to be printed.
     */
    public synchronized void println(int x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print a long and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(long)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  a The <code>long</code> to be printed.
     */
    public synchronized void println(long x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print a float and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(float)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>float</code> to be printed.
     */
    public synchronized void println(float x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print a double and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(double)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>double</code> to be printed.
     */
    public synchronized void println(double x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print an array of characters and then terminate the line.  This method
     * behaves as though it invokes <code>{@link #print(char[])}</code> and
     * then <code>{@link #println()}</code>.
     *
     * @param x  an array of chars to print.
     */
    public synchronized void println(char x[]) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print a String and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(String)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>String</code> to be printed.
     */
    public synchronized void println(String x) {
        checkIndent();
        print(x);
        println();
    }

    /**
     * Print an Object and then terminate the line.  This method behaves as
     * though it invokes <code>{@link #print(Object)}</code> and then
     * <code>{@link #println()}</code>.
     *
     * @param x  The <code>Object</code> to be printed.
     */
    public synchronized void println(Object x) {
        checkIndent();
        print(x);
        println();
    }
}
