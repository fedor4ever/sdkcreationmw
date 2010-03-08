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



package com.nokia.wtk.util.wbxml;

/* org.xml.sax */
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.helpers.DefaultHandler;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Enum;

/**
 * Utilities for WML documents.
 * <p>
 * Contains:
 * <ul>
 * <li>Typesafe enumerations used for WML Elements and Attributes.</li>
 * <li>SAX Document Handler.</li>
 * </ul>
 *
 */
public class WML {
    // Disallow public instantiation
    //private WML() {}

    /**
     * Typesafe enumeration for a WML tag used to define element types.
     */
    public static class Tag extends Enum {

        /**
         * Create a new WML Element Type enum value.
         * @param id identifier for enum value
         * @param isBlock if <code>true</code> then tag represents a block element
         */
        Tag(String id, boolean isBlock) {
            this(id, isBlock, 0);
        }

        /**
         * Create a new WML Element Type enum value.
         * @param id identifier for enum value
         * @param isBlock if <code>true</code> then tag represents a block element
         * @param bitIndex bitmap index associated with enum value
         */
        private Tag(String id, boolean isBlock, int bitIndex) {
            super(id);
            this.blockTag = isBlock;
            this.bitIndex = bitIndex;
        }

        /**
         * Return <code>true</code> if tag refers to a block,
         * i.e. it has both a start and an end token.
         */
        public boolean isBlock() {
            return blockTag;
        }

        /**
         * If the tag causes a line break to the flow of
         * data, then this method will return true.
         */
        private boolean breaksFlow() {
            return breakTag;
        }

        /**
         * Return the bit value if tag is one of a set of combinable bitmask values.
         */
        public int getBitIndex() {
            return bitIndex;
        }

        /**
         * Fetch a tag constant for a well-known tag name (i.e. one of
         * the tags in the set <code>allTags</code>).  If the given
         * name does not represent one of the well-known tags, then
         * null will be returned.
         */
        private static Tag get(String tagName) {
            for (int i = 0; i < allTags.length; i++ ) {
                if (allTags[i].getID().equals(tagName)) {
                    return allTags[i];
                }
            }
            return null;
        }

        // --- Tag Names -----------------------------------

        public static final int ActionField = 10;   // Bitmap index for "Action" fields

        public static final Tag A           = new Tag("a", true);
        public static final Tag ANCHOR      = new Tag("anchor", true);
        public static final Tag ACCESS      = new Tag("access", false);
        public static final Tag B           = new Tag("b", true, 3);
        public static final Tag BIG         = new Tag("big", true, 6);
        public static final Tag BR          = new Tag("br", false);
        public static final Tag CARD        = new Tag("card", true);
        public static final Tag DO          = new Tag("do", true);
        public static final Tag EM          = new Tag("em", true, 1);
        public static final Tag FIELDSET    = new Tag("fieldset", true);
        public static final Tag GO          = new Tag("go", true);
        public static final Tag HEAD        = new Tag("head", true);
        public static final Tag I           = new Tag("i", true, 4);
        public static final Tag IMG         = new Tag("img", false, 8);
        public static final Tag INPUT       = new Tag("input", false);
        public static final Tag META        = new Tag("meta", false);
        public static final Tag NOOP        = new Tag("noop", false);
        public static final Tag ONEVENT     = new Tag("onevent", true);
        public static final Tag OPTGROUP    = new Tag("optgroup", true);
        public static final Tag OPTION      = new Tag("option", true);
        public static final Tag P           = new Tag("p", true);
        public static final Tag POSTFIELD   = new Tag("postfield", true);
        public static final Tag PRE         = new Tag("pre", true, 9);
        public static final Tag PREV        = new Tag("prev", true);
        public static final Tag REFRESH     = new Tag("refresh", true);
        public static final Tag SELECT      = new Tag("select", true);
        public static final Tag SETVAR      = new Tag("setvar", false);
        public static final Tag SMALL       = new Tag("small", true, 7);
        public static final Tag STRONG      = new Tag("strong", true, 2);
        public static final Tag TABLE       = new Tag("table", true);
        public static final Tag TD          = new Tag("td", true);
        public static final Tag TEMPLATE    = new Tag("template", true);
        public static final Tag TIMER       = new Tag("timer", false);
        public static final Tag TR          = new Tag("tr", true);
        public static final Tag U           = new Tag("u", true, 5);
        public static final Tag WML         = new Tag("wml", true);

