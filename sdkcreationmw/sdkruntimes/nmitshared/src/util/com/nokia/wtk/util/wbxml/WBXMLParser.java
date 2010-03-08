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
 * WBXMLParser.java                 2000/03/20
 */

// ========================================================================
//  Name        : WBXMLParser.java
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

import java.io.*;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Locale;
import java.util.Vector;

import org.xml.sax.*;
import org.xml.sax.helpers.*;

import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.CharacterEncoding;

/**
 * A SAX Parser for WBXML
 *
 */
//TODO: More testing needed for precise locator information (both doc events and errors)
//TODO: Need to add support for PROV content
public class WBXMLParser extends DefaultHandler {
    /**
     * Creates a new WBXMLParser.
     */
    public WBXMLParser() {
        if ((tokenDictionaries == null) || (tokenDictionaries.isEmpty())) {
            //
            // Register Well-known Public Identifiers and Token Dictionaries:
            //
            registerTokenDictionary(0x02,   // WML 1.0 (DEPRECATED)
                                    "-//WAPFORUM//DTD WML 1.0//EN",
                                    null);
            registerTokenDictionary(0x03,   // WTA Event 1.0 (DEPRECATED)
                                    "-//WAPFORUM//DTD WTA 1.0//EN",
                                    null);
            registerTokenDictionary(0x04,   // WML 1.1
                                    "-//WAPFORUM//DTD WML 1.1//EN",
                                    new WMLTokenDictionary("1.1"));
            registerTokenDictionary(0x05,   // Service Indication 1.0
                                    "-//WAPFORUM//DTD SI 1.0//EN",
                                    new SITokenDictionary("1.0"));
            registerTokenDictionary(0x06,   // Service Loading 1.0
                                    "-//WAPFORUM//DTD SL 1.0//EN",
                                    new SLTokenDictionary("1.0"));
            registerTokenDictionary(0x07,   // Cache Operation 1.0
                                    "-//WAPFORUM//DTD CO 1.0//EN",
                                    new COTokenDictionary("1.0"));
            registerTokenDictionary(0x08,   // Channel 1.1 (NOT SUPPORTED)
                                    "-//WAPFORUM//DTD CHANNEL 1.1//EN",
                                    null);
            registerTokenDictionary(0x09,   // WML 1.2
                                    "-//WAPFORUM//DTD WML 1.2//EN",
                                    new WMLTokenDictionary("1.2"));
            registerTokenDictionary(0x0A,   // WML 1.3
                                    "-//WAPFORUM//DTD WML 1.3//EN",
                                    new WMLTokenDictionary("1.3"));
            registerTokenDictionary(0x0B,   // DR 1.0
                                    "-//OMA//DTD DRMREL 1.0//EN",
                                    new DRTokenDictionary("1.0"));
        }
    }


    /**
     * Sets the default PublicID to use if not defined.
     * @param pid the default PublicID
     */
    public void setDefaultPublicID(String pid) {
        if ((pid != null) && (pid.length() > 0)) {
            defaultPublicID = new PublicID(pid);
        }
    }

    /**
     * Gets the default PublicID to use if not defined.
     * @return the default PublicID
     */
    private PublicID getDefaultPublicID() {
        return defaultPublicID;
    }

    /**
     * Turn this on for VERY detailed output
     */
    static void detailOut(String s) {
        // MessageWriters.DetailMsg.println("WBXMLParser: " + s);
    }

    /**
     * Unit test for the WBXMLParser.
     * @param args command parameters
     */
    public static void main(String[] args) {
        if (args.length == 1) {
            try {
                final WBXMLParser parser = new WBXMLParser();
                parser.setContentHandler(new DefaultHandler() {
                    Locator locator = null;

                    public void setDocumentLocator(Locator locator) {
                        this.locator = locator;
                        System.out.println("WBXMLParser: setDocumentLocator(): locator=" +
                            WBXMLParser.toString(locator));
                    }

                    public void startDocument() throws SAXException {
                        System.out.println("WBXMLParser: startDocument(): version = " +
                          StringUtils.getDisplayableString(parser.getVersionString())
                          + ", publicID=" + StringUtils.getDisplayableString(locator.getPublicId()));
                        System.out.println(" ........... character encoding = " +
                          StringUtils.getDisplayableString(parser.getCharacterEncoding().toString()));
                        System.out.println(" ........... current position = " +
                          locator.getColumnNumber());
                        System.out.println(" ........... string table = " +
                          parser.getStringTable());
                    }

                    public void endDocument() throws SAXException {
                        System.out.println("WBXMLParser: endDocument()");
                        System.out.println(" ........... Read " + locator.getColumnNumber() +                              " bytes.");
                    }

                    public void startElement(String namespaceURI, String localName,
                        String qname, Attributes attrs) throws SAXException {
                            System.out.println("WBXMLParser: startElement(" +
                              StringUtils.getDisplayableString(qname) + ", " +
                              WBXMLParser.toString(attrs) + ")");
                    }

                    public void endElement(String namespaceURI, String localName, String qname)
                        throws SAXException {
                            System.out.println("WBXMLParser: endElement(" +                                                    StringUtils.getDisplayableString(qname) + ")");
                    }

                    public void characters(char[] buf, int offset, int len) throws SAXException {
                        System.out.println("WBXMLParser: characters(" +
                          StringUtils.getDisplayableString(new String(buf, offset, len)) + ")");
                    }

                    public void ignorableWhitespace(char[] buf, int offset, int len) throws SAXException {
                        System.out.println("WBXMLParser: ignorableWhitespace(" +
                          StringUtils.getDisplayableString(new String(buf, offset, len)) + ")");
                    }

                    public void processingInstruction(String target, String data) throws SAXException {
                        System.out.println("WBXMLParser: processingInstruction(" +
                          StringUtils.getDisplayableString(target) + ", " +
                          StringUtils.getDisplayableString(data) + ")");
                    }

                    public boolean getFeature(String feature) throws SAXNotSupportedException {
                        System.out.println("WBXMLParser: getFeature(" +
                          StringUtils.getDisplayableString(feature));
                        throw new SAXNotSupportedException(feature);
                    }
                });
                String wbxmlURI = new File(args[0]).toURL().toString();
                parser.parse(wbxmlURI);
            } catch (SAXParseException ex) {
                LocatorImpl locator = new LocatorImpl();
                locator.setColumnNumber(ex.getColumnNumber()-1);
                locator.setLineNumber(ex.getLineNumber());
                locator.setPublicId(ex.getPublicId());
                locator.setSystemId(ex.getSystemId());
                System.err.println("*** Exception: " + ex + "; locator=" + WBXMLParser.toString(locator));
                ex.printStackTrace(System.err);
            } catch (Exception ex) {
                System.err.println("*** Exception: " + ex);
                ex.printStackTrace(System.err);
            }
        } else {
            System.err.println("Usage: " + WBXMLParser.class.getName() + " <wbxml file>");
        }
    }

    /**
     * Allow an application to request a locale for errors and warnings.
     *
     * <p>SAX parsers are not required to provide localisation for errors
     * and warnings; if they cannot support the requested locale,
     * however, they must throw a SAX exception.  Applications may
     * not request a locale change in the middle of a parse.</p>
     *
     * @param locale A Java Locale object.
     * @exception org.xml.sax.SAXException Throws an exception
     *            (using the previous or default locale) if the
     *            requested locale is not supported.
     * @see org.xml.sax.SAXException
     * @see org.xml.sax.SAXParseException
     */
    public void setLocale(Locale locale) throws SAXException {
        // Ignored until there are messages that need to be localized
    }

    /**
     * Allow an application to register a custom entity resolver.
     *
     * <p>If the application does not register an entity resolver, the
     * SAX parser will resolve system identifiers and open connections
     * to entities itself (this is the default behaviour implemented in
     * HandlerBase).</p>
     *
     * <p>Applications may register a new or different entity resolver
     * in the middle of a parse, and the SAX parser must begin using
     * the new resolver immediately.</p>
     *
     * @param resolver The object for resolving entities.
     * @see EntityResolver
     * @see HandlerBase
     */
    public void setEntityResolver(EntityResolver resolver) {
        this.entityResolver = resolver;
    }

    /**
     * Allow an application to register a DTD event handler.
     *
     * <p>If the application does not register a DTD handler, all DTD
     * events reported by the SAX parser will be silently
     * ignored (this is the default behaviour implemented by
     * HandlerBase).</p>
     *
     * <p>Applications may register a new or different
     * handler in the middle of a parse, and the SAX parser must
     * begin using the new handler immediately.</p>
     *
     * @param handler The DTD handler.
     * @see DTDHandler
     * @see HandlerBase
     */
    public void setDTDHandler(DTDHandler handler) {
        // Ignored
    }

    /**
     * Allow an application to register a document event handler.
     *
     * <p>If the application does not register a document handler, all
     * document events reported by the SAX parser will be silently
     * ignored (this is the default behaviour implemented by
     * HandlerBase).</p>
     *
     * <p>Applications may register a new or different handler in the
     * middle of a parse, and the SAX parser must begin using the new
     * handler immediately.</p>
     *
     * @param handler The document handler.
     * @see ContentHandler
     * @see HandlerBase
     */
    public void setContentHandler(ContentHandler handler) {
        this.contentHandler = handler;
    }

    /**
     * Allow an application to register an error event handler.
     *
     * <p>If the application does not register an error event handler,
     * all error events reported by the SAX parser will be silently
     * ignored, except for fatalError, which will throw a SAXException
     * (this is the default behaviour implemented by HandlerBase).</p>
     *
     * <p>Applications may register a new or different handler in the
     * middle of a parse, and the SAX parser must begin using the new
     * handler immediately.</p>
     *
     * @param handler The error handler.
     * @see ErrorHandler
     * @see SAXException
     * @see HandlerBase
     */
    public void setErrorHandler(ErrorHandler handler) {
        this.errorHandler = handler;
    }

    /**
     * Enables or disables document event messages.
     * @param enabled If <code>true</code> then echo document events as "detail" messages.
     */
    public void setDocumentEventMessagesEnabled(boolean enabled) {
        getDocumentEventMessageHandler().setDocumentEventMessagesEnabled(enabled);
    }

