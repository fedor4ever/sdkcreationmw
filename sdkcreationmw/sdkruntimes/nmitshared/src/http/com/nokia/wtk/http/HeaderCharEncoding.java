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



package com.nokia.wtk.http;

import java.io.*;
import java.nio.charset.Charset;

import com.nokia.wtk.util.BASE64;
import com.nokia.wtk.util.MimeQuotedPrintable;

/**
 * Encode/Decode header values from non-ASCII text.  The format is:
 * 
 *   =?OrigCharSet?EncodingType?EncodedText?=
 *
 * Usage:
 *
 *  These routines should be used as close to doing I/O as possible,
 *  decode when reading in/ encode when writing out message headers.   
 *  Users of header-editing UI's should never see the encoded version 
 *  of these headers.
 *
 * See MIME Part 3 RFC 2047
 */
public class HeaderCharEncoding {

    private static final String START_ENC = "=?";
    private static final String END_ENC = "?=";
    private static final String Q_ENCODING_TYPE = "?Q?";
    private static final String B_ENCODING_TYPE = "?B?";

    private static Charset gSystemCharenc = null;

    /**
     * Get system default char encoding.  
     * Instantiate a Reader and ask it what its encoding is.
     */
    private static String getSystemEncoding() {
        if (gSystemCharenc == null) {
            InputStreamReader r = new InputStreamReader(System.in);
            String name = r.getEncoding();
            try { 
                gSystemCharenc = Charset.forName(name);
            } catch (IllegalArgumentException ex) {
                gSystemCharenc = null;
            }

            if (gSystemCharenc == null) {
              try { 
                gSystemCharenc = Charset.forName("utf-8");
              } catch (IllegalArgumentException ex) { }
            }
        }
        return gSystemCharenc.name();
    }

    /**
     * Is this string already acceptable?
     * (i.e., in the ASCII printable range, excluding space character)
     */
    private static boolean isPrintable(String s) {
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if (c < 0x20 || c > 0x7E) {
                return false;
            }
        }
        return true;
    }

    /**
     * Encode input string into quoted-printable string using system default encoding.
     * @param value input string
     * @return encoded string.  Returns the input string on failure.
     */
    public static String encodeQP(String value) {
        return encodeQP(value, getSystemEncoding());
    }

    /**
     * Encode input string into quoted-printable string.  If ASCII string,
     *    pass string through unencoded.
     * @param value input string
     * @param charset character encoding of input string
     * @return encoded string.  Returns the input string on failure.
     */
    public static String encodeQP(String value, String charset) {
        if (value == null || value.length() == 0) {
            return value;
        }
        if (isPrintable(value)) {
            return value;
        }
        String text = null;
        try {
          byte[] raw = value.getBytes(charset);
          text = MimeQuotedPrintable.encode(raw);
        } catch (UnsupportedEncodingException ex) {
            System.out.println("Encoding failed: " + ex);
            return value;
        }
        return format(text, charset, Q_ENCODING_TYPE);
    }

    /**
     * Encode input string into a Base64-encoded string using system default encoding.
     * @param value input string
     * @return encoded string.  Returns the input string on failure.
     */
    public static String encodeBase64(String value) {
        return encodeBase64(value, getSystemEncoding());
    }

    /**
     * Encode input string into a Base64-encoded string.
     * @param value input string
     * @param charset character encoding of input string
     * @return encoded string.  Returns the input string on failure.
     */
    public static String encodeBase64(String value, String charset) {
        if (value == null || value.length() == 0) {
            return value;
        }
        if (isPrintable(value)) {
            return value;
        }
        String text = null;
        try {
          byte[] raw = value.getBytes(charset);
          text = BASE64.encode(raw);
        } catch (UnsupportedEncodingException ex) {
            System.out.println("Encoding failed: " + ex);
            return value;
        }
        return format(text, charset, B_ENCODING_TYPE);
    }

    /**
     * Form the text string into a MIME header value
     * per RFC 2047
     */
    private static String format(String text, String charset, String type_str) {
        StringBuffer sb = new StringBuffer();
        sb.append(START_ENC);
        sb.append(charset);
        sb.append(type_str);
        sb.append(text);
        sb.append(END_ENC);
        return sb.toString();
    }

    /**
     * Decode input bytes into a String per RFC 2047.
     * @param value input bytes
     * @return decoded string.  Returns the input as string on any failure.
     */
    public static String decode(byte[] value) {
        String noEncString = new String(value);
        int charset_start = -1;
        int enc_start = -1;
        int text_start = -1;
        int text_end = -1;

        log("Decoding " + noEncString);
        for (int i = 0; (i+1) < value.length; i++) {
            if (value[i] == '=' && value[i+1] == '?') {
                charset_start = i + 2;
                log("charset_start: " + charset_start);
                i++;  // eat second character
            } else if (value[i] == '?' && value[i+1] == '=' && text_start > 0) {
                text_end = i;
                log("text_end: " + text_end);
            } else if (charset_start > 0 && enc_start < 0 && value[i] == '?') {
                enc_start = i + 1;
                log("enc_start: " + enc_start);
            } else if (charset_start > 0 && text_start < 0 && value[i] == '?') {
                text_start = i + 1;
                log("text_start: " + text_start);
            }
        }
        if (charset_start < 0 || text_end < 0) {
            return noEncString;
        }
        String charset = new String(value, charset_start, enc_start - charset_start - 1);
        String enc = new String(value, enc_start, text_start - enc_start - 1);
        String enc_text = new String(value, text_start, text_end - text_start);
        log("Got charset=" + charset + "  enc=" + enc + " text=" + enc_text);

        byte[] dec_bytes = null;
        if (enc.equalsIgnoreCase("Q")) {
            dec_bytes = MimeQuotedPrintable.decode(enc_text);
        } else if (enc.equalsIgnoreCase("B")) {
            try {
                dec_bytes= BASE64.decode(enc_text);
                System.out.println("Decoded " + dec_bytes.length + " bytes of Base64");
            } catch (IOException ex) {
                System.out.println("Decode failed: " + ex);
            }
        } else {
            log("Illegal encoding string: " + enc);
            return noEncString;
        }

        String dec_text = null;
        try {
            dec_text = new String(dec_bytes, charset);
        } catch (UnsupportedEncodingException ex) {
            log("Unsupported encoding: " + charset);
            return new String(dec_bytes);
        }
        return dec_text;
    }

    /**
     * UNIT TEST
     */
    public static void main(String[] args) {
        
        // charset: 2
        // encoding: 13
        // text: 15
        // end: 38
        String dec_test = "=?iso-8859-1?Q?This=20is=20some=20text?=";
        String enc_test = "This is \na test \nOK?";

        String rtn = decode(dec_test.getBytes());
        log("Decoded:" + rtn);

        rtn = encodeQP(enc_test, "ISO-8859-1");
        log("Encoded QP:" + rtn);
        rtn = encodeBase64(enc_test, "ISO-8859-1");
        log("Encoded B:" + rtn);
    }

    private static void log(String s) {
        // System.out.println(s);
    }
}
