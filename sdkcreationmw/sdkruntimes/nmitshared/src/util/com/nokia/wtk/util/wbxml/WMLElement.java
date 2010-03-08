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

/* java.util */
import java.util.Vector;
import java.util.HashMap;
import java.util.Enumeration;

/* java.io */
import java.io.PrintWriter;
import java.io.StringWriter;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.MessageWriters;

/**
 * A WML Element.
 *
 */
public class WMLElement implements MessageWriters {

    // Disallow public default instantiation
    private WMLElement() {}

    /**
     * Create a new WML element.
     * @param type the element type
     */
    WMLElement(WML.Tag type) {
        this.type = type;
        this.parent = null;
    }

    /**
     * Set the parent for this element.
     * @param parent the parent element
     */
    protected void setParent(WMLElement parent) {
        this.parent = parent;
    }

    /**
     * Return the parent for this element.
     */
    public WMLElement getParent() {
        return this.parent;
    }

    /**
     * Return the type of this element.
     */
    public WML.Tag getType() {
        return this.type;
    }

    /**
     * Return the element's id attribute.  This attribute
     *@return the id attribute's string value or null if the element
     * does not have this attribute.
     */
    public String getIdAttribute() {
        return getAttribute(WML.Attribute.ID);
    }

    /**
     * Return the element's class attribute
     *@return the class attribute's string value or null if the element
     * does not have this attribute.
     */
    public String getClassAttribute() {
        return getAttribute(WML.Attribute.CLASS);
    }

    /**
     * Return the element's xml:lang attribute
     *@return the xml:lang's attribute's string value or null if the element
     * does not have this attribute.
     */
    public String getXmlLangAttribute() {
        return getAttribute(WML.Attribute.XML_LANG);
    }

    /**
     * Return <code>true</code> if the type of this element matches that provided.
     * @param type the element type to match against
     */
    public boolean isType(WML.Tag type) {
        return this.type.equals(type);
    }

    /**
     * Return <code>true</code> if this element is part of a block,
     * i.e. one that has both a start and an end tag.
     */
    public boolean isBlockType() {
        return getType().isBlock();
    }

    /**
     * Add a sub-element to this element.
     * @param element the sub-element to add
     */
    public void addElement(WMLElement element) {
        if (element != null) {
            if (elements == null) {
                elements = new Vector();
            }
            elements.addElement(element);
            element.setParent(this);
        }
    }

    /**
     * Remove a sub-element from this element.
     * @param element the sub-element to remove
     * @return <code>true</code> if the argument was a sub-element
     *         of this element; <code>false</code>  otherwise.
     */
    boolean removeElement(WMLElement element) {
        if ((element != null) && (elements != null)) {
            return elements.removeElement(element);
        }
        return false;
    }

    /**
     * Return <code>true</code> if an element is a sub-element of this element.
     * @param element the element
     */
    public boolean containsElement(WMLElement element) {
        return (elements != null) ? elements.contains(element) : false;
    }

    /**
     * Return the number of sub-elements in this element.
     */
    public int getElementCount() {
        return (elements != null) ? elements.size() : 0;
    }

    /**
     * Return the number of sub-elements of a given type in this element.
     * @param type the element type to match against
     */
    public int getElementCount(WML.Tag type) {
        int count = 0;
        for (Enumeration e = elements(); e.hasMoreElements(); ) {
            if (((WMLElement)e.nextElement()).isType(type))
                ++count;
        }
        return count;
    }

    /**
     * Return a sub-element from this element.
     * @param index the sub-element index (0-based)
     */
    public WMLElement getElementAt(int index) {
        return (elements != null) ? (WMLElement)elements.elementAt(index) : null;
    }

    /**
     * Return the index of this element within the parent element
     * or zero if this is a root element.
     */
    public int getElementIndex() {
        return (parent != null) ? parent.elements.indexOf(this) : 0;
    }

    /**
     * Return an enumeration of sub-elements for this element.
     */
    public Enumeration elements() {
        if (elements == null) {
            elements = new Vector();
        }
        return elements.elements();
    }

    /**
     * Return <code>true</code> if attribute is defined in this element.
     * @param attr the attribute type
     */
    public boolean containsAttribute(WML.Attribute attr) {
        return (attributeKeys != null) ? attributeKeys.contains(attr) : false;
    }

    /**
     * Return an attribute from this element.
     * @param attr the attribute type
     */
    public String getAttribute(WML.Attribute attr) {
        return (containsAttribute(attr)) ? (String)attributeValues.elementAt(attributeKeys.indexOf(attr)) : null;
    }

