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


static char rcsid[] = "$Id: GwTrace.c,v 1.5 2005/11/14 05:21:12 slava Exp $";

#include <s_lib.h>
#include <xrpc.h>
#include <xregapi.h>
#include <xdscli.h>

#define ECMT_MSG_HEADER_SIZE 20 

typedef I32u UID;
typedef struct _GwTrace {
    XRpcServer* server;
    XRpcSession* session;
    File* file;
    Vector includeUid;
    Vector excludeUid;
} GwTrace;

static const Str pname = "gwtrace";
static Event exitEvent;

/**
 * Signal handler
 */
static void GWTRACE_Interrupt(int sig)
{
    switch(sig) {
#ifndef _WIN32
    case SIGPIPE:
        break;
#endif
    case SIGINT:
    default:
        EVENT_Set(&exitEvent);
        break;
    }
    signal(sig, GWTRACE_Interrupt);
}

static void GWTRACE_Session(XRpcSession* session, void* arg)
{
    GwTrace* trace = arg;
    ASSERT(trace->session == session);
    XRPC_RunSession(session);
    EVENT_Set(&exitEvent);
}

static Bool GWTRACE_AcceptMessage(GwTrace* trace, UID uid)
{
    if (!VECTOR_IsEmpty(&trace->includeUid)) {
        return VECTOR_Contains(&trace->includeUid, (VElement)uid);
    } else if (!VECTOR_IsEmpty(&trace->excludeUid)) {
        return !VECTOR_Contains(&trace->excludeUid, (VElement)uid);
    } else {
        /* by default accept all */
        return True;
    }
}

static void GWTRACE_Dump(GwTrace* trace, Str what, const XRpcContainer* param)
{
    /* decode parameters */
    const XRpcIntElement* uidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_LISTENER_UID_PARAM);

    const XRpcBinaryElement* dataParam =
        XRPC_GetBinaryElementByName(param, 
        ECMTGW_LISTENER_DATA_PARAM);

    if (uidParam && dataParam) {
        int uid = XRPC_GetInt(uidParam);
        if (GWTRACE_AcceptMessage(trace, uid)) {
            int len = XRPC_GetBinaryDataSize(dataParam);
            const XRpcByte * data = XRPC_GetBinaryData(dataParam);

            /* dump to the console */
            PRINT_Output("%s: UID=0x%08x, %d byte(s)\n",what,uid,len);
            PRINT_Dump(PRINT_Output, data, len, 0);

            /* write to a file */
            if (trace->file) {
                int total = len + ECMT_MSG_HEADER_SIZE;
                if (FILE_Printf(trace->file,"0x%08X0x%08X",total,uid) > 0 &&
                    FILE_Write(trace->file, data, len) > 0) {
                    FILE_Flush(trace->file);
                } else {

                    /* I/O error. print an error message and exit */
                    Str fname = FILE_Name(trace->file);
                    if (fname) {
                        PRINT_Error("%s: error writing %s\n",pname,fname);
                    }
                    EVENT_Set(&exitEvent);
                }
            }
        }
    }
}

STATIC XRpcElement* GWTRACE_XRpcSend(void* ctx, const XRpcContainer* param)
{
    GWTRACE_Dump(ctx, "Send", param);
    return NULL;
}

STATIC XRpcElement* GWTRACE_XRpcReceive(void* ctx, const XRpcContainer* param)
{
    GWTRACE_Dump(ctx, "Receive", param);
    return NULL;
}

/**
 * Method resolver for "com.nokia.EcmtGwListener" protocol
 */
