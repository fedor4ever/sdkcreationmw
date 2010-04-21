/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef _XRPCRT_H_
#define _XRPCRT_H_

#ifndef _XRPC_REG_API_H_
#  include <xregapi.h>
#endif /* _XRPC_REG_API_H_ */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * Returns one and only instance of XRpcServer, NULL on failure. This
 * instance is created on demand when you first time call this function.
 * The failure may occur if XRpcRtEx.dll is not present.
 */
XRpcServer * XRPC_API XRPCRT_Server(void);

/**
 * Returns the port on which XRPC server is listening, zero on failure.
 * Creates the server if it hasn't been created yet. The failure may
 * occur if XRpcRtEx.dll is not present.
 */
XRpcPort XRPC_API XRPCRT_ServerPort(void);

/**
 * Returns one and only instance of XRpcRegistry, NULL on failure. This 
 * instance is created on demand when you first time call this function.
 * The failure may occur if XRpcRtEx.dll is not present.
 */
XRpcRegistry * XRPC_API XRPCRT_Registry(void);

#ifdef __cplusplus
} /* and of extern "C" */
#endif  /* __cplusplus */

#endif /* _XRPCRT_H_ */