    /**
     * Set an attribute value in this element.
     * @param attr the attribute type
     * @param value the attribute value
     */
    public void putAttribute(WML.Attribute attr, String value) {
        if (attr != null) {
            if (attributeKeys == null) {
                attributeKeys = new Vector();
                attributeValues = new Vector();
            }
            if (containsAttribute(attr)) {
                int index = attributeKeys.indexOf(attr);
                attributeValues.setElementAt(value, index);
            } else {
                attributeKeys.addElement(attr);
                attributeValues.addElement(value);
            }
        }
    }

    /**
     * Return an enumeration of defined attribute types for this element.
     */
    public Enumeration attributeTypes() {
        if (attributeKeys == null) {
            attributeKeys = new Vector();
            attributeValues = new Vector();
        }
        return attributeKeys.elements();
    }

    /**
     * Return any inline content..
     */
    WMLContent getInlineContent() {
        // Search recursively for a WMLContent element:
        return (WMLContent)applyToDescendents(new Applicator() {
            public boolean apply(WMLElement element) {
                if (element instanceof WMLContent) {
                    putValue(element);
                    setBreak(true);
                }
                return true;   // Continue search
            }
        });
    }

    /**
     * Return the nesting level for this element.
     */
    private final int getNestingLevel() {
        int level = 0;
        for (WMLElement here = this; here.getParent() != null; here = here.getParent()) {
            ++level;
        }

        return level;
    }

    /**
     * Return an indentation string that reflects the nesting level for this element.
     * @param unit a base string that will be replicated to the nesting level of this element
     */
    protected final String getIndent(String unit) {
        if (unit == null) unit = "  ";
        StringBuffer sbuf = new StringBuffer();
        int level = getNestingLevel();
        for (int i = 0; i < level; i++) {
            sbuf.append(unit);
        }
        return sbuf.toString();
    }

    /**
     * Return an indentation string that reflects the nesting level for this element.
     */
    protected final String getIndent() {
        return getIndent(null);
    }

    /**
     * Return <tt>true</tt> if this element may be collapsed onto one line during dump.
     */
    private final boolean isCollapsable() {
        boolean collapsable = (getNestingLevel() > 3);  // Minimum nesting level
        if (collapsable) {
            int siblingCount = getParent().getElementCount();
            int siblingContentCount = getParent().getElementCount(WML.Tag._CONTENT_);
            collapsable = ((siblingCount - siblingContentCount) <= 1);
            //TODO: Apply more rules?
        }
        return collapsable;
    }

    /**
     * Dump the contents of this element.
     * @param w a text-output stream to use for output
     */
    public void dumpElement(PrintWriter w) {
        boolean collapsable = isCollapsable();
        boolean containsCollapsable = (getElementCount() > 0) ? getElementAt(0).isCollapsable()
                                                              : false;
        // Tag line:
        if (!collapsable) {
            w.print(getIndent());
        }
        dumpElementTag(w);
        if (!(collapsable || containsCollapsable)) {
            w.println();
        }

        // Show any extra element information:
//        dumpElementExtraInfo(w);

        if (isBlockType() && (getElementCount() > 0)) {
            // Show any child elements:
            dumpChildElements(w);

            // Tag terminator:
            if (!containsCollapsable) {
                w.print(getIndent());
            }

            w.print("</" + getType() + ">");

            if (!collapsable) {
                w.println();
            }
        }

        // Extra space after top-level elements (WML,HEAD,TEMPLATE,CARD):
        if (getNestingLevel() < 2) {
            w.println();
        }
    }

    /**
     * Return the string representation of this element.
     */
    public String getAsText() {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);

        // Begin tag line:
        pw.print("<" + getType());

        // Show any attributes:
        dumpElementAttributes(pw);

        // End tag line:
        pw.print(">");

