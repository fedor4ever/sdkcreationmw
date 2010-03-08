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
 * SLTokenDictionary.java               2000/03/23
 */

// ========================================================================
//  Name        : SLTokenDictionary.java
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

/* com.nokia.wtk.util */
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.CharacterEncoding;

/**
 * A WBXML TokenDictionary for Binary Service Loading messages
 *
 */
class SLTokenDictionary implements WBXMLParser.TokenDictionary {
    /**
     * Creates a new SLTokenDictionary.
     *
     * @param version the SL version (Currently, only "1.0")
     */
    public SLTokenDictionary(String version) {
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
        return MimeType.PUSH_SL_BINARY;
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
     * Gets the string representation of a string extension.  SL doesn't support these.
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
     * Gets the string representation of opaque data.
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
        return enc.decodeString(bytes, 0, bytes.length);
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
     * The set of available tag token definitions for SL 1.0
     */
    private static final TagToken[] tagSet_1_0 = {
        new TagToken(0x05,  "sl"),
    };


    /**
     * The set of available attribute start definitions for SL 1.0
     */
    private static final AttrStart[] startSet_1_0 = {
        new AttrStart(0x05, "action",   "execute-low"),
        new AttrStart(0x06, "action",   "execute-high"),
        new AttrStart(0x07, "action",   "cache"),
        new AttrStart(0x08, "href", null),
        new AttrStart(0x09, "href", "http://"),
        new AttrStart(0x0A, "href", "http://www."),
        new AttrStart(0x0B, "href", "https://"),
        new AttrStart(0x0C, "href", "https://www."),
    };


    /**
     * The set of available attribute value definitions for SL 1.0
     */
    private static final AttrValue[] valueSet_1_0 = {
        new AttrValue(0x85, ".com/"),
        new AttrValue(0x86, ".edu/"),
        new AttrValue(0x87, ".net/"),
        new AttrValue(0x88, ".org/"),
    };


    /** The SL version. */
    private String version;

    /** The set of available tag token definitions. */
    private TagToken[] tagSet = {};

    /** The set of available start definitions. */
    private AttrStart[] startSet = {};

    /** The set of available value definitions. */
    private AttrValue[] valueSet = {};
}   // End of SLTokenDictionary
