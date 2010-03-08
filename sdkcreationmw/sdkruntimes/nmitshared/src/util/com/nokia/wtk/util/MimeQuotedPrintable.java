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



/* -*- Mode: java; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is the Grendel mail/news client.
 *
 * The Initial Developer of the Original Code is Netscape Communications
 * Corporation.  Portions created by Netscape are Copyright (C) 1997
 * Netscape Communications Corporation.  All Rights Reserved.
 *
 * Created: Jamie Zawinski <jwz@netscape.com>, 28 Aug 1997.
 */

package com.nokia.wtk.util;

import java.util.*;
import java.io.*;

/**
 * Encodes/Decodes string into quoted-printable form.
 */
public class MimeQuotedPrintable {

    /**
     * Encode byte array into MIME quoted-printable string.
     * @param bytes to be encoded.
     * @return String encoded text.
     * @see RFC 2047
     */
    public static String encode(byte[] inArr) {
        ByteBuf inbuf = new ByteBuf(inArr);
        ByteBuf outbuf = new ByteBuf();
        MimeQuotedPrintableEncoder enc = new MimeQuotedPrintableEncoder();
        enc.translate(inbuf, outbuf);
        return outbuf.toString();
    }

    /**
     * Decode from MIME quoted-printable string into byte array.
     * @param bytes to be decoded.
     * @return String decoded text.
     * @see RFC 2047
     */
    public static byte[] decode(String instr) {
        ByteBuf inbuf = new ByteBuf(instr);
        ByteBuf outbuf = new ByteBuf();
        MimeQuotedPrintableDecoder dec = new MimeQuotedPrintableDecoder();
        dec.translate(inbuf, outbuf);
        return outbuf.toBytes();
    }

    /**
     * UNIT TEST
     */
    public static void main(String[] args) {
       String test = "This is a test \nAnd some more here.";
       if (args.length > 0) {
           StringBuffer sb = new StringBuffer();
           for (int i = 0; i < args.length; i++) {
               sb.append(args[i]);
               sb.append(" ");
           }
           test = sb.toString(); 
       }
       System.out.println("Encoded: " + encode(test.getBytes()));
    }
}

/*
 * Most of the following code was lifted from Mozilla public sources.  
 * Thus the legal disclaimer at the top.
 */

/** 
 * Implements a plaintext -> Quoted-Printable encoder.
 */
class MimeQuotedPrintableEncoder {

  boolean had_whitespace = false;
  int output_column = 0;
  byte last_code = NUL;

  static private final byte NUL = 0;   // don't encode
  static private final byte ESC = 1;   // always encode
  static private final byte SPC = 2;   // horizontal whitespace
  static private final byte BOL = 3;   // encode if at beginning of line
  static private final byte CR  = 4;   // carriage return
  static private final byte LF  = 5;   // linefeed