    /**
     * Sets the writer to use for document event messages.
     * @param writer the writer to use.
     */
    public void setDocumentEventMessagesWriter(Writer writer) {
        if (writer instanceof PrintWriter) {
            getDocumentEventMessageHandler().setWriter((PrintWriter)writer);
        } else if (writer != null) {
            getDocumentEventMessageHandler().setWriter(new PrintWriter(writer));
        } else {
            getDocumentEventMessageHandler().setWriter(null);
        }
    }

    /**
     * Parse an XML document.
     *
     * <p>The application can use this method to instruct the SAX parser
     * to begin parsing an XML document from any valid input
     * source (a character stream, a byte stream, or a URI).</p>
     *
     * <p>Applications may not invoke this method while a parse is in
     * progress (they should create a new Parser instead for each
     * additional XML document).  Once a parse is complete, an
     * application may reuse the same Parser object, possibly with a
     * different input source.</p>
     *
     * @param source The input source for the top-level of the
     *        XML document.
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     * @see org.xml.sax.InputSource
     * @see #parse(java.lang.String)
     * @see #setEntityResolver
     * @see #setDTDHandler
     * @see #setContentHandler
     * @see #setErrorHandler
     */
    public void parse(InputSource source) throws SAXException, IOException {
        InputStream in = source.getByteStream();
        if (in == null) {
            throw new IOException("WBXMLParser: " + MISSING_STREAM);
        }
        CharacterEncoding encoding = CharacterEncoding.findEncoding(source.getEncoding());
        if (encoding == null) {
            encoding = CharacterEncoding.getDefault();
        }
        this.inAttrState = false;
        this.tagCodePage = 0;
        this.attrCodePage = 0;
        this.reader = new Reader(in, encoding);

        // Initialize Locator:
        reader.getLocator().setSystemId(source.getSystemId());
        reader.getLocator().setPublicId(source.getPublicId());
        getDocumentEventMessageHandler().setDocumentLocator(reader.getLocator());
        if (contentHandler != null) {
            contentHandler.setDocumentLocator(reader.getLocator());
        }

        readVersion();
        detailOut("parse: version: " + StringUtils.getDisplayableString(getVersionString()));

        int pidOffset = readPublicID();
        if (pidOffset < 0) {
            detailOut("parse:  PublicID: " + getPublicID());
        }

        int ianaMIBenum = readCharacterEncoding(encoding);
        detailOut("parse:  character encoding: " + StringUtils.getDisplayableString(getCharacterEncoding().toString()) + " (" + ianaMIBenum + ")");

        int stringTableOffset = readStringTable();
        detailOut("parse:  string table: " + getStringTable() + " starting at bytecode offset: " + toHexString(stringTableOffset));
        detailOut("parse:  bytecode offset after string table: " + toHexString(reader.getPosition() + 1));

        if (pidOffset >= 0) {
            String pidString = getStringTable().getString(pidOffset);
            if (pidString != null) {
                this.publicID = new PublicID(pidString);
                reader.getLocator().setPublicId(pidString);
                detailOut("parse:  PublicID: " + getPublicID());
            }
        }

        if (getTokenDictionary() == null) {
            if (getPublicID() == null) {
                // No PublicID defined:
                throw new IOException("WBXMLParser: " + UNDEFINED_PUBLICID);
            }

            // No TokenDictionary defined for the PublicID:
            throw new IOException("WBXMLParser: " + UNKNOWN_TOKEN_DICTIONARY + ": \"" + getPublicID().id + "\"");
        } else {
            detailOut("parse:  TokenDictionary is defined: " + getTokenDictionary().getClass().getName());
        }

        // Start of document:
        // Notification is placed here because the handler can now query the parser
        // for useful information like version, publicID, systemID, character encoding.
        reader.getLocator().setColumnNumber(reader.getByteCount());
        getDocumentEventMessageHandler().startDocument();
        if (contentHandler != null) {
            contentHandler.startDocument();
        }
        if (!reader.isEndOfStream()) {
            try {
                parseDocumentBody();
            } catch (SAXException ex) {
                if (!reader.isEndOfStream()) {
                    throw ex;
                }
            } catch (IOException ex) {
                if (!reader.isEndOfStream()) {
                    throw ex;
                }
            }
        }

        detailOut("parse:  " + reader.getByteCount() + " bytes read.");
        // Document complete.
        reader.getLocator().setColumnNumber(reader.getPosition());
        getDocumentEventMessageHandler().endDocument();
        if (contentHandler != null) {
            contentHandler.endDocument();
        }

        detailOut("parse:  final token=" + toString(reader.getLastToken()) +
                " at bytecode offset " + toHexString(reader.getPosition()));

        // Check for trailing "junk" bytes:
        reader.getLocator().setColumnNumber(reader.getByteCount());
        int byteCount = reader.getByteCount();
        while (!reader.isEndOfStream()) {
            try { reader.readByte(); }
            catch (Exception ex) { break; }
        }
        if (reader.getByteCount() > byteCount) {
            getErrorHandler().error(new SAXParseException("WBXMLParser: " + EXCESS_BYTES + ": +" + (reader.getByteCount() - byteCount) + " bytes read", reader.getLocator()));
        }
    }

    /**
     * Parse an XML document from a system identifier (URI).
     *
     * <p>This method is a shortcut for the common case of reading a
     * document from a system identifier.</p>
     *
     * <p>If the system identifier is a URL, it must be fully resolved
     * by the application before it is passed to the parser.</p>
     *
     * @param systemID The system identifier (URI).
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     * @see #parse(org.xml.sax.InputSource)
     */
    public void parse(String systemID) throws SAXException, IOException {
        InputSource source = new InputSource(new java.net.URL(systemID).openStream());
        this.systemID = systemID;
        source.setSystemId(systemID);
        source.setEncoding(CharacterEncoding.getDefault().toString());
        parse(source);
    }

