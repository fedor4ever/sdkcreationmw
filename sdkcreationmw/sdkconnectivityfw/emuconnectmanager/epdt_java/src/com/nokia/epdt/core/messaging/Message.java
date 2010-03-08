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


package com.nokia.epdt.core.messaging;

import java.io.UnsupportedEncodingException;
import org.apache.log4j.Logger;

import com.nokia.epdt.core.Constants;

/**
 * Message
 *
 * message = header, body;
 * header = message length, client uid;
 * message length = hexadecimal integer;
 * client uid = hexadecimal integer;
 * body = byte, {byte};
 * hexadecimal integer = ’0’, ’x’, 8 * hexadecimal character;
 * hexadecimal character = '0'|’1’|’2’|’3’|’4’|’5’|’6’|’7’|’8’|’9’|’a’|’b’|’d’|’e’|’f’;
 */
public final class Message implements Constants {
    private static Logger log = Logger.getLogger(Message.class);
    private int uid;
    private byte[] body;
    private byte[] message;

    /**
     * Creates a new instance of Message
     * @param uid message unique identifier
     * @param body message body (not copied)
     */
    public Message(int uid, byte[] body) {
        this.uid = uid;
        this.body = ((body != null) ? body : new byte[0]);
    }

    /** A table of hex digits */
    private static final byte[] HEX = {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    /**
     * Creates a string representation of the integer argument as an unsigned
     * integer in base 16. The unsigned integer value is the argument plus 2^32
     * if the argument is negative; otherwise, it is equal to the argument.
     * This value is converted to a string of ASCII digits in hexadecimal
     * (base 16) with extra leading '0x' and '0's.
     *
     * @param hex the destination buffer
     * @param offset offset in the destination buffer
     * @param value an integer
     */
    private static void toHex(byte [] hex, int offset, int value) {
        hex[offset] = '0';
        hex[offset+1] = 'x';
        for (int i=0; i<8; i++) {
            hex[offset+9-i] = HEX[value & 0xf];
            value >>= 4;
        }
    }

    private static final byte MIN_HEX = (byte)0x30;	// '0'
    private static final byte MAX_HEX = (byte)0x66;	// 'f'
    private static final int HEX_TO_INT[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,   // 0x30 .. 0x39
        -1, -1, -1, -1, -1, -1, -1,     // 0x3a .. 0x40
        10, 11, 12, 13, 14, 15,	        // 0x41 .. 0x46
        -1, -1, -1, -1, -1, -1, -1,     // 0x47 .. 0x4d
        -1, -1, -1, -1, -1, -1, -1,     // 0x4e .. 0x54
        -1, -1, -1, -1, -1, -1, -1,     // 0x55 .. 0x5b
        -1, -1, -1, -1, -1,             // 0x5c .. 0x60
        10, 11, 12, 13, 14, 15          // 0x61 .. 0x66
    };

    /**
     * Efficient conversion of ASCII hext representation into an integer
     * number. The hex number of 10 characters long and must begin with
     * "0x" prefix. Note that this method doesn't create a single object
     * unless it throws an exception. It only operates primitive types.
     *
     * @param hex the message bytes
     * @return the integer value
     * @throws NumberFormatException if the input is invalid
     */
    private static int fromHex(byte hex[]) throws MessageFormatException {
        int n = 0;
        if (hex == null || hex.length == 0) {
            throw new NumberFormatException();
        } else if (hex.length < 10) {
            mfx(hex);
        } else {
            byte x = hex[1];
            if (hex[0] != '0') mfx(hex);
            if (x != 'x' && x != 'X') mfx(hex);
            for (int i=2; i<=9; i++) {
                byte b = hex[i];
                if (b >= MIN_HEX && b <= MAX_HEX) {
                    int nibble = HEX_TO_INT[b-MIN_HEX];
                    if (nibble < 0) mfx(hex);
                    n = (n << 4) | nibble;
                }
            }
        }
        return n;
    }

    /**
     * Throws nicely formatted MessageFormatException
     * @param bytes the offending bytes
     * @throws MessageFormatException always thrown
     */
    private static void mfx(byte[] bytes) throws MessageFormatException {
        try {
            int n = bytes.length;
            if (n > 10) n = 10;
            else if (n < 0) n = 0;
            throw new MessageFormatException(new String(bytes,0,n,"ASCII"));
        } catch (UnsupportedEncodingException x) {
            throw new MessageFormatException();
        }
    }

    /**
     * Gets the entire message, including the length/uid header.
     * @return the entire message.
     */
    public byte[] getMessage() {
        if (message == null) {
            byte [] msg = new byte[getLength()];
            toHex(msg, 0, msg.length);
            toHex(msg, 10, uid);
            System.arraycopy(body, 0, msg, 20, body.length);
            message = msg;
        }
        return message;
    }

    /**
     * Getter for the message body.
     * @return the message body.
     */
    public byte[] getBody() {
        return body;
    }

    /**
     * Getter for the message length, including the header.
     * @return the length of the entire message
     */
    public int getLength() {
        return body.length + 20;
    }

    /**
     * Getter for property uid.
     * @return Value of property uid.
     */
    public int getUid() {
        return uid;
    }

    /**
     * Returns a string representation of the object.
     *
     * The toString method for class Message returns a string consisting of
     * the length of the message data, uid of the message and data.
     *
     * @return a string representation of the object.
     */
    public String toString() {
        try {
            return "Message[length=" + getLength() + ",uid=0x" +
                Integer.toHexString(uid) + ",data=" +
                new String(body,"ASCII") + "]";
        } catch (Exception x) {
            return x.getMessage();
        }
    }

    /**
     * Parses the byte array argument as a message object.
     * @param length message length
     * @param uid_body_array byte array representing message uid and body as bytes
     * @return the message object
     * @throws MessageFormatException if the byte array cannot be parsed
     * 			              as a message object.
     */
    public static Message parseMessage(int length, byte[] uid_body_array) throws MessageFormatException {
        // Message length value and true length have to be same
        if (uid_body_array.length + MESSAGE_UID_HEX_NUMERAL_LENGTH == length) {
            return parseMessage(uid_body_array);
        } else {
            throw new MessageFormatException("Message length is incorrect");
        }
    }

    /**
     * Parses the byte array argument as a message object.
     * @param uid_body_array byte array representing message uid and body as bytes
     * @return the message object
     * @throws MessageFormatException if the byte array cannot be parsed
     * 			              as a message object.
     */
    public static Message parseMessage(byte[] uid_body_array) throws MessageFormatException {
        // Try to get message uid
        if (uid_body_array.length >= MESSAGE_UID_HEX_NUMERAL_LENGTH) {
            int uid = fromHex(uid_body_array);
            // Try to get message body
            byte[] body = new byte[uid_body_array.length - MESSAGE_UID_HEX_NUMERAL_LENGTH];
            System.arraycopy(uid_body_array, MESSAGE_UID_HEX_NUMERAL_LENGTH, body, 0, body.length);
            if (log.isDebugEnabled()) {
                log.debug("Message for 0x" + Integer.toHexString(uid) +
                    ", " + body.length + " bytes");
            }
            return new Message(uid, body);
        } else {
            throw new MessageFormatException("Message UID is incorrect");
        }
    }

    /**
     * Parse hex byte array to integer
     * @throws MessageFormatException if the byte array cannot be parsed as a integer.
     */
    public static int parseInt(byte[] bytes) throws MessageFormatException {
        if (bytes.length != 10) throw new MessageFormatException();
        return fromHex(bytes);
    }
}