  static private final byte map[] = {
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   000-007
    ESC, SPC,  LF, ESC, ESC,  CR, ESC, ESC,      //   010-017
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   020-027
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   030-037
    SPC, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   040-047   !"#$%&'
    NUL, NUL, NUL, NUL, NUL, NUL, BOL, NUL,      //   050-057  ()*+,-./
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   060-067  01234567
    NUL, NUL, NUL, NUL, NUL, ESC, NUL, NUL,      //   070-077  89:;<=>?

    NUL, NUL, NUL, NUL, NUL, NUL, BOL, NUL,      //   100-107  @ABCDEFG
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   110-117  HIJKLMNO
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   120-127  PQRSTUVW
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   130-137  XYZ[\]^_
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   140-147  `abcdefg
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   150-157  hijklmno
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   160-167  pqrstuvw
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, ESC,      //   170-177  xyz{|}~

    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   200-207
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   210-217
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   220-227
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   230-237
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   240-247
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   250-257
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   260-267
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   270-277

    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   300-307
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   310-317
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   320-327
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   330-337
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   340-347
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   350-357
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   360-367
    ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,      //   370-377
  };

  static private final byte hex[] = {
    (byte)'0', (byte)'1', (byte)'2', (byte)'3', (byte)'4', (byte)'5', (byte)'6', (byte)'7',
    (byte)'8', (byte)'9', (byte)'A', (byte)'B', (byte)'C', (byte)'D', (byte)'E', (byte)'F'
  };

  static private final byte         crlf[] =      "\r\n".getBytes();
  static private final byte      eq_crlf[] =     "=\r\n".getBytes();
  static private final byte eq_crlf_crlf[] = "=\r\n\r\n".getBytes();


  /** Given a sequence of input bytes, produces a sequence of output bytes
      using the quoted-printable encoding.  If there are bytes in `out'
      already, the new bytes are appended, so the caller should do
      `out.setLength(0)' first if that's desired.
   */
  public final void translate(ByteBuf in, ByteBuf out) {

    byte inb[] = in.toBytes();
    int in_length = in.length();

    for (int i = 0; i < in_length; i++) {
      byte b = inb[i];
      int indx = b & 0xff;
      byte c = map[indx];

      if (c == NUL) {                           // non-special character
        out.append(b);
        output_column++;
        had_whitespace = false;

      } else if (c == SPC) {                    // whitespace
        out.append(b);
        output_column++;
        had_whitespace = true;

      } else if (c == LF && last_code == CR) {
        // We already processed this linebreak;
        // ignore the LF in the CRLF pair.

      } else if (c == CR || c == LF) {          // start of linebreak

        if (had_whitespace) {
          // Whitespace cannot be allowed to occur at the end of the line.
          // So we encode " \n" as " =\n\n", that is, the whitespace, a
          // soft line break, and then a hard line break.
          //
          out.append(eq_crlf_crlf);
          had_whitespace = false;
        } else {
          out.append(crlf);
        }
        output_column = 0;

      } else if (c == BOL && output_column != 0) {      // beginning-of-line
        out.append(b);                                  // special, while not
        output_column++;                                // at beginning-of-line
        had_whitespace = false;

      } else {                                  // special character
        out.append((byte) '=');
        out.append((byte) hex[0xF & (b >> 4)]);
        out.append((byte) hex[0xF & b]);
        output_column += 3;
        had_whitespace = false;
      }

      last_code = c;

      // Assert.Assertion(output_column <= 76);

      if (output_column >= 73) {
        out.append(eq_crlf);
        output_column = 0;
        had_whitespace = false;
      }
    }
  }

  /** Tell the quoted-printable encoder that no more input data will be
      forthcoming.  This may result in output, as a result of flushing the
      internal buffer.  This object must not be used again after calling eof().
      If there are bytes in `out' already, the new bytes are appended,
      so the caller should do `out.setLength(0)' first if that's desired.
   */
  public final void eof(ByteBuf out) {
    if (output_column != 0) {
      out.append(eq_crlf);
      output_column = 0;
      had_whitespace = false;
    }
  }
}

/** 
 * Implements a Quoted-Printable -> plaintext decoder.
 */
class MimeQuotedPrintableDecoder {

  private byte token[] = new byte[3];      // input and output buffer
  private int token_length = 0;            // input buffer length

  static private final byte NUL = 127;     // must be greater than 16
  static private final byte ESC = 126;     // must be greater than 16
  static private final byte CR  = 125;     // must be greater than 16
  static private final byte LF  = 124;     // must be greater than 16

  static private final byte map[] = {
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   000-007
    NUL, NUL,  LF, NUL, NUL,  CR, NUL, NUL,      //   010-017
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   020-027
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   030-037
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   040-047   !"#$%&'
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   050-057  ()*+,-./
      0,   1,   2,   3,   4,   5,   6,   7,      //   060-067  01234567
      8,   9, NUL, NUL, NUL, ESC, NUL, NUL,      //   070-077  89:;<=>?

    NUL,  10,  11,  12,  13,  14,  15, NUL,      //   100-107  @ABCDEFG
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   110-117  HIJKLMNO
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   120-127  PQRSTUVW
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   130-137  XYZ[\]^_
    NUL,  10,  11,  12,  13,  14,  15, NUL,      //   140-147  `abcdefg
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   150-157  hijklmno
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   160-167  pqrstuvw
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   170-177  xyz{|}~

    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   200-207
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   210-217
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   220-227
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   230-237
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   240-247
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   250-257
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   260-267
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   270-277

    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   300-307
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   310-317
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   320-327
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   330-337
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   340-347
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   350-357
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   360-367
    NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,      //   370-377
  };


