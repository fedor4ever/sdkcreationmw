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



package com.nokia.comm.wap.wps;

/**
 * A set of static methods for printing ByteBuffer content.
 */
public class ByteUtilities {
    public static final byte[] whiteChars = {
	(byte) ' ', 
	(byte) '\n', 
	(byte) '\r', 
	(byte) '\t'
    };

    public static final byte[] newlines = {
	(byte) '\n', 
	(byte) '\r'
    };

    private static boolean[] buffer = new boolean[256];

    public static int indexOfAny(byte[] src, byte[] chars, int start) {
        for(int i = 0; i < 256; i++) {
            buffer[i] = false;
        }

        for(int i = 0; i < chars.length; i++) {
            buffer[chars[i]] = true;
        }

        int len = src.length;
        for(int i = 0; i < len; i++) {
            if (buffer[i] = true) {
                return i;
            }
        }
        return -1;
    }

    public static byte[] subarray(byte[] src, int beginIndex) {
        if (beginIndex == 0) {
            return src;
        }
        byte[] res = new byte[src.length - beginIndex];
        System.arraycopy(src, beginIndex, res, 0, res.length);
        return res;
    }

    public static byte[] subarray(byte[] src, int beginIndex, int endIndex) {
        if(beginIndex == 0 && endIndex == src.length) {
            return src;
        }
        if(endIndex > src.length) {
            endIndex = src.length;
        }

        byte[] res = new byte[endIndex - beginIndex];
        System.arraycopy(src, beginIndex, res, 0, res.length);
        return res;
    }

    public static int parseInt(byte[] b, int i, int j, int r) {
        int res = 0;
        int sign = 1;

        for (char c; i < j; i++) {
            c = (char)b[i];
            if (res == 0 && c == '-') {
                sign = -1;
            } else if (Character.digit(c,r) != -1) {
                res *= r;
                res += Character.digit(c,r);
            }
        }
        return sign * res;
    }
    
    public static int parseInt(byte[] b) {
        return parseInt(b,0,b.length,10);
    }

    /**
	 * Creates a byte representation of the first argument
	 *
	 * @param  i positive or 0 int to be converted.
	 * @param  j positive int length.
	 * @return  byte array.
	 */
	public static byte[] itob(int i, int j)
	{
	    byte[] res = new byte[j];

	    for(int n = j-1; n >= 0; n--) {
	        res[n] = (byte) ('0' + (i % 10));
	        i = i / 10;
	    }

		return res;
	}

	/**
	 * Converts a string into a byte array by casting the characters into bytes,
	 * i.e the lower 8 bits of a char value are assigned to the corresponding
	 * byte.
	 * @param     s     the string to be converted.
	 */
	public static byte[] strToBytes(String s) {
		// Should use s.getBytes("8859_1") ?
		char[] ca = new char[s.length()];
		byte[] ba = new byte[s.length()];
		s.getChars(0,ca.length,ca,0);
		for (int i = 0; i < ca.length; i++) {
    			ba[i] = (byte)ca[i];
		}
		return ba;
	}
    
	public static boolean equal(byte[] b1, byte[] b2) {
	    if(b1.length != b2.length) {
	        return false;
	    }

	    for(int i=0; i < b1.length; i++) {
	        if(b1[i] != b2[i]) {
	            return false;
	        }
	    }

	    return true;
	}

	public static byte[] join(int i, byte[] ba) {
	    byte[] res = new byte[ba.length + 1];
	    res[0] = (byte) i;
	    System.arraycopy(ba, 0, res, 1, ba.length);
	    return res;
	}

	public static byte[] join(byte[] ba, int i) {
	    byte[] res = new byte[ba.length + 1];

	    i = i % 256;
	    res[ba.length] = (byte) ((i > 127) ? i - 256: i);
	    System.arraycopy(ba, 0, res, 0, ba.length);

	    return res;
	}

	public static byte[] join(byte[] ba1, byte[] ba2) {
	    byte[] res = new byte[ba1.length + ba2.length];
	    System.arraycopy(ba1, 0, res, 0, ba1.length);
	    System.arraycopy(ba2, 0, res, ba1.length, ba2.length);
	    return res;
	}

	public static String toString(byte[] b) {
		if (b == null) {
			return "null";
		}

        	StringBuffer sb = new StringBuffer();
		sb.append("[ ");

        	for (int i = 0; i < b.length; i++) {
			sb.append(Character.forDigit((b[i] >> 4) & 0x0f,16));
			sb.append(Character.forDigit(b[i] & 0x0f,16));
			sb.append(' ');
		}
		sb.append(']');

		return sb.toString();
	}

	public static void main(String[] arg) throws NumberFormatException {

	    int i = Integer.parseInt(arg[0]);
	    int l = Integer.parseInt(arg[1]);
	    byte[] b = itob(i, l);
	    String s = new String(b);

        System.out.println("Convert: " + i + " -> " + s + " ; " + b.length);
    }
}


