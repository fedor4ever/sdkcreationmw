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

/**
 * Calculates a 16 bit cyclic redundancy check.
 */
public final class CRC16 {

    /**
     * The currently computed CRC, set to 0 initally.
     */
    public short value;

    /**
     * Returns the least significant byte of this CRC
     * @return the least significant byte
     */
    public byte lowByte() {
        return (byte)(value & 0xff);
    }

    /**
     * Returns the most significant byte of this CRC
     * @return the most significant byte
     */
    public byte highByte() {
        return (byte)(value >>> 8);
    }

    /**
     * Resets CRC value to 0.
     */
    public void reset() {
        value = 0;
    }

    /**
     * Updates CRC with a single byte.
     *
     * @param b the input byte
     */
    public void update(byte b) {
        value = update(value, b);
    }

    /**
     * Updates CRC with an array of bytes.
     *
     * @param b the input array of bytes
     */
    public void update(byte[] b) {
        int len = b.length;
        for (int i=0; i<len; i++) {
            value = update(value, b[i]);
        }
    }

    /**
     * Updates CRC with an array of bytes.
     *
     * @param b   the input array of bytes
     * @param off the start offset in array <code>b</code>
     * @param len the number of bytes.
     */
    public void update(byte[] b, int off, int len) {
        for (int i=0; i<len; i++) {
            value = update(value, b[off++]);
        }
    }

    /**
     * Returns a string representation of this object. This method
     * is intended to be used only for debugging purposes, and the
     * content and format of the returned string may vary from one
     * version to another. The returned string may be empty but may not
     * be <code>null</code>.
     *
     * @return  a string representation of this object.
     */
    public String toString() {
        String s = Integer.toHexString(value);
        switch (s.length()) {
        case 1:  return "0x000" + s;
        case 2:  return "0x00" + s;
        case 3:  return "0x0" + s;
        default: return "0x" + s;
        }
    }

    //======================================================================
    //      	S T A T I C    M E T H O D S
    //======================================================================

    /**
     * Updates CRC with an array of bytes.
     *
     * @param crc the input CRC
     * @param b the input array of bytes
     * @return the updated CRC value
     */
    public static short update(short crc, byte[] b) {
        int len = b.length;
        for (int i=0; i<len; i++) {
            crc = update(crc, b[i]);
        }
        return crc;
    }

    /**
     * Updates CRC with an array of bytes.
     *
     * @param crc the input CRC
     * @param b   the input array of bytes
     * @param off the start offset in array <code>b</code>
     * @param len the number of bytes.
     * @return the updated CRC value
     */
    public static short update(short crc, byte[] b, int off, int len) {
        for (int i=0; i<len; i++) {
            crc = update(crc, b[off++]);
        }
        return crc;
    }

    /**
     * Updates CRC with a single byte.
     *
     * @param crc the input CRC
     * @param b the input byte
     * @return the updated CRC value
     */
    public static short update(short crc, byte b) {
        int x = b;
        int y = ((int)crc) & 0xffff;
        for (int i=7; i >=0; i--) {
            x <<= 1;
            int z = (x >>> 8) & 1;
            if ((y & 0x8000) != 0) {
                y = ((y << 1) + z) ^ 0x1021;
            } else {
                y = (y << 1) + z;
            }
        }
        return (short)y;
    }
}
