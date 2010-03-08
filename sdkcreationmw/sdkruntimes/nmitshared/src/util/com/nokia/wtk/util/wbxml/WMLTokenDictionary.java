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
 * WMLTokenDictionary.java                  2000/03/23
 */

// ========================================================================
//  Name        : WMLTokenDictionary.java
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
import java.util.Vector;

/* org.xml.sax */
import org.xml.sax.SAXException;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.CharacterEncoding;

/**
 * A WBXML TokenDictionary for Binary WML
 *
 */
class WMLTokenDictionary implements WBXMLParser.TokenDictionary {
    /**
     * Creates a new WMLTokenDictionary.
     * @param version the WML version ("1.1", "1.2" or "1.3")
     */
    public WMLTokenDictionary(String version) {
        //TODO: codepage must be zero for all public methods.
        //      More work required if different codepages must be handled.
        this.version = version;
        tagSet = createTagSet(version);
        startSet = createStartSet(version);
        valueSet = createValueSet(version);
    }

    /**
     * Gets the default PublicID to use if not defined.
     * @return the default PublicID
     */
    static String getDefaultPublicID() {
        return DEFAULT_WML_PUBLICID;
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
        return MimeType.WML_BINARY;
    }

    /**
     * Gets the string representation of a tag token.
     * @param codepage the codepage for the token
     * @param token the token value
     * @return the string representation of the tag
     */
    public String getTag(int codepage, byte token) {
        String tag = null;
        if (codepage == 0) {    // Only code page 0 is recognized.
            for (int i = 0; (i < tagSet.length) && (tag == null); i++) {
                if (tagSet[i].codepage != codepage) {
                    continue;
                }
                if (tagSet[i].token == token) {
                    tag = tagSet[i].tag;
                }
            }
        }
        return tag;
    }

    /**
     * Gets the string representation of an attribute start token.
     * @param codepage the codepage for the token
     * @param token the token value
     * @param valueReturn a single-element array for returning
     *                    an implicit attribute value if it exists
     * @return the string representation of the attribute start token
     */
    public String getAttrStart(int codepage, byte token, String[] valueReturn) {
        String start = null;
        if (codepage == 0) {    // Only code page 0 is recognized.
            for (int i = 0; (i < startSet.length) && (start == null); i++) {
                if (startSet[i].codepage != codepage) {
                    continue;
                }
                if (startSet[i].token == token) {
                    start = startSet[i].start;
                    valueReturn[0] = startSet[i].value;
                }
            }
        }
        return start;
    }

    /**
     * Gets the string representation of an attribute value token.
     * @param codepage the codepage for the token
     * @param token the token value
     * @return the string representation of the attribute value
     */
    public String getAttrValue(int codepage, byte token) {
        String value = null;
        if (codepage == 0) {    // Only code page 0 is recognized.
            for (int i = 0; (i < valueSet.length) && (value == null); i++) {
                if (valueSet[i].codepage != codepage) {
                    continue;
                }
                if (valueSet[i].token == token) {
                    value = valueSet[i].value;
                }
            }
        }
        return value;
    }

    /**
     * Gets the string representation of a string extension.
     * @param token the token value (EXT_I_0, EXT_I_1, or EXT_I_2)
     * @param data the character data for the extension.
     *             This is the data that followed the token as an inline string.
     * @return the string representation of the extension
     */
    public String getExtensionValue(byte token, String data) {
        String value = null;
        switch (token) {
        case WBXMLParser.GlobalToken.EXT_I_0:
            // Variable Substitution - escaped
            // Name of the variable provided as the extension character data.
            if ((data != null) && (data.length() > 0)) {
                value = expandVariableReference(data, VARIABLE_ESCAPE);
            } else {
                //TODO: What to do if variable name not found?
            }
            break;
        case WBXMLParser.GlobalToken.EXT_I_1:
            // Variable Substitution - unescaped
            // Name of the variable provided as the extension character data.
            if ((data != null) && (data.length() > 0)) {
                value = expandVariableReference(data, VARIABLE_UNESCAPE);
            } else {
                //TODO: What to do if variable name not found?
            }
            break;
        case WBXMLParser.GlobalToken.EXT_I_2:
            // Variable Substitution - no transformation
            // Name of the variable provided as the extension character data.
            if ((data != null) && (data.length() > 0)) {
                value = expandVariableReference(data, VARIABLE_NOESCAPE);
            } else {
                //TODO: What to do if variable name not found?
            }
            break;
        }
        return value;
    }

