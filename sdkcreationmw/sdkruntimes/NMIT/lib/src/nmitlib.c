/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nmitlibp.h"

/* NMIT interface context */
struct _NmitInterface {
    XRpcServer * server;
    XRpcRegistry * registry;
    XRpcPort port;
    NmitLoad load;
    NmitToFront toFront;
    NmitRun run;
    NmitExit exit;
    void * context;
    char* xregId;
};

/**
 * Callback invoked by XREG_List. Fills the hashtable with the existing
 * instance ids.
 */
static XRpcBool XREG_API NMIT_ListCB(const XRegServiceInfo * svc, void* ctx)
{
    HashTable * ids = ctx;
    XRpcString id = XREG_GetServiceProp(svc, DSCLI_INSTANCE_ID_PROP);
    if (id) {
        char* copy = STRING_Dup(id);
        if (copy) {
            if (!HASH_Put(ids, copy, copy)) {
                MEM_Free(copy);
            }
        }
    }
    return True;
}

/**
 * Registers our XRPC interface with the XRPC Registry. Returns the XRPC 
 * Registry service ID, NULL on failure
 */
static char * NMIT_Register(Str prod, XRpcRegistry * reg, XRpcPort port)
{
    int instID = 0;
    char * id;
    char iid[16];
    char pid[16];
    char* logonID = XRPC_GetLogonID();
    XRegProperty p[4];
    HashTable ids;
    int n = 0;

    sprintf(pid,"%u",getpid());

    /* fill in the properties */
    p[n].name = DSCLI_PRODUCT_ID_PROP;
    p[n++].value = prod;
    p[n].name = DSCLI_INSTANCE_ID_PROP;
    p[n++].value = iid;
    p[n].name = DSCLI_PROCESS_ID_PROP;
    p[n++].value = pid;
    if (logonID) {
        p[n].name = DSCLI_LOGON_ID_PROP;
        p[n++].value = logonID;
    }

    /* lock the database and get the list of used instance ids */
    HASH_Init(&ids, 0, hashCompareStringKey, stringHashProc, hashFreeKeyProc);
    XREG_Lock(reg, DSCLI_PROTOCOL);
    XREG_List(reg, DSCLI_PROTOCOL, NMIT_ListCB, &ids); 

    /* select unused instance id */
    do {
        sprintf(iid, "%07d", ++instID);
    } while (HASH_Contains(&ids, iid));

    /* use it as the instance id string */
    TRACE1("NMIT: selected instance ID %s\n",iid);

    /* register our instance id and unlock the database */
    id = XREG_RegisterService(reg,DSCLI_PROTOCOL,NULL,port,p,n);
    if (id) {
        TRACE1("NMIT: registered XRPC service %s\n",id);
    }
    XREG_Unlock(reg);
    XRPC_Free(logonID);
    HASH_Destroy(&ids);
    return id;
}

/** 
 * The "load" method handler.
 */
static 
XRpcElement * 
XRPC_API 
NMIT_HandleLoad(
    void * context,                 /* method specific context */
    const XRpcContainer * params)   /* parameters of the call */
{
    NmitInterface* nmit = context;
    unsigned int browser = 0;
    const NmitUcs2 * url = NULL;
    const XRpcStringElement* urlElem;
    const XRpcIntElement* browserElem;
    TRACE("NMIT: load command received\n");
    urlElem = XRPC_GetStringElementByName(params, DSCLI_LOAD_URL_PARAM);
    browserElem = XRPC_GetIntElementByName(params, DSCLI_LOAD_BROWSER_PARAM);
    if (urlElem) url = XRPC_GetStringUCS2(urlElem);
    if (browserElem) browser = XRPC_GetInt(browserElem);
    return _XRPC_CreateBoolElement(nmit->load(nmit->context, url, browser));
}

/** 
 * The "toFront" method handler.
 */
static 
XRpcElement* 
XRPC_API 
NMIT_HandleToFront(
    void* context,                  /* method specific context */
    const XRpcContainer* params)    /* parameters of the call */
{
    NmitInterface* nmit = context;
    TRACE("NMIT: toFront command received\n");
    nmit->toFront(nmit->context);
    return NULL;
}

/** 
 * The "run" method handler. Returns process id of the created process,
 * zero on failure.
 */
