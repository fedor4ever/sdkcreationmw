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
 * $Id: XRpcResponse.java,v 1.10 2005/08/26 22:45:27 slava Exp $
 */

// ========================================================================
//  Name        : XRpcResponse.java
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
 * Represents an XRPC response
 *
 */
public final class XRpcResponse extends XRpcBasicContainer {

    private XRpcBasicContainer result = null;

    public XRpcResponse(String id, XRpcStatus status) {
        if (id == null) throw new IllegalArgumentException("null call id");
        setAttribute(XRpcAttr.ID, id);
        setAttribute(XRpcAttr.STATUS, status.toString());
    }

    public XRpcResponse(String id) {
        this(id, XRpcStatus.OK);
    }

    public XRpcResponse(String id, XRpcDataElement result) {
        this(id);
        if (result != null && !XRpcVoid.getInstance().equals(result)) {
            addElement(new Result(result));
        }
    }

    public XRpcTag getTag() {
        return XRpcTag.RESPONSE;
    }

    public String getCallID() {
        return getAttributeValue(XRpcAttr.ID);
    }

    public XRpcDataElement[] getResults() {
        if (result == null) {
            return XRpcBasicContainer.NO_DATA_ELEMENTS;
        } else {
            return result.getDataElements();
        }
    }

    /**
     * Write this element and all its children elements into an XML stream.
     */
    public void write(XRpcWriter writer) {
        synchronized (writer.getLock()) {
            super.write(writer);
            writer.flush();
        }
    }

    /**
     * Container for result data block
     */
    private final static class Result extends XRpcBasicContainer {
        Result(XRpcDataElement element) {
            addElement(element);
        }
        public XRpcTag getTag() { return XRpcTag.RESULT; }
    }
}
