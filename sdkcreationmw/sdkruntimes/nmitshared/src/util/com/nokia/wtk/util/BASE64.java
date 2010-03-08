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
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

/**
 * This class implements a BASE64 encoder and decoder as specified
 * in RFC 1521 and 3548.
 *
 */
public abstract class BASE64 {

    /** Default constructor is suppressed, ensuring non-instantiability. */
    private BASE64() {
        throw new InternalError("BASE64 is a collection of static methods");
    }

    private static final int ENCODE_CHUNK_SIZE = 3;
    private static final int DECODE_CHUNK_SIZE = 4;

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

    /**
     * This character array provides the character to value map
     * for the "URL and filename safe" BASE64 encoding defined
     * in RFC3548.
     */
    private static final char base64SafeEncodeMap[] = {
    //   0   1   2   3   4   5   6   7
        'A','B','C','D','E','F','G','H', // 0
        'I','J','K','L','M','N','O','P', // 1
        'Q','R','S','T','U','V','W','X', // 2
        'Y','Z','a','b','c','d','e','f', // 3
        'g','h','i','j','k','l','m','n', // 4
        'o','p','q','r','s','t','u','v', // 5
        'w','x','y','z','0','1','2','3', // 6
        '4','5','6','7','8','9','-','_'  // 7
    };

    private static final byte base64DecodeMap[] = new byte[256];
    private static final byte base64SafeDecodeMap[] = new byte[256];

    static {
        int i;
        for (i=0; i<256; i++) {
            base64DecodeMap[i] = -1;
            base64SafeDecodeMap[i] = -1;
        }
        for (i=0; i<64; i++) {
            base64DecodeMap[base64EncodeMap[i]] = (byte)i;
            base64SafeDecodeMap[base64SafeEncodeMap[i]] = (byte)i;
        }
    }

    /**
     * Reads a chunk of data from an IO stream.
     * Returns the number of bytes read from the input stream.
     * Certain characters are ignored.
     *
     * @param in the stream to read
     * @param chunk the place to read the chunk to
     *
     * @return the number of bytes read from the input stream, <code>-1</code>
     *         if the end of stream has been reached
     * @throws IOException if the input stream throws it
     */
    private static int readChunk(InputStream in, byte [] chunk)
    throws IOException {
        int c = -1, count = 0, pad = -1;
        while (count < DECODE_CHUNK_SIZE && (c = in.read()) > 0) {
            byte b = (byte)c;
            switch (b) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;

            case '=':
                if (pad < 0) pad = count;
                chunk[count++] = b;
                break;

            default:
                chunk[count++] = b;
                if (pad >= 0) pad = -1;
                break;
            }
        }