    /**
     * Gets the string representation of a integer extension.
     * @param token the token value (EXT_T_0, EXT_T_1, or EXT_T_2)
     * @param data the integer data for the extension
     * @param stringTable the string table for use by the extension handler
     * @return the string representation of the extension
     */
    public String getExtensionValue(byte token, int data, WBXMLParser.StringTable stringTable) throws SAXException {
        String value = null;
        String varName;
        switch (token) {
        case WBXMLParser.GlobalToken.EXT_T_0:
            // Variable Substitution - escaped
            // Name of the variable is in the string table
            // at the offset indicated by the extension data.
            varName = stringTable.getString(data);
            if ((varName != null) && (varName.length() > 0)) {
                value = expandVariableReference(varName, VARIABLE_ESCAPE);
            } else {
                //TODO: What to do if variable name not found?
            }
            break;
        case WBXMLParser.GlobalToken.EXT_T_1:
            // Variable Substitution - unescaped
            // Name of the variable is in the string table
            // at the offset indicated by the extension data.
            varName = stringTable.getString(data);
            if ((varName != null) && (varName.length() > 0)) {
                value = expandVariableReference(varName, VARIABLE_UNESCAPE);
            } else {
                //TODO: What to do if variable name not found?
            }
            break;
        case WBXMLParser.GlobalToken.EXT_T_2:
            // Variable Substitution - no transformation
            // Name of the variable is in the string table
            // at the offset indicated by the extension data.
            varName = stringTable.getString(data);
            if ((varName != null) && (varName.length() > 0)) {
                value = expandVariableReference(varName, VARIABLE_NOESCAPE);
            } else {
                //TODO: What to do if variable name not found?
            }
            break;
        }
        return value;
    }

    /**
     * Gets the string representation of a single-byte extension.
     * @param token the token value (EXT_0, EXT_1, or EXT_2)
     * @return the string representation of the extension
     */
    public String getExtensionValue(byte token) {
        String value = null;
        switch (token) {
        case WBXMLParser.GlobalToken.EXT_0:
        case WBXMLParser.GlobalToken.EXT_1:
        case WBXMLParser.GlobalToken.EXT_2:
            // These tokens are reserved for future use
            value = "";     // Accept silently
            break;
        }
        return value;
    }