    /**
     * Parse a WBXML document body.
     * This is defined as zero or more PIs followed by
     * a single root element and then zero or more PIs.
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    private void parseDocumentBody() throws SAXException, IOException {
        byte token = reader.readByte();

        // Zero or more leading Processing Instructions:
        while (token == GlobalToken.PI) {
            token = parseProcessingInstruction();
        }

        // A single root element:
        token = handlePossibleCodePageSwitch(token);
        reader.getLocator().setColumnNumber(reader.getByteCount());
        if (!(isPossibleElementToken(token) && parseElement(token))) {
            getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + MISSING_ROOT_ELEMENT, reader.getLocator()));
        }
        token = reader.getLastToken();

        // Zero or more trailing Processing Instructions:
        while (token == GlobalToken.PI) {
            token = parseProcessingInstruction();
        }
    }

    /**
     * Parse a WBXML tag(element) either as a literal or by using the current token dictionary.
     * @param token the tag token
     * @return <code>true</code> if an element was found
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    private boolean parseElement(byte token) throws SAXException, IOException {
        token = handlePossibleCodePageSwitch(token);
        if (isGlobalToken(token) && !isLiteralToken(token)) {
            return false;
        }
        int elementStartPos = reader.getPosition();

        // Grab the upper flag bits:
        boolean expectAttrs = ((token & 0x80) != 0);
        boolean expectContent = ((token & 0x40) != 0);
        // Mask off the upper flag bits so lookup can be done without them:
        byte elemToken = (byte)(token & 0x3F);
        String elemName = null;
        if (elemToken == GlobalToken.LITERAL) {
            elemName = getStringTable().getString(reader.readInteger());
        } else {    // Try token dictionary:
            elemName = getTokenDictionary().getTag(this.tagCodePage, elemToken);
        }

        detailOut("parseElement: token=" + toString(token) +
           ", expectAttrs=" + expectAttrs + ", expectContent=" + expectContent +
           ", elemToken=" + toString(elemToken) + ", elemName=" +
           StringUtils.getDisplayableString(elemName) + " at bytecode offset " +
           toHexString(reader.getPosition()));

        if (isValidName(elemName)) {
            Attributes attrList = (expectAttrs) ? parseAttrs(elemName) : null;
            if (expectAttrs && (attrList.getLength() == 0)) {
                reader.getLocator().setColumnNumber(reader.getPosition() - 1);
                getErrorHandler().warning(new SAXParseException("WBXMLParser: " + MISSING_ATTRIBUTES
                                                              + " (element = \"" + elemName + "\")",
                                                                reader.getLocator()));
            }
            reader.getLocator().setColumnNumber(elementStartPos);
            getDocumentEventMessageHandler().startElement(null, elemName, elemName, attrList);
            if (contentHandler != null) {
                contentHandler.startElement(null, elemName, elemName, attrList);
            }
            if (expectContent) {
                if (!parseContent(elemName)) {
                    reader.getLocator().setColumnNumber(reader.getPosition());
                    getErrorHandler().warning(new SAXParseException("WBXMLParser: " + MISSING_CONTENT
                                                                  + " (element = \"" + elemName + "\")",
                                                                    reader.getLocator()));
                }
            }
            reader.getLocator().setColumnNumber(reader.getPosition());
            getDocumentEventMessageHandler().endElement(null, elemName, elemName);
            if (contentHandler != null) {
                contentHandler.endElement(null, elemName, elemName);
            }
            return true;
        } else if (elemName == null) {
            reader.getLocator().setColumnNumber(elementStartPos);
            getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNKNOWN_ELEMENT_TOKEN
                                                             + " (token = " + toString(token) + ")",
                                                               reader.getLocator()));
            return false;
        } else {
            reader.getLocator().setColumnNumber(elementStartPos);
            getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + INVALID_ELEMENT_NAME
                                                             + " (name = \"" + elemName + "\")",
                                                               reader.getLocator()));
            return false;
        }
    }

    /**
     * Parse WBXML tag attributes.
     * Attributes consist of zero or more attribute sequences terminated by an END token.
     * Each attribute sequence consists of an ATTRSTART followed by zero or more ATTRVALUEs.
     * ATTRSTART and ATTRVALUE tokenisation will be parsed by {@link #readPossibleAttrStart}
     * and {@link #readPossibleAttrValue}, respectively.
     * @param elemName the name of the element being parsed.
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    private Attributes parseAttrs(String elemName) throws SAXException, IOException {
        AttributesImpl attrList = new AttributesImpl();
        String[] valueReturn = new String[2];
        this.inAttrState = true;
        for (byte token = reader.readByte(); token != GlobalToken.END;) {

            detailOut("parseAttrs(\"" + elemName + "\"): token=" + toString(token));
            int attrStartPos = reader.getByteCount();
            try { token = readPossibleAttrStart(token, valueReturn); }
            catch (SAXException ex) { this.inAttrState = false; throw ex; }
            catch (IOException ex) { this.inAttrState = false; throw ex; }
            String attrName = valueReturn[0];
            if (isValidName(attrName)) {

                detailOut("parseAttr: name=" +
                   StringUtils.getDisplayableString(attrName) + ", value=" +
                   StringUtils.getDisplayableString(valueReturn[0]));

                StringBuffer valueBuf = new StringBuffer();
                // Check for implicit attribute start value:
                if (valueReturn[1] != null) {
                    valueBuf.append(valueReturn[1]);
                }
                // Now read zero or more attribute value tokens:
                // (This is terminated either by an END or another ATTRSTART)
                while (token != GlobalToken.END) {
                    try { token = readPossibleAttrValue(elemName, attrName, token, valueReturn); }
                    catch (SAXException ex) { this.inAttrState = false; throw ex; }
                    catch (IOException ex) { this.inAttrState = false; throw ex; }
                    if (valueReturn[0] != null) {
                        valueBuf.append(valueReturn[0]);
                    } else {
                        token = handlePossibleCodePageSwitch(token);
                        if (token != GlobalToken.END) {
                            if (isPossibleAttrStartToken(token)) {
                                break;
                            } else {    // Unknown attr value token:
                                reader.getLocator().setColumnNumber(reader.getPosition());
                                try {
                                    getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNKNOWN_ATTRVALUE_TOKEN
                                                                                     + " (token = " + toString(token) + ")",
                                                                                       reader.getLocator()));
                                }
                                catch (SAXException ex) { this.inAttrState = false; throw ex; }
                            }
                        }
                    }
                }
                // Add completed attribute name/value pair to the list:
                attrList.addAttribute(null, attrName, attrName, "CDATA", valueBuf.toString());
            } else if (attrName == null) {
                // Unknown attribute start token
                reader.getLocator().setColumnNumber(attrStartPos);
                try {
                    getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNKNOWN_ATTRSTART_TOKEN
                                                                     + " (token = " + toString(token) + ")",
                                                                       reader.getLocator()));
                }
                catch (SAXException ex) { this.inAttrState = false; throw ex; }
            } else {    // Bad attribute name
                reader.getLocator().setColumnNumber(attrStartPos);
                try {
                    getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + INVALID_ATTR_NAME
                                                                     + " (name = \"" + attrName + "\")",
                                                                       reader.getLocator()));
                }
                catch (SAXException ex) { this.inAttrState = false; throw ex; }
            }
        }
        this.inAttrState = false;
        return attrList;
    }

    /**
     * Parse WBXML element content until an END token is encountered.
     * This is defined as one or more unordered instances of the following:
     * <ul>
     * <li><code>element</code></li>
     * <li><code>pi</code></li>
     * <li><code>opaque</code></li>
     * <li><code>string</code></li>
     * <li><code>extension</code></li>
     * <li><code>entity</code></li>
     * </ul>
     * @param elemName the name of the element being parsed.
     * @return <code>true</code> if content found
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    private boolean parseContent(String elemName) throws SAXException, IOException {
        boolean contentFound = false;
        for (byte token = reader.readByte(); token != GlobalToken.END;) {

            detailOut("parseContent(\"" + elemName + "\"): token = " + WBXMLParser.toString(token));

            // Processing Instruction?
            if (token == GlobalToken.PI) {
                token = parseProcessingInstruction();
                contentFound = true;
                continue;
            }

            // Opaque?
            String s = null;
            if (token == GlobalToken.OPAQUE) {
                int opaqueSize = reader.readInteger();
                reader.getLocator().setColumnNumber(reader.getByteCount());
                byte[] bytes = reader.readBytes(opaqueSize);
                getDocumentEventMessageHandler().opaqueData(bytes);
                if (bytes.length > 0) {
                    // Opaque data is application-dependent:
                    s = getTokenDictionary().filterOpaqueData(elemName, null, bytes, reader.getEncoding());
                    if ((s != null) && (s.length() > 0)) {
                        getDocumentEventMessageHandler().characters(s.toCharArray(), 0, s.length());
                        if (contentHandler != null) {
                            contentHandler.characters(s.toCharArray(), 0, s.length());
                        }
                    } else if (s == null) {
                        // OPAQUE data is invalid or not allowed
                        getErrorHandler().error(new SAXParseException("WBXMLParser: " + INVALID_OPAQUE_DATA
                                                                    + ": " + toString(bytes),
                                                                      reader.getLocator()));
                    }
                }
                contentFound = true;
                // Advance to next token:
                token = reader.readByte();
                continue;
            }

            // String?
            s = readString(token);
            if (s != null) {
                // Plain CDATA is subject to filtering:
                s = getTokenDictionary().filterCDATA(elemName, null, s);
                if ((s != null) && (s.length() > 0)) {
                    reader.getLocator().setColumnNumber(reader.getPosition());
                    getDocumentEventMessageHandler().characters(s.toCharArray(), 0, s.length());
                    if (contentHandler != null) {
                        contentHandler.characters(s.toCharArray(), 0, s.length());
                    }
                }
                contentFound = true;
                // Advance to next token:
                token = reader.readByte();
                continue;
            }

            // Character Entity?
            s = readCharacterEntity(token);
            if (s != null) {
                //TODO: should character entities be filtered?
                //s = getTokenDictionary().filterCDATA(elemName, null, s);
                if ((s != null) && (s.length() > 0)) {
                    reader.getLocator().setColumnNumber(reader.getPosition());
                    getDocumentEventMessageHandler().characters(s.toCharArray(), 0, s.length());
                    if (contentHandler != null) {
                        contentHandler.characters(s.toCharArray(), 0, s.length());
                    }
                }
                contentFound = true;
                // Advance to next token:
                token = reader.readByte();
                continue;
            }

            // Extension?
            s = readExtension(token);
            token = reader.getLastToken();
            if (s != null) {
                if (s.length() > 0) {
                    reader.getLocator().setColumnNumber(reader.getPosition());
                    getDocumentEventMessageHandler().characters(s.toCharArray(), 0, s.length());
                    if (contentHandler != null) {
                        contentHandler.characters(s.toCharArray(), 0, s.length());
                    }
                }
                contentFound = true;
                // Advance to next token:
                token = reader.readByte();
                continue;
            }

            // Element?
            token = handlePossibleCodePageSwitch(token);
            if (isPossibleElementToken(token) && parseElement(token)) {
                contentFound = true;
                // Advance to next token:
                token = reader.readByte();
                continue;
            }

            if (token != GlobalToken.END) {
                // Unknown or unexpected content token
                reader.getLocator().setColumnNumber(reader.getPosition());
                getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNEXPECTED_CONTENT_TOKEN
                                                                 + " (element = \"" + elemName + "\", token = " + toString(token) + ")",
                                                                   reader.getLocator()));
            }
        }
        return contentFound;
    }

    /**
     * Parse a WBXML processing instruction after having already read a PI token.
     * This must start with a target name (defined as 'attrStart'), followed by
     * zero or more values (defined by 'attrValue' sequences).
     * The processing instruction is always terminated by an END token.
     * @return the next token to be parsed
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    private byte parseProcessingInstruction() throws SAXException, IOException {
        byte token = reader.readByte();
        String[] valueReturn = new String[2];
        token = readPossibleAttrStart(token, valueReturn);
        String target = valueReturn[0];
        if (isValidName(target)) {
            detailOut("parseProcessingInstruction: target=" +
                StringUtils.getDisplayableString(valueReturn[0]) + ", value=" +
                StringUtils.getDisplayableString(valueReturn[1]));
            StringBuffer valueBuf = new StringBuffer();
            // Check for implicit attribute start value:
            if (valueReturn[1] != null) {
                valueBuf.append(valueReturn[1]);
            }
            // Now read zero or more attribute value tokens:
            while (token != GlobalToken.END) {
                token = readPossibleAttrValue(null, target, token, valueReturn);
                if (valueReturn[0] != null) {
                    valueBuf.append(valueReturn[0]);
                } else if (token != GlobalToken.END) {
                    reader.getLocator().setColumnNumber(reader.getPosition());
                    getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + BAD_PI_VALUE_TOKEN
                                                                     + " (token = " + toString(token) + ")",
                                                                       reader.getLocator()));
                }
            }
            // Read past END token
            token = reader.readByte();
            // Done parsing PI.
            reader.getLocator().setColumnNumber(reader.getPosition());
            getDocumentEventMessageHandler().processingInstruction(target, valueBuf.toString());
            if (contentHandler != null) {
                contentHandler.processingInstruction(target, valueBuf.toString());
            }
        } else if (target == null) {
            reader.getLocator().setColumnNumber(reader.getPosition());
            getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNKNOWN_PI_TARGET
                                                             + " (token = " + toString(token) + ")",
                                                               reader.getLocator()));
        } else {    // Bad target name
            reader.getLocator().setColumnNumber(reader.getPosition());
            getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + INVALID_PI_TARGET_NAME
                                                             + " (name = \"" + target + "\")",
                                                               reader.getLocator()));
        }

        return token;
    }

    /**
     * Checks for a valid element, attribute or PI target name.
     * @param name an element name
     * @return <code>true</code> if the name is a valid name
     */
    private static boolean isValidName(String name) {
        boolean isValid = false;

        if ((name != null) && (name.length() > 0)) {
            isValid = true;
            for (int i = 0; i < name.length(); i++) {
                char c = name.charAt(i);
                // Check for valid XML name characters:
                if (Character.isLetter(c)) {
                    continue;
                } else if (c == '_') {
                    continue;
                } else if (c == ':') {
                    continue;
                } else if ((i > 0) && Character.isDigit(c)) {
                    continue;
                } else if ((i > 0) && (c == '-')) {
                    continue;
                } else if ((i > 0) && (c == '.')) {
                    continue;
                } else if ((i > 0) && (Character.getType(c) == Character.MODIFIER_LETTER)) {
                    continue;
                } else if ((i > 0) && (Character.getType(c) == Character.OTHER_LETTER)) {
                    continue;
                } else {
                    isValid = false;
                    break;
                }
            }
        }
        return isValid;
    }

