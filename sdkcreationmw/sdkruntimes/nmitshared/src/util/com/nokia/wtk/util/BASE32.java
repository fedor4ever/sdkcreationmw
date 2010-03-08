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
 * This class implements a BASE32 encoder and decoder as specified
 * in RFC 3548.
 *
 */
public abstract class BASE32 {

    /** Default constructor is suppressed, ensuring non-instantiability. */
    private BASE32() {
        throw new InternalError("BASE32 is a collection of static methods");
    }

    /**
     * This character array provides the character to value map
     * based on RFC3548.
     */
    private static final char encodeMap[] = {
    //   0   1   2   3   4   5   6   7
        'A','B','C','D','E','F','G','H', // 0
        'I','J','K','L','M','N','O','P', // 1
        'Q','R','S','T','U','V','W','X', // 2
        'Y','Z','2','3','4','5','6','7'  // 3
    };

    private static final String ENCODING = "UTF-8";;
    private static final int ENCODE_CHUNK_SIZE = 5;
    private static final int DECODE_CHUNK_SIZE = 8;
    private static final byte caseSensitiveDecodeMap[] = new byte[256];
    private static final byte caseInsensitiveDecodeMap[] = new byte[256];
    private static final byte PAD[][] = {
        new byte[] {},
        new byte[] {'=','=','=','=','=','='},
        new byte[] {'=','=','=','='},
        new byte[] {'=','=','='},
        new byte[] {'='},
        new byte[] {}
    };