static 
XRpcElement* 
XRPC_API 
NMIT_HandleRun(
    void* context,                  /* method specific context */
    const XRpcContainer * params)   /* parameters of the call */
{
    NmitProcessId pid;
    NmitInterface* nmit = context;
    const XRpcStringElement* exeElem;
    const XRpcContainer * argsElem;
    TRACE("NMIT: run command received\n");
    exeElem = XRPC_GetStringElementByName(params, DSCLI_RUN_EXE_PARAM);
    argsElem = XRPC_GetArrayByName(params, DSCLI_RUN_ARGS_PARAM);
    ASSERT(exeElem);
    memset(&pid, 0, sizeof(pid));
    if (exeElem) {
        XRpcStringUCS2 exe = XRPC_GetStringUCS2(exeElem);
        if (exe) {
            NmitStr * argv = NULL;
            int argc = argsElem ? XRPC_ElementCount(argsElem) : 0;
            if (argc > 0) {
                argv = MEM_NewArray(NmitStr,argc);
                if (argv) {
                    int i, n = 0;
                    for (i=0; i<argc; i++) {
                        const XRpcStringElement * strElem = 
                            XRPC_IsStringElement(
                            XRPC_GetDataElementAt(argsElem,i));
                        if (strElem) {
                            argv[n] = XRPC_GetStringUCS2(strElem);
                            if (argv[n]) n++;
                        }
                    }
                    argc = n;
                }
            }
            nmit->run(nmit->context, exe, argv, argc, &pid);
        }
    }
    return _XRPC_CreateLongElement(pid);
}

/** 
 * The "exit" method handler.
 */
static 
XRpcElement* 
XRPC_API 
NMIT_HandleExit(
    void* context,                  /* method specific context */
    const XRpcContainer * params)   /* parameters of the call */
{
    NmitInterface* nmit = context;
    TRACE("NMIT: exit command received\n");
    nmit->exit(nmit->context);
    return NULL;
}

/**
 * Returns the handler for the specified remote call.
 */
static
XRpcBool 
XRPC_API 
NMIT_GetMethod(
    void * context,         /* protocol specific context */
    XRpcString name,        /* method name */
    XRpcMethodInfo * info)  /* receives handler and context */
{
    NmitInterface * nmit = context;

    /* we want to run handlers on the same thread */
    info->synchronous = True;
    info->context = context;
    if (StrCmp(name,DSCLI_LOAD_METHOD) == 0) {
        if (nmit->load) {
            info->handler = NMIT_HandleLoad;
            return XRpcTrue;
        }
    } else if (StrCmp(name,DSCLI_TO_FRONT_METHOD) == 0) {
        if (nmit->toFront) {
            info->handler = NMIT_HandleToFront;
            return XRpcTrue;
        }
    } else if (StrCmp(name,DSCLI_RUN_METHOD) == 0) {
        if (nmit->run) {
            info->handler = NMIT_HandleRun;
            return XRpcTrue;
        }
    } else if (StrCmp(name,DSCLI_EXIT_METHOD) == 0) {
        if (nmit->exit) {
            info->handler = NMIT_HandleExit;
            return XRpcTrue;
        }
    } else {
        TRACE1("NMIT: unknown method %s\n", name);
    }
    return XRpcFalse;
}

/**
 * Initializes the NMIT emulator-side DSCLI context. Returns NULL
 * on failure.
 */
NmitInterface *
NMIT_API 
NMIT_Create(
    const char * aPid,      /* the product id string */
    NmitLoad aLoad,         /* the load callback */
    NmitRun aRun,           /* the run callback */
    NmitToFront aToFront,   /* the toFront callback */
    NmitExit aExit,         /* the exit callback */
    void * aContext)        /* opaque pointer */
{
    NmitInterface* nmit;
    XRPC_Init();
    HASH_InitModule();
    nmit = MEM_New(NmitInterface);
    if (nmit) {
        nmit->server = XRPCRT_Server();
        nmit->registry = XRPCRT_Registry();
        nmit->port = XRPCRT_ServerPort();
        if (nmit->server && nmit->registry && nmit->port) {
            XRpcProtocol protocol;
            nmit->load = aLoad;
            nmit->toFront = aToFront;
            nmit->run = aRun;
            nmit->exit = aExit;
            nmit->context = aContext;
            
            /* Register the DSCLI protocol with the XRPC runtime */
            memset(&protocol, 0, sizeof(protocol));
            protocol.getmethod = NMIT_GetMethod;
            XRPC_RegisterProtocol(nmit->server, DSCLI_PROTOCOL, 
                "DevSimCLI Protocol", "Nokia", 1, 0, &protocol, nmit);
            
            nmit->xregId = NMIT_Register(aPid, nmit->registry, nmit->port);
            if (nmit->xregId) {
                /* we are up and running */
                return nmit;
            }
        }
        MEM_Free(nmit);
    }
    HASH_Shutdown();
    XRPC_Deinit();
    return NULL;
}

/**
 * Deallocates the NMIT DSCLI context.
 */
void NMIT_API NMIT_Delete(NmitInterface * aNmit)
{
    if (aNmit) {
        XREG_UnregisterService(aNmit->registry, aNmit->xregId);
        XRPC_UnregisterProtocol(aNmit->server, DSCLI_PROTOCOL); 
        MEM_Free(aNmit);
        HASH_Shutdown();
        XRPC_Deinit();
    }
}
