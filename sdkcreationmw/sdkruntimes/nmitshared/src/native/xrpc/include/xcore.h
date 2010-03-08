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


#ifndef _XRPC_CORE_H_
#define _XRPC_CORE_H_ 1

/* *************************************************************************
//
//  Describes the "xrpc.core" protocol implemented by XRPC library
//
// *************************************************************************/

#define XCORE_PROTOCOL                  "xrpc.core"
#define XCORE_METHOD_PING               "ping"
#define XCORE_METHOD_LIST_PROTOCOLS     "listProtocols"
#define XCORE_METHOD_GET_PROTOCOL_INFO  "getProtocolInfo"

/*
 * Parameter for the "getProtocolInfo" call
 */

#define XCORE_PARAM_PROTO_NAME          "name"

/*
 * Definition of xrpc.core.ProtocolVersion structure. After XRPC-encoding
 * it looks like this:
 *
 *   <struct type="xrpc.core.ProtocolVersion">
 *     <int name="minor">1</int>
 *     <int name="major">0</int>
 *   </struct>
 */

#define XCORE_STRUCT_PROTO_VER          "xrpc.core.ProtocolVersion"
#define XCORE_STRUCT_PROTO_VER_MAJOR    "major"
#define XCORE_STRUCT_PROTO_VER_MINOR    "minor"

/*
 * Definition of xrpc.core.ProtocolInfo structure. After XRPC-encoding
 * it looks like this:
 *
 *   <struct type="xrpc.core.ProtocolInfo">
 *      <string name="name">xrpc.core</string>
 *      <string name="description">XRPC core protocol</string>
 *      <string name="vendor">Nokia</string>
 *      <string name="vendorURL">http://www.nokia.com/</string>
 *      <string name="protocolURL">http://www.nokia.com/</string>
 *      <struct name="version" type="xrpc.core.ProtocolVersion">
 *        <int name="minor">1</int>
 *        <int name="major">0</int>
 *      </struct>
 *    </struct>
 */

#define XCORE_STRUCT_PROTO_INFO         "xrpc.core.ProtocolInfo"
#define XCORE_STRUCT_PROTO_INFO_NAME    "name"
#define XCORE_STRUCT_PROTO_INFO_DESCR   "description"
#define XCORE_STRUCT_PROTO_INFO_VENDOR  "vendor"
#define XCORE_STRUCT_PROTO_INFO_VURL    "vendorURL"
#define XCORE_STRUCT_PROTO_INFO_PURL    "protocolURL"
#define XCORE_STRUCT_PROTO_INFO_VER     "version"

#endif /* _XRPC_CORE_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