    static {
        int i;
        for (i=0; i<256; i++) {
            caseSensitiveDecodeMap[i] = -1;
            caseInsensitiveDecodeMap[i] = -1;
        }
        for (i=0; i<32; i++) {
            byte j = (byte)i;
            caseSensitiveDecodeMap[encodeMap[i]] = j;
            caseInsensitiveDecodeMap[encodeMap[i]] = j; // upper case
            caseInsensitiveDecodeMap[Character.toLowerCase(encodeMap[i])] = j;
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
     * This method implements a BASE32 decoder. It reads the input stream,
     * decodes it and writes the decoded bytes to the output stream.
     *
     * @param in the input BASE32 encoded stream
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
        int count;
        int written = 0;
        byte [] chunk = new byte[DECODE_CHUNK_SIZE];
        while ((count = readChunk(in, chunk)) > 0) {
            byte b0 = -1,b1 =-1,b2 =-1,b3 =-1,b4 =-1,b5 =-1,b6 =-1,b7 =-1;
            switch (count) {
            case 8:
                if ((b7 = decodeMap[chunk[7]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[7]);
                }
                // NOBREAK
            case 7:
                if ((b6 = decodeMap[chunk[6]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[6]);
                }
                if ((b5 = decodeMap[chunk[5]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[5]);
                }
                // NOBREAK
            case 5:
                if ((b4 = decodeMap[chunk[4]]) < 0) {
                    throw new IOException("BASE3: invalid character "+chunk[4]);
                }
                // NOBREAK
            case 4:
                if ((b3 = decodeMap[chunk[3]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[3]);
                }
                if ((b2 = decodeMap[chunk[2]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[2]);
                }
                // NOBREAK
            case 2:
                if ((b1 = decodeMap[chunk[1]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[1]);
                }
                if ((b0 = decodeMap[chunk[0]]) < 0) {
                    throw new IOException("BASE32: invalid character "+chunk[0]);
                }
                break;
            case 6:
            case 3:
            case 1:
                throw new IOException("BASE32 format violation");
            }

            // Reconstruct the original data
            //
            // +--- octet 0 ---+--- octet 1 ---+--- octet 2 ---+
            // |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
            // +---------+-----+---+---------+-+-------+-------+
            // |4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1
            // +-0.index-+-1.index-+-2.index-+-3.index-+-4.index
            //
            // +--- octet 3 ---+--- octet 4 ---+
            // |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
            // +-+---------+---+-----+---------+
            //  0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|
            // --+-5.index-+-6.index-+-7.index-+

            switch (count) {
            case 2:
                out.write((b0 << 3) | (b1 >> 2));
                written += 1;
                break;
            case 4:
                out.write((b0 << 3) | (b1 >> 2));
                out.write(((b1 & 0x03) << 6) | (b2 << 1) | (b3 >> 4));
                if ((b3 & 0x0f) != 0) {
                    throw new IOException("BASE32 format violation");
                }
                written += 2;
                break;
            case 5:
                out.write((b0 << 3) | (b1 >> 2));
                out.write(((b1 & 0x03) << 6) | (b2 << 1) | (b3 >> 4));
                out.write(((b3 & 0x0f) << 4) | (b4 >> 1));
                if ((b4 & 0x01) != 0) {
                    throw new IOException("BASE32 format violation");
                }
                written += 3;
                break;
            case 7:
                out.write((b0 << 3) | (b1 >> 2));
                out.write(((b1 & 0x03) << 6) | (b2 << 1) | (b3 >> 4));
                out.write(((b3 & 0x0f) << 4) | (b4 >> 1));
                out.write(((b4 & 0x01) << 7) | (b5 << 2) | (b6 >> 3));
                if ((b6 & 0x07) != 0) {
                    throw new IOException("BASE32 format violation");
                }
                written += 4;
                break;
            case 8:
                out.write((b0 << 3) | (b1 >> 2));
                out.write(((b1 & 0x03) << 6) | (b2 << 1) | (b3 >> 4));
                out.write(((b3 & 0x0f) << 4) | (b4 >> 1));
                out.write(((b4 & 0x01) << 7) | (b5 << 2) | (b6 >> 3));
                out.write(((b6 & 0x07) << 5) | b7);
                written += 5;
                break;
            }
        }
        return written;
    }

    /**
     * Decodes BASE32 encoded binary data.
     *
     * @param text the BASE32 encoded data
     * @param map the decoding map.
     *
     * @return the decoded data
     * @exception IOException if the input string does not conform to the
     *                        BASE32 specification
     */
    private static byte [] decode(String text, byte[] map) throws IOException {
        if (text == null || text.length() == 0) {
            return new byte[0];
        } else {
            // Estimate the size of the output data
            int size = (text.length()/DECODE_CHUNK_SIZE)*ENCODE_CHUNK_SIZE;
            switch (text.length()%DECODE_CHUNK_SIZE) {
            case 2: size += 1; break;
            case 4: // NOBREAK
            case 5: size += 3; break;
            case 7: size += 4; break;
            }
            ByteArrayOutputStream out = new ByteArrayOutputStream(size);
            decode(new ByteArrayInputStream(text.getBytes(ENCODING)),out,map);
            return out.toByteArray();
        }
    }

    /**
     * This implements a case insensitive BASE32 decoder as specified in
     * RFC 3548. It reads the input stream, decodes it and writes the
     * decoded bytes to the output stream.
     *
     * @param in the input BASE32 encoded stream
     * @param out the output stream receiving the decoded bytes
     *
     * @return number of bytes written to the output stream
     * @throws IOException if the input stream throws it of the input data
     *                     don't conform to the BASE32 specification
     */
    public static int decode(InputStream in, OutputStream out)
    throws IOException {
        return decode(in, out, caseInsensitiveDecodeMap);
    }

    /**
     * Decodes BASE32 encoded data according to BASE32 algorithm as specified
     * in RFC 3548, using the case insensitive decoding map.
     *
     * @param text the BASE32 encoded data
     *
     * @return the decoded data
     * @exception IOException if the input string does not conform to the
     *                        BASE32 specification
     */
    public static byte [] decode(String text) throws IOException {
        return decode(text, caseInsensitiveDecodeMap);
    }

    /**
     * This implements a case sensitive BASE32 decoder as specified in
     * RFC 3548. It reads the input stream, decodes it and writes the
     * decoded bytes to the output stream.
     *
     * @param in the input BASE32 encoded stream
     * @param out the output stream receiving the decoded bytes
     *
     * @return number of bytes written to the output stream
     * @throws IOException if the input stream throws it of the input data
     *                     don't conform to the BASE32 specification
     */
    public static int strictDecode(InputStream in, OutputStream out)
    throws IOException {
        return decode(in, out, caseSensitiveDecodeMap);
    }

    /**
     * Decodes BASE32 encoded data according to BASE32 algorithm as specified
     * in RFC 3548, using the case sensitive decoding map.
     *
     * @param text the BASE32 encoded data
     *
     * @return the decoded data
     * @exception IOException if the input string does not conform to the
     *                        BASE32 specification
     */
    public static byte [] strictDecode(String text)
    throws IOException {
        return decode(text, caseSensitiveDecodeMap);
    }

    /**
     * This implements a BASE32 character encoder.
     *
     * @param in the input stream
     * @param out the output stream receiving the BASE32 encoded data
     * @param pad <code>true</code> to pad the BASE32 encoded string
     * with the pad character <code>'='</code> up to the nearest length
     * divisible by 8.
     *
     * @return number of bytes read from the input stream
     * @throws IOException if the input or output stream throws it
     */
    public static int encode(InputStream in, OutputStream out, boolean pad)
    throws IOException {

        // +--- octet 0 ---+--- octet 1 ---+--- octet 2 ---+
        // |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        // +---------+-----+---+---------+-+-------+-------+
        // |4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1
        // +-0.index-+-1.index-+-2.index-+-3.index-+-4.index
        //
        // +--- octet 3 ---+--- octet 4 ---+
        // |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        // +-+---------+---+-----+---------+
        //  0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|
        // --+-5.index-+-6.index-+-7.index-+

        int len;
        int count = 0;
        byte [] chunk = new byte[ENCODE_CHUNK_SIZE];
        while ((len = in.read(chunk)) > 0) {
            int b0,b1,b2,b3,b4;
            count += len;
            switch (len) {
            case 1:
                b0 = (chunk[0] & 0xff);
                out.write(encodeMap[b0 >> 3]);
                out.write(encodeMap[(b0 & 0x07) << 2]);
                break;
            case 2:
                b0 = (chunk[0] & 0xff);
                b1 = (chunk[1] & 0xff);
                out.write(encodeMap[b0 >> 3]);
                out.write(encodeMap[((b0 & 0x07) << 2) | (b1 >> 6)]);
                out.write(encodeMap[(b1 & 0x3e) >> 1]);
                out.write(encodeMap[(b1 & 0x01) << 4]);
                break;
            case 3:
                b0 = (chunk[0] & 0xff);
                b1 = (chunk[1] & 0xff);
                b2 = (chunk[2] & 0xff);
                out.write(encodeMap[b0 >> 3]);
                out.write(encodeMap[((b0 & 0x07) << 2) | (b1 >> 6)]);
                out.write(encodeMap[(b1 & 0x3e) >> 1]);
                out.write(encodeMap[((b1 & 0x01) << 4) | (b2 >> 4)]);
                out.write(encodeMap[(b2 & 0x0f) << 1]);
                break;
            case 4:
                b0 = (chunk[0] & 0xff);
                b1 = (chunk[1] & 0xff);
                b2 = (chunk[2] & 0xff);
                b3 = (chunk[3] & 0xff);
                out.write(encodeMap[b0 >> 3]);
                out.write(encodeMap[((b0 & 0x07) << 2) | (b1 >> 6)]);
                out.write(encodeMap[(b1 & 0x3e) >> 1]);
                out.write(encodeMap[((b1 & 0x01) << 4) | (b2 >> 4)]);
                out.write(encodeMap[((b2 & 0x0f) << 1) | (b3 >> 7)]);
                out.write(encodeMap[(b3 & 0x7c) >> 2]);
                out.write(encodeMap[(b3 & 0x03) << 3]);
                break;
            case 5:
                b0 = (chunk[0] & 0xff);
                b1 = (chunk[1] & 0xff);
                b2 = (chunk[2] & 0xff);
                b3 = (chunk[3] & 0xff);
                b4 = (chunk[4] & 0xff);
                out.write(encodeMap[b0 >> 3]);
                out.write(encodeMap[((b0 & 0x07) << 2) | (b1 >> 6)]);
                out.write(encodeMap[(b1 & 0x3e) >> 1]);
                out.write(encodeMap[((b1 & 0x01) << 4) | (b2 >> 4)]);
                out.write(encodeMap[((b2 & 0x0f) << 1) | (b3 >> 7)]);
                out.write(encodeMap[(b3 & 0x7c) >> 2]);
                out.write(encodeMap[((b3 & 0x03) << 3) | (b4 >> 5)]);
                out.write(encodeMap[b4 & 0x1f]);
                break;
            }
            if (pad && len < ENCODE_CHUNK_SIZE) {
                out.write(PAD[len]);
            }
        }
        return count;
    }

    /**
     * This implements a BASE32 character encoder.
     *
     * @param in the input stream
     * @param out the output stream receiving the BASE32 encoded data
     *
     * @return number of bytes read from the input stream
     * @throws IOException if the input or output stream throws it
     */
    public static int encode(InputStream in, OutputStream out)
    throws IOException {
        return encode(in, out, true);
    }

    /**
     * Encodes binary data into a String. If byte array is <code<null</code>
     * or empty, returns an empty string.
     *
     * @param data the data to encode
     * @param pad <code>true</code> to pad the BASE32 encoded string
     * with the pad character <code>'='</code> up to the nearest length
     * divisible by 8.
     *
     * @return the BASE32 encoded data as a string
     */
    public static String encode(byte [] data, boolean pad) {
        if (data != null && data.length > 0) {
            try {
                // Calculate the size of the output buffer
                int size = DECODE_CHUNK_SIZE*(data.length/ENCODE_CHUNK_SIZE);
                if (data.length%ENCODE_CHUNK_SIZE != 0) {
                    if (pad) {
                        size += DECODE_CHUNK_SIZE;
                    } else {
                        size += PAD[data.length%ENCODE_CHUNK_SIZE].length;
                    }
                }
                ByteArrayOutputStream out = new ByteArrayOutputStream(size);
                encode(new ByteArrayInputStream(data), out, pad);
                return new String(out.toByteArray(),ENCODING);
            } catch (IOException x) {
                // this should never happen
                x.printStackTrace();
            }
        }
        return "";
    }

    /**
     * Encodes binary data into a String. If byte array is <code<null</code>
     * or empty, returns an empty string.
     *
     * @param data the data to encode
     *
     * @return the BASE32 encoded data as a string
     */
    public static String encode(byte [] data) {
        return encode(data, true);
    }

    /**
     * Strips the pad characters off the end of the (presumably) BASE32
     * encoded string.
     *
     * @param s the BASE32 encoded string.
     * @return the input string stripped of the trailing pad characters
     */
    public static String stripPad(String s) {
        int len = s.length();
        while (len > 0 && s.charAt(len-1) == '=') len--;
        return ((len == s.length()) ? s : s.substring(0,len));
    }
}
