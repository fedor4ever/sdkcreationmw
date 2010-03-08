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



package com.nokia.wtk.util.wbxml;

/**
 * A WML String encoder/decoder.
 */
class WMLStringEncoder {
    /**
     * Convert a string to WML-compliant entity-encoded version.
     * @param s a string to encode
     */
    public static String encodeEntities(String s) {
        StringBuffer sbuf = new StringBuffer();
        if ((s != null) && (s.length() > 0)) {
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                if (c == '\u00A0') {            // Non-breaking space
                    sbuf.append("&nbsp;");
                } else if (c == '\u00AD') {     // Soft Hyphen
                    sbuf.append("&shy;");
                } else if (c == '\"') {         // Double quote
                    sbuf.append("&quot;");
                } else if (c == '\'') {         // Single quote
                    sbuf.append("&apos;");
                } else if (c == '<') {          // Less than
                    sbuf.append("&lt;");
                } else if (c == '>') {          // Greater than
                    sbuf.append("&gt;");
                } else if (c == '&') {          // Ampersand
                    sbuf.append("&amp;");
                } else if (((int)c & 0xFFFF) < 0x0100) {    // 8-bit Unicode (Latin Basic,Supl)
                    if (!Character.isISOControl(c)) {
                        sbuf.append(c);     // Normal 8-bit printable character
                    } else {
                        sbuf.append("&#x");
                        sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                        sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                        sbuf.append(";");
                    }
                } else {                                    // 16-bit Unicode
                    sbuf.append("&#x");
                    sbuf.append(Integer.toString(((c >> 12) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  8) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                    sbuf.append(";");
                }
            }
        }
        return sbuf.toString();
    }

    /**
     * Convert a WML-compliant entity-encoded string to a raw string version.
     * @param s a string to decode
     */
    public static String decodeEntities(String s) {
        StringBuffer sbuf = new StringBuffer();
        while ((s != null) && (s.length() > 0)) {
            int amp = s.indexOf('&');
            if (amp >= 0) {
                if (amp > 0) {
                    sbuf.append(s.substring(0, amp));
                    s = s.substring(amp);
                }
                if (s.startsWith("&amp;")) {
                    sbuf.append('&');
                    s = s.substring(5);
                } else if (s.startsWith("&lt;")) {
                    sbuf.append('<');
                    s = s.substring(4);
                } else if (s.startsWith("&gt;")) {
                    sbuf.append('>');
                    s = s.substring(4);
                } else if (s.startsWith("&quot;")) {
                    sbuf.append('\"');
                    s = s.substring(6);
                } else if (s.startsWith("&apos;")) {
                    sbuf.append('\'');
                    s = s.substring(6);
                } else if (s.startsWith("&nbsp;")) {
                    sbuf.append('\u00A0');
                    s = s.substring(6);
                } else if (s.startsWith("&shy;")) {
                    sbuf.append('\u00AD');
                    s = s.substring(5);
                } else {
                    int semicolon = s.indexOf(';');
                    if (semicolon >= 0) {
                        String entity = s.substring(1, semicolon);
                        if (entity.startsWith("#x") || entity.startsWith("#X")) {
                            try {
                                sbuf.append((char)Integer.parseInt(entity.substring(2), 16));
                            } catch (NumberFormatException ex) {
                                // Unknown entity syntax
                                sbuf.append(entity);
                            }
                        } else if (entity.startsWith("#")) {
                            try {
                                sbuf.append((char)Integer.parseInt(entity.substring(1)));
                            } catch (NumberFormatException ex) {
                                // Unknown entity syntax
                                sbuf.append(entity);
                            }
                        } else {
                            // Unknown entity syntax.
                            // Treat entity string as literal string value:
                            sbuf.append(entity);
                        }
                        s = s.substring(semicolon+1);
                    } else {
                        // missing semicolon delimiter
                        //TODO: Any recovery ideas?
                        sbuf.append(s);
                        s = null;
                    }
                }
            } else {
                // No escaped entities; pass thru unaltered:
                sbuf.append(s);
                s = null;
            }
        }
        return sbuf.toString();
    }

}   // End of WMLStringEncoder

