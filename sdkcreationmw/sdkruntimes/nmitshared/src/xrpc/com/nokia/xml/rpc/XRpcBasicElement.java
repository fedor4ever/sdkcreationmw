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
 * $Id: XRpcBasicElement.java,v 1.6 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcBasicElement.java
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
 * A base class for non-empty XRPC elements
 *
 */
abstract class XRpcBasicElement extends XRpcBasicAttrList implements XRpcMutableElement {

    /**
     * Sets the value of the <code>&quot;name&quot;</code> attribute.
     * @param name the element name
     */
    public void setName(String name) {
        setAttribute(XRpcAttr.NAME, name);
    }

    /**
     * Returns the value of the <code>&quot;name&quot;</code> attribute.
     * @returns the element name
     */
    public String getName() {
        return getAttributeValue(XRpcAttr.NAME);
    }

    /**
     * Write this element and all its children elements into an XML stream.
     * @param writer the XML stream
     */
    public void write(XRpcWriter writer) {
        writeEmptyTag(writer);
    }

    /**
     * Write this element into an XML stream as an empty tag.
     * @param writer the XML stream
     */
    protected void writeEmptyTag(XRpcWriter writer) {
        writer.print('<');
        writer.print(getTag().toString());
        XRpcUtils.writeAttrbutes(this, writer.getPrintWriter());
        writer.print("/>");
        writer.println();
    }

    /**
     * Write open tag into an XML stream.
     * @param writer the XML stream
     */
    protected void openTag(XRpcWriter writer) {
        writer.print('<');
        writer.print(getTag().toString());
        XRpcUtils.writeAttrbutes(this, writer.getPrintWriter());
        writer.print('>');
    }

    /**
     * Write closed tag into an XML stream.
     * @param writer the XML stream
     */
    protected void closeTag(XRpcWriter writer) {
        writer.print("</");
        writer.print(getTag().toString());
        writer.print('>');
    }
}
