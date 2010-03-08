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
 * DRTokenDictionary.java                   2000/03/23
 */

// ========================================================================
//  Name        : DRTokenDictionary.java
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
import com.nokia.wtk.util.BASE64;
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.CharacterEncoding;

/**
 * A WBXML TokenDictionary for WAP-Encoded Digital Rights messages.
 *
 */
class DRTokenDictionary implements WBXMLParser.TokenDictionary {
    /**
     * Creates a new DRTokenDictionary.
     *
     * @param version the DR version (Currently, only "1.0")
     */
    public DRTokenDictionary(String version) {
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
        return MimeType.DRM_RIGHTS_BIN;
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
     * Gets the string representation of a string extension.  DR doesn't support these.
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
     * Base64-Encode KeyValue.
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
        if (tag != null && tag.equals("KeyValue") && target != null) {
            String keyVal = BASE64.encode(cdata.getBytes());
            if (keyVal != null) {
                System.out.println("DRTokDict: keyval encoded for attr " + target + " as " + keyVal);
                return keyVal;
            }
        }
        return cdata;
    }

    /**
     * Gets the string representation of opaque data.  The date field is encoded
     * in OPAQUE data for the 'KeyValue' field.
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
        // Unknown opaque.  Use Base64 encoding.
        //
        String outStr = BASE64.encode(bytes);
        System.out.println("DRTokDict: Unknown Opaque data for attr " + target + ":" + outStr);
        return outStr;
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
     * The set of available tag token definitions for DR 1.0
     */
    private static final TagToken[] tagSet_1_0 = {
        new TagToken(0x05,  "o-ex:rights"),
        new TagToken(0x06,  "o-ex:context"),
        new TagToken(0x07,  "o-dd:version"),
        new TagToken(0x08,  "o-dd:uid"),
        new TagToken(0x09,  "o-ex:agreement"),
        new TagToken(0x0A,  "o-ex:asset"),
        new TagToken(0x0B,  "ds:KeyInfo"),
        new TagToken(0x0C,  "ds:KeyValue"),
        new TagToken(0x0D,  "o-ex:permission"),
        new TagToken(0x0E,  "o-dd:play"),
        new TagToken(0x0F,  "o-dd:display"),
        new TagToken(0x10,  "o-dd:execute"),
        new TagToken(0x11,  "o-dd:print"),
        new TagToken(0x12,  "o-ex:constraint"),
        new TagToken(0x13,  "o-dd:count"),
        new TagToken(0x14,  "o-dd:datetime"),
        new TagToken(0x15,  "o-dd:start"),
        new TagToken(0x16,  "o-dd:end"),
        new TagToken(0x17,  "o-dd:interval"),
    };

    /**
     * The set of available attribute start definitions for DR 1.0
     */
    private static final AttrStart[] startSet_1_0 = {
        new AttrStart(0x05, "o-ex:rights", "xmlns:o-ex"),
        new AttrStart(0x06, "o-ex:rights", "xmlns:o-dd"),
        new AttrStart(0x07, "o-ex:rights", "xmlns:ds")
    };


    /**
     * The set of available attribute value definitions for DR 1.0
     */
    private static final AttrValue[] valueSet_1_0 = {
        new AttrValue(0x85, "http://odrl.net/1.1/ODRL-EX"),	// ODRL Expression Language
        new AttrValue(0x86, "http://odrl.net/1.1/ODRL-DD"),	// ODRL Data Dictionary
        new AttrValue(0x87, "http://www.w3.org/2000/09/xmldsig#/"),	// XML Digital Signature
    };


    /** The DR version. */
    private String version;

    /** The set of available tag token definitions. */
    private TagToken[] tagSet = {};

    /** The set of available start definitions. */
    private AttrStart[] startSet = {};

    /** The set of available value definitions. */
    private AttrValue[] valueSet = {};

}   // End of DRTokenDictionary
