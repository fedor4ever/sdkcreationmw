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
 * $Id: XRpcCall.java,v 1.6 2005/08/26 22:45:25 slava Exp $
 */

// ========================================================================
//  Name        : XRpcCall.java
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
 * Encapsulates all the data necessary to make a XRPC call
 *
 */
final class XRpcCall extends XRpcBasicContainer {

    private XRpcBasicContainer params = null;

    public XRpcCall(String tag, String protocol, String method) {
        this(tag, protocol, method, XRpcVoid.getInstance());
    }

    public XRpcCall(String id,
                    String protocol,
                    String method,
                    XRpcDataElement param) {

        setAttribute(XRpcAttr.ID, id);
        setAttribute(XRpcAttr.PROTOCOL, protocol);
        setAttribute(XRpcAttr.METHOD, method);

        if (!XRpcVoid.getInstance().equals(param)) {
            addElement(new Params(param));
        }
    }

    public XRpcCall(String id,
                    String protocol,
                    String method,
                    XRpcDataElement[] params) {

        setAttribute(XRpcAttr.ID, id);
        setAttribute(XRpcAttr.PROTOCOL, protocol);
        setAttribute(XRpcAttr.METHOD, method);

        if (params != null && params.length > 0) {
            addElement(new Params(params));
        }
    }


    public XRpcTag getTag() {
        return XRpcTag.CALL;
    }

    public String getCallID() {
        return getAttributeValue(XRpcAttr.ID);
    }

    public String getProtocol() {
        return getAttributeValue(XRpcAttr.PROTOCOL);
    }

    public String getMethod() {
        return getAttributeValue(XRpcAttr.METHOD);
    }

    public XRpcDataElement[] getParams() {
        if (params == null) {
            return XRpcBasicContainer.NO_DATA_ELEMENTS;
        } else {
            return params.getDataElements();
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
     * Container for parameter block
     */
    public final static class Params extends XRpcBasicContainer {
        Params(XRpcDataElement element) {
            addElement(element);
        }
        Params(XRpcDataElement [] elements) {
            addElements(elements);
        }
        public XRpcTag getTag() { return XRpcTag.PARAMS; }
    }
}