STATIC XRpcBool GWTRACE_GetMethod(void* ctx, XRpcString m, XRpcMethodInfo* info)
{
    info->synchronous = XRpcTrue;
    info->context = ctx;
    if (StrCmp(m, ECMTGW_LISTENER_SEND_METHOD) == 0) {
        info->handler = GWTRACE_XRpcSend;
        return XRpcTrue;
    } else if (StrCmp(m, ECMTGW_LISTENER_RECV_METHOD) == 0) {
        info->handler = GWTRACE_XRpcReceive;
        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

/**
 * Callback for XREG_List. Grabs the port number
 */
static XRpcBool XREG_API GWTRACE_ListCB(const XRegServiceInfo* info, void* ctx)
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
 * Initializes GwTrace app context, connects to the gateway and
 * accepts one incoming connection.
 */
static Bool GWTRACE_Init(GwTrace* trace, Str host, Port gwPort, Str file)
{
    trace->server = XRPC_CreateServer(NULL);
    if (trace->server) {

        /* Register our side of the SEI proxu protocol */
        XRpcProtocol protocol;
        memset(&protocol, 0, sizeof (protocol));
        protocol.getmethod = GWTRACE_GetMethod;
        if (XRPC_RegisterProtocol(trace->server, ECMTGW_LISTENER_PROTOCOL,
            "EcmtGW listener", "Nokia", 1, 0, &protocol, trace)) {

            /* connect to the Gateway  */
            PRINT_Verbose("%s: connecting to %s:%hu\n",pname,
                host ? host : "localhost", gwPort);
            trace->session = XRPC_Connect(trace->server, host, gwPort);
            if (trace->session) {

                if (file) {
                    trace->file = FILE_Open(file, WRITE_BINARY_MODE, NULL);
                    if (!trace->file) {
                        PRINT_Error("%s: can't create file %s\n",pname,file);
                    }
                }

                if (!file || trace->file) {
                    /* Create thread to parse incoming XML */
                    if (XRPC_CreateSessionThread(trace->session,
                        GWTRACE_Session,trace)) {
                        return True; 
                    }
                    if (trace->file) FILE_Close(trace->file);
                }
            }
            XRPC_UnregisterProtocol(trace->server, ECMTGW_LISTENER_PROTOCOL);
        }
        XRPC_FreeServer(trace->server);
    }
    return False;
}

/**
 * Callback for parsing UID options. Returns False to stop parsing
 */
static Bool GWTRACE_ParseUidOpt(CmdLine * c, Str param, void * ctx)
{
    UID uid;
    Vector* list = ctx;
    if (PARSE_U32(param, &uid, 0)) {
        return VECTOR_Add(list, (VElement)uid);
    } else {
        CMDLINE_Msg(c, "%s: not a valid UID: %s",pname, param);
        return False;
    }
}

/**
 * The program entry point
 */
int main(int argc, char * argv[]) 
{
    int mask = ECMTGW_LISTEN_DEFAULT_MASK;
    Bool traceReceive = False;
    Bool traceSend  = False;
    Str host = NULL;
    Str file = NULL;
    CmdLine* c;
    GwTrace trace;
    XRpcRegistry * r;

    /* Initialize the XRPC library */
    XRPC_Init();

    /* First step of initializing GwTrace context */ 
    memset(&trace, 0, sizeof(trace));
    VECTOR_Init(&trace.includeUid, 0, NULL, NULL);
    VECTOR_Init(&trace.excludeUid, 0, NULL, NULL);

    /* Parse command line */
    c = CMDLINE_Create(pname);
    if (c) {
        CmdOpt* includeOpt;
        CmdOpt* excludeOpt;
        Bool done = False;
        Bool help = False;

        CMDLINE_SetMaxArgs(c, 1);
        CMDLINE_AddTrueOpt(c,'h',"help",
            "print this help and exit",&help);
        CMDLINE_AddTrueOpt(c,'s',"sent",
            "trace packets sent to the handset",&traceSend);
        CMDLINE_AddTrueOpt(c,'r',"receive",
            "trace packets received from the handset",&traceReceive);
        includeOpt = CMDLINE_AddOpt(c,'u',"include",
            "include this UID in the trace (repeatable)",
            GWTRACE_ParseUidOpt, &trace.includeUid, "UID");
        excludeOpt = CMDLINE_AddOpt(c,'x',"exclude",
            "exclude this UID from the trace (repeatable)",
            GWTRACE_ParseUidOpt, &trace.excludeUid, "UID");
        CMDLINE_SetParamName(CMDLINE_AddStrOpt(c,'o',"output",
            "write binary Ecmt messages into a file",&file),"FILE");

        CMDLINE_SetRepeatable(includeOpt);
        CMDLINE_SetRepeatable(excludeOpt);
        CMDLINE_Exclude(includeOpt, excludeOpt);
        if (!CMDLINE_Parse1(c,argv+1,argc-1,0,&host) || help) {
            CMDLINE_Usage(c, "[HOST]", 0);
            CMDLINE_Delete(c);
            VECTOR_Destroy(&trace.includeUid);
            VECTOR_Destroy(&trace.excludeUid);
            XRPC_Deinit();
            return 0;
        }
        CMDLINE_Delete(c);
    }

    if (traceReceive || traceSend) {
        mask = 0;
        if (traceReceive) mask |= ECMTGW_LISTEN_MASK_RECEIVE;
        if (traceSend) mask |= ECMTGW_LISTEN_MASK_SEND;
    }

    /* connect to the registry */
    r = XREG_ConnectRegistry(host, XREG_DEFAULT_PORT);
    if (r) {

        /* find the server port */
        XRpcPort gwPort = 0;
        XREG_List(r, ECMTGW_PROTOCOL, GWTRACE_ListCB, &gwPort);
        XREG_FreeRegistry(r);

        if (gwPort) {
            if (EVENT_Init(&exitEvent)) {
                if (GWTRACE_Init(&trace, host, gwPort, file)) {

                    /* Install signal handlers */
                    #ifndef _WIN32
                    signal(SIGPIPE, GWTRACE_Interrupt);
                    #endif /* _WIN32 */
                    signal(SIGINT, GWTRACE_Interrupt);

                    /* Enable notifications */
                    XRPC_FormatNotify(XRPC_GetClient(trace.session), 
                        ECMTGW_PROTOCOL, 
                        ECMTGW_REGISTER_LISTENER_METHOD,"%"
                        ECMTGW_LISTENER_PROTOCOL_PARAM"!s%"
                        ECMTGW_LISTENER_MASK_PARAM"!i", 
                        ECMTGW_LISTENER_PROTOCOL, mask);

                    /* Wait */
                    EVENT_Wait(&exitEvent);

                    /* cleanup */
                    XRPC_FreeSession(trace.session);
                    XRPC_FreeServer(trace.server);
                    if (trace.file) FILE_Close(trace.file);
                }
                EVENT_Destroy(&exitEvent);
            }
        } else {
            PRINT_Error("%s: Ecmt Gateway is not running.\n",pname);
        }
    } else if (host) {
        PRINT_Verbose("%s: XRPC registry is not running on %s\n",pname,host);
        PRINT_Error("%s: Ecmt Gateway is not running on %s.\n",pname,host);
    } else {
        PRINT_Verbose("%s: XRPC registry is not running\n",pname);
        PRINT_Error("%s: Ecmt Gateway is not running.\n",pname);
    }

    VECTOR_Destroy(&trace.includeUid);
    VECTOR_Destroy(&trace.excludeUid);

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
