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
#include <xcore.h>
#include <xregapi.h>
#include <s_lib.h>

static const XRpcString protocol = "EchoProtocol";

/* waits for call to complete */
static void wait(XRpcCall * call)
{
    if (call) {
        XRPC_Wait(call);
        switch (XRPC_CallStatus(call)) {
        case XRpcStatusNoTarget:    Output("XRPC: no target\n"); break;
        case XRpcStatusNoMethod:    Output("XRPC: no method\n"); break;
        case XRpcStatusSysErr:      Output("XRPC: system error\n"); break;
        case XRpcStatusOK:          Output("XRPC: OK\n"); break;
        }
        XRPC_FreeCall(call);
    } else {
        Error("CLIENT: cannot submit remote call\n");
    }
}

static XRpcBool XREG_API TEST_ListCB(const XRegServiceInfo * info, void * ctx)
{
    XRegServiceInfo * out = (XRegServiceInfo*)ctx;
    if (strcmp(info->name, protocol) == 0) {
        if (info->host) out->host = STRING_Dup(info->host);
        out->id = STRING_Dup(info->id);
        out->port = info->port;
        return False;
    }
    return True;
}

static XRpcBool TEXT_ProtInfoCB(const XRpcProtocolInfo * info, void * context)
{
    if (info->descr) {
        Output("  %s (%s)\n",info->name, info->descr);
    } else {
        Output("  %s\n",info->name);
    }
    return XRpcTrue;
}

int main(int argc, char * argv[]) 
{
    XRpcRegistry * r;

    XRPC_Init();
    RANDOM_InitModule();

    /* connect to the registry */
    r = XREG_CreateRegistry(0,XRegTypeClient);
    if (r) {

        /* find the first EchoProtocol there */
        XRegServiceInfo info;
        memset(&info, 0, sizeof(info));
        XREG_List(r, protocol, TEST_ListCB, &info);
        if (info.id) {
            char pid[12];
            XRpcSession * session;
            Socket s = INVALID_SOCKET;
            IPaddr ip = HOST_IPADDR(127,0,0,1);
            if (info.host) VERIFY(INET_ResolveAddr(info.host, &ip));
            VERIFY(SOCKET_GetTcp(0,&s));

            /* test properties API */
            sprintf(pid,"%d",getpid());
            XREG_SetProperty(r, info.id, "client", pid);
            XREG_ClearProperty(r, info.id, "foo");

            /* connect to the EchoProtocol service  */
            Output("connecting to "IPADDR_FORMAT":%hu\n",
                HOST_IPADDR_FORMAT_ARG(ip),info.port);

            session = XRPC_Connect(NULL, info.host, info.port);
            if (session) {
                XRpcClient * client = XRPC_GetClient(session);

                Output("Listing remote protocols:\n");
                XRPC_ListRemoteProtocols(client, TEXT_ProtInfoCB, NULL);

                Output("invoking method %s.%s\n",
                    XCORE_PROTOCOL,XCORE_METHOD_PING);
                wait(XRPC_Call(client,XCORE_PROTOCOL,
                    XCORE_METHOD_PING,NULL));
                Output("invoking method %s.%s\n",
                    XCORE_PROTOCOL,XCORE_METHOD_LIST_PROTOCOLS);
                wait(XRPC_Call(client,XCORE_PROTOCOL,
                    XCORE_METHOD_LIST_PROTOCOLS,NULL));
                Output("invoking method %s.%s\n",
                    XCORE_PROTOCOL,XCORE_METHOD_GET_PROTOCOL_INFO);
                wait(XRPC_FormatCall(client,XCORE_PROTOCOL,
                    XCORE_METHOD_GET_PROTOCOL_INFO,"%name!s",
                    XCORE_PROTOCOL));
                wait(XRPC_FormatCall(client,XCORE_PROTOCOL,
                    XCORE_METHOD_GET_PROTOCOL_INFO,"%name!s",protocol));

                Output("invoking method %s.Ping\n",protocol);
                wait(XRPC_FormatCall(client,protocol,"Ping","%s!s","ping"));
                Output("invoking method %s.Foo\n",protocol);
                wait(XRPC_FormatCall(client,protocol,"Foo","%s!s","foo"));
                Output("invoking method %s.Bar\n",protocol);
                wait(XRPC_FormatCall(client,protocol,"Bar","%s!s","bar"));

                XRPC_FreeSession(session);
            }
            MEM_Free((void*)info.id);
            MEM_Free((void*)info.host);
        } else {
            Error("ERROR: %s protocol is not registered.\n",protocol);
        }
        XREG_FreeRegistry(r);
    } else {
        Error("ERROR: XRPC registery is not running\n");
    }

    RANDOM_Shutdown();
    XRPC_Deinit();
    return 0;
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