  /** Given a sequence of input bytes using the quoted-printable encoding,
      produces a sequence of unencoded output bytes.  Note that some (small)
      amount of buffering may be necessary, if the input byte stream didn't
      fall on an appropriate boundary.  If there are bytes in `out' already,
      the new bytes are appended, so the caller should do `out.setLength(0)'
      first if that's desired.
   */
  public final void translate(ByteBuf in, ByteBuf out) {

    /* To Do, maybe someday: when decoding quoted-printable, we are required
       to strip trailing whitespace from lines -- since when encoding in QP,
       one is required to quote such trailing whitespace, any trailing
       whitespace which remains must have been introduced by a stupid gateway.
       We don't do this right now, as it would require arbitrary lookahead
       instead of just a fixed distance of three bytes.
     */

    byte inb[] = in.toBytes();
    int in_length = in.length();

    int i = 0;

    while (i < in_length) {

      // First, copy as many consecutive bytes as we can.
      if (token_length == 0) {
        int j = i;
        while (i < in_length && map[inb[i]] != ESC)
          i++;
        if (i != j)
          out.append(inb, j, i-j);

        if (i >= in_length)
          break;
      }

      // at this point, either token_length > 0, or we've seen ESC

      byte b = inb[i];
      byte c = map[b];
      i++;

      token[token_length++] = b;

      if (token_length != 3)
        continue;

      token_length = 0;

      // assert (map[token[0]] == ESC)

      byte n1 = map[token[1]];
      byte n2 = map[token[2]];

      if (n1 == CR || n1 == LF) {               // "=\r", "=\n", or "=\r\n"
        if (n1 == LF || n2 != LF) {
          // lone CR or LF -- push out or put back following character.
          if (n2 == ESC) {
            token[0] = token[2];
            token_length = 1;
          } else {
            out.append(token[2]);
          }
        }

      } else if (n1 <= 15 || n2 <= 15) {        // "=xy" where x and y are hex
        out.append((byte) ((n1 << 4) | n2));

      } else {                                  // something unparsable.
        out.append(token);                      // write it out unchanged.
      }

    }
  }

  /** Tell the quoted-printable decoder that no more input data will be
      forthcoming.  This may result in output, as a result of flushing the
      internal buffer.  This object must not be used again after calling eof().
      If there are bytes in `out' already, the new bytes are appended,
      so the caller should do `out.setLength(0)' first if that's desired.
   */
  public final void eof(ByteBuf out) {
    // ignore the last incomplete token, if any -- it means "no trailing CRLF".
    token_length = 0;
    token = null;
  }
}

/**
 * This class is similar to java/lang/StringBuffer with the following changes:
 * <UL><LI>Stores bytes, not chars
 * <LI>None of the methods are synchronized
 * <LI>There is no sharing of the character array
 * <LI>Converting to a String requires a copy of the character data
 * <LI>A bunch of useful operations are provided (comparisons, etc.)
 * </UL>
 */
final class ByteBuf {
  private byte value[];
  private int count;

  /**
   * Constructs an empty String buffer.
   */
  public ByteBuf() {
    this(16);
  }

  /**
   * Constructs an empty byte buffer with the specified initial length.
   * @param length  the initial length
   */
  public ByteBuf(int length) {
    value = new byte[length];
  }

  /**
   * Constructs a byte buffer with the specified initial value.
   * @param str the initial value of the buffer
   */
  public ByteBuf(String str) {
    this(str.length() + 16);
    append(str);
  }

