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

/* java.io */
import java.io.IOException;
import java.io.OutputStream;
import java.io.ByteArrayOutputStream;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;

/* com.nokia.epdt.core.messaging */
import com.nokia.epdt.core.messaging.Message;

/**
 * Creates Winsock ECMT messages
 *
 */
public abstract class WinsockMessageFactory implements WinsockConstants {

    /**
     * Creates a new configuration request message. The returned information
     * may or may not contain proxy information, depending on whether Wisock
     * is enabled.
     *
     * @return a new message
     */
    public static Message createConfigRequestMessage() {
        byte [] data = new byte [] { MESSAGE_TYPE_CONFIG_REQUEST };
        return new Message(WINSOCK_TARGET_UID, data);
    }

    /**
     * Creates a new proxy configuration request message.
     *
     * @return a new message
     */
    public static Message createProxyRequestMessage() {
        byte [] data = new byte [] { MESSAGE_TYPE_PPOXY_CONFIG_REQUEST };
        return new Message(WINSOCK_TARGET_UID, data);
    }

    /**
     * Creates a new configuration message.
     *
     * @param ps the data model
     * @return a new message
     * @throws ValidateException if the model is in an invalid state
     */
    public static Message createConfigMessage(PropertySet ps) {
        ByteArrayOutputStream buf = new ByteArrayOutputStream();
        buf.write(MESSAGE_TYPE_CONFIG);
        try {
            writeProperty(buf, ps, USE_WINSOCK_PROP);
            writeProperty(buf, ps, WINSOCK_USE_PROXY_PROP);
            writeProperty(buf, ps, WINSOCK_PROXY_SERVER_PROP);
            writeProperty(buf, ps, WINSOCK_PROXY_PORT_PROP);
        } catch (IOException x) {
            // never thrown
        }

        return new Message(WINSOCK_TARGET_UID, buf.toByteArray());
    }

    /**
     * Writes a string to the output stream, in the configuration message
     * format. The string is written as a null terminated sequence of UCS2
     * characters in network (big endian, MSB first) byte order.
     *
     * @param out the stream to write to
     * @param s the string to write.
     * @throws IOException in unlikely case of I/O error
     */
    private static void writeString(OutputStream out, String s)
    throws IOException {
        if (s != null) {
            int len = s.length();
            for (int i=0; i<len; i++) {
                char c = s.charAt(i);
                out.write((c >> 8) & 0xff);  // msb
                out.write(c & 0xff);         // lsb
            }
        }
        out.write(0);
        out.write(0);
    }

    /**
     * Writes a single property as a pair of null terminated UCS2 strings
     * in network (big endian, MSB first) byte order.
     *
     * @param out the stream to write to
     * @param ps the property set
     * @param key the property name
     * @throws IOException in unlikely case of I/O error
     */
    private static void writeProperty(OutputStream out, PropertySet ps, String key)
    throws IOException {
        Property p = ps.getProperty(key);
        if (p != null) {
            writeString(out, key);
            writeString(out, p.getValueAsText());
        }
    }
}
