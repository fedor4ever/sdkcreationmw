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
import java.io.PrintWriter;

/**
 * A WML Content Element.
 *
 */
public class WMLContent extends WMLElement {

    /**
     * Creates a new WML content element.
     */
    WMLContent() {
        super(WML.Tag._CONTENT_);
        setExpandable(true);
    }

    /**
     * Sets the content for this element.
     * @param content the content
     */
    public void setContent(String content) {
        this.content = content;
    }

    /**
     * Appends content to this element.
     * @param content the content
     */
    void appendContent(String content) {
        if (content != null) {
            if (this.content != null) {
                this.content = this.content + content;
            } else {
                this.content = content;
            }
        }
    }

    /**
     * Returns the content for this element.
     */
    public String getContent() {
        return this.content;
    }

    /**
     * Returns the raw (unexpanded) content for this element.
     */
    public String getRawContent() {
        return content;
    }

    /**
     * Sets the the variable expansion mode for this element.
     * @param expandable if <code>true</code> then variable expansion is allowed
     */
    public void setExpandable(boolean expandable) {
        this.expandable = expandable;
    }

    /**
     * Returns <code>true</code> if variable expansion is allowed for this element.
     */
    public boolean isExpandable() {
        return expandable;
    }

    /**
     * Returns the string representation of this element for debugging purposes.
     */
    public String toString() {
        return "WMLContent[\"" + WMLStringEncoder.encodeEntities(content) + "\"]";
    }

    /**
     * Returns the string representation of this element for debugging purposes.
     */
    public String getEncodedContent() {
        return WMLStringEncoder.encodeEntities(content);
    }

    /**
     * Dumps the contents of this element.
     * @param w a text-output stream to use for output
     */
    public void dumpElementTag(PrintWriter w) {
        // Content elements display with no tag syntax:
        w.print(WMLStringEncoder.encodeEntities(content));
    }

    /**
     * Returns <code>true</code> if a string is empty or contains only whitespace.
     * @param s the string to test
     */
    static boolean isWhitespace(String s) {
        boolean isWhite = true;
        if ((s != null) && (s.length() > 0)) {
            for (int index = 0; index < s.length(); index++) {
                if (!Character.isWhitespace(s.charAt(index))) {
                    isWhite = false;
                    break;
                }
            }
        }
        return isWhite;
    }

    private boolean expandable;
    private String content;
    private transient String expandedContent;
}

