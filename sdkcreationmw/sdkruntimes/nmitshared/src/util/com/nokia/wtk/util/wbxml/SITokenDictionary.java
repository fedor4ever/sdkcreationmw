/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/**
 * SITokenDictionary.java                   2000/03/23
 */

// ========================================================================
//  Name        : SITokenDictionary.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.wtk.util.wbxml;

/* java.util */
import java.util.Date;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.BASE64;
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.CharacterEncoding;

/**
 * A WBXML TokenDictionary for Binary Service Indication messages
 *
 */
class SITokenDictionary implements WBXMLParser.TokenDictionary {
    /**
     * Creates a new SITokenDictionary.
     *
     * @param version the SI version (Currently, only "1.0")
     */
    public SITokenDictionary(String version) {
        this.version = version;
        if ("1.0".equals(version)) {
            tagSet = tagSet_1_0;
            startSet = startSet_1_0;
            valueSet = valueSet_1_0;
        }
        // Defaults to empty token tables
    }

    /**
     * Gets the version string for the dictionary.
     * @return the content version string
     */
    public String getVersion() {
        return this.version;
    }

    /**
     * Gets the content (MIME) type supported by the dictionary.
     * @return the content type
     */
    public String getContentType() {
        return MimeType.PUSH_SI_BINARY;
    }

    /**
     * Gets the string representation of a tag token.
     *
     * @param token the token value
     * @return the string representation of the tag
     */
    public String getTag(int codepage, byte token) {
        String tag = null;
        for (int i = 0; (i < tagSet.length) && (tag == null); i++) {
            if (tagSet[i].token == token) {
                tag = tagSet[i].tag;
            }
        }
        return tag;
    }

    /**
     * Gets the string representation of an attribute start token.
     *
     * @param token the token value
     * @param valueReturn a single-element array for returning
     *                    an implicit attribute value if it exists
     * @return the string representation of the attribute start token
     */
    public String getAttrStart(int codepage, byte token, String[] valueReturn) {
        String start = null;
        for (int i = 0; (i < startSet.length) && (start == null); i++) {
            if (startSet[i].token == token) {
                start = startSet[i].start;
                valueReturn[0] = startSet[i].value;
            }
        }
        return start;
    }

    /**
     * Gets the string representation of an attribute value token.
     *
     * @param token the token value
     * @return the string representation of the attribute value
     */
    public String getAttrValue(int codepage, byte token) {
        String value = null;
        for (int i = 0; (i < valueSet.length) && (value == null); i++) {
            if (valueSet[i].token == token) {
                value = valueSet[i].value;
            }
        }
        return value;
    }

    /**
     * Gets the string representation of a string extension.  SI doesn't support these.
     *
     * @param token the token value (EXT_I_0, EXT_I_1, or EXT_I_2)
     * @param data the character data for the extension.
     *             This is the data that followed the token as an inline string.
     * @return the string representation of the extension
     */
    public String getExtensionValue(byte token, String data) {
        String value = null;
        switch (token) {
        case WBXMLParser.GlobalToken.EXT_I_0:
        case WBXMLParser.GlobalToken.EXT_I_1:
        case WBXMLParser.GlobalToken.EXT_I_2:
            value = "";     // Accept silently
            break;
        }
        return value;
    }

    /**
     * Gets the string representation of a integer extension.
     *
     * @param token the token value (EXT_T_0, EXT_T_1, or EXT_T_2)
     * @param data the integer data for the extension
     * @param stringTable the string table for use by the extension handler
     * @return the string representation of the extension
     */
    public String getExtensionValue(byte token, int data, WBXMLParser.StringTable stringTable) {
        String value = null;
        switch (token) {
        case WBXMLParser.GlobalToken.EXT_T_0:
        case WBXMLParser.GlobalToken.EXT_T_1:
        case WBXMLParser.GlobalToken.EXT_T_2:
            value = "";     // Accept silently
            break;
        }
        return value;
    }

    /**
     * Gets the string representation of a single-byte extension.
     *
     * @param token the token value (EXT_0, EXT_1, or EXT_2)
     * @return the string representation of the extension
     */
    public String getExtensionValue(byte token) {
        String value = null;
        switch (token) {
        case WBXMLParser.GlobalToken.EXT_0:
        case WBXMLParser.GlobalToken.EXT_1:
        case WBXMLParser.GlobalToken.EXT_2:
            value = "";     // Accept silently
            break;
        }
        return value;
    }

    /**
     * Gets the filtered string representation of character data.
     *
     * @param tag the name of the element being parsed or <code>null</code> if PI.
     * @param target the name of the element attribute being parsed
     *               or, if <code>tag</code> is <code>null</code>, then
     *               this is the processing instruction target being parsed.
     *               If if <code>target</code> is <code>null</code>, then
     *               element content is being parsed.
     * @param cdata the infiltered character data
     * @return the filtered character data
     */
    public String filterCDATA(String tag, String target, String cdata) {
        return cdata;
    }