    /**
     * Checks for global token.
     * @param token the token
     * @return <code>true</code> if token is a valid global token
     */
    private static boolean isGlobalToken(byte token) {
        switch (token) {
        case GlobalToken.SWITCH_PAGE:
        case GlobalToken.END:
        case GlobalToken.ENTITY:
        case GlobalToken.STR_I:
        case GlobalToken.LITERAL:
        case GlobalToken.EXT_I_0:
        case GlobalToken.EXT_I_1:
        case GlobalToken.EXT_I_2:
        case GlobalToken.PI:
        case GlobalToken.LITERAL_C:
        case GlobalToken.EXT_T_0:
        case GlobalToken.EXT_T_1:
        case GlobalToken.EXT_T_2:
        case GlobalToken.STR_T:
        case GlobalToken.LITERAL_A:
        case GlobalToken.EXT_0:
        case GlobalToken.EXT_1:
        case GlobalToken.EXT_2:
        case GlobalToken.OPAQUE:
        case GlobalToken.LITERAL_AC:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks for global LITERAL token.
     * @param token the token
     * @return <code>true</code> if token is a valid LITERAL token
     */
    private static boolean isLiteralToken(byte token) {
        switch (token) {
        case GlobalToken.LITERAL:
        case GlobalToken.LITERAL_C:
        case GlobalToken.LITERAL_A:
        case GlobalToken.LITERAL_AC:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks for possible element token.
     * @param token the token
     * @return <code>true</code> if token is a valid element token
     */
    private static boolean isPossibleElementToken(byte token) {
        boolean isValid = false;

        if ((byte)(token & 0x3F) == GlobalToken.LITERAL) {
            // Literal element value.
            isValid = true;
        } else if (!isGlobalToken(token)) {
            // Possible element token but subsequent dictionary lookup will confirm.
            isValid = true;
        }
        return isValid;
    }

    /**
     * Checks for possible attribute start token.
     * @param token the token
     * @return <code>true</code> if token is a valid attribute start token
     */
    private static boolean isPossibleAttrStartToken(byte token) {
        boolean isValid = false;

        if (token == GlobalToken.LITERAL) {
            // Literal attribute value.
            isValid = true;
        } else if (!isGlobalToken(token) && ((token & 0x80) == 0x00)) {
            // Regular ATTRSTART token format.
            // Subsequent dictionary lookup will confirm.
            isValid = true;
        }
        return isValid;
    }

    /**
     * Reads a possible attribute start value.
     * @param token the token
     * @param valueReturn a two-element array for returning both the attribute name
     *                    and an implicit attribute value if it exists
     * @return the next token to be parsed
     * @exception java.io.IOException if attempting to read past end of stream
     * @exception org.xml.sax.SAXParseException if a string table offset is invalid
     */
    private byte readPossibleAttrStart(byte token, String[] valueReturn) throws IOException, SAXException {
        // Initialize return values:
        valueReturn[0] = null;
        valueReturn[1] = null;

        if (token == GlobalToken.LITERAL) {
            // The caller takes the responsibility of detecting and reporting errors.
            valueReturn[0] = getStringTable().getString(reader.readInteger());
            // Advance to next token:
            token = reader.readByte();
        } else {
            token = handlePossibleCodePageSwitch(token);
            if (!isGlobalToken(token) && ((token & 0x80) == 0x00)) {
                // Attribute start token values are always in {0..7F} (MSB not set)
                // The caller takes the responsibility of detecting and reporting errors.
                String s = getTokenDictionary().getAttrStart(this.attrCodePage, token, valueReturn);
                // Adjust return value locations as getAttrStart places value in 1st element:
                valueReturn[1] = valueReturn[0];
                valueReturn[0] = s;
                // Advance to next token:
                token = reader.readByte();
            }
        }
        detailOut("readPossibleAttrStart: ===> " + StringUtils.getDisplayableString(valueReturn[0]));

        return token;
    }

    /**
     * Reads a possible attribute value.
     * @param elemName the name of the element being parsed or <code>null</code> if PI.
     * @param attrName the name of the element attribute or PI target being parsed.
     * @param token the token
     * @param valueReturn a single-element array for returning the attribute value
     * @return the next token to be parsed
     * @exception java.io.IOException if attempting to read past end of stream
     * @exception org.xml.sax.SAXParseException if a string table offset is invalid
     */
    private byte readPossibleAttrValue(String elemName, String attrName, byte token, String[] valueReturn) throws IOException, SAXException {
        detailOut("readPossibleAttrValue: token=" + toString(token) + ", current pos = " +
                Integer.toHexString(reader.getPosition()));
        valueReturn[0] = readString(token);                 // String?
        if (valueReturn[0] != null) {
            // Plain CDATA is subject to filtering:
            valueReturn[0] = getTokenDictionary().filterCDATA(elemName, attrName, valueReturn[0]);
            if (valueReturn[0] == null) {
                valueReturn[0] = "";    // Must at least indicate that the token was recognized.
            }
            // Advance to next token:
            token = reader.readByte();
        }
        if (valueReturn[0] == null) {
            valueReturn[0] = readExtension(token);          // Extension?
            token = reader.getLastToken();
            if (valueReturn[0] != null) {
                // Advance to next token:
                token = reader.readByte();
            }
        }
        if (valueReturn[0] == null) {
            valueReturn[0] = readCharacterEntity(token);    // Character Entity?
            if (valueReturn[0] != null) {
/*/
                //TODO: should character entities be filtered?
                valueReturn[0] = getTokenDictionary().filterCDATA(elemName, attrName, valueReturn[0]);
                if (valueReturn[0] == null) {
                    valueReturn[0] = ""; // Must at least indicate that the token was recognized.
                }
//*/
                // Advance to next token:
                token = reader.readByte();
            }
        }
        if ((valueReturn[0] == null) && (token == GlobalToken.OPAQUE)) {    // Opaque Data?
            int opaqueSize = reader.readInteger();
            reader.getLocator().setColumnNumber(reader.getByteCount());
            byte[] bytes = reader.readBytes(opaqueSize);
            getDocumentEventMessageHandler().opaqueData(bytes);
            if (bytes.length > 0) {
                // Opaque data is application-dependent:
                valueReturn[0] = getTokenDictionary().filterOpaqueData(elemName, attrName, bytes, reader.getEncoding());
            }
            if (valueReturn[0] == null) {
                // OPAQUE data is invalid or not allowed
                getErrorHandler().error(new SAXParseException("WBXMLParser: " + INVALID_OPAQUE_DATA
                                                            + ": " + toString(bytes),
                                                              reader.getLocator()));
                valueReturn[0] = ""; // Must at least indicate that the token was recognized.
            }
            // Advance to next token:
            token = reader.readByte();
        }
        if (valueReturn[0] == null) {
            // Finally, check for possible attribute value token:
            token = handlePossibleCodePageSwitch(token);
            if (!isGlobalToken(token) && ((token & 0x80) != 0)) {
                // Attribute value tokens must have most significant bit set.
                // The caller takes the responsibility of detecting and reporting errors.
                valueReturn[0] = getTokenDictionary().getAttrValue(this.attrCodePage, token);
                // Advance to next token:
                token = reader.readByte();
            }
        }
        detailOut("readPossibleAttrValue: ===> " + StringUtils.getDisplayableString(valueReturn[0]));
        return token;
    }

    /**
     * Reads a possible extension value.
     * This method has a possible side-effect of advancing the reader,
     * so callers should take this into account when tracking current token value.
     * @param token the token
     * @return the string or <code>null</code> if token is not an extension token
     * @exception org.xml.sax.SAXException if attempting to read past end of stream
     */
    private String readExtension(byte token) throws SAXException {
        token = handlePossibleCodePageSwitch(token);

        reader.getLocator().setColumnNumber(reader.getByteCount());
        String s = null;
        switch (token) {
        case GlobalToken.EXT_I_0:
        case GlobalToken.EXT_I_1:
        case GlobalToken.EXT_I_2:
            String inline = reader.readString();
            detailOut("readExtension(String): token=" + toString(token) + ", inline=" +
                StringUtils.getDisplayableString(inline));
            s = getTokenDictionary().getExtensionValue(token, inline);
            detailOut(" ... ===> " + StringUtils.getDisplayableString(s));
            if (s == null) {
                // Invalid or disallowed extension
                getErrorHandler().error(new SAXParseException("WBXMLParser: " + INVALID_EXTENSION
                                                            + " (token=" + toString(token) + ", inline string=\"" + StringUtils.getDisplayableString(inline) + "\")",
                                                              reader.getLocator()));
                s = ""; // Must at least indicate that the token was recognized.
            }
            break;
        case GlobalToken.EXT_T_0:
        case GlobalToken.EXT_T_1:
        case GlobalToken.EXT_T_2:
            int offset = reader.readInteger();
            detailOut("readExtension(Integer): token=" + toString(token) + ", offset=" + offset);
            s = getTokenDictionary().getExtensionValue(token, offset, getStringTable());
            detailOut(" ... ===> " + StringUtils.getDisplayableString(s));
            if (s == null) {
                // Invalid or disallowed extension
                getErrorHandler().error(new SAXParseException("WBXMLParser: " + INVALID_EXTENSION
                                                            + " (token=" + toString(token) + ", offset=" + offset + ", stringtable entry=\"" + StringUtils.getDisplayableString(getStringTable().getString(offset)) + "\")",
                                                              reader.getLocator()));
                s = ""; // Must at least indicate that the token was recognized.
            }
            break;
        case GlobalToken.EXT_0:
        case GlobalToken.EXT_1:
        case GlobalToken.EXT_2:
            s = getTokenDictionary().getExtensionValue(token);
            if (s == null) {
                // Invalid or disallowed extension
                getErrorHandler().error(new SAXParseException("WBXMLParser: " + INVALID_EXTENSION
                                                            + " (token=" + toString(token) + ")",
                                                              reader.getLocator()));
                s = ""; // Must at least indicate that the token was recognized.
            }
            break;
        }
        return s;
    }

    /**
     * Handles a possible "code page switch" token.
     * If the token is a valid "code page switch" token, then the code page will be switched
     * as required after reading the page index and the following token will be read
     * and returned.  If the token is not a valid "switch page" token, then no additional
     * tokens will be read and the token will be returned unaltered.
     * @param token the token
     * @return either the same token or the token following the "code page switch" sequence.
     * @exception org.xml.sax.SAXException if attempting to read past end of stream
     */
    private byte handlePossibleCodePageSwitch(byte token) throws SAXException {
        if (token == GlobalToken.SWITCH_PAGE) {
            int offset = reader.getPosition();
            int codePage = (int)reader.readByte() & 0xFF;
            if (this.inAttrState) {
                this.attrCodePage = codePage;
            } else {
                this.tagCodePage = codePage;
            }
            detailOut("SWITCH_PAGE(" + ((this.inAttrState)?"ATTR":"TAG") + "): ===> " +
                codePage + " at bytecode offset " + toHexString(offset));
            // Advance to next token:
            token = reader.readByte();
        }
        return token;
    }

    /**
     * Reads the document version by decoding the <code>version</code> byte.
     * @exception org.xml.sax.SAXException if attempting to read past end of stream
     */
    private void readVersion() throws SAXException {
        byte b = reader.readByte();
        int major = ((b & 0xF0) >>> 4) + 1;
        int minor = (b & 0x0F); // Pretend that minor is always less than 10
        this.versionString = Integer.toString(major) + "." + Integer.toString(minor);
    }

    /**
     * Gets the WBXML version string.
     * @return the WBXML version string
     */
    public String getVersionString() {
        return this.versionString;
    }

    /**
     * Gets the decoded document version string.
     * @return the document version string
     */
    public String getContentVersion() {
        return (getTokenDictionary() != null) ? getTokenDictionary().getVersion() : null;
    }

    /**
     * Gets the decoded document embedded content (MIME) type.
     * @return the document content type
     */
    public String getContentType() {
        return (getTokenDictionary() != null) ? getTokenDictionary().getContentType() : null;
    }

    /**
     * Reads the public identifier by decoding the <code>publicid</code> number.
     * @return the offset of the <code>publicid</code> string in the string table
     *         or <code>(-1)</code> if the public identifier was defined by number.
     * @exception org.xml.sax.SAXException if the public identifier is invalid
     *            or if attempting to read past end of stream.
     */
    private int readPublicID() throws SAXException {
        PublicID pid = null;
        int id = reader.readInteger();
        if (id == 0) {
            // PublicID is defined as an entry in the string table
            int offset = reader.readInteger();  // get the entry offset
            // ... but the string table hasn't been read yet so return the offset:
            return offset;
        } else if (id == 1) {
            // Unknown or missing PublicID:
            reader.getLocator().setColumnNumber(reader.getPosition());
            pid = getDefaultPublicID();
            if (pid != null) {
                getErrorHandler().warning(new SAXParseException("WBXMLParser: " + UNKNOWN_PUBLICID
                                                              + " (ID = 1) ... defaulting to \"" + pid.id + "\"",
                                                                reader.getLocator()));
            } else {
                getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNKNOWN_PUBLICID
                                                                 + " (ID = 1)",
                                                                   reader.getLocator()));
            }
        } else {
            // Look for a well-known PublicID:
            pid = getPublicID(id);
            if (pid == null) {
                // Unknown publicid:
                reader.getLocator().setColumnNumber(reader.getPosition());
                getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNKNOWN_PUBLICID
                                                                 + " (ID = " + id + ")",
                                                                   reader.getLocator()));
            } else {
                reader.getLocator().setPublicId(pid.id);
            }
        }
        this.publicID = pid;
        return (-1);    // Complete
    }

