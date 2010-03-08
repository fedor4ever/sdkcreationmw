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


#include <xrpc.h>
#include <xregapi.h>

/* 
 * These constants describe the protocol. In the real life they should be
 * in a header file shared by both server and client.
 */
#define PROTOCOL            "Test"
#define METHOD_ECHO         "echo"
#define ECHO_PARAM          "s"
#define METHOD_STRLEN       "strlen"
#define STRLEN_PARAM        "s"

/** 
 * Calls the remote "echo" method. Returns XRpcTrue if the call succeeds,
 * XRpcFalse in case of failure
 */
static XRpcBool CLIENT_Echo(XRpcClient* client, XRpcString s)
{
    XRpcBool ok = XRpcFalse;
    XRpcString format =  "%"ECHO_PARAM"!s";
    XRpcCall* call = XRPC_FormatCall(client,PROTOCOL,METHOD_ECHO,format,s);
    if (call) {
        XRPC_Wait(call);
        ok = (XRPC_CallStatus(call) == XRpcStatusOK);
        XRPC_FreeCall(call);
    }
    return ok;
}

/** 
 * Calls the remote "strlen" method. Returns the length of the string, -1
 * if the remote call fails.
 */
static int CLIENT_Strlen(XRpcClient* client, XRpcString s)
{
    int len = -1;
    XRpcString format =  "%"STRLEN_PARAM"!s";
    XRpcCall* call = XRPC_FormatCall(client,PROTOCOL,METHOD_STRLEN,format,s);
    if (call) {
        XRPC_Wait(call);
        if (XRPC_CallStatus(call) == XRpcStatusOK) {
            /* Check the type of the return value */
            const XRpcElement* elem = XRPC_CallResult(call);
            if (elem && XRPC_GetDataType(elem) == XRpcTypeInt) {
                /* extract the int value from the data element */
                len = XRPC_GetInt(XRPC_IsIntElement(XRPC_IsDataElement(elem)));
            }
        }
        XRPC_FreeCall(call);
    }
    return len;
}

/**
 * Callback for XREG_List. Grabs the port number
 */
static XRpcBool XREG_API CLIENT_ListCB(const XRegServiceInfo* info, void* ctx)
{
    XRpcPort* port = ctx;
    *port = info->port;

    /* 
     * If more than one server has been registered with the XRPC Registry,
     * this callback may be called more than once. We pick the first one and
     * return XRpcFalse to stop the enumeration 
     */
    return XRpcFalse;
}

/**
 * Calls CLIENT_Strlen and CLIENT_Echo with the specified parameter.
 */
static void CLIENT_Test(XRpcClient* client, XRpcString s)
{
    int len = CLIENT_Strlen(client, s);
    printf("CLIENT: TEST_Strlen(\"%s\") = %d\n",s, len);
    CLIENT_Echo(client, s);
}

/**
 * The program entry point
 */
int main(int argc, char * argv[]) 
{
    XRpcRegistry * r;

    /* Initialize the XRPC library */
    XRPC_Init();

    /* connect to the registry */
    r = XREG_CreateRegistry(0, XRegTypeClient);
    if (r) {

        /* find the server port */
        XRpcPort port = 0;
        XREG_List(r, PROTOCOL, CLIENT_ListCB, &port);

        if (port) {
            XRpcSession * session;

            /* connect to the server  */
            printf("CLIENT: connecting to port %hu\n",port);
            session = XRPC_Connect(NULL, NULL, port);
            if (session) {

                /* call the remote methods */
                XRpcClient* client = XRPC_GetClient(session);
                if (argc > 1) {
                    int i;
                    for (i=1; i<argc; i++) {
                        CLIENT_Test(client, argv[i]);
                    }
                } else {
                    /* by default, send "Hello world!" string */
                    CLIENT_Test(client, "Hello world!");
                }

                /* cleanup */
                XRPC_FreeSession(session);
            }
        } else {
            printf("CLIENT: protocol \""PROTOCOL"\" is not registered.\n");
        }
        XREG_FreeRegistry(r);
    } else {
        printf("CLIENT: XRPC registry is not running\n");
    }

    /* Deinitialize the XRPC library */
    XRPC_Deinit();
    return 0;
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
