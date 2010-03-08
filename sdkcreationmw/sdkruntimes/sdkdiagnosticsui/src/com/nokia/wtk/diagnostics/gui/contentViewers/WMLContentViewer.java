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





package com.nokia.wtk.diagnostics.gui.contentViewers;

/* java.io */
import java.io.PrintWriter;
import java.io.StringWriter;

/* java.net */
import java.net.URL;

/* javax.swing.tree */
import javax.swing.tree.TreeModel;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.wbxml.WMLDeck;

/**
 * Content data model for a WML deck
 */
public class WMLContentViewer implements ContentViewer {

    private static final String CARD_LABEL = "Current Card";
    private static final String VERSION_LABEL = "WML v";
    private static final String SOURCE_UNITS = "source characters";
    private static final String BINARY_UNITS = "encoded bytes";
    private static final String UNKNOWN_SIZE = "Unknown";
    private static final String XML_PREAMBLE = "<?xml version=\"1.0\"?>";
    private static final String DISCLAIMER =
    "DISCLAIMER: This source was generated from parsed binary WML content.\n" +
    "This representation of the deck contents does not necessarily preserve\n" +
    "original whitespace or accurately decode any CDATA Section contents,\n" +
    "but otherwise is an accurate representation of the original deck contents\n" +
    "as determined from its WBXML encoding.  If a precise representation is required,\n" +
    "then use the \"Element Tree\" or, if available, the \"Original Source\" view.";

    /** The WML deck represented by this model */
    private WMLDeck deck;
    private String stats;

    /**
     * Creates the <code>WMLDeckModel</code>
     */
    public WMLContentViewer(WMLDeck deck) {
        this.deck = deck;
        stats = createStatsString();
    }

    /**
     * Returns the WML deck represented by this model
     */
    public WMLDeck getDeck() {
        return deck;
    }

    /**
     * Returns the MIME type of the source code representation of this
     * content.
     */
    public String getMimeType() {
        return MimeType.WML_SOURCE;
    }

    /**
     * Returns the MIME type of the binary representation of this content.
     * If content is not encoded, return <code>null</code>
     */
    public String getBinaryMimeType() {
        return MimeType.WML_BINARY;
    }

    /**
     * Returns the URL where the content has been retrieved from
     */
    public String getURL() {
        URL url = deck.getURL();
        return ((url == null) ? null : url.toExternalForm());
    }

    /**
     * Sets the URL where the content has been retrieved from.
     * Normally, this should be a variation of the original URL (i.e.
     * different cards of the same deck)
     */
    public void setURL(String newURL) {
        try {

            // set new URL
            URL url = new URL(newURL);
            deck.setURL(url);

            // update the statistics string
            stats = createStatsString();

        } catch (Exception x) {
            x.printStackTrace();
        }
    }

    /**
     * Returns the status string that reflects the current state of the
     * model.
     */
    public String getStats() {
        return stats;
    }

    /**
     * Returns the tree model of the current page or <code>null</code>
     * if current page cannot be represented as a tree
     */
    public TreeModel getTreeModel() {
        return deck.getTreeModel();
    }

    /**
     * Returns <code>true</code> if current page came in a binary form
     * or has a binary representation.
     */
    public boolean isBinary() {
        return (deck.getBinarySize() > 0);
    }

    /**
     * Return the bytecode representation of this content, or
     * <code>null</code> if it does not have a binary form.
     */
    public byte [] getByteCode() {
        return deck.getByteCode();
    }

    /**
     * Returns the decoded content or <code>null</code> if current
     * page has no binary representation
     */
    public String getDecodedContent() {
        StringWriter sw = new StringWriter();
        PrintWriter w = new PrintWriter(sw);
        w.println(XML_PREAMBLE);
        w.println(deck.getDoctypeString());
        w.println();
        /* w.println("<!-- " + "Deck Source: \"" + deck.getURL() + "\" -->");
        String[] disclaimer = PropertyEditors.tokenize(DISCLAIMER, "\n\r");
        for (int i = 0; i < disclaimer.length; i++) {
            w.println("<!-- " + disclaimer[i] + " -->");
        }
        w.println();  */
        deck.dumpElement(w);
        w.flush();
        w.close();
        return sw.toString();
    }

    /**
     * Returns the original source or <code>null</code> if the original
     * source is unavailable
     */
    public String getOriginalSource() {
        return deck.getSource();
    }

    /**
     * Creates a text string representing a size quantity.
     * @param size the size quantity
     * @param suffix the suffix for the size quantity
     * @return the string
     */
    private static String getSizeLabel(int size, String suffix) {
        StringBuffer buff = new StringBuffer();
        buff.append((size > 0) ? Integer.toString(size) : UNKNOWN_SIZE);
        if ((suffix != null) && (suffix.length() > 0)) {
            buff.append(" ");
            buff.append(suffix);
        }
        return buff.toString();
    }

    /**
     * Creates the status string that reflects the current state of the
     * model.
     */
    private String createStatsString() {

        StringBuffer sb = new StringBuffer();
        sb.append(VERSION_LABEL).append(deck.getVersion());
        sb.append(", ").append(getSizeLabel(deck.getSourceSize(),SOURCE_UNITS));
        sb.append(", ").append(getSizeLabel(deck.getBinarySize(),BINARY_UNITS));
        if (deck.getEncoding() != null) {
            sb.append(", ").append(deck.getEncoding());
        }
        return sb.toString();
    }
}