  public ByteBuf(byte bytes[]) {
    this(bytes, 0, bytes.length);
  }

  public ByteBuf(byte bytes[], int offset, int length) {
    value = new byte[length];
    System.arraycopy(bytes, offset, value, 0, length);
    count = length;
  }

  /**
   * Returns the length (character count) of the buffer.
   */
  public int length() {
    return count;
  }

  /**
   * Ensures that the capacity of the buffer is at least equal to the
   * specified minimum.
   * @param minimumCapacity the minimum desired capacity
   */
  public void ensureCapacity(int minimumCapacity) {
    int maxCapacity = value.length;

    if (minimumCapacity > maxCapacity) {
      int newCapacity = (maxCapacity + 1) * 2;
      if (minimumCapacity > newCapacity) {
        newCapacity = minimumCapacity;
      }

      byte newValue[] = new byte[newCapacity];
      System.arraycopy(value, 0, newValue, 0, count);
      value = newValue;
    }
  }

  /**
   * Sets the length of the String. If the length is reduced, characters
   * are lost. If the length is extended, the values of the new characters
   * are set to 0.
   * @param newLength the new length of the buffer
   * @exception StringIndexOutOfBoundsException  If the length is invalid.
   */
  public void setLength(int newLength) {
    if (newLength < 0) {
      throw new StringIndexOutOfBoundsException(newLength);
    }
    if (count < newLength) {
      ensureCapacity(newLength);
      for (; count < newLength; count++) {
        value[count] = (byte)'\0';
      }
    }
    count = newLength;
  }

  /**
   * Copies the characters of the specified substring (determined by
   * srcBegin and srcEnd) into the character array, starting at the
   * array's dstBegin location. Both srcBegin and srcEnd must be legal
   * indexes into the buffer.
   * @param srcBegin  begin copy at this offset in the String
   * @param srcEnd  stop copying at this offset in the String
   * @param dst   the array to copy the data into
   * @param dstBegin  offset into dst
   * @exception StringIndexOutOfBoundsException If there is an invalid index into the buffer.
   */
  public void getBytes(int srcBegin, int srcEnd, byte dst[], int dstBegin) {
    if ((srcBegin < 0) || (srcBegin >= count)) {
      throw new StringIndexOutOfBoundsException(srcBegin);
    }
    if ((srcEnd < 0) || (srcEnd > count)) {
      throw new StringIndexOutOfBoundsException(srcEnd);
    }
    if (srcBegin < srcEnd) {
      System.arraycopy(value, srcBegin, dst, dstBegin, srcEnd - srcBegin);
    }
  }

  /**
   * Changes the byte at the specified index to be ch.
   * @param index the index of the character
   * @param ch    the new character
   * @exception StringIndexOutOfBoundsException If the index is invalid.
   */
  public void setByteAt(int index, byte b) {
    if ((index < 0) || (index >= count)) {
      throw new StringIndexOutOfBoundsException(index);
    }
    value[index] = b;
  }

  /**
   * Appends an array of bytes to the end of this buffer.
   * @param str the characters to be appended
   * @return  the ByteBuf itself, NOT a new one.
   */
  public ByteBuf append(byte str[]) {
    int len = str.length;
    ensureCapacity(count + len);
    System.arraycopy(str, 0, value, count, len);
    count += len;
    return this;
  }

  /**
   * Appends a part of an array of characters to the end of this buffer.
   * @param str the characters to be appended
   * @param offset  where to start
   * @param len the number of characters to add
   * @return  the ByteBuf itself, NOT a new one.
   */
  public ByteBuf append(byte str[], int offset, int len) {
    ensureCapacity(count + len);
    System.arraycopy(str, offset, value, count, len);
    count += len;
    return this;
  }

  public ByteBuf append(ByteBuf buf) {
    append(buf.toBytes(), 0, buf.length());
    return this;
  }

