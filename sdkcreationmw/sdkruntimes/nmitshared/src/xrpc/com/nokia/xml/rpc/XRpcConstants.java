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
 * $Id: XRpcConstants.java,v 1.4 2005/08/26 22:45:26 slava Exp $
 */

// ========================================================================
//  Name        : XRpcConstants.java
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
 * Constants used by the XRPC package
 *
 */
public interface XRpcConstants {

    /**
     * XML encoding used by XRPC
     */
    public static final String XRPC_ENCODING = "utf-8";

    /**
     * Standard XML header
     */
    public static final String XML_HEADER =
        "<?xml version='1.0' encoding='" +
        XRPC_ENCODING + "' standalone='yes'?>";

    /**
     * The text representation of a boolean <code>true</code> value.
     */
    public static final String TRUE = "true";

    /**
     * The text representation of a boolean <code>false</code> value.
     */
    public static final String FALSE = "false";

    //=======================================================================
    //      C O R E    P R O T O C O L
    //=======================================================================

    /*
     * The name of the core protocol
     */

    public static final String XCORE_PROTOCOL = "xrpc.core";

    /*
     * Method names
     */

    public static final String XCORE_METHOD_PING = "ping";
    public static final String XCORE_METHOD_LIST_PROTOCOLS = "listProtocols";
    public static final String XCORE_METHOD_GET_PROTOCOL_INFO = "getProtocolInfo";

    /*
     * Parameter for the "getProtocolInfo" call
     */

    public static final String XCORE_PARAM_PROTO_NAME = "name";

    /*
     * Definition of xrpc.core.ProtocolVersion structure. After XRPC-encoding
     * it looks like this:
     *
     *    <struct type="xrpc.core.ProtocolVersion">
     *      <int name="minor">1</int>
     *      <int name="major">0</int>
     *    </struct>
     */

    public static final String XCORE_STRUCT_PROTO_VER = "xrpc.core.ProtocolVersion";
    public static final String XCORE_STRUCT_PROTO_VER_MAJOR =   "major";
    public static final String XCORE_STRUCT_PROTO_VER_MINOR =   "minor";

    /*
     * Definition of xrpc.core.ProtocolInfo structure. After XRPC-encoding
     * it looks like this:
     *
     *   <struct name="xrpc.core.ProtocolInfo">
     *      <string name="name">Core</string>
     *      <string name="description">Core Protocol</string>
     *      <string name="vendor">Nokia</string>
     *      <string name="vendorURL">http://www.nokia.com/</string>
     *      <string name="protocolURL">http://www.nokia.com/</string>
     *      <struct name="version" type="xrpc.core.ProtocolVersion">
     *        <int name="minor">1</int>
     *        <int name="major">0</int>
     *      </struct>
     *    </struct>
     */

    public static final String XCORE_STRUCT_PROTO_INFO = "xrpc.core.ProtocolInfo";
    public static final String XCORE_STRUCT_PROTO_INFO_NAME =   "name";
    public static final String XCORE_STRUCT_PROTO_INFO_DESCR =  "description";
    public static final String XCORE_STRUCT_PROTO_INFO_VENDOR = "vendor";
    public static final String XCORE_STRUCT_PROTO_INFO_VURL =   "vendorURL";
    public static final String XCORE_STRUCT_PROTO_INFO_PURL =   "protocolURL";
    public static final String XCORE_STRUCT_PROTO_INFO_VER =    "version";
}
