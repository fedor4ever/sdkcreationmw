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

/* java.io */
import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;

/* org.xml.sax */
import org.xml.sax.InputSource;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.CharacterEncoding;

/**
 * Decodes binary WML
 */
public abstract class DecoderTool {

    private DecoderTool() {
    }

    /**
     * Decodes binary WBXML into the source code. Throws an exception if
     * the input stream does not look like a valid WBXML.
     *
     * @return <code>true</code> is something has been decoding, or
     *         <code>false</code> if input stream appears to contain
     *         parsable WBXML but nothing has been written to the output
     *         stream.
     */
    public static boolean decode(InputStream in, OutputStream out)
    throws Exception {

        WMLElement root = decodeWBXML(in);
        if (root != null) {
            PrintWriter writer = new PrintWriter(out);
            root.dumpElement(writer);
            writer.flush();
            return true;
        }

        return false;
    }

    /**
     * Decodes binary WBXML into a WML deck. Throws an exception if
     * the data does not look like a valid WBXML.
     *
     * @return the parsed WML deck.
     */
    public static WMLDeck decode(byte [] data) throws Exception {
        WMLDeck deck = decode(new ByteArrayInputStream(data));
        if (deck != null) {
            deck.setByteCode(data);
            return deck;
        }
        return null;
    }

    /**
     * Decodes binary WBXML into a WML deck. Throws an exception if
     * the input stream does not look like a valid WBXML.
     *
     * @return the parsed WML deck.
     */
    public static WMLDeck decode(InputStream in) throws Exception {
        String enc = CharacterEncoding.getFileEncoding().getJavaName();
        WMLElement root = decodeWBXML(in, enc);
        if (root instanceof WMLDeck) {
            return (WMLDeck)root;
        } else {
            return null;
        }
    }

    /**
     * Decodes binary WBXML into a hierarchy of WMLElements. Note that this
     * methods can be used for decoding push content.
     *
     * @return the root element of the parsed content.
     */
    public static WMLElement decodeWBXML(InputStream in) throws Exception {
        return decodeWBXML(in, "utf-8");
    }

    /**
     * Decodes binary WBXML into a hierarchy of WMLElements. Note that this
     * methods can be used for decoding push content.
     *
     * @return the root element of the parsed content.
     */
    public static WMLElement decodeWBXML(InputStream in, String enc)
    throws Exception {

        InputSource source = new InputSource(in);
        source.setEncoding(enc);
        source.setSystemId("wbxml");

        WML.DocumentHandler dochandler = new WML.DocumentHandler();

        WBXMLParser parser = new WBXMLParser();
        parser.setDefaultPublicID(WMLTokenDictionary.getDefaultPublicID());
        parser.setContentHandler(dochandler);
        parser.parse(source);

        WMLDeck deck = dochandler.getRootWMLDeck();
        if (deck != null) {
            deck.setVersion(parser.getContentVersion());
            deck.setEncoding(parser.getCharacterEncoding());
            return deck;
        } else {
            return dochandler.getRootElement();
        }
    }

    /**
     * The main method to run this class from the command line.
     */
    public static void main(String [] args) {
        if (args.length < 1 || args.length > 2) {
            System.err.println("Usage: " + DecoderTool.class.getName() + " <input> [output]");
        } else {
            try {
                boolean ok;
                InputStream in = new FileInputStream(args[0]);
                if (args.length == 2) {
                    OutputStream out = new FileOutputStream(args[1]);
                    ok = decode(in,out);
                    out.close();
                } else {
                    ok = decode(in,System.out);
                }
                if (!ok) {
                    System.err.println("Hmm... Looks like a valid WBXML file, but something is wrong with it.");
                    System.err.println("Perhaps, this content is not fully supported by the WAP Toolkit");
                }
            } catch (Throwable x) {
                x.printStackTrace();
            }
        }
        System.exit(0);
    }
}
