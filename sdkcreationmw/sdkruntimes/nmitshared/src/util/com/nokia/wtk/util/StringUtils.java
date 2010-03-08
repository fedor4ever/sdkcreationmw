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
import java.io.StringWriter;

/* java.util */
import java.util.List;
import java.util.BitSet;
import java.util.ArrayList;
import java.util.StringTokenizer;

/* java.text */
import java.text.MessageFormat;

/**
 * Collection of static utilities related to Strings.
 *
 */
public abstract class StringUtils {

    public final static String NULL = "null";

    /** Default constructor is suppressed, ensuring non-instantiability. */
    private StringUtils() {
        throw new InternalError("StringUtils is a container of static methods");
    }

    /** A table of hex digits */
    private static final char[] hexDigit = {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    /** Bytes that don't need to be URL encoded */
    private static BitSet dontNeedEncoding;
    static {
        dontNeedEncoding = new BitSet(256);
        int i;
        for (i='a'; i<='z'; i++) dontNeedEncoding.set(i);
        for (i='A'; i<='Z'; i++) dontNeedEncoding.set(i);
        for (i='0'; i<='9'; i++) dontNeedEncoding.set(i);
        dontNeedEncoding.set('-');
        dontNeedEncoding.set('.');
        dontNeedEncoding.set('*');
    }

    /**
     * URL-encodes the string using default (utf-8) encoding.
     *
     * @param   s   <code>String</code> to be encoded.
     *
     * @return  the encoded <code>String</code>, or <code>null</code>
     *          if the characters to bytes convertion failed.
     */
    public static String urlEncode(String s) {
        return urlEncode(s, "utf-8");
    }

    /**
     * URL-encodes the string.
     *
     * @param   s   <code>String</code> to be encoded.
     * @param   enc the character encoding to use for converting characters
     *              into bytes
     *
     * @return  the encoded <code>String</code>, or <code>null</code>
     *          if the characters to bytes convertion failed.
     */
    public static String urlEncode(String s, String enc) {

        // encode the string
        byte [] bytes;
        try {
            bytes = s.getBytes(enc);
        } catch (Exception x) {
            Trace.printStackTrace(x);
            return null;
        }

        // the string buffer will be allocated on demand when we encounter
        // the first character that needs to be encoded
        StringBuffer out = null;
        for (int i=0; i<bytes.length; i++) {
            byte b = bytes[i];
            if (dontNeedEncoding.get(((int)b) & 0xff)) {
                if (out != null) {
                    out.append((char)b);
                }
            } else {
                if (out == null) {
                    // allocate the output buffer and copy the beginning
                    // of the string
                    out = new StringBuffer(bytes.length + 2);
                    for (int j=0; j<i; j++) {
                        out.append((char)(bytes[j]));
                    }
                }
                out.append('%');
                toHexString(out, b);
            }
        }

        // if the string buffer is null, then the string is good as is
        return ((out == null) ? s : out.toString());
    }

    /**
     * Converts a nibble to a hex character
     * @param	nibble	the nibble to convert.
     * @return the hex character
     */
    private static char toHex(int nibble) {
        return hexDigit[(nibble & 0xF)];
    }

    /**
     * Returns the string representation of a byte.
     * @param b the byte
     * @return the string representation of a byte.
     */
    public static String toHexString(byte b) {
        return toHexString(null, b).toString();
    }

    /**
     * Returns the string representation of a byte array.
     * @param b the byte array
     * @return the string representation of a byte array.
     */
    public static String toHexString(byte [] b) {
        return toString(b, null, " ", null);
    }

    /**
     * Appends the string representation of a byte to the string buffer.
     * @param sb the StringBuffer
     * @param b the byte
     * @return the string buffer
     */
    public static StringBuffer toHexString(StringBuffer sb, byte b) {
        if (sb == null) sb = new StringBuffer(2);
        sb.append(toHex(b >> 4));   // upper nibble
        sb.append(toHex(b     ));   // lower nibble
        return sb;
    }

    /**
     * Returns the string representation of a byte array.
     * @param bytes the byte array
     * @return the string representation of a byte array.
     */
    public static String toString(byte[] bytes) {
        return toString(bytes, "[", ",", "]");
    }

    /**
     * Returns the string representation of a byte array.
     * @param bytes the byte array
     * @param beg string to begin the string with
     * @param sep string to separate bytes
     * @param end string to terminate the string with
     * @return the string representation of a byte array.
     */
    public static String toString(byte[] bytes,
                                  String beg, String sep, String end) {
        return toString(null, bytes, beg, sep, end).toString();
    }

    /**
     * Returns the string representation of a byte array.
     * @param sb the StringBuffer
     * @param bytes the byte array
     * @param begin string to begin the string with
     * @param sep string to separate bytes from each other
     * @param end string to terminate the string with
     * @return the string representation of a byte array.
     */
    public static StringBuffer toString(StringBuffer sb, byte[] bytes,
                                        String begin, String sep, String end) {
        if (bytes != null) {
            if (sb == null) {
                // allocate string buffer of exact size to avoid reallocation
                int len = bytes.length;
                int n1 = ((begin == null) ? 0 : begin.length());
                int n2 = ((sep == null)   ? 0 : sep.length());
                int n3 = ((end == null)   ? 0 : end.length());
                sb = new StringBuffer(2*len + n2*(len-1) + n1 + n3);
            }
            if (begin != null) sb.append(begin);
            for (int i = 0; i < bytes.length; i++) {
                if (i > 0 && sep != null) sb.append(sep);
                toHexString(sb, bytes[i]);
            }
            if (end != null) sb.append(end);
        } else {
            if (sb == null) {
                sb = new StringBuffer(NULL);
            } else {
                sb.append(NULL);
            }
        }
        return sb;
    }

    /**
     * Converts array of objects into a comma separated string enclosed
     * into curly brackets.
     */
    public static String toString(Object [] array) {
        return toString(null, array).toString();
    }

    /**
     * Converts array of objects into a comma separated string enclosed
     * into curly brackets and appends it to a StringBuffer.
     *
     * @param sb the destination buffer
     * @param array array of objects
     */
    public static StringBuffer toString(StringBuffer sb, Object [] array) {
        return toString(sb, array, "{", ",", "}");
    }

    /**
     * Converts array of objects into a string enclosed into curly brackets.
     *
     * @param array array of objects
     * @param sep string to separate bytes from each other
     */
    public static String toString(Object [] array, String sep) {
        return toString(array, "{", sep, "}");
    }

    /**
     * Converts array of objects into a comma separated string enclosed
     * into curly brackets.
     *
     * @param array array of objects
     * @param begin string to begin the string with
     * @param end string to terminate the string with
     */
    public static String toString(Object [] array, String begin, String end) {
        return toString(array, begin, ",", end);
    }

    /**
     * Converts array of objects into a string.
     *
     * @param array array of objects
     * @param begin string to begin the string with
     * @param sep string to separate bytes from each other
     * @param end string to terminate the string with
     */
    public static String toString(Object [] array, String begin, String sep,
                                  String end) {
        return toString(null, array, begin, sep, end).toString();
    }

    /**
     * Converts array of objects into a string and appends it to
     * a StringBuffer
     *
     * @param array array of objects
     * @param begin string to begin the string with
     * @param sep string to separate bytes from each other
     * @param end string to terminate the string with
     */
    public static StringBuffer toString(StringBuffer sb, Object [] array,
                                        String begin, String sep, String end) {
        if ( array != null ) {
            if (sb == null) sb = new StringBuffer(array.length*5+2); // guess
            if (begin != null) sb.append(begin);
            for (int i=0; i<array.length; i++) {
                if (i > 0 && sep != null) sb.append(sep);
                if (array[i] == null) {
                    sb.append(NULL);
                } else {
                    sb.append(array[i].toString());
                }
            }
            if (end != null) sb.append(end);
        } else {
            if (sb == null) {
                sb = new StringBuffer(NULL);
            } else {
                sb.append(NULL);
            }
        }
        return sb;
    }

    /**
     * Returns the human-readable representation of a string value.
     * @param s a string to encode
     */
    public static String getDisplayableString(String s) {
        if (s != null) {
            StringBuffer sbuf = new StringBuffer(s.length());
            sbuf.append('\"');
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                if (c == '\0') {                // Null character
                    sbuf.append("\0");
                } else if (c == '\n') {         // Newline
                    sbuf.append("\\n");
                } else if (c == '\r') {         // Carriage return
                    sbuf.append("\\r");
                } else if (c == '\t') {         // Horizontal tab
                    sbuf.append("\\t");
                } else if (c == '\"') {         // Double quote
                    sbuf.append("\\\"");
                } else if (c == '\'') {         // Single quote
                    sbuf.append("\\\'");
                } else if (c == '\\') {         // Literal backslash
                    sbuf.append("\\\\");
                } else if ((((int)c & 0xFFFF) < 128) && !Character.isISOControl(c)) {
                    sbuf.append(c);     // Normal 8-bit printable character
                } else if (((int)c & 0xFFFF) < 256) { // 8-bit Unicode (US-ASCII)
                    sbuf.append("\\x");
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                } else {                            // 16-bit Unicode
                    sbuf.append("\\u");
                    sbuf.append(Integer.toString(((c >> 12) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  8) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                }
            }
            sbuf.append('\"');
            return sbuf.toString();
        } else {
            return NULL;
        }
    }

    /**
     * Returns the human-readable representation of a string value.
     * Escapes markups so that result is embeddable within an HTML string
     * @param s a string to encode
     */
    public static String getDisplayableHtmlString(String s) {
        if (s != null) {
            StringBuffer sbuf = new StringBuffer(s.length());
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                if (c == '\0') {                // Null character
                    sbuf.append("\0");
                } else if (c == '\n') {         // Newline
                    sbuf.append("\\n");
                } else if (c == '\r') {         // Carriage return
                    sbuf.append("\\r");
                } else if (c == '\t') {         // Horizontal tab
                    sbuf.append("\\t");
                } else if (c == '\"') {         // Double quote
                    sbuf.append("\\\"");
                } else if (c == '\'') {         // Single quote
                    sbuf.append("\\\'");
                } else if (c == '<') {          // Less than
                    sbuf.append("&lt;");
                } else if (c == '>') {          // Greater than
                    sbuf.append("&gt;");
                } else if (c == '\\') {         // Literal backslash
                    sbuf.append("\\\\");
                } else if ((((int)c & 0xFFFF) < 256) && !Character.isISOControl(c)) {
                    sbuf.append(c);     // Normal 8-bit printable character
                } else if (((int)c & 0xFFFF) < 256) { // 8-bit Unicode (US-ASCII)
                    sbuf.append("\\x");
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                } else {                            // 16-bit Unicode
                    sbuf.append("\\u");
                    sbuf.append(Integer.toString(((c >> 12) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  8) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                }
            }
            return sbuf.toString();
        } else {
            return NULL;
        }
    }

    /**
     * Bare-bones version of getDisplayableHtmlString() above.
     * Prepare a string for a HTML-aware text display.
     * Useful for error strings generated by parsers
     *
     * @param s original string
     * @return  String with '<', '>', and '&' replaced with HTML escape
     *          sequence, while leaving in all other chars.
     */
    public static String htmlEscape(String s) {
        if (s == null) return null;
        int len = s.length();
        StringBuffer sb = new StringBuffer(len);
        for (int i=0; i<len; i++) {
            char c = s.charAt(i);
            switch (c) {
            case '<':   sb.append("&lt;");  break;
            case '>':   sb.append("&gt;");  break;
            case '&':   sb.append("&amp;"); break;
            default:    sb.append(c);       break;
            }
        }
        return sb.toString();
    }

    /**
     * Creates a StringBuffer containing first n characters of the specified
     * string.
     */
    private static StringBuffer createStringBuffer(String s, int n) {
        StringBuffer sb = new StringBuffer(n);
        for (int i=0; i<n; i++) {
            sb.append(s.charAt(i));
        }
        return sb;
    }

    /**
     * Converts a string into XML-compliant entity-encoded version.
     * @param s a string to encode
     * @return the encoded string
     */
    public static String xmlize(String s) {
        StringBuffer sbuf = null;
        if ((s != null) && (s.length() > 0)) {
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                if (c == '&') {                 // Ampersand
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&amp;");
                } else if (c == '<') {          // Less than
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&lt;");
                } else if (c == '>') {          // Greater than
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&gt;");
                } else if (c == '\"') {         // Double quote
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&quot;");
                } else if (c == '\'') {         // Single quote
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&apos;");
                } else if ((((int)c & 0xFFFF) < 128) && !Character.isISOControl(c)) {
                    // Normal 8-bit printable character
                    if (sbuf != null) sbuf.append(c);
                } else if (((int)c & 0xFFFF) < 256) {

                    // 8-bit Unicode (US-ASCII)
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&#x");
                    sbuf.append(toHex(c >>  4));
                    sbuf.append(toHex(c      ));
                    sbuf.append(";");
                } else {

                    // 16-bit Unicode
                    if (sbuf == null) sbuf = createStringBuffer(s, i);
                    sbuf.append("&#x");
                    sbuf.append(toHex(c >> 12));
                    sbuf.append(toHex(c >>  8));
                    sbuf.append(toHex(c >>  4));
                    sbuf.append(toHex(c      ));
                    sbuf.append(";");
                }
            }
        }
        return ((sbuf == null) ? s : sbuf.toString());
    }

    /**
     * Returns a new string resulting from replacing all occurrences
     * of <code>ch</code> character in <code>str</code> string with
     * <code>rep</code> string.
     * <p>
     * If the character <code>ch</code> does not occur in the character
     * sequence represented by <code>str</code> string, or if <code>str</code>
     * is <code>null</code>, then a reference to <code>str</code> (or
     * <code>null</code>) is returned. Otherwise, a new <code>String</code>
     * object is created that represents a character sequence identical to
     * the character sequence represented by <code>str</code> string, except
     * that every occurrence of <code>ch</code> is replaced by <code>rep</code>
     * string.
     *
     * @param str the original string
     * @param ch  the character to replace
     * @param rep the string to replace character <code>ch</code> with;
     *            <code>null</code> or empty string to remove the characters
     *
     * @return  a string derived from <code>str</code> by replacing every
     *          occurrence of <code>ch</code> with <code>rep</code>.
     */
    public static String replace(String str, char ch, String rep) {

        // allow null replacement string
        if (rep == null) rep = "";
        int repLen = rep.length();

        // check if replace operation is a NOP
        if ((repLen == 1 && rep.charAt(0) == ch) ||
            str == null || str.length()==0) {
            return str;
        }

        int len = str.length();
        StringBuffer sb = null;
        int nextChar = 0;

        // do not allocate new string unless we found the first character
        // to replace
        for (int i=0; i<len; i++) {
            if (str.charAt(i) == ch) {
                if (sb == null) sb = new StringBuffer(len + repLen);
                if (i > nextChar) sb.append(str.substring(nextChar,i));
                if (repLen > 0) sb.append(rep);
                nextChar = i+1;
            }
        }

        // if we have replaced at least one char, nextChar must be positive
        if (nextChar > 0) {
            if (nextChar < len) {
                sb.append(str.substring(nextChar,len));
            }
            return sb.toString();
        }

        // no occurrences
        return str;
    }

    /**
     * Returns a new string resulting from replacing all characters
     * from <code>chars</code> string in <code>str</code> string with
     * <code>rep</code> string.
     *
     * @param str the original string
     * @param chars the characters to replace
     * @param rep the string to replace characters with; <code>null</code>
     *            or empty string to remove the characters
     *
     * @return  a string derived from <code>str</code> by replacing every
     *          occurrence of the <code>chars</code> characters with
     *          <code>rep</code>.
     */
    public static String replaceChars(String str, String chars, String rep) {

        // allow null replacement string
        if (rep == null) rep = "";
        int repLen = rep.length();

        // check if replace operation is a NOP
        if (chars.length() == 0 || str == null || str.length()==0) {
            return str;
        }

        int len = str.length();
        StringBuffer sb = null;
        int nextChar = 0;

        // do not allocate new string unless we found the first character
        // to replace
        for (int i=0; i<len; i++) {
            if (chars.indexOf(str.charAt(i)) >= 0) {
                if (sb == null) sb = new StringBuffer(len + repLen);
                if (i > nextChar) sb.append(str.substring(nextChar,i));
                if (repLen > 0) sb.append(rep);
                nextChar = i+1;
            }
        }

        // if we have replaced at least one char, nextChar must be positive
        if (nextChar > 0) {
            if (nextChar < len) {
                sb.append(str.substring(nextChar,len));
            }
            return sb.toString();
        }

        // no occurrences
        return str;
    }

    /**
     * Returns a new string resulting from removing all characters
     * except for those that occur in the <code>chars</code> string
     * from <code>str</code>.
     * <p>
     * If the characters <code>chars</code> do not occur in the character
     * sequence represented by <code>str</code> string, or if <code>str</code>
     * is <code>null</code>, then a reference to <code>str</code> (or
     * <code>null</code>) is returned. Otherwise, a new <code>String</code>
     * object is created that represents a character sequence identical to
     * the character sequence represented by <code>str</code> string, except
     * that every character that does not occur in <code>chars</code> is
     * removed.
     *
     * @param str the original string
     * @param chars  the characters to retain
     *
     * @return  a string derived from <code>str</code> by removing all
     * characters except for those that occur in the <code>chars</code>.
     */
    public static String retain(String str, String chars) {

        // check if retain operation is a NOP
        if (str == null) return null;
        if (chars == null || chars.length() == 0) return str;

        // do not allocate new string until we find the first character
        // to remove
        StringBuffer sb = null;
        int len = str.length();
        for (int i=0; i<len; i++) {
            char c = str.charAt(i);
            if (chars.indexOf(c) < 0) {
                if (sb == null) {
                    sb = new StringBuffer(len - 1);
                    for (int j=0; j<i; j++) {
                        sb.append(str.charAt(j));
                    }
                }
            } else if (sb != null) {
                sb.append(c);
            }
        }

        return (sb == null) ? str : sb.toString();
    }

    /**
     * Initialize bitset for {@link isPrintable()}
     */
    private static synchronized void initPrintChars() {
        if (printChars == null) {
            printChars = new BitSet(128);
            for (int i=0; i<PRINTABLE.length(); i++) {
                printChars.set(PRINTABLE.charAt(i));
            }
        }
    }

    private static BitSet printChars = null;
    private static final String PRINTABLE =
" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    /**
     * Returns <code>true</code> if characters may be considered "printable"
     */
    public static boolean isPrintable(char ch) {
        if (!Character.isISOControl(ch)) {
            if (printChars == null) initPrintChars();
            return printChars.get(ch);
        }
        return false;
    }

    /**
     * Dumps binary data, presending raw bytes and their character equivalents.
     * The output looks like this:
     * <pre>
     * 00000000  01 04 04 00 7F E7 55 03 6F 76 65 72 76 69 65 77  ......U.overview
     * 00000010  00 36 03 4F 76 65 72 76 69 65 77 00 01 60 64 03  .6.Overview..`d.
     * 00000020  57 68 61 74 20 69 73 20 57 41 50 3F 00 01 26 26  What is WAP?..&&
     * </pre>
     *
     * @param data the data to be displayed
     */
    public static String hexDump(byte[] data) {
        if (data == null) {
            return NULL;
        } else {
            StringWriter w = new StringWriter();
            hexDump(0, new PrintWriter(w), data, 0, data.length);
            return w.toString();
        }
    }

    /**
     * Dumps binary data, presending raw bytes and their character equivalents.
     * The output looks like this:
     * <pre>
     * 00000000  01 04 04 00 7F E7 55 03 6F 76 65 72 76 69 65 77  ......U.overview
     * 00000010  00 36 03 4F 76 65 72 76 69 65 77 00 01 60 64 03  .6.Overview..`d.
     * 00000020  57 68 61 74 20 69 73 20 57 41 50 3F 00 01 26 26  What is WAP?..&&
     * </pre>
     *
     * @param w consumes the output
     * @param data the data to be displayed
     */
    public static void hexDump(PrintWriter w, byte[] data) {
        if (data != null) {
            hexDump(0, w, data, 0, data.length);
        }
    }

    /**
     * Dumps binary data, presending raw bytes and their character equivalents.
     * The output looks like this:
     * <pre>
     * 00000000  01 04 04 00 7F E7 55 03 6F 76 65 72 76 69 65 77  ......U.overview
     * 00000010  00 36 03 4F 76 65 72 76 69 65 77 00 01 60 64 03  .6.Overview..`d.
     * 00000020  57 68 61 74 20 69 73 20 57 41 50 3F 00 01 26 26  What is WAP?..&&
     * </pre>
     *
     * @param w consumes the output
     * @param data the data to be displayed
     */
    public static void hexDump(PrintWriter w, byte[] data, int off, int len) {
        if (data != null) {
            hexDump(0, w, data, off, len);
        }
    }

    /**
     * Dumps binary data, presending raw bytes and their character equivalents.
     * The output looks like this:
     * <pre>
     * 00000000  01 04 04 00 7F E7 55 03 6F 76 65 72 76 69 65 77  ......U.overview
     * 00000010  00 36 03 4F 76 65 72 76 69 65 77 00 01 60 64 03  .6.Overview..`d.
     * 00000020  57 68 61 74 20 69 73 20 57 41 50 3F 00 01 26 26  What is WAP?..&&
     * </pre>
     *
     * @param start start "offset" (the left column)
     * @param w consumes the output
     * @param data the data to be displayed
     */
    public static void hexDump(int start, PrintWriter w, byte[] data) {
        if (data != null) {
            hexDump(start, w, data, 0, data.length);
        }
    }

    /**
     * Dumps binary data, presending raw bytes and their character equivalents.
     * The output looks like this:
     * <pre>
     * 00000000  01 04 04 00 7F E7 55 03 6F 76 65 72 76 69 65 77  ......U.overview
     * 00000010  00 36 03 4F 76 65 72 76 69 65 77 00 01 60 64 03  .6.Overview..`d.
     * 00000020  57 68 61 74 20 69 73 20 57 41 50 3F 00 01 26 26  What is WAP?..&&
     * </pre>
     *
     * @param start start "offset" (the left column)
     * @param w consumes the output
     * @param data the data to be displayed
     * @param off the start offset in the data array
     * @param len number of bytes to dump.
     */
    public static
    void hexDump(int start, PrintWriter w, byte[] data, int off, int len) {
        if (data != null || len != 0) {
            int rowSize = 16;     // Number of bytes to show in a row
            int addrBits = (((off+len) < 0x10000) ? 16 :
                            ((off+len) < 0x1000000 ? 24 : 32));

            int nRows = (len + rowSize - 1) / rowSize;
            for (int row = 0; row < nRows; row++) {

                // Create row address label:
                int addr = start + row * rowSize;
                for (int i=addrBits-4; i>=0; i-=4) w.print(toHex(addr >> i));
                w.print(" ");

                // Show the bytes plus their renderable characters:
                for (int offset = 0; offset < rowSize; offset++) {
                    int index = (row * rowSize) + offset;
                    if (index < len) {
                        // Separate bytes by a single space
                        w.print(" ");
                        int b = data[off + index];
                        w.print(toHex(b >> 4));   // upper nibble
                        w.print(toHex(b     ));   // lower nibblebble
                    } else {
                        // Pad partial line with spaces
                        // so that the character version will align correctly:
                        w.print("   ");
                    }
                }
                // Add character version of row data:
                w.print("  ");
                for (int offset = 0; offset < rowSize; offset++) {
                    int index = (row * rowSize) + offset;
                    if (index < data.length) {
                        char ch = (char)data[off + index];
                        if (isPrintable(ch)) {
                            w.print(ch);  // displayable character
                        } else {
                            w.print('.');
                        }
                    } else {
                        // Pad partial line with spaces
                        // so that all lines have equal length
                        w.print(' ');
                    }
                }
                w.println();
            }
        }
    }

    /**
     * Take the given string and chop it up into a series of strings
     * on boundaries determined by the delimiter set provided.
     * @param input the string to tokenize
     * @param delim the set of delimiters to use.
     *              If null or empty, then default to whitespace.
     * @param emptyok if <code>true</code>, the allow empty tokens
     *                between each delimiter character.
     * @return the resulting array of string tokens, even if of zero length.
     */
    public static String[] tokenize(String input, String delim, boolean emptyok) {
        if ((delim == null) || (delim.length() == 0)) {
            delim = " \t\n\r\f";
        }
        StringTokenizer t = new StringTokenizer((input == null) ? "" : input, delim, emptyok);
        List v = new ArrayList();
        if (emptyok) {
            // Leading or adjacent delimiter characters generate empty tokens:
            for (boolean expectToken = true; t.hasMoreTokens();) {
                String token = t.nextToken();
                if (delim.indexOf(token) >= 0) {
                    // Delimiter token:
                    if (expectToken) {
                        v.add("");
                        expectToken = false;
                    } else {
                        expectToken = true;
                    }
                } else {
                    // Normal token:
                    v.add(token);
                    expectToken = false;
                }
            }
        } else {
            // Regular mode: delimiter characters only delimit tokens:
            while (t.hasMoreTokens()) {
                v.add(t.nextToken());
            }
        }
        return (String[])v.toArray(new String[v.size()]);
    }

    /**
     * Take the given string and chop it up into a series of strings
     * on boundaries determined by the delimiter set provided.
     * @param input the string to tokenize
     * @param delim the set of delimiters to use.
     *              If null or empty, then default to whitespace.
     * @return the resulting array of string tokens, even if of zero length.
     */
    public static String[] tokenize(String input, String delim) {
        return tokenize(input, delim, false);
    }

    /**
     * Formats a message with one parameter.
     *
     * @param format    the format string
     * @param p1        the parameter
     *
     * @return          the formatted message
     * @see MessageFormat
     */
    public static String format(String format, Object p1) {
        Object [] params = new Object [] { p1 };
        try { return MessageFormat.format(format, params); }
        catch (Throwable x) {
            Trace.printStackTrace(x);
            return "????";
        }
    }

    /**
     * Formats a message with one integer parameter.
     *
     * @param format    the format string
     * @param p1        the parameter
     *
     * @return          the formatted message
     * @see MessageFormat
     */
    public static String format(String format, int p1) {
        Object [] params = new Object [] { new Integer(p1) };
        try { return MessageFormat.format(format, params); }
        catch (Throwable x) {
            Trace.printStackTrace(x);
            return "????";
        }
    }

    /**
     * Formats a message with two parameters.
     *
     * @param format    the format string
     * @param p1        the first parameter
     * @param p2        the second parameter
     *
     * @return          the formatted message
     * @see MessageFormat
     */
    public static String format(String format, Object p1, Object p2) {
        Object [] params = new Object [] { p1, p2 };
        try { return MessageFormat.format(format, params); }
        catch (Throwable x) {
            Trace.printStackTrace(x);
            return "????";
        }
    }

    /**
     * Formats a message with two integer parameters.
     *
     * @param format    the format string
     * @param p1        the first parameter
     * @param p2        the second parameter
     *
     * @return          the formatted message
     * @see MessageFormat
     */
    public static String format(String format, int p1, int p2) {
        Object [] params = new Object [] { new Integer(p1), new Integer(p2) };
        try { return MessageFormat.format(format, params); }
        catch (Throwable x) {
            Trace.printStackTrace(x);
            return "????";
        }
    }
}
