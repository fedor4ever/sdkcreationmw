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
#include <s_lib.h>

typedef struct _EchoContext {
    char *  name;
} EchoContext;

typedef struct _ServiceContext {
    Socket socket;
    XRpcRegistry * reg;
    char* id;
} ServiceContext;

static Event exitEvent;

void TEST_SignalHandler(int sig)
{
    Output("*** Interrupt\n");
    EVENT_Set(&exitEvent);
}

void TEST_ExitThread(void * arg)
{
    ServiceContext * context = arg;
    EVENT_Wait(&exitEvent);
    XREG_SetListener(context->reg, NULL, NULL);
    XREG_UnregisterService(context->reg, context->id);
    SOCKET_Close(context->socket);
}

XRpcElement * 
XRPC_API 
ECHO_Method(
    void *                  context,    /* method specific context */
    const XRpcContainer *   params)     /* parameters of the call */
{
    EchoContext * echo = (EchoContext* )context;
    XRpcElement * result = _XRPC_CreateStringElement(echo->name);
    Output("Hello, I'm the %s method\n",echo->name);

    MEM_Free(echo->name);
    MEM_Free(echo);

    return result;
}

XRpcBool 
XRPC_API 
TEST_GetMethod(
    void *                  context,    /* protocol specific context */
    XRpcString              name,       /* method name */
    XRpcMethodInfo *        info)       /* receives handler and context */
{
    EchoContext * echo = MEM_New(EchoContext);
    echo->name = STRING_Dup(name);
    info->context = echo;
    info->handler = ECHO_Method;
    info->synchronous = (StrCmp(name,"Ping") == 0);
    return XRpcTrue;
}

void XRPC_API TEST_SessionOpen(void * context, XRpcSession * session)
{
    Output("TEST: session %08lX started\n",session);
}

void XRPC_API TEST_SessionClose(void * context, XRpcSession * session)
{
    Output("TEST: session %08lX finished\n",session);
}

void XREG_API TEST_DumpServiceInfo(const XRegServiceInfo * service)
{
    Output("   id:       %s\n",service->id);
    Output("   protocol: %s\n",service->name);
    Output("   host:     %s\n",(service->host ? service->host : "localhost"));
    Output("   port:     %hu\n",service->port);
    if (service->nprops) {
        int i;
        Output("   properties:\n");
        for (i=0; i<service->nprops; i++) {
            XRegProperty * p = service->props + i;
            Output("     %2d. %s=%s\n", (i+1), p->name, p->value);
        }
    }
}

void XREG_API TEST_RegistryListener(const XRegEvent * e, void * ctx)
{
    switch (e->type) {
    case XRegEventRegisterService:
        Output("TEST: service registered:\n");
        TEST_DumpServiceInfo(&e->registerService.service);
        break;
    case XRegEventUnregisterService:
        Output("TEST: service unregistered:\n");
        TEST_DumpServiceInfo(&e->registerService.service);
        break;
    case XRegEventRebind:
        Output("TEST: re-connected to the registry\n");
        break;
    case XRegEventShutdown:
        Output("TEST: disconnected from the registry\n");
        break;
    default:
        ASSMSG1("Unknown XRPC registry event type %d",e->type);
        break;
    }
}

int main(int argc, char * argv[])
{
    ServiceContext ctx;
    Port port = 1111;

    XRPC_Init();
    RANDOM_InitModule();

    if (argc == 2) port = atoi(argv[1]);

    EVENT_Init(&exitEvent);
    signal(SIGINT, TEST_SignalHandler);

    if (SOCKET_GetTcp(port, &ctx.socket)) {
        ctx.reg = XREG_CreateRegistry(0,XRegTypeAny);
        if (ctx.reg) {
            ThrID exitThread;
            XRpcProtocol p;
            XRegProperty prop;
            
            Str pname = "EchoProtocol";
            XRpcServer * server = XRPC_CreateServer(NULL);

            memset(&p, 0, sizeof(p));
            p.getmethod = TEST_GetMethod;
            p.sopen = TEST_SessionOpen;
            p.sclose = TEST_SessionClose;

            XRPC_RegisterProtocol(server,pname,"Test","slava",1,0,&p,NULL);
            XREG_SetListener(ctx.reg, TEST_RegistryListener, NULL);
            XREG_Lock(ctx.reg, pname);

            prop.name = "foo";
            prop.value = "bar";
            ctx.id = XREG_RegisterService(ctx.reg,pname,NULL,port,&prop,1);
            XREG_Unlock(ctx.reg);
            if (ctx.id) {
                const XRegServiceInfo * serviceInfo;
                Output("SERVER: registered %s with XRPC Registry, id=%s\n",
                    pname,ctx.id);

                /* test the "queryService" API */
                serviceInfo = XREG_Query(ctx.reg, ctx.id);
                ASSERT(serviceInfo);

                /* test set/clear property API */
                XREG_SetProperty(ctx.reg, ctx.id, "one", "two");
                XREG_ClearProperty(ctx.reg, ctx.id, "one");

                /* create the "exit thread" so that we can handle signals */
                THREAD_Create(&exitThread, TEST_ExitThread, &ctx);

                Output("TEST: listening on port %hu\n",port);
                XRPC_ServerListenSocket(server, ctx.socket, XRpcFalse);
                XREG_Free(ctx.id);
            }
            XRPC_FreeServer(server);
            XREG_FreeRegistry(ctx.reg);

            /* exit thread will close the socket */
            EVENT_Set(&exitEvent);
            THREAD_Join(exitThread);
        } else {
            Error("TEST: cannot create registry (port busy?)\n");
        }
    } else {
        Error("TEST: cannot listen on port %hu\n",port);
    }

    EVENT_Destroy(&exitEvent);
    RANDOM_Shutdown();
    XRPC_Deinit();

    MEM_DumpStat();
    MEM_Dump();
    return 0;
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