        /**
         * All text content must be in a paragraph element.
         * If a paragraph didn't exist when content was
         * encountered, a paragraph is manufactured.
         */
        public static final Tag P_IMPLIED = new Tag("p-implied", true);

        /**
         * An implied break tag.
         */
        public static final Tag BR_IMPLIED = new Tag("br-implied", false);

        /**
         * An implied whitespace tag.
         */
        public static final Tag WS_IMPLIED = new Tag("ws-implied", false);

        /**
         * All text content is labeled with this tag.
         * <p>
         * This is a tag synthesized by the WML DocumentHandler.
         * Since elements are identified by their tag type,
         * we create a some fake tag types to mark the elements
         * that were manufactured.
         */
        public static final Tag _CONTENT_ = new Tag("XML Content Text", false);

        /**
         * All processing instructions are labeled with this tag.
         * <p>
         * This is a tag synthesized by the WML DocumentHandler.
         * Since elements are identified by their tag type,
         * we create a some fake tag types to mark the elements
         * that were manufactured.
         */
        public static final Tag _PI_ = new Tag("XML Processing Instruction", false);

        /**
         * All processing instructions are labeled with this tag.
         * <p>
         * This is a tag synthesized by the WML DocumentHandler.
         * Since elements are identified by their tag type,
         * we create a some fake tag types to mark the elements
         * that were manufactured.
         */
        public static final Tag _COMMENT_ = new Tag("XML Comment", false);


        /**
         * This is the set of actual wml tags that
         * are known about by the default wml reader.
         * This set does not include tags that are
         * manufactured by the reader.
         */
        private static Tag allTags[]  = {
            A,                  ANCHOR,             ACCESS,
            B,                  BIG,                BR,
            CARD,               DO,                 EM,
            FIELDSET,           GO,                 HEAD,
            I,                  IMG,                INPUT,
            META,               NOOP,               PRE,
            PREV,               P,                  POSTFIELD,
            ONEVENT,            OPTGROUP,           OPTION,
            REFRESH,            SELECT,             SETVAR,
            SMALL,              STRONG,             TABLE,
            TD,                 TR,                 TEMPLATE,
            TIMER,              U,                  SETVAR,
            WML
        };

        private boolean blockTag;
        private boolean breakTag;
        private int bitIndex;
    }   // End of Tag


    /**
     * Need a tag for "unknown" tags - those that form the "LITERAL"
     * global tokens (there are four of them) defined in the BXML
     * specification.
     */
    public static class UnknownTag extends Tag {
        /**
         * Create a new Unknown WML Element Type enum value.
         * @param id identifier for enum value
         * @param isBlock if <code>true</code> then tag represents a block element
         */
        public UnknownTag(String id, boolean isBlock) {
            super(((id != null) && (id.length() > 0) ? id : "unknown"), isBlock);
        }
    }   // End of UnknownTag


    /**
     * Typesafe enumeration representing a WML Attribute.
     */
    public static class Attribute extends Enum {

        /**
         * Create a new WML Attribute enum value
         * @param id identifier for enum value
         */
        Attribute(String id) {
            super(id);
        }

        /**
         * Fetch an attribute constant for a well-known attribute name
         * (i.e. one of the attributes in the set <code>allAttrs</code>).
         * If the given name does not represent one of the well-known
         * attributes, then null will be returned.
         */
        private static Attribute get(String attrName) {
            for (int i = 0; i < allAttrs.length; i++ ) {
                if (allAttrs[i].getID().equals(attrName)) {
                    return allAttrs[i];
                }
            }
            return null;
        }

        // --- Attribute Names -----------------------------------

