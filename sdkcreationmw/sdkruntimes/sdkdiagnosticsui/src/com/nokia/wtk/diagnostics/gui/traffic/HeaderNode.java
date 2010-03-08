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






package com.nokia.wtk.diagnostics.gui.traffic;

import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
/**
 * <p>Title: </p>
 * <p>Description: A tree node for a content or request header.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */

class HeaderNode extends ContentNode implements Resource {

        /**
         * Creates a new HeaderNode.
         * @param name the header name
         * @param value the header value
         */
        HeaderNode(String name, String value) {
            super(true);
            this.name = name;
            this.value = value;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText( boolean asHTML ) {
            StringBuffer sbuf = new StringBuffer();
            if ( asHTML ) {
                sbuf.append("<font " + ColorStyle.BLACK + ">");
            }
            sbuf.append(name);
            sbuf.append(" = ");
            if ( asHTML ) {
                sbuf.append(openBracket);
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            } else {
                sbuf.append( "[" );
            }
            sbuf.append(getValueString( asHTML ));
            if ( asHTML ) {
                sbuf.append("</font>");
                sbuf.append(closeBracket);
                sbuf.append("</font>");
            } else {
                sbuf.append( "]" );
            }
            return sbuf.toString();
        }


        /**
         * Returns a human-readable representation of the header value.
         */
        private String getValueString( boolean asHTML ) {
            if (value != null) {
                StringBuffer sbuf = new StringBuffer(value.length());
                for (int i = 0; i < value.length(); i++) {
                    char c = value.charAt(i);
                    if (c == '\0') {                // Null character
                        sbuf.append("\0");
                    } else if (c == '\n') {         // Newline
                        sbuf.append("\\n");
                    } else if (c == '\r') {         // Carriage return
                        sbuf.append("\\r");
                    } else if (c == '\t') {         // Horizontal tab
                        sbuf.append("\\t");
                    } else if (c == '\\') {         // Literal backslash
                        sbuf.append("\\\\");
                    } else if (c == '<') {          // Less-than must be escaped to prevent HTML interpretation
                        if ( asHTML ) {
                            sbuf.append( "&lt;");
                        } else {
                            sbuf.append( c );
                        }
                    } else if (c == '>') {          // Greater-than must be escaped to prevent HTML interpretation
                        if ( asHTML ) {
                            sbuf.append("&gt;");
                        } else {
                            sbuf.append( c );
                        }
                    } else if (c == '&') {          // Ampersand must be escaped to prevent HTML interpretation
                        if ( asHTML ) {
                            sbuf.append("&amp;");
                        } else {
                            sbuf.append( c );
                        }
                    } else if (c == '\"') {         // Double quote must be escaped to prevent HTML interpretation
                        if ( asHTML ) {
                            sbuf.append("&quot;");
                        } else {
                            sbuf.append( c );
                        }
                    } else if (c == '\'') {         // Single quote must be escaped to prevent HTML interpretation
                        if ( asHTML ) {
                            sbuf.append("&apos;");
                        } else {
                            sbuf.append( c );
                        }
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
                return sbuf.toString();
            } else {
                return "";
            }
        }
        private String name;
        private String value;


}