    /**
     * Gets the string representation of opaque data.  The date field is encoded
     * in OPAQUE data for 'si-expires' and 'created' attributes.
     *
     * @param tag the name of the element being parsed or <code>null</code> if PI.
     * @param target the name of the element attribute being parsed
     *               or, if <code>tag</code> is <code>null</code>, then
     *               this is the processing instruction target being parsed.
     *               If if <code>target</code> is <code>null</code>, then
     *               element content is being parsed.
     * @param bytes the opaque data
     * @param enc the character encoding opaque data
     * @return the filtered opaque data as a string
     */
    public String filterOpaqueData(String tag, String target, byte[] bytes, CharacterEncoding enc) {
            //
            // Date field
            //
            if (tag != null && tag.equals("indication") && target != null) {
                    String newDate = convertDate(bytes);
                    if (newDate != null) {
                            // System.out.println("SITokDict: date decoded for attr " + target + " as " + newDate);
                            return newDate;
                    }
            }
            //
            // Unknown opaque.  Use Base64 encoding.
            //
        String outStr = BASE64.encode(bytes);
// System.out.println("SITokDict: Unknown Opaque data for attr " + target);
        return outStr;
    }

    /**
     * Get date field.  Per SI Spec, date MUST be encoded with
     * each number in the string represented by its 4-bit binary
     * value.  Any non-numeric characters are discarded.
     *
     * Example:
     *	"1999-04-30T06:40:00Z" is encoded as hex octets:
     *		19, 99, 04, 30, 06, 40
     *
     * @return String representation of this format or null on error.
     */
    private static String convertDate(byte[] b) {
        Date d = null;
        String dateFieldTable[] = {
                null, null, null, "00", "00", "00"
        };
        //
        // We need at least the year, month and day fields.
        //
        if (b.length < 4) {
                // System.out.println("SITokDict:Date field too short");
                return null;
        }
        StringBuffer sb = new StringBuffer();
        int i;
        for (i = 0; i < b.length; i++) {
                byte n1 = (byte)((b[i] & 0xF0) >> 4);
                byte n2 = (byte)(b[i] & 0x0F);
                char c1 = (char)(n1 + '0');
                char c2 = (char)(n2 + '0');
                //
                // Once we are beyond each field, we assign the string
                // and start accumulating the next field.
                //
                switch (i) {
                   case 2:	// Year
                   case 3:	// Month
                   case 4:	// Day
                   case 5:	// Hour
                   case 6:	// Minute
                        dateFieldTable[i - 2] = sb.toString();
                        sb = new StringBuffer();
                        break;
                }
                sb.append(c1);
                sb.append(c2);
        }

        // Last field
        dateFieldTable[i - 2] = sb.toString();
        //
        // Format the string
        //
        return new String(
                dateFieldTable[0] + "-" +
                dateFieldTable[1] + "-" +
                dateFieldTable[2] + "T" +
                dateFieldTable[3] + ":" +
                dateFieldTable[4] + ":" +
                dateFieldTable[5] + "Z");

    }

    /**
     * Inner Class: A tag(element) definition.
     */
    private static class TagToken {
        TagToken(int token, String tag) {
            this.token = (byte)token;
            this.tag = tag;
        }

        byte token;
        String tag;
    }   // End of TagToken


    /**
     * Inner Class: An attribute start definition.
     */
    private static class AttrStart {
        AttrStart(int token, String start, String value) {
            this.token = (byte)token;
            this.start = start;
            this.value = value;
        }

        byte token;
        String start;
        String value;
    }   // End of AttrStart


    /**
     * Inner Class: An attribute value definition.
     */
    private static class AttrValue {
        AttrValue(int token, String value) {
            this.token = (byte)token;
            this.value = value;
        }

        byte token;
        String value;
    }   // End of AttrValue


    /**
     * The set of available tag token definitions for SI 1.0
     */
    private static final TagToken[] tagSet_1_0 = {
        new TagToken(0x05,  "si"),
        new TagToken(0x06,  "indication"),
        new TagToken(0x07,  "info"),
        new TagToken(0x08,  "item"),
    };


    /**
     * The set of available attribute start definitions for SI 1.0
     */
    private static final AttrStart[] startSet_1_0 = {
        new AttrStart(0x05, "action",   "signal-none"),
        new AttrStart(0x06, "action",   "signal-low"),
        new AttrStart(0x07, "action",   "signal-medium"),
        new AttrStart(0x08, "action",   "signal-high"),
        new AttrStart(0x09, "action",   "delete"),
        new AttrStart(0x0A, "created",  null),
        new AttrStart(0x0B, "href", null),
        new AttrStart(0x0C, "href", "http://"),
        new AttrStart(0x0D, "href", "http://www."),
        new AttrStart(0x0E, "href", "https://"),
        new AttrStart(0x0F, "href", "https://www."),
        new AttrStart(0x10, "si-expires",   null),
        new AttrStart(0x11, "si-id",    null),
        new AttrStart(0x12, "class",    null)
    };


    /**
     * The set of available attribute value definitions for SI 1.0
     */
    private static final AttrValue[] valueSet_1_0 = {
        new AttrValue(0x85, ".com/"),
        new AttrValue(0x86, ".edu/"),
        new AttrValue(0x87, ".net/"),
        new AttrValue(0x88, ".org/"),
    };


    /** The SI version. */
    private String version;

    /** The set of available tag token definitions. */
    private TagToken[] tagSet = {};

    /** The set of available start definitions. */
    private AttrStart[] startSet = {};

    /** The set of available value definitions. */
    private AttrValue[] valueSet = {};
}   // End of SITokenDictionary
