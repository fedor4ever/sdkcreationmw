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

/* javax.swing.tree */
import javax.swing.tree.TreeModel;

/**
 * A content viewer interface. The model is immutable except for the URL
 * and the 'stats' string which may reflect current WML card (in case of
 * a WML content). The model is generic enough to represent WML, XHTML or
 * anything else.
 *
 */
public interface ContentViewer {

    /**
     * Returns the MIME type of the source code representation of this
     * content.
     */
    public String getMimeType();

    /**
     * Returns the MIME type of the binary representation of this content.
     * If content is not encoded, return <code>null</code>
     */
    public String getBinaryMimeType();

    /**
     * Returns the URL where the content has been retrieved from
     */
    public String getURL();

    /**
     * Sets the URL where the content has been retrieved from.
     * Normally, this should be a variation of the original URL (i.e.
     * different cards of the same deck)
     */
    public void setURL(String url);

    /**
     * Returns the status string that reflects the current state of the
     * model.
     */
    public String getStats();

    /**
     * Returns the tree model of the current page or <code>null</code>
     * if current page cannot be represented as a tree
     */
    public TreeModel getTreeModel();

    /**
     * Returns <code>true</code> if current page came in a binary form
     * or has a binary representation.
     */
    public boolean isBinary();

    /**
     * Return the bytecode representation of this content, or
     * <code>null</code> if it does not have a binary form.
     */
    public byte [] getByteCode();

    /**
     * Returns the decoded content or <code>null</code> if current
     * page has no binary representation
     */
    public String getDecodedContent();

    /**
     * Returns the original source or <code>null</code> if the original
     * source is unavailable
     */
    public String getOriginalSource();
}
