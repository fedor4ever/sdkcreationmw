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


package com.nokia.epdt.plugins.s60.winsock;

/* java.util */
import java.util.Properties;

/**
 * Parses Winsock ECMT messages
 *
 */
public abstract class WinsockMessageParser {

    // Parsing states
    private static final int PARSE_START = 0;
    private static final int PARSE_KEY = 1;
    private static final int PARSE_VALUE = 2;
    private static final int PARSE_STATE_COUNT = 3;

    /**
     * Parses the Winsock configuration message.
     *
     * @param data the message data
     * @param offset the offset where the data start
     * @return the parsed message parameters
     */
    public static Properties parseConfigMessage(byte [] data, int offset) {
        return parseConfigMessage(data, offset, data.length - offset);
    }

    /**
     * Parses the Winsock configuration message.
     *
     * @param data the message data
     * @param offset the offset where the data start
     * @param len the size of the data
     * @return the parsed message parameters
     */
    static Properties parseConfigMessage(byte [] data, int offset, int len) {
        Properties params = new Properties();
        // The body contains null separated key-value pairs of UCS2
        // strings in network (big endian, MSB first) byte order
        int state = PARSE_START;  // 0 = nothing, 1 = key, 2 = value
        StringBuffer key = new StringBuffer();
        StringBuffer value = new StringBuffer();
        for (int i=offset+1; i<len; i+=2) {
            int msb = ((int)data[i-1]) & 0xff;
            int lsb = ((int)data[i]) & 0xff;
            char c = (char)((msb << 8) | lsb);
            if (c == 0) {
                state = (state+1) % PARSE_STATE_COUNT;
                if (state == PARSE_START) {
                    params.setProperty(key.toString(), value.toString());
                    key.setLength(0);
                    value.setLength(0);
                }
            } else {
                switch (state) {
                case PARSE_START:  state = PARSE_KEY; // no break
                case PARSE_KEY:    key.append(c); break;
                case PARSE_VALUE:  value.append(c); break;
                }
            }
        }

        if (state == PARSE_VALUE) {
            // final key-value pair
            params.setProperty(key.toString(), value.toString());
        }

        return params;
    }
}