    /**
     * Reads the character encoding by decoding the <code>MIBEnum</code> value.
     * @return the IANA MIBenum code used for the encoding
     * @exception org.xml.sax.SAXException
     *            if the <code>MIBEnum</code> value is invalid or unsupported
     *            or if attempting to read past end of stream
     */
    private int readCharacterEncoding(CharacterEncoding defaultEncoding) throws SAXException {
        reader.getLocator().setColumnNumber(reader.getByteCount());
        int ianaMIBenum = reader.readInteger();
        if (ianaMIBenum != 0) {
            CharacterEncoding enc = CharacterEncoding.findEncoding(ianaMIBenum);
            if (enc != null) {
                reader.setEncoding(enc);
            } else {
                getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + BAD_IANAMIBENUM_VALUE
                                                                 + " (value = " + ianaMIBenum + ")",
                                                                   reader.getLocator()));
            }
        } else {    // Unregistered values encoded as zero.
            //TODO: Should a warning be sent for unregistered values?
            reader.setEncoding(defaultEncoding);
        }
        return ianaMIBenum;
    }

    /**
     * Gets the document character encoding.
     * @return the character encoding
     */
    public CharacterEncoding getCharacterEncoding() {
        return reader.getEncoding();
    }

    /**
     * Reads a possible string value either inline or from the string table.
     * @param token the token
     * @return the string or <code>null</code> if token is not a string reference
     * @exception org.xml.sax.SAXException if a string table offset is invalid
     *            or if attempting to read past end of stream
     */
    private String readString(byte token) throws SAXException {
        String s = null;
        if (token == GlobalToken.STR_I) {
            // Inline string:
            s = reader.readString();
        } else if (token == GlobalToken.STR_T) {
            // String obtained by offset into string table:
            s = getStringTable().getString(reader.readInteger());
        }
        return s;
    }

    /**
     * Reads a possible character entity value.
     * This is encoded as a single UCS-4 character.
     * @param token the token
     * @return the string or <code>null</code> if token is not a character entity token
     * @exception org.xml.sax.SAXException if attempting to read past end of stream
     */
    private String readCharacterEntity(byte token) throws SAXException {
        String s = null;
        if (token == GlobalToken.ENTITY) {
            reader.getLocator().setColumnNumber(reader.getByteCount());
            int ucs4Character = reader.readInteger();
            // Convert UCS-4 character into UTF-16 Unicode:
            if ((ucs4Character >= 0) && (ucs4Character <= 0xFFFF)) {
                s = new String(new char[] {(char)(ucs4Character & 0xFFFF)});
            } else if ((ucs4Character >= 0x10000) && (ucs4Character <= 0x10FFFF)) {
                int highSurrogate = 0xD800 + (((ucs4Character - 0x10000) >>> 10) & 0x3FF);
                int lowSurrogate = 0xDC00 + ((ucs4Character - 0x10000) & 0x3FF);
                s = new String(new char[] {(char)highSurrogate, (char)lowSurrogate});
            } else {
                // Don't know how to convert UCS4 code points above 0x10FFFF.
                s = null;   // Entity ignored
                getErrorHandler().warning(new SAXParseException("WBXMLParser: " + BAD_ENTITY_VALUE
                                                              + " (value = " + ucs4Character + ")",
                                                                reader.getLocator()));
            }
        }
        return s;
    }

    /**
     * Registers a new Public Identifier and token dictionary.
     * @param token the WBXML token used to identify the Public Identifier
     * @param pid the Public Identifier
     * @param dict the token dictionary
     */
    public static void registerTokenDictionary(int token, String pid, TokenDictionary dict) {
        if (publicIDs == null) {
            publicIDs = new Hashtable(10);
        }
        if (tokenDictionaries == null) {
            tokenDictionaries = new Hashtable(10);
        }
        if (pid != null) {
            // Add definition for the publicID:
            publicIDs.put(new Integer(token), new PublicID(token, pid));

            if (dict != null) {
                // Add dictionary support for the publicID:
                tokenDictionaries.put(pid, dict);
            } else {
                // Remove dictionary support for the publicID:
                tokenDictionaries.remove(pid);
            }
        } else {
            Integer key = new Integer(token);
            PublicID p = (PublicID)publicIDs.get(key);
            if (p != null) {
                // Remove definition for the publicID:
                publicIDs.remove(key);
                // Remove dictionary support for the publicID:
                tokenDictionaries.remove(p.id);
            }
        }
    }

    /**
     * Gets the current TokenDictionary.
     * @return the current TokenDictionary
     */
    private TokenDictionary getTokenDictionary() {
        if (tokenDictionary == null) {
            if (getPublicID().id != null) {
                if (tokenDictionaries != null) {
                    tokenDictionary = (TokenDictionary)
                                      tokenDictionaries.get(getPublicID().id);
                    if ((tokenDictionary == null) && (getDefaultPublicID() != null)) {
                        tokenDictionary = (TokenDictionary)
                                        tokenDictionaries.get(getDefaultPublicID().id);
                    }
                }

            } else {
                tokenDictionary = null;
            }
        }
        return this.tokenDictionary;
    }

    /**
     * Gets the public identifier.
     * @return the public identifier
     */
    public PublicID getPublicID() {
        return this.publicID;
    }

    /**
     * Gets the public identifier for a given token value.
     * @param token the WBXML token used to identify the Public Identifier
     * @return the public identifier for the given token value.
     */
    private static PublicID getPublicID(int token) {
        PublicID pid = null;
        if ((publicIDs != null) && (token > 1)) {
            pid = (PublicID)publicIDs.get(new Integer(token));
        }
        return pid;
    }

    /**
     * Reads the string table.
     * @return the bytecode offset of the string table
     * @exception org.xml.sax.SAXException if string table larger than specified
     *            or if attempting to read past end of stream
     */
    private int readStringTable() throws SAXException {
        int stringTableOffset = reader.getByteCount();
        this.stringTable = reader.readStringTable();
        return stringTableOffset;
    }

    /**
     * Gets the string table.
     * @return the string table
     */
    private StringTable getStringTable() {
        return this.stringTable;
    }

    /**
     * Gets the ContentHandler instance to use for issuing document event "detail" messages.
     * @return the DocumentHandler instance
     */
    private DiagnosticContentHandler getDocumentEventMessageHandler() {
        if (documentEventMessagesHandler == null) {
            documentEventMessagesHandler = new DiagnosticContentHandler() {
                private Locator locator;
                private PrintWriter writer;
                private boolean documentEventMessagesEnabled;
                public void setDocumentLocator(Locator locator) {
                    this.locator = locator;
                }
                public void setWriter(PrintWriter writer) {
                    this.writer = writer;
                }
                public void setDocumentEventMessagesEnabled(boolean enabled) {
                    this.documentEventMessagesEnabled = enabled;
                }
                public void startDocument() throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        int offset = locator.getColumnNumber();
                        writer.println("WBXMLParser.parse.startDocument(): at bytecode offset: " + toHexString(offset));
                    }
                }
                public void endDocument() throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        int offset = locator.getColumnNumber();
                        writer.println("WBXMLParser.parse.endDocument(): at bytecode offset: " + toHexString(offset));
                    }
                }
                public void startElement(String nsURI, String locName, String qname,
                  Attributes attrs) throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        int offset = locator.getColumnNumber();
                        writer.println("WBXMLParser.parse.startElement(\"" + qname + "\", " + WBXMLParser.toString(attrs) + "): at bytecode offset: " + toHexString(offset));
                    }
                }
                public void endElement(String nsURI, String locName, String qname)
                        throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        int offset = locator.getColumnNumber();
                        writer.println("WBXMLParser.parse.endElement(\"" + qname + "\"): at bytecode offset: " + toHexString(offset));
                    }
                }
                public void characters(char[] ch, int start, int length) throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        int offset = locator.getColumnNumber();
                        writer.println("WBXMLParser.parse.characters(" + StringUtils.getDisplayableString(new String(ch, start, length)) + ", " + start + ", " + length + "): at bytecode offset: " + toHexString(offset));
                    }
                }
                public void opaqueData(byte[] data) throws SAXException {
                    if (writer != null) {
                        int offset = locator.getColumnNumber() - data.length;
                        writer.println("WBXMLParser.parse.opaqueData(" + WBXMLParser.toString(data) + "): at bytecode offset: " + toHexString(offset));
                    }
                }
                public void ignorableWhitespace(char[] ch, int start, int length) throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        // Message Never Sent
                    }
                }
                public void processingInstruction(String target, String data) throws SAXException {
                    if ((writer != null) && documentEventMessagesEnabled) {
                        int offset = locator.getColumnNumber();
                        writer.println("WBXMLParser.parse.processingInstruction(\"" + target + "\", \"" + data + "\"): at bytecode offset: " + toHexString(offset));
                    }
                }
            };
        }

        return documentEventMessagesHandler;
    }

    /**
     * Gets the ErrorHandler instance to use.
     * If none has been registered, then use the default error handler.
     * @return the ErrorHandler instance
     */
    private ErrorHandler getErrorHandler() {
        if (errorHandler != null) {
            return errorHandler;
        } else {
            if (defaultErrorHandler == null) {
                defaultErrorHandler = new DefaultErrorHandler();
            }
            return defaultErrorHandler;
        }
    }


    /**
     * Inner Class: A diagnostic ContentHandler for the WBXML parser.
     */
    private abstract class DiagnosticContentHandler extends DefaultHandler {
        public abstract void opaqueData(byte[] data) throws SAXException;
        public abstract void setWriter(PrintWriter writer);
        public abstract void setDocumentEventMessagesEnabled(boolean enabled);
    }


    /**
     * Inner Class: The default ErrorHandler for the WBXML parser.
     */
    private class DefaultErrorHandler implements ErrorHandler {
        /**
         * Handles a recoverable parsing warning.
         * @exception org.xml.sax.SAXParseException if a string table offset is invalid
         */
        public void warning(SAXParseException exception) throws SAXException {
            int offset = exception.getColumnNumber();
            MessageWriters.WarnMsg.println("WBXMLParser.parse: "
                                         + exception.getMessage()
                                         + " at bytecode offset: " + toHexString(offset));
        }

        /**
         * Handles a recoverable parsing error.
         * @exception org.xml.sax.SAXParseException if a string table offset is invalid
         */
        public void error(SAXParseException exception) throws SAXException {
            int offset = exception.getColumnNumber();
            MessageWriters.ErrorMsg.println("WBXMLParser.parse: "
                                          + exception.getMessage()
                                          + " at bytecode offset: " + toHexString(offset));
        }

        /**
         * Handles a non-recoverable parsing error.
         * @exception org.xml.sax.SAXParseException if a string table offset is invalid
         */
        public void fatalError(SAXParseException exception) throws SAXException {
            int offset = exception.getColumnNumber();
            throw new SAXException("WBXMLParser.parse: "
                                 + exception.getMessage()
                                 + " at bytecode offset: " + toHexString(offset));
        }
    }   // End of DefaultErrorHandler


    /**
     * Inner Class: A reader for WBXML content.
     */
    private class Reader {
        /**
         * Creates a new Reader.
         * @param content the content buffer
         * @param enc the character encoding of text entities
         * @exception org.xml.sax.SAXException if <code>enc</code> is invalid or unsupported
         */
        private Reader(InputStream in, CharacterEncoding enc) throws SAXException {
            detailOut("new WBXMLParser.Reader: in=" + in + ", enc = " + enc);
            this.contentSource = in;
            this.eos = false;
            this.byteCount = 0;
            this.locator = new LocatorImpl();
            this.locator.setColumnNumber(0);
            this.locator.setLineNumber(0);
            setEncoding(enc);
        }

        /**
         * Checks for end of stream.
         * @return <code>true</code> if reader has reached the end of the stream.
         */
        private boolean isEndOfStream() {
            return eos;
        }

        /**
         * Gets the current position.
         * This is the location of the last byte read.
         * @return the current bytecode offset
         */
        private int getPosition() {
            return getByteCount() - 1;
        }

        /**
         * Gets the number of bytes read so far.
         * This is also the location of the next byte to read.
         * @return the number of bytes read
         */
        private int getByteCount() {
            return byteCount;
        }

        /**
         * Gets the current SAX Locator implementation.
         * @return the SAX Locator implementation
         */
        private LocatorImpl getLocator() {
            return locator;
        }

        /**
         * Sets the character encoding for text entities.
         * @param enc the character encoding of text entities
         * @exception org.xml.sax.SAXException if <code>enc</code> is invalid or unsupported
         */
        private void setEncoding(CharacterEncoding enc) throws SAXException {
            this.characterEncoding = enc;
            // Determine the encoding-specific string terminator sequence:
            this.stringTerminator = (enc != null) ? enc.getStringTerminator() : null;
            detailOut("Reader.setEncoding: enc=" + enc + ", terminator=" +
                WBXMLParser.toString(stringTerminator));
            if (enc == null) {
                getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNSUPPORTED_ENCODING
                                                                 + " (enc = " + enc + ")",
                                                                   getLocator()));
            }
        }

        /**
         * Gets the current character encoding.
         * @return the character encoding
         */
        private CharacterEncoding getEncoding() {
            return characterEncoding;
        }

        /**
         * Gets the last bytecode sequence for debugging purposes.
         * @return the last bytecode sequence read by readInteger() or readString().
         */
        private byte[] getLastBytecodeSequence() {
            return lastBytecodeSequence;
        }

        /**
         * Gets the last byte value read.
         */
        private byte getLastToken() {
            return lastToken;
        }

        /**
         * Reads a byte from the current position.
         * @return the byte value.
         * @exception org.xml.sax.SAXException if attempting to read past end of stream
         */
        private byte readByte() throws SAXException {
            int byteVal;
            try {
                byteVal = contentSource.read();
                if (byteVal >= 0) {
                    lastToken = (byte)byteVal;
                    ++byteCount;
                }
            }
            catch (IOException ex) { byteVal = (-1); }
            if (byteVal < 0) {
                eos = true;
                throw new SAXException(new IOException("WBXMLParser: " + END_OF_STREAM));
            }
            detailOut("Reader.readByte: " + WBXMLParser.toString((byte)byteVal));
            return (byte)byteVal;
        }

        /**
         * Reads an array of bytes from the current position.
         * @param length the number of bytes to read.
         * @return the byte array value.
         * @exception org.xml.sax.SAXException if attempting to read past end of stream
         */
        private byte[] readBytes(int length) throws SAXException {
            byte[] buf = new byte[length];
            for (int i = 0; i < length; i++) {
                buf[i] = readByte();
            }
            detailOut("Reader.readBytes(" + length + "): " + WBXMLParser.toString(buf));
            return buf;
        }

        /**
         * Reads a multi-byte integer from the current position.
         * @return the integer value.
         * @exception org.xml.sax.SAXException if attempting to read past end of stream
         */
        private int readInteger() throws SAXException {
            int value = 0;
            lastBytecodeSequence = null;
            Vector bytes = new Vector(2);
            for (boolean moreBytes = true; moreBytes;) {
                byte b = readByte();
                bytes.add(new Byte(b));
                value = (value << 7) | (b & 0x7F);
                moreBytes = ((b & 0x80) != 0);
            }
            byte[] buf = new byte[bytes.size()];
            for (int i = 0; i < buf.length; i++) {
                buf[i] = ((Byte)bytes.elementAt(i)).byteValue();
            }
            lastBytecodeSequence = buf;
            detailOut("Reader.readInteger: " + value);
            return value;
        }

        /**
         * Reads a string from the current position.
         * @return the string value.
         * @exception org.xml.sax.SAXException if attempting to read past end of stream
         */
        private String readString() throws SAXException {
            StringBuffer stringValue = new StringBuffer();
            getLocator().setColumnNumber(getByteCount());
            if (!readString(stringValue, 0)) {
                getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + UNTERMINATED_STRING,
                                                                   getLocator()));
            }
            return stringValue.toString();
        }

        /**
         * Reads a string from the current position.
         * @param retnbuf a buffer to return the string value.
         * @param maxlen if greater than 0 then limit to that many bytes.
         *               Otherwise, read until a string termination sequence is found
         * @return <code>true</code> if the string was terminated properly.
         * @exception org.xml.sax.SAXException if attempting to read past end of stream
         */
        private boolean readString(StringBuffer retnbuf, int maxlen) throws SAXException {
            // Determine length of string by scanning for string terminator sequence.
            // The string terminator sequence is a function of the character encoding.
            // This is really lame, but that's what WBXML expects a decoder to do.
            int length = 0;
            int stringTerminatorLength = 0;
            boolean stringTerminated = false;
            detailOut("Reader.readString(" + maxlen + ")");
            lastBytecodeSequence = null;
            Vector readAhead = new Vector(10);
            // Scan for string terminator sequence and count intervening bytes:
            for (boolean done = false; !done;) {
                // Read ahead the same number of bytes as is in the string terminator sequence:
                for (int i = 0; i < stringTerminator.length; i++, length++) {
                    if ((maxlen > 0) && (length >= maxlen)) {
                        // Early termination due to maxlen:
                        done = true;
                        break;
                    }
                    readAhead.add(new Byte(readByte()));
                }
                if (!done) {
                    // Presume a string terminator sequence match and try to detect a mismatch:
                    done = true;
                    for (int i = 0; i < stringTerminator.length; i++) {
                        int index = length - stringTerminator.length + i;
                        byte bufferedByte = ((Byte)readAhead.elementAt(index)).byteValue();
                        if (stringTerminator[i] != bufferedByte) {
                            done = false;
                            break;
                        }
                    }
                    if (done) {
                        // String terminator found, so assign length:
                        stringTerminatorLength = stringTerminator.length;
                        stringTerminated = true;
                    }
                }
            }

            String s = null;
            if (length <= stringTerminatorLength) {
                s = "";
                lastBytecodeSequence = new byte[stringTerminatorLength];
                if (stringTerminatorLength > 0) {
                    System.arraycopy(stringTerminator, 0, lastBytecodeSequence, 0, stringTerminatorLength);
                }
                //lastBytecodeSequence = stringTerminator;
            } else {
                byte[] buf = new byte[length];
                for (int i = 0; i < length; i++) {
                    buf[i] = ((Byte)readAhead.elementAt(i)).byteValue();
                }
                lastBytecodeSequence = buf;
                detailOut("Reader.readString: " + WBXMLParser.toString(buf));
                s = characterEncoding.decodeString(buf, 0, (length - stringTerminatorLength));
            }
            detailOut("Reader.readString: " + StringUtils.getDisplayableString(s));
            retnbuf.replace(0, retnbuf.length(), s);
            return stringTerminated;
        }

        /**
         * Reads a string table from the current position.
         * @return the string table read.
         * @exception org.xml.sax.SAXException if string table larger than specified
         *            or if attempting to read past end of stream
         */
        private StringTable readStringTable() throws SAXException {
            StringTable table = new StringTable();
            table.readStringTable(this);
            return table;
        }


        private InputStream contentSource;
        private int byteCount;
        private boolean eos;    // End of Stream
        private byte lastToken = (byte)(-1);
        private byte[] lastBytecodeSequence;
        private LocatorImpl locator;
        private CharacterEncoding characterEncoding;
        private byte[] stringTerminator;
    }   // End of Reader


    /**
     * Inner Class: The WBXML string table implementation.
     */
    public class StringTable {
        /**
         * Reads the string table.
         * @param reader the reader to use to read the string table.
         * @exception org.xml.sax.SAXException if string table larger than specified
         *            or if attempting to read past end of stream
         */
        private void readStringTable(Reader reader) throws SAXException {
            this.tableSize = reader.readInteger();  // String table size in bytes
            detailOut("readStringTable: size=" + tableSize);
            if (this.tableSize > 0) {
                offsets = new Vector(5);
                byteArrays = new Vector(5);
                stringTable = new Hashtable(5);
                int startPos = reader.getByteCount();
                int endPos = startPos + this.tableSize;
                int currentPos;
                int maxlen = this.tableSize;
                for (currentPos = startPos; currentPos < endPos; currentPos = reader.getByteCount()) {
                    int offset = currentPos - startPos;
                    StringBuffer stringValue = new StringBuffer();
                    reader.getLocator().setColumnNumber(currentPos);
                    boolean terminated = reader.readString(stringValue, maxlen);
                    detailOut("readStringTable: offset=" + offset + ", limit=" + maxlen +
                        ", string=\"" + stringValue.toString() + "\", terminated=" + terminated);
                    if (!terminated) {
                        // Note: This should be an error, but there are too many offending encoders!
                        // OTOH, it is easily recoverable with no bad side effects, so a warning is probably okay.
                        getErrorHandler().warning(new SAXParseException("WBXMLParser: " + UNTERMINATED_STRING
                                                                    + ": " + WBXMLParser.toString(stringValue.toString()),
                                                                      reader.getLocator()));
                    }
                    Integer offsetKey = new Integer(offset);
                    offsets.add(offsetKey);
                    stringTable.put(offsetKey, stringValue.toString());
                    byte[] lastBytecodeSequence = reader.getLastBytecodeSequence();
                    if (lastBytecodeSequence != null) {
                        maxlen -= lastBytecodeSequence.length;
                    }
                    byteArrays.add(lastBytecodeSequence);
                }
                if (currentPos > endPos) {
                    // Can this "error" ever occur?!
                    reader.getLocator().setColumnNumber(reader.getPosition());
                    getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + STRINGTABLE_TOO_BIG
                                                                     + " (" + (currentPos-startPos) + " > " + this.tableSize + ")",
                                                                       reader.getLocator()));
                }
            }
        }

        /**
         * Gets a string from the table.
         * @param offset the offset of the string in the string table
         * @return the string or <code>null</code> if not found.
         * @exception org.xml.sax.SAXException if string table offset is invalid
         */
        public String getString(int offset) throws SAXException {
            String s = null;
            if (stringTable != null) {
                Integer key = new Integer(offset);
                s = (String)stringTable.get(key);
                if (s == null) {
                    //reader.getLocator().setColumnNumber(reader.getPosition());
                    getErrorHandler().fatalError(new SAXParseException("WBXMLParser: " + BAD_STRINGTABLE_OFFSET
                                                                     + " (offset = " + offset + ")",
                                                                       reader.getLocator()));
                }
            }
            return s;
        }

        /**
         * @return the string representation of this object.
         */
        public String toString() {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append("[");
            sbuf.append(this.tableSize);
            sbuf.append(" bytes] ");
            sbuf.append('{');
            if ((stringTable != null) && (stringTable.size() > 0)) {
                Enumeration e = offsets.elements();
                for (int index = 0; e.hasMoreElements(); index++) {
                    Integer offset = (Integer)e.nextElement();
                    String value = (String)stringTable.get(offset);
                    sbuf.append('\n');
                    sbuf.append("    offset ");
                    sbuf.append(offset.intValue());
                    sbuf.append(" = ");
                    sbuf.append(StringUtils.getDisplayableString(value));
                    sbuf.append(" = ");
                    sbuf.append(WBXMLParser.toString((byte[])byteArrays.elementAt(index)));
                }
            }
            sbuf.append('}');
            return sbuf.toString();
        }


        private int tableSize;          // Number of bytes in the string table
        private Vector offsets;         // to preserve order during debug dump
        private Vector byteArrays;      // actual bytecode for debug dump
        private Hashtable stringTable;  // key = offset; value = string
    }   // End of StringTable


    /**
     * Inner Class: A descriptor for a Public Identifier.
     */
    private static class PublicID {
        /**
         * Create a new PublicID.
         * @param value the token value
         * @param id the public identifier string
         */
        PublicID(int value, String id) {
            this.value = value;
            this.id = id;
        }

        /**
         * Create a new PublicID.
         * @param id the public identifier string
         */
        PublicID(String id) {
            this(0x01, id);
        }

        /**
         * @return the string representation of this object.
         */
        public String toString() {
            StringBuffer sbuf = new StringBuffer();
            if (id != null) {
                sbuf.append('\"');
                sbuf.append(id);
                sbuf.append('\"');
            } else {
                sbuf.append("null");
            }
            sbuf.append(" (");
            sbuf.append(value);
            sbuf.append(')');
            return sbuf.toString();
        }

        int value;      // The token value
        String id;      // The PublicID string
    }   // End of PublicID


    /**
     * Interface: Global WBXML tokens.
     */
    public static interface GlobalToken {
        /** Change the code page for the current token state. Followed by a single u_int8 indicating the new code page number. */
        public static final byte SWITCH_PAGE =  (byte)0x00;
        /** Indicates the end of an attribute list or the end of an element. */
        public static final byte END =          (byte)0x01;
        /** A character entity. Followed by a <code>mb_u_int32</code> encoding the character entity number. */
        public static final byte ENTITY =       (byte)0x02;
        /** Inline string. Followed by a <code>termstr</code>. */
        public static final byte STR_I =        (byte)0x03;
        /** An unknown tag or attribute name. Followed by a <code>mb_u_int32</code> that encodes an offset into the string table. */
        public static final byte LITERAL =      (byte)0x04;
        /** Inline string document-type-specific extension token. Token is followed by a <code>termstr</code>. */
        public static final byte EXT_I_0 =      (byte)0x40;
        /** Inline string document-type-specific extension token. Token is followed by a <code>termstr</code>. */
        public static final byte EXT_I_1 =      (byte)0x41;
        /** Inline string document-type-specific extension token. Token is followed by a <code>termstr</code>. */
        public static final byte EXT_I_2 =      (byte)0x42;
        /** Processing instruction. */
        public static final byte PI =           (byte)0x43;
        /** Unknown tag, with content. */
        public static final byte LITERAL_C =    (byte)0x44;
        /** Inline integer document-type-specific extension token. Token is followed by a <code>mb_u_int32</code>. */
        public static final byte EXT_T_0 =      (byte)0x80;
        /** Inline integer document-type-specific extension token. Token is followed by a <code>mb_u_int32</code>. */
        public static final byte EXT_T_1 =      (byte)0x81;
        /** Inline integer document-type-specific extension token. Token is followed by a <code>mb_u_int32</code>. */
        public static final byte EXT_T_2 =      (byte)0x82;
        /** String table reference. Followed by a <code>mb_u_int32</code> encoding a byte offset from the beginning of the string table. */
        public static final byte STR_T =        (byte)0x83;
        /** Unknown tag, with attributes. */
        public static final byte LITERAL_A =    (byte)0x84;
        /** Single-byte document-type-specific extension token. */
        public static final byte EXT_0 =        (byte)0xC0;
        /** Single-byte document-type-specific extension token. */
        public static final byte EXT_1 =        (byte)0xC1;
        /** Single-byte document-type-specific extension token. */
        public static final byte EXT_2 =        (byte)0xC2;
        /** Opaque document-type-specific data. */
        public static final byte OPAQUE =       (byte)0xC3;
        /** Unknown tag, with content and attributes. */
        public static final byte LITERAL_AC =   (byte)0xC4;
    }   // End of GlobalToken


    /**
     * Interface: A generic handler for WBXML tokens.
     *            An implementation instance should be registered
     *            for each PublicID that must be supported.
     */
    public static interface TokenDictionary {
        /**
         * Gets the version string for the dictionary.
         * @return the content version string
         */
        public String getVersion();

        /**
         * Gets the content (MIME) type supported by the dictionary.
         * @return the content type
         */
        public String getContentType();

        /**
         * Gets the string representation of a tag token.
         * @param codepage the codepage for the token
         * @param token the token value
         * @return the string representation of the tag
         */
        public String getTag(int codepage, byte token);

        /**
         * Gets the string representation of an attribute start token.
         * @param codepage the codepage for the token
         * @param token the token value
         * @param valueReturn a single-element array for returning
         *                    an implicit attribute value if it exists
         * @return the string representation of the attribute start token
         */
        public String getAttrStart(int codepage, byte token, String[] valueReturn);

        /**
         * Gets the string representation of an attribute value token.
         * @param codepage the codepage for the token
         * @param token the token value
         * @return the string representation of the attribute value
         */
        public String getAttrValue(int codepage, byte token);

        /**
         * Gets the string representation of a string extension.
         * @param token the token value (EXT_I_0, EXT_I_1, or EXT_I_2)
         * @param data the character data for the extension
         * @return the string representation of the extension
         *         or <code>null</code> if invalid or unsupported
         */
        public String getExtensionValue(byte token, String data);

        /**
         * Gets the string representation of a integer extension.
         * @param token the token value (EXT_T_0, EXT_T_1, or EXT_T_2)
         * @param data the integer data for the extension
         * @param stringTable the string table for use by the extension handler
         * @return the string representation of the extension
         *         or <code>null</code> if invalid or unsupported
         * @exception org.xml.sax.SAXException if string table offset is invalid
         */
        public String getExtensionValue(byte token, int data, StringTable stringTable) throws SAXException;

        /**
         * Gets the string representation of a single-byte extension.
         * @param token the token value (EXT_0, EXT_1, or EXT_2)
         * @return the string representation of the extension
         *         or <code>null</code> if invalid or unsupported
         */
        public String getExtensionValue(byte token);

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
        public String filterCDATA(String tag, String target, String cdata);

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
         *         or <code>null</code> if invalid or unsupported
         */
        public String filterOpaqueData(String tag, String target, byte[] bytes, CharacterEncoding enc);

    }   // End of TokenDictionary


    /**
     * Returns the hexadecimal string representation of a numeric value.
     * @param value the value
     * @return the hexadecimal string representation.
     */
    private static String toHexString(int value) {
        return "" + Integer.toString(value, 16).toUpperCase() + " hex";
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a byte.
     * @param b the byte
     * @return the string representation of a byte.
     */
    private static String toString(byte b) {
        int hi = ((b & 0xF0) >>> 4);    // upper nybble
        int lo = (b & 0x0F);            // Lower nybble
        return "" + ((hi >= 10) ? (char)('A' + (hi - 10)) : (char)('0' + hi))
                  + ((lo >= 10) ? (char)('A' + (lo - 10)) : (char)('0' + lo));
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a byte array.
     * @param bytes the byte array
     * @return the string representation of a byte array.
     */
    private static String toString(byte[] bytes) {
        StringBuffer sbuf = new StringBuffer();
        if (bytes != null) {
            sbuf.append('[');
            for (int i = 0; i < bytes.length; i++) {
                if (i > 0) sbuf.append(',');
                sbuf.append(toString(bytes[i]));
            }
            sbuf.append(']');
        } else {
            sbuf.append("null");
        }
        return sbuf.toString();
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a char array.
     * @param chars the char array
     * @return the string representation of a char array.
     */
    private static String toString(char[] chars) {
        StringBuffer sbuf = new StringBuffer();
        if (chars != null) {
            sbuf.append('[');
            for (int i = 0; i < chars.length; i++) {
                if (i > 0) sbuf.append(',');
                sbuf.append("\'\\u");
                sbuf.append(toString((byte)(chars[i] >>> 8)));
                sbuf.append(toString((byte)(chars[i] & 0xFF)));
                sbuf.append('\'');
            }
            sbuf.append(']');
        } else {
            sbuf.append("null");
        }
        return sbuf.toString();
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns a displayable representation of a string value.
     * @param s the string value
     * @return the displayable string.
     */
    private static String toString(String s) {
        if (s != null) {
            StringBuffer sbuf = new StringBuffer(s.length());
            sbuf.append('\"');
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                if (c == '\0') {                // Null character
                    sbuf.append("\0");
                } else if (c == '\n') {         // Newline
                    sbuf.append("\\n");
                } else if (c == '\r') {         // Carriage return
                    sbuf.append("\\r");
                } else if (c == '\t') {         // Horizontal tab
                    sbuf.append("\\t");
                } else if (c == '\"') {         // Double quote
                    sbuf.append("\\\"");
                } else if (c == '\'') {         // Single quote
                    sbuf.append("\\\'");
                } else if (c == '\\') {         // Literal backslash
                    sbuf.append("\\\\");
                } else if ((((int)c & 0xFFFF) < 128) && !Character.isISOControl(c)) {
                    sbuf.append(c);     // Normal 8-bit printable character
                } else if (((int)c & 0xFFFF) < 256) { // 8-bit Unicode (US-ASCII)
                    sbuf.append("\\x");
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                } else {                            // 16-bit Unicode
                    sbuf.append("\\u");
                    sbuf.append(Integer.toString(((c >> 12) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  8) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c >>  4) & 0x000F), 16).toUpperCase());
                    sbuf.append(Integer.toString(((c      ) & 0x000F), 16).toUpperCase());
                }
            }
            sbuf.append('\"');
            return sbuf.toString();
        } else {
            return "null";
        }
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a SAX AttributeList.
     * @param bytes the byte array
     * @return the string representation of a byte array.
     */
    private static String toString(Attributes attrs) {
        StringBuffer sbuf = new StringBuffer();
        if (attrs != null) {
            sbuf.append('{');
            for (int i = 0; i < attrs.getLength(); i++) {
                if (i > 0) sbuf.append(',');
                sbuf.append(attrs.getQName(i));
                sbuf.append('=');
                sbuf.append(StringUtils.getDisplayableString(attrs.getValue(i)));
            }
            sbuf.append('}');
        } else {
            sbuf.append("null");
        }
        return sbuf.toString();
    }

    /**
     * DEBUG CONVENIENCE METHOD
     * Returns the string representation of a SAX Locator.
     * @param bytes the byte array
     * @return the string representation of a byte array.
     */
    private static String toString(Locator locator) {
        StringBuffer sbuf = new StringBuffer();
        if (locator != null) {
            sbuf.append('[');
            sbuf.append("publicID=");
            sbuf.append(StringUtils.getDisplayableString(locator.getPublicId()));
            sbuf.append(", systemID=");
            sbuf.append(StringUtils.getDisplayableString(locator.getSystemId()));
            sbuf.append(", colummn#=");
            sbuf.append(locator.getColumnNumber());
            sbuf.append(", line#=");
            sbuf.append(locator.getLineNumber());
            sbuf.append(']');
        } else {
            sbuf.append("null");
        }
        return sbuf.toString();
    }


    // A bunch of error message strings:
    private static final String MISSING_STREAM = "Missing Source Stream";
    private static final String END_OF_STREAM = "End of Stream";
    private static final String EXCESS_BYTES = "Excess Bytecode After End of Document";
    private static final String UNDEFINED_PUBLICID = "No PublicID Defined";
    private static final String UNKNOWN_PUBLICID = "Unknown or Missing PublicID";
    private static final String BAD_IANAMIBENUM_VALUE = "Invalid or Unsupported Character Encoding";
    private static final String UNSUPPORTED_ENCODING = "Unsupported Character Encoding";
    private static final String UNKNOWN_TOKEN_DICTIONARY = "No Token Dictionary Defined for the PublicID";
    private static final String MISSING_ROOT_ELEMENT = "Missing Root Element";
    private static final String STRINGTABLE_TOO_BIG = "String Table Larger than Specified";
    private static final String BAD_STRINGTABLE_OFFSET = "Bad String Table Offset";
    private static final String UNTERMINATED_STRING = "Unterminated String Value";
    private static final String UNKNOWN_ELEMENT_TOKEN = "Unknown Element Token";
    private static final String INVALID_ELEMENT_NAME = "Invalid Element Name";
    private static final String UNKNOWN_ATTRSTART_TOKEN = "Unknown Attribute Start Token";
    private static final String INVALID_ATTR_NAME = "Invalid Attribute Name";
    private static final String UNKNOWN_ATTRVALUE_TOKEN = "Unknown Attribute Value Token";
    private static final String MISSING_ATTRIBUTES = "Missing Attribute(s) (expected one or more)";
    private static final String UNEXPECTED_CONTENT_TOKEN = "Unknown or Unexpected Content Token";
    private static final String INVALID_OPAQUE_DATA = "Invalid or Unexpected Opaque Data";
    private static final String INVALID_EXTENSION = "Invalid or Unexpected Extension";
    private static final String MISSING_CONTENT = "Missing Content";
    private static final String BAD_ENTITY_VALUE = "Character Entity not Representable in UTF16";
    private static final String BAD_PI_VALUE_TOKEN = "Bad Value Token in Processing Instruction";
    private static final String UNKNOWN_PI_TARGET = "Unknown Target in Processing Instruction";
    private static final String INVALID_PI_TARGET_NAME = "Invalid Target Name in Processing Instruction";


    // Well-known Public Identifiers:
    private static Hashtable publicIDs = null;

    // A registry of TokenDictionaries.
    // key = PublicID string
    // value = TokenDictionary instance
    private static Hashtable tokenDictionaries = null;


    private Reader reader = null;
    private String versionString = null;
    private PublicID defaultPublicID = null;
    private PublicID publicID = null;
    private String systemID = null;
    private StringTable stringTable = null;
    private TokenDictionary tokenDictionary = null;
    private boolean inAttrState = false;
    private int tagCodePage = 0;
    private int attrCodePage = 0;
    private DiagnosticContentHandler documentEventMessagesHandler;

    // Callbacks for event notification:
    private EntityResolver entityResolver;
    private ContentHandler contentHandler;
    private ErrorHandler errorHandler;
    private ErrorHandler defaultErrorHandler;
}   // End of WBXMLParser

