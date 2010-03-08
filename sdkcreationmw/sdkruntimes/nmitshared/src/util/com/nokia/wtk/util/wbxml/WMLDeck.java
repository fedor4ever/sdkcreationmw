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

/* java.net */
import java.net.URL;

/* javax.swing.tree */
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeModel;

/* javax.swing.event */
import javax.swing.event.TreeModelListener;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.CharacterEncoding;

/**
 * A WML Deck.
 *
 */
public class WMLDeck extends WMLElement {

    /**
     * Create an empty WML Deck
     */
    public WMLDeck() {
        super(WML.Tag.WML);
        this.url = null;
    }

    /**
     * Set the URL that this deck was created with.
     * @param url the URL
     */
    public void setURL(URL url) {
        this.url = url;
    }

    /**
     * Return the URL that this deck was created with.
     * @return the URL that this deck was created with.
     */
    public URL getURL() {
        return url;
    }

    /**
     * Set the deck's source string
     * @param data the deck's source
     */
    public synchronized void setSource(String data) {
        sourceText = data;
    }

    /**
     * Get the deck's source string
     * @return the deck's source string
     */
    public String getSource() {
        return sourceText;
    }

    /**
     * Set the deck's byte code
     * @param data the deck's byte code
     */
    public synchronized void setByteCode(byte[] data) {
        byteCode = data;
    }

    /**
     * Get the deck's byte code
     * @return the deck's byte code
     */
    public byte[] getByteCode() {
        return byteCode;
    }

    /**
     * Set the deck's character encoding
     * @param encoding the deck's character encoding
     */
    public void setEncoding(CharacterEncoding encoding) {
        this.encoding = encoding;
    }

    /**
     * Get the deck's character encoding
     * @return the deck's character encoding
     */
    public CharacterEncoding getEncoding() {
        return encoding;
    }

    /**
     * Get the number of characters in the deck's source
     * @return the size
     */
    public synchronized int getSourceSize() {
        return (sourceText != null) ? sourceText.length() : 0;
    }

    /**
     * Get the number of bytes in the deck's bytecode
     * @return the size
     */
    public synchronized int getBinarySize() {
        return (byteCode != null) ? byteCode.length : 0;
    }

    /**
     * Set the WML version of the deck
     * @param version the WML version of the deck
     */
    synchronized public void setVersion(String version) {
        wmlVersionString = version;
    }

    /**
     * @return the WML version of the deck
     */
    public String getVersion() {
        if (wmlVersionString == null) {
            wmlVersionString = "1.1";
        }
        return wmlVersionString;
    }

    /**
     * @return the WML version of the deck
     */
    synchronized public String getDoctypeString() {
        String wmlDoctype = null;
        if ("1.1".equals(getVersion())) {
            wmlDoctype = DOCTYPE_WML_1_1;
        } else if ("1.2".equals(getVersion())) {
            wmlDoctype = DOCTYPE_WML_1_2;
        } else if ("1.3".equals(getVersion())) {
            wmlDoctype = DOCTYPE_WML_1_3;
        } else {
            wmlDoctype = DOCTYPE_WML_UNKNOWN;
        }
        return wmlDoctype;
    }

    /**
     * Gets a TreeModel for WML Deck Elements.
     * @return a TreeModel for WML Deck Elements.
     */
    public TreeModel getTreeModel() {
        if (treeModel == null) {
            treeModel = new TreeModel() {
                public Object getRoot() {
                    return WMLDeck.this;
                }
                public Object getChild(Object parent, int index) {
                    if (parent instanceof WMLElement) {
                        WMLElement pElem = (WMLElement)parent;
                        return pElem.getElementAt(index);
                    } else {
                        return null;
                    }
                }
                public int getChildCount(Object parent) {
                    if (parent instanceof WMLElement) {
                        WMLElement pElem = (WMLElement)parent;
                        return pElem.getElementCount();
                    } else {
                        return 0;
                    }
                }
                public int getIndexOfChild(Object parent, Object child) {
                    if (child instanceof WMLElement) {
                        WMLElement cElem = (WMLElement)child;
                        return cElem.getElementIndex();
                    } else {
                        return 0;
                    }
                }
                public boolean isLeaf(Object node) {
                    if (node instanceof WMLElement) {
                        WMLElement elem = (WMLElement)node;
                        return ((elem instanceof WMLContent) || (elem instanceof XMLProcessingInstruction));
                    } else {
                        return true;
                    }
                }
                public void valueForPathChanged(TreePath path, Object newValue) {
                    // ignored
                }
                public void addTreeModelListener(TreeModelListener l) {
                    // ignored
                }
                public void removeTreeModelListener(TreeModelListener l) {
                    // ignored
                }
            };
        }
        return treeModel;
    }


    private static final String DOCTYPE_WML_1_1 =
      "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.1//EN\" \"http://www.wapforum.org/DTD/wml_1.1.xml\">";
    private static final String DOCTYPE_WML_1_2 =
      "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.2//EN\" \"http://www.wapforum.org/DTD/wml12.dtd\">";
    private static final String DOCTYPE_WML_1_3 =
      "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.3//EN\" \"http://www.wapforum.org/DTD/wml13.dtd\">";
    private static final String DOCTYPE_WML_UNKNOWN =
      "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML ?.?//EN\" \"http://www.wapforum.org/DTD/wml??.dtd\">";

    private static WMLDeck currentDeck = null;      // the current WML Deck

    // a Swing TreeModel for the deck:
    private transient TreeModel treeModel = null;

    private URL url;                                // the deck source URL
    private String sourceText;                      // the deck's source code
    private byte[] byteCode;                        // the deck's byte code
    private CharacterEncoding encoding;             // the deck's character encoding
    private String wmlVersionString;                // the WML version of the deck source
}