        public static final Attribute ACCEPT_CHARSET = new Attribute("accept-charset");
        public static final Attribute ACCESSKEY = new Attribute("accesskey");
        public static final Attribute ALIGN = new Attribute("align");
        public static final Attribute ALT = new Attribute("alt");
        public static final Attribute CACHE_CONTROL = new Attribute("cache-control");
        public static final Attribute CLASS = new Attribute("class");
        public static final Attribute COLSPAN = new Attribute("colspan");
        public static final Attribute COLUMNS = new Attribute("columns");
        public static final Attribute CONTENT = new Attribute("content");
        public static final Attribute DOMAIN = new Attribute("domain");
        public static final Attribute EMPTYOK = new Attribute("emptyok");
        public static final Attribute ENCTYPE = new Attribute("enctype");
        public static final Attribute FORMAT = new Attribute("format");
        public static final Attribute FORUA = new Attribute("forua");
        public static final Attribute HEIGHT = new Attribute("height");
        public static final Attribute HREF = new Attribute("href");
        public static final Attribute HSPACE = new Attribute("hspace");
        public static final Attribute HTTP_EQUIV = new Attribute("http-equiv");
        public static final Attribute ID = new Attribute("id");
        public static final Attribute INAME = new Attribute("iname");
        public static final Attribute IVALUE = new Attribute("ivalue");
        public static final Attribute LABEL = new Attribute("label");
        public static final Attribute LOCALSRC = new Attribute("localsrc");
        public static final Attribute MAXLENGTH = new Attribute("maxlength");
        public static final Attribute METHOD = new Attribute("method");
        public static final Attribute MODE = new Attribute("mode");
        public static final Attribute MULTIPLE = new Attribute("multiple");
        public static final Attribute NAME = new Attribute("name");
        public static final Attribute NEWCONTEXT = new Attribute("newcontext");
        public static final Attribute ONENTERBACKWARD = new Attribute("onenterbackward");
        public static final Attribute ONENTERFORWARD = new Attribute("onenterforward");
        public static final Attribute ONPICK = new Attribute("onpick");
        public static final Attribute ONTIMER = new Attribute("ontimer");
        public static final Attribute OPTIONAL = new Attribute("optional");
        public static final Attribute ORDERED = new Attribute("ordered");
        public static final Attribute PATH = new Attribute("path");
        public static final Attribute ROWSPAN = new Attribute("rowspan");
        public static final Attribute SCHEME = new Attribute("scheme");
        public static final Attribute SENDREFERER = new Attribute("sendreferer");
        public static final Attribute SIZE = new Attribute("size");
        public static final Attribute SRC = new Attribute("src");
        public static final Attribute TABINDEX = new Attribute("tabindex");
        public static final Attribute TITLE = new Attribute("title");
        public static final Attribute TYPE = new Attribute("type");
        public static final Attribute VALUE = new Attribute("value");
        public static final Attribute VSPACE = new Attribute("vspace");
        public static final Attribute WIDTH = new Attribute("width");
        public static final Attribute XML_LANG = new Attribute("xml:lang");
        public static final Attribute XML_SPACE = new Attribute("xml:space");


        private static Attribute allAttrs[] = {
            ACCEPT_CHARSET,     ALIGN,              ALT,
            CACHE_CONTROL,      CLASS,              COLUMNS,
            CONTENT,            DOMAIN,             EMPTYOK,
            FORMAT,             FORUA,              HEIGHT,
            HREF,               HSPACE,             HTTP_EQUIV,
            ID,                 INAME,              IVALUE,
            LABEL,              LOCALSRC,           MAXLENGTH,
            METHOD,             MODE,               MULTIPLE,
            NAME,               NEWCONTEXT,         ONENTERBACKWARD,
            ONENTERFORWARD,     ONPICK,             ONTIMER,
            OPTIONAL,           ORDERED,            PATH,
            SCHEME,             SENDREFERER,        SIZE,
            SRC,                TABINDEX,           TITLE,
            TYPE,               VALUE,              VSPACE,
            WIDTH,              XML_LANG,           XML_SPACE
        };
    }   // End of Attribute


    public static class UnknownAttribute extends Attribute {

        /**
         * Create a new Unknown Attribute enum value
         * @param id identifier for enum value
         */
        public UnknownAttribute(String id) {
            super(id);
        }
    }   // End of UnknownAttribute


    /**
     * This is used in cases where the value for the attribute
     * has not been specified.
     */
    public static final String NULL_ATTRIBUTE_VALUE = "#DEFAULT";



    /**
     * Inner Class: A SAX ContentHandler that will create a WMLDeck.
     */
    public static class DocumentHandler extends DefaultHandler {

            private static void detailOut(String s) {
                //MessageWriters.DetailMsg.println("WMLDeckBuilder: " + s);
            }

        public void setDocumentLocator(Locator locator) {
            this.locator = locator;
        }

        public void startDocument() {
            detailOut("startDocument()");
        }

        public void endDocument() {
            detailOut("endDocument()");
        }

