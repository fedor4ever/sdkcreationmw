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
import java.io.Writer;
import java.io.PrintWriter;
import java.io.OutputStream;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;

/**
 * This captures context used when pretty printing hierarchical content.
 * Other than that, it behaves as a normal {@link PrintWriter}.
 *
 */
public class IndentWriter extends PrintWriter {

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
     * Create a new <code>IndentWriter</code> with an initial indent
     * level of 0.
     *
     * @param w a {@link Writer}
     */
    public IndentWriter(Writer w) {
        this(w, 0);
    }

    /**
     * Create a new <code>IndentWriter</code>.
     *
     * @param w a {@link Writer}
     * @param level the initial indent level
     */
    public IndentWriter(Writer w, int level) {
        super(w, true);
        indentLevel = level;
    }

    /**
     * Create a new <code>IndentWriter</code> with an initial indent
     * level of 0.
     *
     * @param out an output stream
     */
    public IndentWriter(OutputStream out) {
        this(out, 0);
    }

    /**
     * Create a new <code>IndentWriter</code>.
     *
     * @param out an output stream
     * @param level the initial indent level (number of spaces)
     */
    public IndentWriter(OutputStream out, int level) {
        this(new BufferedWriter(new OutputStreamWriter(out)),level);
    }

    /**
     * Create a new <code>IndentWriter</code>.
     *
     * @param out an output stream
     * @param enc the encoding to use
     *
     * @throws UnsupportedEncodingException if the specified encoding is
     * not supported
     */
    public IndentWriter(OutputStream out, String enc)
    throws UnsupportedEncodingException {
        this(out, enc, 0);
    }

    /**
     * Create a new <code>IndentWriter</code>.
     *
     * @param out an output stream
     * @param enc the encoding to use
     * @param level the initial indentation level
     *
     * @throws UnsupportedEncodingException if the specified encoding is
     * not supported
     */
    public IndentWriter(OutputStream out, String enc, int level)
    throws UnsupportedEncodingException {
        this(new BufferedWriter(new OutputStreamWriter(out,enc)),level);
    }

    /**
     * Sets indent string. The indent string is written n times at the
     * beginning of each line where n is the current indetation level.
     *
     * @param s the indent string.
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

    /**
     * Terminates the current line and increments the indentation level.
     */
    public void indent() {
        indent(true);
    }

    /**
     * Terminates the current line and decrements the indentation level.
     */
    public void unindent() {
        unindent(true);
    }

    /**
     * Increments the indentation level. If the <code>newLine</code> parameter
     * is <code>true</code> and the current line is not empty, terminates
     * the current line.
     *
     * @param newLine <code>true</code> to terminate the current line.
     */
    public void indent(boolean newLine) {
        synchronized (lock) {
            if (newLine && !blank) println();
            indentLevel++;
        }
    }

    /**
     * Decrements the indentation level. If the <code>newLine</code> parameter
     * is <code>true</code> and the current line is not empty, terminates
     * the current line.
     *
     * @param newLine <code>true</code> to terminate the current line.
     */
    public void unindent(boolean newLine) {
        synchronized (lock) {
            if (newLine && !blank) println();
            indentLevel--;
        }
    }

    /**
     * Gets the current indentation level.
     * @return the current indentation level.
     */
    public int getIndentLevel() {
        return indentLevel;
    }

    /**
     * Assigns the current indentation level.
     * @param level the new indentation level
     * @deprecated you should not set the indentation level directly.
     *  Use the indent() and unindent() methods instead.
     */
    public void setIndentLevel(int level) {
        indentLevel = level;
    }

    //=======================================================================
    //  These methods make sure that we keep indentation even if someone is
    //  using this class as a PrintWriter. We make an assumtion that people
    //  terminate the line by calling println(). We don't handle end-of-line
    //  characters embedded in strings
    //=======================================================================

    /**
     * Writes a portion of an array of characters.
     *
     * @param buf the characters to write
     * @param off offset of the first chatacter to write
     * @param len number of characters to write
     */
    public void write(char buf[], int off, int len) {
        checkIndent();
        super.write(buf, off, len);
    }

    /**
     * Writes a portion of a string.
     *
     * @param s the string to write
     * @param off offset of the first chatacter to write
     * @param len number of characters to write
     */
    public void write(String s, int off, int len) {
        checkIndent();
        super.write(s, off, len);
    }

    /**
     * Terminate the current line by writing the line separator string.
     * The line separator string is defined by the system property
     * <code>line.separator</code>, and is not necessarily a single newline
     * character (<code>'\n'</code>).
     */
    public void println() {
        synchronized (lock) {
            super.println();
            flush();
            blank = true;
        }
    }

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
