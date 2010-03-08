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






package com.nokia.wtk.diagnostics.views.support;

import javax.swing.Icon;
import javax.swing.tree.DefaultMutableTreeNode;

/**
 * <p>Title: </p>
 * <p>Description: A parent class for all nodes in the various data trees</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */

abstract
public
class ContentNode
extends DefaultMutableTreeNode {

        /**
         * Creates a new ContentNode.
         */
        public ContentNode() {
            this(false);
        }

        /**
         * Creates a new ContentNode.
         * @param isLeaf if <code>true</code> then this is a leaf node
         */
        public ContentNode(boolean isLeaf) {
            super(null, !isLeaf);
        }

        /**
         * Gets the text representation of an object.
         * @param o the object
         * @return the text representation of the object
         */
        public static String toString(Object o) {
            if (o != null) {
                return o.toString();
            } else {
                StringBuffer sbuf = new StringBuffer();
                sbuf.append("&lt;");
                sbuf.append("<font " + ColorStyle.BRIGHTRED + ">");
                sbuf.append("null");
                sbuf.append("</font>");
                sbuf.append("&gt;");
                return sbuf.toString();
            }
        }

        /**
         * Gets the text representation of this node.
         * This is used to provide a renderable label in the target tree view.
         * @return the text representation of this node
         */
        protected String getAsText() {
          StringBuffer sbuf = new StringBuffer();
          sbuf.append( "<html>" );
          sbuf.append("<font face=sansserif size=2 " + ColorStyle.BLUE + ">");
          sbuf.append( getAsText( true ) );
          sbuf.append("</font>");
          sbuf.append("</html>");
          return sbuf.toString();
        }

        /**
         * This *should* be overridden in every subclass, but it may not be. 
         * The subclass *may* override getAsText() instead.
         * @param asHTML
         * @return
         */
        public abstract String getAsText( boolean asHTML );
        
        /**
         * Gets the text representation of this object.
         * @return the text representation of this object
         */
       public String toString() {
            StringBuffer sbuf = new StringBuffer("<html>");
            sbuf.append("<font face=sansserif size=2 " + ColorStyle.BLUE + ">");
            sbuf.append(getAsText( true ));
            sbuf.append("</font>");
            sbuf.append("</html>");
            return sbuf.toString();
        }
        
        public Icon getIcon() { return null; }

}