        public void startElement(String urlName, String locName, String qname, Attributes attrs) {
            WMLElement elem = createElement(qname);
            if (attrs != null) {
                for (int i = 0; i < attrs.getLength(); i++) {
                    String attrName = attrs.getQName(i);
                    String attrVal = attrs.getValue(i);
                    elem.putAttribute(createAttribute(attrName), attrVal);
                }
            }
            if (currElem != null) {
                currElem.addElement(elem);
            } else if (root == null) {
                // Save root element for query later:
                root = elem;
            }
            currElem = elem;
            if (currContent != null) {
                currContent = null;
            }
            StringBuffer sbuf = new StringBuffer();
            if (attrs != null) {
                sbuf.append('{');
                for (int i = 0; i < attrs.getLength(); i++) {
                    if (i > 0) sbuf.append(',');
                    sbuf.append(attrs.getQName(i));
                    sbuf.append('=');
                    sbuf.append('\"');
                    sbuf.append(attrs.getValue(i));
                    sbuf.append('\"');
                }
                sbuf.append('}');
            } else {
                sbuf.append("{}");
            }
            detailOut("startElement(qname=\"" + qname + "\", attrs=" + sbuf.toString() + ")");
        }

        public void endElement(String urlName, String locName, String qname) {
            if (currElem != null) {
                currElem = currElem.getParent();
            }
            if (currContent != null) {
                currContent = null;
            }
            detailOut("endElement(qname=\"" + qname + "\")");
        }

        public void characters(char[] buf, int offset, int len) {
            if ((currElem != null) && (buf != null) && (len > 0)) {
                if (currContent == null) {
                    currContent = new WMLContent();
                    currElem.addElement(currContent);
                }
                currContent.appendContent(new String(buf, offset, len));
            }
            detailOut("characters(\"" + new String(buf, offset, len) + "\")");
        }

        public void ignorableWhitespace(char[] buf, int offset, int len) {
            detailOut("ignorableWhitespace(\"" + new String(buf, offset, len) + "\")");
        }

        public void processingInstruction(String target, String data) {
            detailOut("processingInstruction(target=\"" + target + "\", data=\"" + data + "\")");
            if (currElem != null) {
                currElem.addElement(new XMLProcessingInstruction(target, data));
            }
        }

        /**
         * Creates a WML element from a tag name.
         * @param name the tag name
         * @return a WML element
         */
        private WMLElement createElement(String name) {
            WMLElement elem = null;
            if (name == null) {
                elem = new WMLElement(new WML.UnknownTag("", true));
            } else {
                WML.Tag elemTag = WML.Tag.get(name);
                if (elemTag == null) {
                    elem = new WMLElement(new WML.UnknownTag(name, true));
                } else if (elemTag.equals(WML.Tag.WML)) {
                    elem = new WMLDeck();
                } else {
                    elem = new WMLElement(elemTag);
                }
            }
            return elem;
        }


        /**
         * Creates a WML attribute from a tag name.
         * @param name the tag name
         * @return a WML attribute
         */
        private WML.Attribute createAttribute(String name) {
            WML.Attribute attr = null;
            if (name == null) {
                attr = new WML.UnknownAttribute("");
            } else {
                attr = WML.Attribute.get(name);
                if (attr == null) {
                    attr = new WML.UnknownAttribute(name);
                }
            }
            return attr;
        }

        /**
         * Gets the root element.
         */
        public WMLElement getRootElement() {
            return root;
        }

        /**
         * Gets the WML Deck root element.
         */
        public WMLDeck getRootWMLDeck() {
            if (root instanceof WMLDeck) {
                return (WMLDeck)root;
            } else {
                return null;
            }
        }

        private WMLElement root = null;
        private WMLElement currElem = null;
        private WMLContent currContent = null;
        private Locator locator = null;
    }   // End of WML.DocumentHandler


    /**
     * Inner Class: WML String utility functions.
     */
    public static class StringUtil {
        /**
         * Convert a string to WML-compliant entity-encoded version.
         * @param s a string to encode
         */
        public static String encodeEntities(String s) {
            return WMLStringEncoder.encodeEntities(s);
        }

        /**
         * Convert a WML-compliant entity-encoded string to a raw string version.
         * @param s a string to decode
         */
        public static String decodeEntities(String s) {
            return WMLStringEncoder.decodeEntities(s);
        }
    }   // End of WML.String
}