    /**
     * Gets the filtered string representation of character data.
     * @param tag the name of the element being parsed or <code>null</code> if PI.
     * @param target the name of the element attribute being parsed
     *               or, if <code>tag</code> is <code>null</code>, then
     *               this is the processing instruction target being parsed.
     *               If if <code>target</code> is <code>null</code>, then
     *               element content is being parsed.
     * @param cdata the unfiltered character data
     * @return the filtered character data
     */
    public String filterCDATA(String tag, String target, String cdata) {
        if ((cdata != null) && (cdata.length() > 0)) {
            // Expand any literal dollar sign characters
            // that the WML encoder has previously collapsed,
            // so that runtime variable syntax expansion will work properly.
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < cdata.length(); i++) {
                char c = cdata.charAt(i);
                if (c == '$') {
                    sbuf.append("$$");
                } else {
                    sbuf.append(c);
                }
            }
            cdata = sbuf.toString();
        }
        return cdata;
    }

    /**
     * Gets the string representation of opaque data.
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
        // It should be the case that only a CDATA Section may generate OPAQUE data.
        // But that's not how WML behaves at present, so therefore OPAQUE data is not allowed
        //return enc.decodeString(bytes, 0, bytes.length);
        return null;
    }

    /**
     * Gets the expanded variable reference for a variable name.
     * @param varName the variable name
     * @param type the type of variable expansion
     *             (<code>VARIABLE_ESCAPE</code>, <code>VARIABLE_UNESCAPE</code> or <code>VARIABLE_NOESCAPE</code>)
     * @return the expanded variable reference
     */
    private String expandVariableReference(String varName, int type) {
        String s = null;
        switch (type) {
        case VARIABLE_ESCAPE:
            s = "$(" + varName + ":escape)";
            break;
        case VARIABLE_UNESCAPE:
            s = "$(" + varName + ":unesc)";
            break;
        case VARIABLE_NOESCAPE:
            s = "$(" + varName + ":noesc)";
            break;
        }
        return s;
    }


    /**
     * Inner Class: A tag(element) definition.
     */
    private static class TagToken {
        TagToken(int codepage, int token, String tag) {
            this.codepage = (byte)codepage;
            this.token = (byte)token;
            this.tag = tag;
        }

        TagToken(int token, String tag) {
            this(0, token, tag);
        }

        byte codepage;
        byte token;
        String tag;
    }   // End of TagToken


    /**
     * Inner Class: An attribute start definition.
     */
    private static class AttrStart {
        AttrStart(int codepage, int token, String start, String value) {
            this.codepage = (byte)codepage;
            this.token = (byte)token;
            this.start = start;
            this.value = value;
        }

        AttrStart(int token, String start, String value) {
            this(0, token, start, value);
        }

        byte codepage;
        byte token;
        String start;
        String value;
    }   // End of AttrStart


    /**
     * Inner Class: An attribute value definition.
     */
    private static class AttrValue {
        AttrValue(int codepage, int token, String value) {
            this.codepage = (byte)codepage;
            this.token = (byte)token;
            this.value = value;
        }

        AttrValue(int token, String value) {
            this(0, token, value);
        }

        byte codepage;
        byte token;
        String value;
    }   // End of AttrValue


    /**
     * Creates a set of available tag token definitions for a WML version.
     */
    private static TagToken[] createTagSet(String version) {
        Vector v = new Vector();

        if ("1.1".equals(version) || "1.2".equals(version) || "1.3".equals(version)) {
            // Missing: 0x00 .. 0x1A
            if (!"1.1".equals(version)) {
                v.add(new TagToken(0x1B,  "pre"));
            }
            v.add(new TagToken(0x1C,  "a"));
            v.add(new TagToken(0x1D,  "td"));
            v.add(new TagToken(0x1E,  "tr"));
            v.add(new TagToken(0x1F,  "table"));
            v.add(new TagToken(0x20,  "p"));
            v.add(new TagToken(0x21,  "postfield"));
            v.add(new TagToken(0x22,  "anchor"));
            v.add(new TagToken(0x23,  "access"));
            v.add(new TagToken(0x24,  "b"));
            v.add(new TagToken(0x25,  "big"));
            v.add(new TagToken(0x26,  "br"));
            v.add(new TagToken(0x27,  "card"));
            v.add(new TagToken(0x28,  "do"));
            v.add(new TagToken(0x29,  "em"));
            v.add(new TagToken(0x2A,  "fieldset"));
            v.add(new TagToken(0x2B,  "go"));
            v.add(new TagToken(0x2C,  "head"));
            v.add(new TagToken(0x2D,  "i"));
            v.add(new TagToken(0x2E,  "img"));
            v.add(new TagToken(0x2F,  "input"));
            v.add(new TagToken(0x30,  "meta"));
            v.add(new TagToken(0x31,  "noop"));
            v.add(new TagToken(0x32,  "prev"));
            v.add(new TagToken(0x33,  "onevent"));
            v.add(new TagToken(0x34,  "optgroup"));
            v.add(new TagToken(0x35,  "option"));
            v.add(new TagToken(0x36,  "refresh"));
            v.add(new TagToken(0x37,  "select"));
            v.add(new TagToken(0x38,  "small"));
            v.add(new TagToken(0x39,  "strong"));
            // Missing: 0x3A
            v.add(new TagToken(0x3B,  "template"));
            v.add(new TagToken(0x3C,  "timer"));
            v.add(new TagToken(0x3D,  "u"));
            v.add(new TagToken(0x3E,  "setvar"));
            v.add(new TagToken(0x3F,  "wml"));
            // Missing: 0x40 .. 0xFF
        }

        TagToken[] tagTokens = new TagToken[v.size()];
        v.copyInto(tagTokens);
        return tagTokens;
    }


    /**
     * Creates a set of available start definitions for a WML version.
     */
    private static AttrStart[] createStartSet(String version) {
        Vector v = new Vector();

        if ("1.1".equals(version) || "1.2".equals(version) || "1.3".equals(version)) {
            // Missing: 0x00 .. 0x04
            v.add(new AttrStart(0x05, "accept-charset",     null));
            v.add(new AttrStart(0x06, "align",              "bottom"));
            v.add(new AttrStart(0x07, "align",              "center"));
            v.add(new AttrStart(0x08, "align",              "left"));
            v.add(new AttrStart(0x09, "align",              "middle"));
            v.add(new AttrStart(0x0A, "align",              "right"));
            v.add(new AttrStart(0x0B, "align",              "top"));
            v.add(new AttrStart(0x0C, "alt",                null));
            v.add(new AttrStart(0x0D, "content",            null));
            // Missing: 0x0E
            v.add(new AttrStart(0x0F, "domain",             null));
            v.add(new AttrStart(0x10, "emptyok",            "false"));
            v.add(new AttrStart(0x11, "emptyok",            "true"));
            v.add(new AttrStart(0x12, "format",             null));
            v.add(new AttrStart(0x13, "height",             null));
            v.add(new AttrStart(0x14, "hspace",             null));
            v.add(new AttrStart(0x15, "ivalue",             null));
            v.add(new AttrStart(0x16, "iname",              null));
            // Missing: 0x17
            v.add(new AttrStart(0x18, "label",              null));
            v.add(new AttrStart(0x19, "localsrc",           null));
            v.add(new AttrStart(0x1A, "maxlength",          null));
            v.add(new AttrStart(0x1B, "method",             "get"));
            v.add(new AttrStart(0x1C, "method",             "post"));
            v.add(new AttrStart(0x1D, "mode",               "nowrap"));
            v.add(new AttrStart(0x1E, "mode",               "wrap"));
            v.add(new AttrStart(0x1F, "multiple",           "false"));
            v.add(new AttrStart(0x20, "multiple",           "true"));
            v.add(new AttrStart(0x21, "name",               null));
            v.add(new AttrStart(0x22, "newcontext",         "false"));
            v.add(new AttrStart(0x23, "newcontext",         "true"));
            v.add(new AttrStart(0x24, "onpick",             null));
            v.add(new AttrStart(0x25, "onenterbackward",    null));
            v.add(new AttrStart(0x26, "onenterforward",     null));
            v.add(new AttrStart(0x27, "ontimer",            null));
            v.add(new AttrStart(0x28, "optional",           "false"));
            v.add(new AttrStart(0x29, "optional",           "true"));
            v.add(new AttrStart(0x2A, "path",               null));
            // Missing: 0x2B .. 0x2D
            v.add(new AttrStart(0x2E, "scheme",             null));
            v.add(new AttrStart(0x2F, "sendreferer",        "false"));
            v.add(new AttrStart(0x30, "sendreferer",        "true"));
            v.add(new AttrStart(0x31, "size",               null));
            v.add(new AttrStart(0x32, "src",                null));
            v.add(new AttrStart(0x33, "ordered",            "false"));
            v.add(new AttrStart(0x34, "ordered",            "true"));
            v.add(new AttrStart(0x35, "tabindex",           null));
            v.add(new AttrStart(0x36, "title",              null));
            v.add(new AttrStart(0x37, "type",               null));
            v.add(new AttrStart(0x38, "type",               "accept"));
            v.add(new AttrStart(0x39, "type",               "delete"));
            v.add(new AttrStart(0x3A, "type",               "help"));
            v.add(new AttrStart(0x3B, "type",               "password"));
            v.add(new AttrStart(0x3C, "type",               "onpick"));
            v.add(new AttrStart(0x3D, "type",               "onenterbackward"));
            v.add(new AttrStart(0x3E, "type",               "onenterforward"));
            v.add(new AttrStart(0x3F, "type",               "ontimer"));
            // Missing: 0x40 .. 0x44
            v.add(new AttrStart(0x45, "type",               "options"));
            v.add(new AttrStart(0x46, "type",               "prev"));
            v.add(new AttrStart(0x47, "type",               "reset"));
            v.add(new AttrStart(0x48, "type",               "text"));
            v.add(new AttrStart(0x49, "type",               "vnd."));
            v.add(new AttrStart(0x4A, "href",               null));
            v.add(new AttrStart(0x4B, "href",               "http://"));
            v.add(new AttrStart(0x4C, "href",               "https://"));
            v.add(new AttrStart(0x4D, "value",              null));
            v.add(new AttrStart(0x4E, "vspace",             null));
            v.add(new AttrStart(0x4F, "width",              null));
            v.add(new AttrStart(0x50, "xml:lang",           null));
            // Missing: 0x51
            v.add(new AttrStart(0x52, "align",              null));
            v.add(new AttrStart(0x53, "columns",            null));
            v.add(new AttrStart(0x54, "class",              null));
            v.add(new AttrStart(0x55, "id",                 null));
            v.add(new AttrStart(0x56, "forua",              "false"));
            v.add(new AttrStart(0x57, "forua",              "true"));
            v.add(new AttrStart(0x58, "src",                "http://"));
            v.add(new AttrStart(0x59, "src",                "https://"));
            v.add(new AttrStart(0x5A, "http-equiv",         null));
            v.add(new AttrStart(0x5B, "http-equiv",         "Content-Type"));
            v.add(new AttrStart(0x5C, "content",            "application/vnd.wap.wmlc;charset="));
            v.add(new AttrStart(0x5D, "http-equiv",         "Expires"));
            if (!"1.1".equals(version)) {
                v.add(new AttrStart(0x5E, "accesskey",      null));
                v.add(new AttrStart(0x5F, "enctype",        null));
                v.add(new AttrStart(0x60, "enctype",        "application/x-www-form-urlencoded"));
                v.add(new AttrStart(0x61, "enctype",        "multipart/form-data"));
            }
            if (!"1.1".equals(version) && !"1.2".equals(version)) {
                v.add(new AttrStart(0x62, "xml:space",      "preserve"));
                v.add(new AttrStart(0x63, "xml:space",      "default"));
                v.add(new AttrStart(0x64, "cache-control",  "no-cache"));
                v.add(new AttrStart(0x65, "href",           "#"));
                v.add(new AttrStart(0x66, "onenterforward", "#"));
                v.add(new AttrStart(0x67, "onenterbackward","#"));
                v.add(new AttrStart(0x68, "ontimer",        "#"));
                v.add(new AttrStart(0x69, "onpick",         "#"));
            }
            // Missing: 0x6A .. 0xFF
        }

        AttrStart[] startTokens = new AttrStart[v.size()];
        v.copyInto(startTokens);
        return startTokens;
    };


    /**
     * Creates a set of available value definitions for a WML version.
     */
    private static AttrValue[] createValueSet(String version) {
        Vector v = new Vector();

        if ("1.1".equals(version) || "1.2".equals(version) || "1.3".equals(version)) {
            // Missing: 0x80 .. 0x84
            v.add(new AttrValue(0x85, ".com/"));
            v.add(new AttrValue(0x86, ".edu/"));
            v.add(new AttrValue(0x87, ".net/"));
            v.add(new AttrValue(0x88, ".org/"));
            v.add(new AttrValue(0x89, "accept"));
            v.add(new AttrValue(0x8A, "bottom"));
            v.add(new AttrValue(0x8B, "clear"));
            v.add(new AttrValue(0x8C, "delete"));
            v.add(new AttrValue(0x8D, "help"));
            v.add(new AttrValue(0x8E, "http://"));
            v.add(new AttrValue(0x8F, "http://www."));
            v.add(new AttrValue(0x90, "https://"));
            v.add(new AttrValue(0x91, "https://www."));
            // Missing: 0x92
            v.add(new AttrValue(0x93, "middle"));
            v.add(new AttrValue(0x94, "nowrap"));
            v.add(new AttrValue(0x95, "onpick"));
            v.add(new AttrValue(0x96, "onenterbackward"));
            v.add(new AttrValue(0x97, "onenterforward"));
            v.add(new AttrValue(0x98, "ontimer"));
            v.add(new AttrValue(0x99, "options"));
            v.add(new AttrValue(0x9A, "password"));
            v.add(new AttrValue(0x9B, "reset"));
            // Missing: 0x9C
            v.add(new AttrValue(0x9D, "text"));
            v.add(new AttrValue(0x9E, "top"));
            v.add(new AttrValue(0x9F, "unknown"));
            v.add(new AttrValue(0xA0, "wrap"));
            v.add(new AttrValue(0xA1, "www.")); // Case corrected in v1.3, was "Www."
            // Missing: 0xA2 .. 0xFF
        }

        AttrValue[] valueTokens = new AttrValue[v.size()];
        v.copyInto(valueTokens);
        return valueTokens;
    };


    // WML variable expansion types:
    private static final int VARIABLE_ESCAPE =      0;
    private static final int VARIABLE_UNESCAPE =    1;
    private static final int VARIABLE_NOESCAPE =    2;

    /** The WML version. */
    private String version;

    /** The set of available tag token definitions. */
    private TagToken[] tagSet = {};

    /** The set of available start definitions. */
    private AttrStart[] startSet = {};

    /** The set of available value definitions. */
    private AttrValue[] valueSet = {};

    /**
     * The default public identifier to use in case it was unspecified in WBXML content.
     * This is defined as the oldest version supported by this token dictionary.
     */
    private final static String DEFAULT_WML_PUBLICID = "-//WAPFORUM//DTD WML 1.1//EN";
}   // End of WMLTokenDictionary