        if (pad >= 0) {
            return pad;
        } else if (count > 0) {
            return count;
        } else {
            return (c < 0) ? -1 : 0;
        }
    }

    /**
     * This method implements a BASE64 decoder. It reads the input stream,
     * decodes it and writes the decoded bytes to the output stream.
     *
     * @param in the input BASE64 encoded stream
     * @param out the output stream receiving the decoded bytes
     * @param decodeMap the decoding map. If this parameter is
     *   <code>null</code> then this code automatically determines
     *   which decoding map to use.
     *
     * @return number of bytes written to the output stream
     * @throws IOException if the input stream throws it
     */
    private static int decode(InputStream in, OutputStream out, byte[] decodeMap)
    throws IOException
    {
        int written = 0;
        byte [] chunk = new byte[DECODE_CHUNK_SIZE];
        boolean selectedDecodeMap;
        boolean eos = false; // end of stream flag
        if (decodeMap == null) {
            selectedDecodeMap = false;
            decodeMap = base64DecodeMap;
        } else {
            selectedDecodeMap = true;
        }

        int count;
        while ((count = readChunk(in, chunk)) >= 0) {
            byte b0 = -1, b1 = -1, b2 = -1, b3 = -1;
            int c0 = '=', c1 = '=', c2 = '=', c3 = '=';
            switch(count) {
            case 4: c3 = (chunk[3] & 0xff); // NOBREAK
            case 3: c2 = (chunk[2] & 0xff); // NOBREAK
            case 2: c1 = (chunk[1] & 0xff); // NOBREAK
            case 1: c0 = (chunk[0] & 0xff);
            case 0: break;
            }

            if (count < DECODE_CHUNK_SIZE) {
                // we have either seen the end of stream or the atom ends
                // with one or more pad characters
                if (eos) {
                    // this has already happened at least once
                    if (count > 0) {
                        throw new IOException("BASE64: format violation");
                    }
                } else {
                    eos = true;
                }
                if (count == 0) continue;
            }

            // auto-detect the decoding map to use
            if (!selectedDecodeMap) {
                if (c0 == '-' || c0 == '_' ||
                    c1 == '-' || c1 == '_' ||
                    c2 == '-' || c2 == '_' ||
                    c3 == '-' || c3 == '_') {
                    selectedDecodeMap = true;
                    decodeMap = base64DecodeMap;
                } else {
                    if (c0 == '+' || c0 == '/' ||
                        c1 == '+' || c1 == '/' ||
                        c2 == '+' || c2 == '/' ||
                        c3 == '+' || c3 == '/') {
                        selectedDecodeMap = true;
                        decodeMap = base64SafeDecodeMap;
                    }
                }
            }

            switch (count) {
            case 4:
                if ((b3 = decodeMap[c3]) < 0) {
                    throw new IOException("BASE64: invalid character "+c3);
                }
                // NOBREAK
            case 3:
                if ((b2 = decodeMap[c2]) < 0) {
                    throw new IOException("BASE64: invalid character " + c2);
                }
                // NOBREAK
            case 2:
                if ((b1 = decodeMap[c1]) < 0) {
                    throw new IOException("BASE64: invalid character " + c1);
                }
                if ((b0 = decodeMap[c0]) < 0) {
                    throw new IOException("BASE64: invalid character " + c0);
                }
                break;
            case 1:
                throw new IOException("BASE64: invalid atom");
            }

            // +--first octet--+-second octet--+--third octet--+
            // |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
            // +-----------+---+-------+-------+---+-----------+
            // |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
            // +--0.index--+--1.index--+--2.index--+--3.index--+

            switch (count) {
            case 2:
                out.write((b0 << 2) | (b1 >> 4));
                if ((b1 & 0x0f) != 0) {
                    throw new IOException("BASE32 format violation");
                }
                break;
            case 3:
                out.write((b0 << 2) | (b1 >> 4));
                out.write(((b1 & 0x0f) << 4) | (b2 >> 2));
                if ((b2 & 0x03) != 0) {
                    throw new IOException("BASE32 format violation");
                }
                break;
            case 4:
                out.write((b0 << 2) | (b1 >> 4));
                out.write(((b1 & 0x0f) << 4) | (b2 >> 2));
                out.write(((b2 & 0x03) << 6) | b3);
                break;
            }
            written += count-1;
        }
        return written;
    }

    /**
     * Decodes BASE64 encoded binary data.
     *
     * @param text the BASE64 encoded data
     * @param decodeMap the decoding map. If this parameter is
     *   <code>null</code> then this code automatically determines
     *   which decoding map to use.
     *
     * @return the decoded data
     * @exception IOException if the input string does not conform to BASE64
     * specification
     */
    private static byte [] decode(String text, byte[] decodeMap)
    throws IOException {
        if (text == null || text.length() == 0) {
            return new byte[0];
        } else {
            int size = (text.length()/DECODE_CHUNK_SIZE+1)*ENCODE_CHUNK_SIZE;
            ByteArrayOutputStream out = new ByteArrayOutputStream(size);
            decode(new ByteArrayInputStream(text.getBytes()), out, decodeMap);
            return out.toByteArray();
        }
    }

    /**
     * This implements a BASE64 decoder as specified in RFC 1521.
     * It reads the input stream, decodes it and writes the decoded
     * bytes to the output stream.
     *
     * @param in the input BASE64 encoded stream
     * @param out the output stream receiving the decoded bytes
     *
     * @return number of bytes written to the output stream
     * @throws IOException if the input stream throws it
     */
    public static int decode(InputStream in, OutputStream out)
    throws IOException {
        return decode(in, out, base64DecodeMap);
    }

    /**
     * Decodes BASE64 encoded binary data according to BASE64 algorithm
     * as specified in RFC1521.
     *
     * @param text the BASE64 encoded data
     *
     * @return the decoded data
     * @exception IOException if the input string does not conform to BASE64
     * specification
     */
    public static byte [] decode(String text) throws IOException {
        return decode(text, base64DecodeMap);
    }

    /**
     * This method implements a "URL and filename safe" BASE64 decoder
     * as specified in RFC 3548. It reads the input stream, decodes it
     * and writes the decoded bytes to the output stream.
     *
     * @param in the input BASE64 encoded stream
     * @param out the output stream receiving the decoded bytes
     *
     * @return number of bytes written to the output stream
     * @throws IOException if the input stream throws it
     */
    public static int safeDecode(InputStream in, OutputStream out)
    throws IOException {
        return decode(in, out, base64SafeDecodeMap);
    }

    /**
     * Decodes BASE64 encoded binary data according to the URL and filename
     * safe BASE64 decoder as specified in RFC 3548.
     *
     * @param text the BASE64 encoded data
     *
     * @return the decoded data
     * @throws IOException if the input stream throws it
     */
    public static byte [] safeDecode(String text)
    throws IOException {
        return decode(text, base64SafeDecodeMap);
    }

    /**
     * This method implements a BASE64 decoder. It reads the input stream,
     * decodes it and writes the decoded bytes to the output stream. This
     * method automatically detects and allows the use of "URL and filename
     * safe" alphabet defined in RFC 3548, as well as the standard BASE64
     * encoding as specified in RFC 1521.
     *
     * @param in the input BASE64 encoded stream
     * @param out the output stream receiving the decoded bytes
     *
     * @return number of bytes written to the output stream
     * @throws IOException if the input stream throws it
     */
    public static int autoDecode(InputStream in, OutputStream out)
    throws IOException {
        return decode(in, out, null);
    }

    /**
     * Decodes BASE64 encoded binary data. This method automatically detects
     * and allows the use of "URL and filename safe" alphabet defined in RFC
     * 3548, as well as the standard BASE64 encoding as specified in RFC 1521.
     *
     * @param text the BASE64 encoded data
     *
     * @return the decoded data
     * @throws IOException if the input stream throws it
     */
    public static byte [] autoDecode(String text)
    throws IOException {
        return decode(text, null);
    }

    /**
     * This implements a BASE64 character encoder.
     *
     * @param in the input stream
     * @param out the output stream receiving the BASE64 encoded data
     * @param encodeMap the byte to character encoding map
     *
     * @return number of bytes read from the input stream
     * @throws IOException if the input stream throws it
     */
    private static int encode(InputStream in, OutputStream out, char[] encodeMap)
    throws IOException {
        int len;
        int count = 0;

        // +--first octet--+-second octet--+--third octet--+
        // |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        // +-----------+---+-------+-------+---+-----------+
        // |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
        // +--0.index--+--1.index--+--2.index--+--3.index--+

        byte [] chunk = new byte[ENCODE_CHUNK_SIZE];
        while ((len = in.read(chunk)) > 0) {
            int a, b, c;
            count += len;
            if (len == 1) {
                a = (chunk[0] & 0xff);
                out.write(encodeMap[a >> 2]);
                out.write(encodeMap[(a & 0x03) << 4]);
                out.write('=');
                out.write('=');
                break;
            } else if (len == 2) {
                a = (chunk[0] & 0xff);
                b = (chunk[1] & 0xff);
                out.write(encodeMap[a >> 2]);
                out.write(encodeMap[((a & 0x03) << 4) | (b >> 4)]);
                out.write(encodeMap[(b & 0x0f) << 2]);
                out.write('=');
                break;
            } else {
                a = (chunk[0] & 0xff);
                b = (chunk[1] & 0xff);
                c = (chunk[2] & 0xff);
                out.write(encodeMap[a >> 2]);
                out.write(encodeMap[((a & 0x03) << 4) | (b >> 4)]);
                out.write(encodeMap[((b & 0x0f) << 2) | (c >> 6)]);
                out.write(encodeMap[c & 0x3f]);
                // continue reading input
            }
        }
        return count;
    }

    /**
     * Encodes binary data into a String. If byte array is <code<null</code>
     * or empty, returns an empty string
     *
     * @param data the data to encode
     * @param encodeMap the byte to character encoding map
     *
     * @return the BASE64 encoded data as a string
     */
    private static String encode(byte [] data, char[] encodeMap) {
        if (data != null && data.length > 0) {
            try {
                int size = 4*(data.length/3);
                if (data.length%3 != 0) size += 4;
                ByteArrayOutputStream out = new ByteArrayOutputStream(size);
                encode(new ByteArrayInputStream(data), out, encodeMap);
                return new String(out.toByteArray());
            } catch (IOException x) {
                // this should never happen
                x.printStackTrace();
            }
        }
        return "";
    }

    /**
     * This implements a BASE64 character encoder as specified in RFC 1521.
     *
     * @param in the input stream
     * @param out the output stream receiving the BASE64 encoded data
     *
     * @return number of bytes read from the input stream
     * @throws IOException if the input stream throws it
     */
    public static int encode(InputStream in, OutputStream out)
    throws IOException {
        return encode(in, out, base64EncodeMap);
    }

    /**
     * Encodes binary data into a String according to BASE64 algorithm
     * as specified in RFC1521. If byte array is <code<null</code> or
     * empty, returns an empty string
     *
     * @param data the data to encode
     * @return the BASE64 encoded data as a string
     */
    public static String encode(byte [] data) {
        return encode(data, base64EncodeMap);
    }

    /**
     * This implements a URL and filename safe BASE64 character encoder as
     * specified in RFC 3548.
     *
     * @param in the input stream
     * @param out the output stream receiving the BASE64 encoded data
     *
     * @return number of bytes read from the input stream
     * @throws IOException if the input stream throws it
     */
    public static int safeEncode(InputStream in, OutputStream out)
    throws IOException {
        return encode(in, out, base64SafeEncodeMap);
    }

    /**
     * Encodes binary data into a String according to the URL and filename
     * safe BASE64 encoding as specified in RFC 3548. If byte array
     * is <code<null</code> or empty, returns an empty string.
     *
     * @param data the data to encode
     * @return the BASE64 encoded data as a string
     */
    public static String safeEncode(byte [] data) {
        return encode(data, base64SafeEncodeMap);
    }

    /**
     * Strips the pad characters off the end of the (presumably) BASE64
     * encoded string.
     *
     * @param s the BASE64 encoded string.
     * @return the input string stripped of the trailing pad characters
     */
    public static String stripPad(String s) {
        int len = s.length();
        while (len > 0 && s.charAt(len-1) == '=') len--;
        return ((len == s.length()) ? s : s.substring(0,len));
    }
}
