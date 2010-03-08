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
/*
 * $Id: XRpcBasicContainer.java,v 1.4 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcBasicContainer.java
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


package com.nokia.xml.rpc;

/* java.util */
import java.util.List;
import java.util.ArrayList;

/**
 * A basic container that may contain other elements
 *
 */
abstract
class       XRpcBasicContainer
extends     XRpcBasicElement
implements  XRpcContainer {

    static XRpcDataElement [] NO_DATA_ELEMENTS = new XRpcDataElement[0];
    static XRpcElement     [] NO_ELEMENTS      = new XRpcElement[0];

    private List elements = null;

    /**
     * Returns number of elements in this container
     */
    public int getElementCount() {
        return ((elements == null) ? 0 : elements.size());
    }

    /**
     * Returns ith data element
     */
    public XRpcDataElement getDataElement(int i) {
        return (XRpcDataElement)getElement(i);
    }

    /**
     * Returns ith element
     */
    public XRpcElement getElement(int i) {
        if (i >= getElementCount()) {
            throw new IndexOutOfBoundsException(Integer.toString(i));
        }
        return (XRpcElement)elements.get(i);
    }

    /**
     * Returns all data elements. If container is empty, returns empty array.
     * If this container is not a data container, throws an exception
     */
    public XRpcDataElement [] getDataElements() {
        int n = getElementCount();
        if (n == 0) {
            return NO_DATA_ELEMENTS;
        } else {
            XRpcDataElement[] copy = new XRpcDataElement[n];
            return (XRpcDataElement[])elements.toArray(copy);
        }
    }

    /**
     * Returns all elements. If container is empty, returns empty array.
     */
    public XRpcElement [] getElements() {
        int n = getElementCount();
        if (n == 0) {
            return NO_ELEMENTS;
        } else {
            XRpcElement[] copy = new XRpcElement[n];
            return (XRpcElement[])elements.toArray(copy);
        }
    }

    /**
     * Write this element and all its children elements into an XML stream.
     */
    public void write(XRpcWriter writer) {
        int n = getElementCount();
        if (n == 0) {
            writeEmptyTag(writer);
        } else {
            openTag(writer);
            writer.indent();
                for (int i=0; i<n; i++) {
                    getElement(i).write(writer);
                }
            writer.unindent();
            closeTag(writer);
            writer.println();
        }
    }

    //======================================================================
    //      These methods make the object mutable
    //======================================================================

    /**
     * Adds element to the container.
     */
    public void addElement(XRpcElement element) {
        if (element == null) throw new NullPointerException();
        if (elements == null) elements = new ArrayList(4);
        elements.add(element);
    }

    /**
     * Adds elements to the container.
     */
    public void addElements(XRpcElement[] data) {
        if (data != null && data.length > 0) {
            int n = data.length;
            for (int i=0; i<n; i++) {
                addElement(data[i]);
            }
        }
    }

    /**
     * Removes all elements.
     */
    public void removeAllElements() {
        if (elements != null) elements.clear();
    }

    /**
     * Removes the element.
     * @return <code>true</code> if element has been removed,
     *         <code>false</code> if it wasn't on the list
     */
    public boolean removeElement(XRpcElement element) {
        if (elements != null) return elements.remove(element);
        return false;
    }
}
