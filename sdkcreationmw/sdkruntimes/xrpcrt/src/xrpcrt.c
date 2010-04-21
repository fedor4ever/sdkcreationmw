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


/* NmitShared/src/native */
#include "../../NmitShared/src/native/slib/include/s_mutex.h"
#include "../../NmitShared/src/native/xrpc/include/xrpc.h"
#include "../../NmitShared/src/native/xrpc/xreg/include/xregapi.h"

/* XRPC Runtime API */
#include "xrpcrt.h"

/* Global data */
static Mutex          xrpcrt_mutex;
static XRpcServer   * xrpcrt_server;
static XRpcRegistry * xrpcrt_registry;
static XRpcPort       xrpcrt_port;

/**
 * Returns one and only instance of XRpcServer. This instance is created 
 * on demand when you first time call this function.
 */
XRpcServer * XRPC_API XRPCRT_Server()
{
    if (!xrpcrt_server) {
        MUTEX_Lock(&xrpcrt_mutex);
        if (!xrpcrt_server) {
            xrpcrt_server = XRPC_CreateServer(NULL);
        }
        MUTEX_Unlock(&xrpcrt_mutex);
    }
    return xrpcrt_server;
}

/**
 * Returns the port on which XRPC server is listening.
 */
XRpcPort XRPC_API XRPCRT_ServerPort()
{
    if (!xrpcrt_port) {
        XRpcServer * server = XRPCRT_Server();
        if (server) {
            MUTEX_Lock(&xrpcrt_mutex);
            if (!xrpcrt_port) {
                xrpcrt_port = XRPC_ServerSpawn(xrpcrt_server, 0);
            }
            MUTEX_Unlock(&xrpcrt_mutex);
        }
    }
    return xrpcrt_port;
}

/**
 * Returns one and only instance of XRpcRegistry. This instance is created
 * on demand when you first time call this function.
 */
XRpcRegistry * XRPC_API XRPCRT_Registry()
{
    if (!xrpcrt_registry) {
        MUTEX_Lock(&xrpcrt_mutex);
        if (!xrpcrt_registry) {
            xrpcrt_registry = XREG_CreateRegistry(0,XRegTypeAny);
        }
        MUTEX_Unlock(&xrpcrt_mutex);
    }
    return xrpcrt_registry;
}

/**
 * Global initialization
 */
STATIC void XRPCRT_Init()
{
    XRPC_Init();
    MUTEX_Init(&xrpcrt_mutex);
    xrpcrt_registry = NULL;
    xrpcrt_server = NULL;
    xrpcrt_port = 0;
}

/**
 * DLL entry point
 */
BOOL 
WINAPI DllMain(
    HINSTANCE hinst,    /* handle to the DLL module */
    DWORD reason,       /* reason for calling function */
    LPVOID reserved)    /* reserved */
{
    TCHAR path[MAX_PATH+1];
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        TRACE("XRPCRT: Loading...\n");
        /* Add reference to ourselves so that we never get unloaded */
        path[GetModuleFileName(hinst, path, MAX_PATH)] = 0;
        VERIFY(LoadLibrary(path));
        XRPCRT_Init();
        break;
    case DLL_PROCESS_DETACH:
        /* We only get here when the process is exiting */
        TRACE("XRPCRT: Unloading...\n");
        break;
    }
    return TRUE;
}