  /**
   * Appends a byte to the end of this buffer.
   * @param ch  the character to be appended
   * @return  the ByteBuf itself, NOT a new one.
   */
  public ByteBuf append(byte b) {
    ensureCapacity(count + 1);
    value[count++] = b;
    return this;
  }

  /**
   * Appends a String to the end of this buffer.  This just appends one byte
   * per char; it strips off the upper 8 bits.  If you want the localized
   * method of converting chars to bytes, use append(String.getBytes()).
   * @param str the String to be appended
   * @return  the ByteBuf itself, NOT a new one.
   */
  public ByteBuf append(String str) {
    if (str == null) {
      str = String.valueOf(str);
    }

    int len = str.length();
    ensureCapacity(count + len);
    for (int i=0 ; i<len ; i++) {
      value[count++] = (byte) str.charAt(i);
    }
    return this;
  }

  /**
   * Inserts an array of bytes into the String buffer.
   * @param offset  the offset at which to insert
   * @param str   the characters to insert
   * @return    the ByteBuf itself, NOT a new one.
   * @exception StringIndexOutOfBoundsException If the offset is invalid.
   */
  public ByteBuf insert(int offset, byte str[]) {
    if ((offset < 0) || (offset > count)) {
      throw new StringIndexOutOfBoundsException();
    }
    int len = str.length;
    ensureCapacity(count + len);
    System.arraycopy(value, offset, value, offset + len, count - offset);
    System.arraycopy(str, 0, value, offset, len);
    count += len;
    return this;
  }

  /**
   * Inserts a byte into the String buffer.
   * @param offset  the offset at which to insert
   * @param ch    the character to insert
   * @return    the ByteBuf itself, NOT a new one.
   * @exception StringIndexOutOfBoundsException If the offset invalid.
   */
  public ByteBuf insert(int offset, byte b) {
    ensureCapacity(count + 1);
    System.arraycopy(value, offset, value, offset + 1, count - offset);
    value[offset] = b;
    count += 1;
    return this;
  }

  /**
   * Converts to a String representing the data in the buffer.
   */
  public String toString() {
    return new String(value, 0, count);
  }

  public byte[] toBytes() {
    return value;
  }

  /** 
   * Write to the given output stream a detailed description of each
   *  byte in this buffer. 
   */
  public void fullDump(PrintStream out) {
    fullDump(out, 0, count);
  }

  /**
   *  Write to the given output stream a detailed description of the given
   *  bytes in this buffer. 
   */
  public void fullDump(PrintStream out, int start, int end) {
    for (int i=start ; i<end ; i++) {
      out.write(value[i]);
      out.print("(" + ((int) value[i]) + ") ");
    }
    out.println("");
  }

  /** Invokes InputStream.read(), appending the bytes to this Bytebuf.
   *  @return the number of bytes read, or -1 if eof.
   */
  public int read(InputStream file, int max_bytes)
    throws IOException {
    ensureCapacity(count + max_bytes);
    int i = file.read(value, count, max_bytes);
    if (i > 0) count += i;
    return i;
  }

  /** 
   * Invokes RandomAccessFile.read(), appending the bytes to this Bytebuf.
   * (A RandomAccessFile is not an InputStream, because Java is a crock.)
   * @return the number of bytes read, or -1 if eof.
   */
  public int read(RandomAccessFile file, int max_bytes)
    throws IOException {
    ensureCapacity(count + max_bytes);
    int i = file.read(value, count, max_bytes);
    if (i > 0) count += i;
    return i;
  }

  /**
   * Writes the contents to the given output stream. 
   */
  public void write(OutputStream out) throws IOException {
    out.write(value, 0, count);
  }

  /** 
   * Writes the contents to the given RandomAccessFile. 
   */
  public void write(RandomAccessFile out) throws IOException {
    out.write(value, 0, count);
  }

  /** 
   * Creates a new InputStream whose content is this ByteBuf.  Note that
   * changing the ByteBuf can affect the stream; the data is <i>not</i> copied. 
   */
  public InputStream makeInputStream() {
    return new ByteArrayInputStream(value, 0, count);
  }
}