        return sw.toString();
    }

    /**
     * Dump the tag line for this element.
     * @param w a text-output stream to use for output
     */
    protected void dumpElementTag(PrintWriter w) {
        // Begin tag line:
        w.print("<" + getType());

        // Show any attributes:
        dumpElementAttributes(w);

        // End tag line:
        w.print((isBlockType() && (getElementCount() > 0)) ? ">" : "/>");
    }

    /**
     * Dump extra element information for this element.
     * @param w a text-output stream to use for output
     */
    protected void dumpElementExtraInfo(PrintWriter w) {
    }

    /**
     * Dump a comment line for this element.
     * @param w a text-output stream to use for output
     * @param comment comment text
     */
    protected final void dumpElementTagComment(PrintWriter w, String comment) {
        if (comment != null) {
            w.println(getIndent() + "<!-- " + comment + " -->");
        }
    }

    /**
     * Dump the attributes for tag line of this element.
     * @param w a text-output stream to use for output
     */
    private final void dumpElementAttributes(PrintWriter w) {
        Enumeration e = attributeTypes();
        while (e.hasMoreElements()) {
            WML.Attribute attr = (WML.Attribute)e.nextElement();
            w.print(" " + attr + "=\"" + WMLStringEncoder.encodeEntities(getAttribute(attr)) + "\"");
        }
    }

    /**
     * Dump the attributes for tag line of this element.
     * @param w a text-output stream to use for output
     */
    private final void dumpChildElements(PrintWriter w) {
        Enumeration e = elements();
        while (e.hasMoreElements()) {
            WMLElement element = (WMLElement)e.nextElement();
            element.dumpElement(w);
        }
    }

    /**
     * Return the string representation of this element for debugging purposes.
     */
    public String toString() {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        dumpElementTag(pw);

        // Get any "extra" element info:
        StringWriter sw2 = new StringWriter();
        dumpElementExtraInfo(new PrintWriter(sw2));
        String extra = sw2.toString();
        if ((extra != null) && (extra.length() > 0))
            pw.print("; " + extra);

        return sw.toString();
    }

    /**
     * Apply an operation to this element and possibly, each of its descendents.
     * @param a an operation to apply to each element
     */
    public Object apply(Applicator a) {
        if ((a != null) && !a.isBreak()) {
            // Apply to this element and to each descendent element if applicable:
            if (a.apply(this)) {
                applyToDescendents(a);
            }
        }
        return a.getValue();
    }

    /**
     * Apply an operation to each descendent of this element.
     * @param a an operation to apply to each element
     */
    public Object applyToDescendents(Applicator a) {
        if ((a != null) && !a.isBreak()) {
            // Apply operation to each descendent:
            Enumeration e = elements();
            while (e.hasMoreElements() && !a.isBreak()) {
                WMLElement element = (WMLElement)e.nextElement();
                element.apply(a);
                if (a.isBreak())
                    break;
            }
        }
        return a.getValue();
    }

    /**
     * Apply an operation to each successive ancestor of this element.
     * @param a an operation to apply to each element
     */
    public Object applyToAncestors(Applicator a) {
        if ((a != null) && !a.isBreak()) {
            for (WMLElement p = getParent(); (p != null) && !a.isBreak(); p = p.getParent()) {
                if (!a.apply(p)) {
                    break;
                }
            }
        }
        return a.getValue();
    }

    /**
     * Apply an operation to this element and possibly, each of its successive ancestors.
     * @param a an operation to apply to each element
     */
    public Object applyToSelfAndAncestors(Applicator a) {
        if ((a != null) && !a.isBreak()) {
            // Apply to this element and to each descendent element if applicable:
            if (a.apply(this)) {
                applyToAncestors(a);
            }
        }
        return a.getValue();
    }


    /**
     * Inner class: An operation to be applied to an element.
     */
    public abstract static class Applicator {
        /** Create an Applicator instance. */
        public Applicator() { this(null); }

        /**
         * Create an Applicator instance with extra parameterization.
         * @param obj a usage-defined object for context, operands, return value, etc.
         */
        public Applicator(Object obj) {
            this.values = new HashMap();
            putValue(null, obj);
            this.inBreak = false;
        }

        /**
         * Apply operation to an element.
         * @param element the element
         * @return <tt>true</tt> if operation should be recursively applied to each descendent
         */
        public abstract boolean apply(WMLElement element);

        /**
         * Set the default property value.
         * @param obj a usage-defined object for context, operands, return value, etc.
         */
        public final void putValue(Object obj) {
            putValue(null, obj);
        }

        /**
         * Get the default property value.
         * @return the usage-defined object for context, operands, return value, etc.
         */
        public final Object getValue() {
            return getValue(null);
        }

        /**
         * Set a property value.
         * @param key the property key.  If null, then the default property is used.
         * @param value the property value.
         */
        public void putValue(Object key, Object value) {
            values.put(key, value);
        }

        /**
         * Get a property value.
         * @param key the property key.  If null, then the default property is used.
         * @return the the property value.
         */
        public Object getValue(Object key) {
            return values.get(key);
        }

        /**
         * Set to <tt>true</tt> to terminate traversal operation.
         * @param inBreak if <tt>true</tt>, then terminate traversal operation
         */
        public final void setBreak(boolean inBreak) {
            this.inBreak = inBreak;
        }

        /**
         * Returns <tt>true</tt> if traversal operation is being terminated
         */
        public final boolean isBreak() {
            return inBreak;
        }


        private HashMap values;
        private boolean inBreak;
    }   // End of Applicator


    private WML.Tag type;           // element type
    private WMLElement parent;      // parent element
    private Vector elements;        // child elements
    private Vector attributeKeys;   // element attribute keys
    private Vector attributeValues; // element attribute values
}



