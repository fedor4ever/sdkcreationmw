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
 * $Id: XRpcEmptyDataElement.java,v 1.3 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcEmptyDataElement.java
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

/**
 * A base class for empty data elements (such as <code>&lt;void&gt;</code>)
 *
 */
abstract class XRpcEmptyDataElement implements XRpcDataElement {

    /**
     * Returns the tag that represents this element in the XML stream.
     *
     * @return the tag for this element
     */
    public final XRpcTag getTag() {
        return getDataType().getTag();
    }

    /**
     * Returns number of attributes
     *
     * @return always zero
     */
    public final int getAttributeCount() {
        return 0;
    }

    /**
     * Returns ith attribute.
     *
     * @param i the index of the attribute
     *
     * @return  never returns anything, always throws
     *          {@link IndexOutOfBoundsException}
     * @throws  {@link IndexOutOfBoundsException} because empty elements have
     *          no attributes
     */
    public final XRpcAttr getAttribute(int i) {
        throw new IndexOutOfBoundsException(Integer.toString(i));
    }

    /**
     * Returns the value of the ith attribute.
     *
     * @param i the index of the attribute
     *
     * @return  never returns anything, always throws
     *          {@link IndexOutOfBoundsException}
     * @throws  {@link IndexOutOfBoundsException} because empty elements have
     *          no attributes
     */
    public final String getAttributeValue(int i) {
        throw new IndexOutOfBoundsException(Integer.toString(i));
    }

    /**
     * Returns the value of the attribute. If this element does not have
     * such attribute, throws {@link IllegalArgumentException}
     *
     * @param attr the type of the attribute
     *
     * @return  never returns anything, always throws
     *          {@link IndexOutOfBoundsException}
     * @throws  {@link IndexOutOfBoundsException} because empty elements have
     *          no attributes
     */
    public final String getAttributeValue(XRpcAttr attr) {
        throw new IllegalArgumentException(attr.toString());
    }

    /**
     * Write this element and all its children elements into an XML stream.
     *
     * @param writer the XML stream
     */
    public final void write(XRpcWriter writer) {
        writer.print('<');
        writer.print(getTag().toString());
        XRpcUtils.writeAttrbutes(this, writer.getPrintWriter());
        writer.print("/>");
    }
}
