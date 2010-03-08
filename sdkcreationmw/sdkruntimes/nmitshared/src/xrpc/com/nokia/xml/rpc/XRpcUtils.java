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
 * $Id: XRpcUtils.java,v 1.5 2005/10/26 02:22:46 slava Exp $
 */

// ========================================================================
//  Name        : XRpcUtils.java
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
import java.io.IOException;
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

/**
 * A set of static XRPC utilities
 */
public final class XRpcUtils {

    /**
     * Disallow instantiation
     */
    private XRpcUtils() {
        throw new InternalError("XRpcUtils cannot be instantiated");
    }

    /** A table of hex digits */
    private static final char[] hexDigit = {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    /**
     * Convert a nibble to a hex character
     * @param	nibble	the nibble to convert.
     */
    private static char toHex(int nibble) {
        return hexDigit[(nibble & 0xF)];
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
     * Tests if two objects "equal to" each other. The objects are considered
     * to be equal if either of the following conditions holds true:
     * <ul>
     * <li>both objects are <code>null</code>.
     * <li>the objects are identical to each other (<code>o1 == o2</code>)
     * <li><code>o1</code> is not <code>null</code> and
     *     <code>o1.equals(o2)</code> returns <code>true</code>.
     * <li><code>o2</code> is not <code>null</code> and
     *     <code>o2.equals(o1)</code> returns <code>true</code>.
     * </ul>
     * Otherwise, this function returns <code>false</code>.
     */
    public static boolean equals(Object o1, Object o2) {
        boolean equal = false;
        if (o1 == o2) {
            equal = true;
        } else if (o1 != null) {
            equal = o1.equals(o2);
        } else if (o2 != null) {
            equal = o2.equals(o1);
        }
        return equal;
    }

    //=======================================================================
    //          S E A R C H I N G
    //=======================================================================

    /**
     * Fins a data element by name.
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return data element with the given name, <code>null</code> if none
     */
    public static XRpcDataElement elementByName(XRpcDataElement[] e,
                                                String name) {
        if (e != null) {
            int n = e.length;
            for (int i=0; i<n; i++) {
                XRpcDataElement elem = e[i];
                String elementName = elem.getAttributeValue(XRpcAttr.NAME);
                if (name.equals(elementName)) {
                    return elem;
                }
            }
        }
        return null;
    }

    /**
     * Fins a data element of specified type by name.
     *
     * @param e array of elements to search
     * @param type the type of the element to find
     * @param name the name of the element to find
     * @return data element with the given name and type,
     * 	       <code>null</code> if none
     */
    public static XRpcDataElement elementByName(XRpcDataElement[] e,
                                                XRpcDataType type,
                                                String name) {
        if (e != null) {
            int n = e.length;
            for (int i=0; i<n; i++) {
                XRpcDataElement elem = e[i];
                String elementName = elem.getAttributeValue(XRpcAttr.NAME);
                if (name.equals(elementName) && elem.getDataType() == type) {
                    return elem;
                }
            }
        }
        return null;
    }

    /**
     * Finds <em>double</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>double</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcDouble findDoubleElementByName(XRpcDataElement[] e,
                                                     String name) {
        return (XRpcDouble)elementByName(e, XRpcDataType.DOUBLE, name);
    }

    /**
     * Finds <em>long</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>long</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcLong findLongElementByName(XRpcDataElement[] e,
                                                 String name) {
        return (XRpcLong)elementByName(e, XRpcDataType.LONG, name);
    }

    /**
     * Finds <em>int</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>int</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcInt findIntElementByName(XRpcDataElement[] e,
                                               String name) {
        return (XRpcInt)elementByName(e, XRpcDataType.INT, name);
    }

    /**
     * Finds <em>short</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>short</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcShort findShortElementByName(XRpcDataElement[] e,
                                                   String name) {
        return (XRpcShort)elementByName(e, XRpcDataType.SHORT, name);
    }

    /**
     * Finds <em>byte</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>byte</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcByte findByteElementByName(XRpcDataElement[] e,
                                                 String name) {
        return (XRpcByte)elementByName(e, XRpcDataType.BYTE, name);
    }

    /**
     * Finds <em>boolean</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>boolean</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcBoolean findBooleanElementByName(XRpcDataElement[] e,
                                                       String name) {
        return (XRpcBoolean)elementByName(e, XRpcDataType.BOOLEAN, name);
    }

    /**
     * Finds <em>string</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>string</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcString findStringElementByName(XRpcDataElement[] e,
                                                     String name) {
        return (XRpcString)elementByName(e, XRpcDataType.STRING, name);
    }

    /**
     * Finds binary data element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return binary data element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcBinaryData findBinaryElementByName(XRpcDataElement[] e,
                                                         String name) {
        return (XRpcBinaryData)elementByName(e, XRpcDataType.BINARY, name);
    }

    /**
     * Finds <em>struct</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>struct</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcDataStruct findStructByName(XRpcDataElement[] e,
                                                  String name) {
        return (XRpcDataStruct)elementByName(e, XRpcDataType.STRUCT, name);
    }

    /**
     * Finds <em>array</em> element by name
     *
     * @param e array of elements to search
     * @param name the name of the element to find
     * @return <em>array</em> element with the specified name,
     * <code>null</code> if none
     */
    public static XRpcDataArray findArrayByName(XRpcDataElement[] e,
                                                String name) {
        return (XRpcDataArray)elementByName(e, XRpcDataType.ARRAY, name);
    }

    //=======================================================================
    //          X M L I Z A T I O N
    //=======================================================================

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
     * Formats object attributes to XML stream.
     */
    public static void writeAttrbutes(XRpcElement e, PrintWriter w) {
        if (e != null) {
            int n = e.getAttributeCount();
            for (int i=0; i<n; i++) {
                XRpcAttr attr = e.getAttribute(i);
                String value = e.getAttributeValue(attr);
                if (value != null) {
                    w.print(' ');
                    w.print(attr);
                    w.print("=\"");
                    w.print(xmlize(value));
                    w.print('"');
                }
            }
        }
    }

    //=======================================================================
    //          B A S E 6 4
    //=======================================================================

    /**
     * This character array provides the character to value map
     * based on RFC1521.
     */
    private static final char base64EncodeMap[] = {
    //   0   1   2   3   4   5   6   7
        'A','B','C','D','E','F','G','H', // 0
        'I','J','K','L','M','N','O','P', // 1
        'Q','R','S','T','U','V','W','X', // 2
        'Y','Z','a','b','c','d','e','f', // 3
        'g','h','i','j','k','l','m','n', // 4
        'o','p','q','r','s','t','u','v', // 5
        'w','x','y','z','0','1','2','3', // 6
        '4','5','6','7','8','9','+','/'  // 7
    };

    private static final byte base64DecodeMap[] = new byte[256];

    static {
        int i;
        for (i=0; i<255; i++) base64DecodeMap[i] = -1;
        for (i=0; i<base64EncodeMap.length; i++) {
            base64DecodeMap[base64EncodeMap[i]] = (byte) i;
        }
    }

    /**
     * Fully reads a chunk of data from an IO stream.
     * Returns <code>true</code> if the chunk of data has been read, or
     * <code>false</code> if end of file has been reached.
     *
     * This code does not handle empty characters in the middle of
     * BASE64 stream which is OK for the purposes of XRPC.
     *
     * @exception IOException of end of file has been reached before
     * the chunk od data has been fully read
     */
    private static boolean base64ReadChunk(InputStream in, byte [] chunk)
    throws IOException {
        int count = in.read(chunk);
        if (count < 0) {
            return false;  // normal end of file condition
        } else if (count == chunk.length) {
            return true;   // got the whole chunk in one shot
        } else {
            throw new IOException("BASE64Decoder: Not enough bytes for an atom.");
        }
    }

    /**
     * This implements a BASE64 character decoder as specified
     * in RFC1521.
     *
     * @return number of bytes written to the output stream
     */
    public static int decodeBASE64(InputStream in, OutputStream out)
    throws IOException
    {
        int written = 0;
        byte [] chunk = new byte[4];

        while (base64ReadChunk(in, chunk)) {
            byte a = -1, b = -1, c = -1, d = -1;
            int rem = ((chunk[3] == '=') ? ((chunk[2] == '=') ? 2 : 3) : 4);
            try {
                switch (rem) {
                case 4:
                    d = base64DecodeMap[chunk[3]];
                    // NOBREAK
                case 3:
                    c = base64DecodeMap[chunk[2]];
                    // NOBREAK
                case 2:
                    b = base64DecodeMap[chunk[1]];
                    a = base64DecodeMap[chunk[0]];
                    break;
                }
            } catch (ArrayIndexOutOfBoundsException x) {
                throw new IOException("BASE64Decoder: invalid atom: 0x" +
                                      Integer.toHexString(a) + ", 0x" +
                                      Integer.toHexString(b) + ", 0x" +
                                      Integer.toHexString(c) + ", 0x" +
                                      Integer.toHexString(d));
            }

            switch (rem) {
            case 2:
                out.write((byte)(((a << 2) & 0xfc) | ((b >>> 4) & 3)) );
                break;
            case 3:
                out.write((byte)(((a << 2) & 0xfc) | ((b >>> 4) & 3)) );
                out.write((byte)(((b << 4) & 0xf0) | ((c >>> 2) & 0xf)));
                break;
            case 4:
                out.write((byte)(((a << 2) & 0xfc) | ((b >>> 4) & 3)) );
                out.write((byte)(((b << 4) & 0xf0) | ((c >>> 2) & 0xf)));
                out.write((byte)(((c << 6) & 0xc0) | (d  & 0x3f)) );
                break;
            }
            written += rem-1;
        }
        return written;
    }

    /**
     * This implements a BASE64 character decoder as specified
     * in RFC1521.
     *
     * @return number of bytes read from the input stream
     */
    public static int encodeBASE64(InputStream in, OutputStream out)
    throws IOException {
        int len;
        int count = 0;
        byte [] chunk = new byte[3];
        while ((len = in.read(chunk)) > 0) {
            byte a, b, c;
            count += len;
            if (len == 1) {
                a = chunk[0];
                b = 0;
                c = 0;
                out.write(base64EncodeMap[(a >>> 2) & 0x3f]);
                out.write(base64EncodeMap[((a << 4) & 0x30) + ((b >>> 4) & 0xf)]);
                out.write('=');
                out.write('=');
                break;
            } else if (len == 2) {
                a = chunk[0];
                b = chunk[1];
                c = 0;
                out.write(base64EncodeMap[(a >>> 2) & 0x3f]);
                out.write(base64EncodeMap[((a << 4) & 0x30) + ((b >>> 4) & 0xf)]);
                out.write(base64EncodeMap[((b << 2) & 0x3c) + ((c >>> 6) & 0x3)]);
                out.write('=');
                break;
            } else {
                a = chunk[0];
                b = chunk[1];
                c = chunk[2];
                out.write(base64EncodeMap[(a >>> 2) & 0x3f]);
                out.write(base64EncodeMap[((a << 4) & 0x30) + ((b >>> 4) & 0xf)]);
                out.write(base64EncodeMap[((b << 2) & 0x3c) + ((c >>> 6) & 0x3)]);
                out.write(base64EncodeMap[c & 0x3F]);
                // continue reading input
            }
        }
        return count;
    }

    /**
     * Encodes binary data into a String according to BASE64 algorithm
     * as specified in RFC1521. If byte array is <code<null</code> or
     * empty, returns an empty string
     */
    public static String encodeBASE64(byte [] data) {
        if (data != null && data.length > 0) {
            try {
                int size = 4*(data.length/3);
                if (data.length%3 != 0) size += 4;
                ByteArrayOutputStream out = new ByteArrayOutputStream(size);
                encodeBASE64(new ByteArrayInputStream(data), out);
                return new String(out.toByteArray());
            } catch (IOException x) {
                // this should never happen
                x.printStackTrace();
            }
        }
        return "";
    }

    /**
     * Decodes BASE64 encoded binary data
     * @exception IOException if the input string does not conform to BASE64
     * specification
     */
    public static byte [] decodeBASE64(String text) throws IOException {
        if (text == null || text.length() == 0) {
            return new byte[0];
        } else {
            int size = text.length()*3/4;
            ByteArrayOutputStream out = new ByteArrayOutputStream(size);
            decodeBASE64(new ByteArrayInputStream(text.getBytes()), out);
            return out.toByteArray();
        }
    }

}
