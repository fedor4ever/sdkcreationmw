/*
* Copyright (c) 2000 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements DSCLI command line interface specification
 *
*/


#include <slib/include/s_lib.h>
#include <wlib/w_lib.h>
#include <nmitlib.h>
#include <xdscli.h>
#include <xrpc/include/xrpc.h>
#include <xrpc/xreg/include/xregapi.h>

#include "SdkProductInfo.h"

// prototype kill-option implementation exists, but it is by default disabled
// #define ENABLE_KILL_OPTION

/* product specific defines */
#define GATEWAY_EXE         "tools\\ecmt\\EcmtGw.exe"
#define DEFAULT_SIM_EXE     "epoc.exe"

#define MIDP_SDK_FILE       "..\\..\\..\\tools\\ecmt\\MIDP.TXT"

/* attachment timeout in milliseconds */
#define ATTACH_TIMEOUT     60000

/* DSCLI command line options */
static const char OPT_NEW []    = "-new";
static const char OPT_USE []    = "-use";
static const char OPT_QINST []  = "-qinst";
static const char OPT_SHELL []  = "-shell";
static const char OPT_HELP []   = "-help";

/* DSCLI commands */
static const char CMD_RUN []    = "run";
static const char CMD_LOAD []   = "load";
static const char CMD_CLOSE []  = "close";

/* "close" options */
static const char CLOSE_EXIT []  = "-exit";
static const char CLOSE_EXIT_ALL []  = "-exitall";

/* non-standard DSCLI commands */
#ifdef _CONSOLE
static const char CMD_HELP []   = "help";
#endif /* _CONSOLE */

/* DSCLI Command Response Delimiter */
static const char DSCLI_DELIM [] = "%%-->>";

/* non-DSCLI command line options */
static const char OPT_EXE []     = "-exe";
static const char OPT_RUN []     = "-run";
static const char OPT_URL []     = "-url";
static const char OPT_TARGET []  = "-target";
static const char OPT_BROWSER [] = "-browser";
static const char OPT_COM []     = "-com";
static const char OPT_DEVICE []  = "-device";
static const char OPT_DEBUG []   = "-debug";

/* "target" options */
static const char TARGET_DEVICE []   = "device";
static const char TARGET_EMULATOR [] = "emulator";

#ifdef ENABLE_KILL_OPTION
static const char CMD_KILL []   = "kill";
static const char OPT_KILL []    = "-kill";
#endif

/* command line flags */
#define OPT_FLAG_NEW        0x0001
#define OPT_FLAG_QINST      0x0002
#define OPT_FLAG_SHELL      0x0004
#define OPT_FLAG_LOAD       0x0008
#define OPT_FLAG_CLOSE      0x0010
#define OPT_FLAG_EXIT       0x0020
#define OPT_FLAG_EXITALL    0x0040
#define OPT_FLAG_HANDSET    0x0080  /* -target:device */

/* application state flags */
#define APP_FLAG_EXITING    0x0100

/* any command */
#define OPT_FLAG_CMD        (OPT_FLAG_LOAD | OPT_FLAG_CLOSE)

static Bool g_debug_output = False;

typedef struct _App App;
typedef struct _AttachInfo {
    App * app;                  /* pointer to the application context */
    Port port;                  /* port to connect to */
    Str pid;                    /* process id as a string */
    Char * instID;              /* instance id of the selected sim */
} AttachInfo;

typedef struct _ConnectInfo {
    char * serviceID;           /* service ID assigned by the XRPC Registry */
    char * host;                /* host to connect to */
    Char * instID;              /* device instance id */
    Port port;                  /* port to connect to */
    Bool defaultDevice;         /* True if it has "default-device" property */
} ConnectInfo;

typedef struct _QInstContext {
    App * app;                  /* pointer to the application context */
    StrBuf * sb;                /* receives instance ids */
} QInstContext;

typedef struct _ConnectContext {
    App * app;                  /* pointer to the application context */
    Vector info;                /* contains pointers to ConnectInfo */
} ConnectContext;

typedef struct _FindContext {
    App * app;                  /* pointer to the application context */
    Str instID;                 /* instance ID of the emulator in question */
    ConnectInfo * info;         /* information about the found emulator */
} FindContext;

typedef struct _RemoteSim {
    XRpcSession * session;      /* the XRPC session */
    XRpcClient * client;        /* the XRPC client interface */
    XRpcPort remotePort;        /* the port we are connected to */
    Char * instID;              /* the instance id */
    Bool gone;                  /* the emulator is gone */
} RemoteSim;

struct _App {
    char * logonID;             /* the logon id */
    Str exe;                    /* custom executable to start */
    Char * defaultExe;          /* default executable to start */
    Str protocol;               /* the name of the XRPC protocol */
    Str productId;              /* the product id */
    Str url;                    /* the URL given as a parameter */
    int com;                    /* the COM port from -com: option */
    Str port;                   /* full port name */
    I32u browser;               /* the UID of the browser from -browser: */
    Str run;                    /* the executable from -run: option */
    Vector runArgs;             /* arguments for the phone executable */
#ifdef ENABLE_KILL_OPTION
    uint64_t kill;              /* process-id from -kill: option */
#endif
    Mutex outputMutex;          /* synchronizes access to the output stream */
    Bool closed;                /* so that we don't print "exiting" twice */
    int flags;                  /* various flags */
    XRpcRegistry * reg;         /* the XRPC Registry client */
    RemoteSim * sim;            /* XRPC connection to the emulator */
};

typedef struct _Help {
    Str command;        /* option string */
    Str parameter;      /* option parameter string (may be NULL) */
    Str help;           /* help description string */
} Help;

/***************************************************************************
 * Thread safe output facility, prepends DSCLI delimiter
 ***************************************************************************/

STATIC void DSCLI_Output2(Str format, ... )
{
    va_list va;
    va_start(va, format);
    PRINT_Stdout(format, va);
    va_end(va);
}

STATIC void DSCLI_Output(App * app, Str format, ... )
{
    va_list va;
    va_start(va, format);
    MUTEX_Lock(&app->outputMutex);
    DSCLI_Output2("%s ",DSCLI_DELIM);
    PRINT_Stdout(format, va);
    MUTEX_Unlock(&app->outputMutex);
    va_end(va);
}

STATIC void DSCLI_Error(App * app, Str format, ... )
{
    va_list va;
    va_start(va, format);
    MUTEX_Lock(&app->outputMutex);
    PRINT_Output("%s ERROR: ",DSCLI_DELIM);
    PRINT_OutputVa(format, va);
    PRINT_Output("\n");
    MUTEX_Unlock(&app->outputMutex);
    va_end(va);
}

STATIC void DSCLI_CommandLineError(App * app, Str arg)
{
    DSCLI_Error(app,"command line syntax error: %s",arg);
}


STATIC void DSCLI_DebugPrint(Str format, ... )
{
#ifdef _CONSOLE
    if (g_debug_output)
    {
        va_list va;
        va_start(va, format);
        PRINT_Stdout(format, va);
        va_end(va);
    }
#endif
}

/* standard responses */

STATIC void DSCLI_Exiting(App * app, Bool maybeCommitSuicide)
{
    if (!(app->flags & APP_FLAG_EXITING)) {
        app->flags |= APP_FLAG_EXITING;
        DSCLI_Output(app, "exiting\n");
    }
#ifndef _CONSOLE
    if (maybeCommitSuicide) {
        TRACE("SDK CLI: I'm about to commit suicide. Goodbye, cruel world...\n");
        /* NOTE: exit() hangs because we are reading stdin */
        TerminateProcess(GetCurrentProcess(),0);
    }
#endif /* _CONSOLE */
}

STATIC void DSCLI_Detaching(App * app)
{
    if (!(app->flags & APP_FLAG_EXITING)) {
        app->flags |= APP_FLAG_EXITING;
        DSCLI_Output(app, "detaching\n");
    }
}

/***************************************************************************
 * Establishes XRPC session with the emulator
 ***************************************************************************/

STATIC RemoteSim* DSCLI_ConnectSim(XRpcPort port, Str instID)
{
    RemoteSim* sim = MEM_New(RemoteSim);
    sim->session = XRPC_Connect(NULL, NULL, port);
    if (sim->session) {
        sim->client = XRPC_GetClient(sim->session);
        sim->remotePort = port;
        sim->instID = STRING_Dup(instID);
        sim->gone = False;
        return sim;
    }
    MEM_Free(sim);
    return NULL;
}

/***************************************************************************
 * Terminates XRPC session with the emulator and deallocates the RemoteSim
 * context.
 ***************************************************************************/

STATIC void DSCLI_Disconnect(App * app)
{
    if (app->sim) {
        if (app->sim->session) {
            XRPC_FreeSession(app->sim->session);
            app->sim->session = NULL;
            app->sim->client = NULL;
        }
        if (app->sim->instID) {
            MEM_Free(app->sim->instID);
            app->sim->instID = NULL;
        }
        MEM_Free(app->sim);
        app->sim = NULL;
    }
}

/***************************************************************************
 * Checks if the device is compatible with us
 ***************************************************************************/

STATIC Bool DSCLI_IsDevice(App * app, const XRegServiceInfo * info)
{
    /* Check if the service is local */
    Str value;
    if (!info->host ||
        !info->host[0] ||
        !StrCmp(info->host,"localhost") ||
        !StrCmp(info->host,"127.0.0.1")) {

        /* Check the product id */
        value = XREG_GetServiceProp(info, DSCLI_PRODUCT_ID_PROP);
        if (value && StrCmp(app->productId, value) == 0) {

            /* Finally, check the logon ID */
            value = XREG_GetServiceProp(info, DSCLI_LOGON_ID_PROP);
            if (value && StrCmp(app->logonID, value) == 0) {

                /* We can work with this guy */
                return True;
            }
        }
    }

    return False;
}

/***************************************************************************
 * Registry enumeration callback
 ***************************************************************************/

STATIC
XRpcBool
XREG_API
DSCLI_RegListCB(
    const XRegServiceInfo * svc,
    void * context)
{
    AttachInfo * attach = context;
    if (DSCLI_IsDevice(attach->app, svc)) {
        XRpcString v;
        if (attach->pid) {
            /* check the process id */
            v = XREG_GetPropValue(svc->props,svc->nprops,DSCLI_PROCESS_ID_PROP);
            if (!v || StrCmp(v, attach->pid)) return True;
        }

        /* query "instance-id" property */
        v = XREG_GetPropValue(svc->props,svc->nprops,DSCLI_INSTANCE_ID_PROP);
        if (v) {
            attach->instID = STRING_Dup(v);
            attach->port = svc->port;
            DSCLI_DebugPrint("SDK CLI: found SDK instance %s, port %hu\n",v,attach->port);
        }
        return False;
    }
    return True;
}

/***************************************************************************
 * XRPC Registry event callback
 ***************************************************************************/

STATIC
void
XREG_API
DSCLI_AttachEventCB(
    const XRegEvent * event,
    void * context)
{
    Event * notify = context;
    switch (event->type) {
    case XRegEventRebind:
        DSCLI_DebugPrint("SDK CLI: XRegEventRebind\n");
        EVENT_Set(notify);
        break;
   case XRegEventRegisterService:
        DSCLI_DebugPrint("SDK CLI: XRegEventRegisterService\n");
        EVENT_Set(notify);
        break;
    case XRegEventUnregisterService:
        DSCLI_DebugPrint("SDK CLI: XRegEventUnregisterService\n");
        break;
    case XRegEventShutdown:
        DSCLI_DebugPrint("SDK CLI: XRegEventShutdown\n");
        EVENT_Set(notify);
        break;
    default:
        DSCLI_DebugPrint("SDK CLI: unknown registry notification %d\n",event->type);
        break;
    }
}

/***************************************************************************
 * Attempts to create client XRPC registry object, returns True if the
 * registry client has already been created. Note that this call is going
 * to fail if no registry is currently running.
 ***************************************************************************/

STATIC Bool DSCLI_GetRegistry(App * app)
{
    if (app->reg) {
        return True;
    } else {
        app->reg = XREG_CreateRegistry(0, XRegTypeClient);
        return (app->reg != NULL);
    }
}

/***************************************************************************
 * Attaches to the running emulator that matches given process id.
 * Optionally, waits the specified number of milliseconds for the
 * emulator to come up
 ***************************************************************************/

STATIC RemoteSim* DSCLI_AttachTo(App * app, int pid, int timeout)
{
    Time now = TIME_Now();
    Time deadline = now + timeout;
    if (!DSCLI_GetRegistry(app) && timeout > 0) {
        while (!DSCLI_GetRegistry(app) && now < deadline) {
            Time maxWait = (deadline - now);
            if (maxWait > 500) maxWait = 500;
            DSCLI_DebugPrint("SDK CLI: waiting for XRPC Registry...\n");
            THREAD_Sleep(RAND_NextInt((int)maxWait));
            now = TIME_Now();
        }
    }

    /* have we finally connected to the registry? */
    if (app->reg) {
        Event event;
        RemoteSim * sim = NULL;
        char spid[16];
        AttachInfo attach;
        memset(&attach, 0, sizeof(attach));
        attach.app = app;
        attach.pid = spid;
        sprintf(spid,"%d",pid);

        /* try if we get lucky without waiting and registering the listener */
        XREG_List(app->reg, app->protocol, DSCLI_RegListCB, &attach);
        if (attach.port) {
            sim = DSCLI_ConnectSim(attach.port, attach.instID);
            if (sim) {
                MEM_Free(attach.instID);
                return sim;
            }
        }

        /* have to wait */
        if (EVENT_Init(&event)) {
            MEM_Free(attach.instID);
            attach.instID = NULL;
            attach.port = 0;
            XREG_SetListener(app->reg, DSCLI_AttachEventCB, &event);
            XREG_List(app->reg, app->protocol, DSCLI_RegListCB, &attach);
            if (attach.port) sim = DSCLI_ConnectSim(attach.port,attach.instID);
            now = TIME_Now();
            while (!sim && now < deadline) {
                long ms = (long)(deadline - now);
                DSCLI_DebugPrint("SDK CLI: waiting for emulator to come up...\n");
                if (EVENT_TimeWait(&event, ms) == WAIT_STATE_ERROR) break;
                if (!EVENT_Reset(&event)) break;
                attach.instID = NULL;
                attach.port = 0;
                XREG_List(app->reg, app->protocol, DSCLI_RegListCB, &attach);
                if (attach.port) {
                    sim = DSCLI_ConnectSim(attach.port, attach.instID);
                    if (sim) break;
                }
                now = TIME_Now();
            }

            XREG_SetListener(app->reg, NULL, NULL);
            MEM_Free(attach.instID);
            EVENT_Destroy(&event);
        }
        return sim;
    }

    return NULL;
}

/***************************************************************************
 * Start a new instance of the emulator
 ***************************************************************************/

STATIC RemoteSim* DSCLI_StartExe(App * app)
{
    RemoteSim* sim = NULL;
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    StrBuf256 cmd;

    STRBUF_InitBufXXX(&cmd);

    /* construct the full name of the emulator executable */
    if (!app->exe) {
        Str path;
        StrBuf256 buf;
        STRBUF_InitBufXXX(&buf);
        path = WIN32_GetModuleFileName(NULL, &buf.sb);
        if (path) {
            Str filePart = FILE_FilePart(path);
            STRBUF_SetLength(&buf.sb, (filePart-path));
            STRBUF_Append(&buf.sb, DEFAULT_SIM_EXE);
            app->exe = app->defaultExe = STRBUF_Dup(&buf.sb);
        }
        STRBUF_Destroy(&buf.sb);
    }

    /* must have exe path at this point */
    if (!app->exe) {
        DSCLI_DebugPrint("SDK CLI: system error\n");
        return NULL;
    }

    /* executable must exist */
    if (!FILE_IsFile(app->exe)) {
        DSCLI_DebugPrint("SDK CLI: no such file: %s\n",app->exe);
        return NULL;
    }

    memset(&startupInfo, 0, sizeof(startupInfo));
    memset(&processInfo, 0, sizeof(processInfo));
    STRBUF_Copy(&cmd.sb, "\"");
    STRBUF_Append(&cmd.sb, app->exe);
    STRBUF_Append(&cmd.sb, "\"");

    DSCLI_DebugPrint("SDK CLI: starting %s\n",STRBUF_Text(&cmd.sb));
    if (CreateProcess(app->exe, cmd.sb.s, NULL, NULL, FALSE, 0,
        NULL, NULL, &startupInfo, &processInfo)) {
        DSCLI_DebugPrint("SDK CLI: started process %d\n",processInfo.dwProcessId);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        /* attach to the newly created instance */
        sim = DSCLI_AttachTo(app, processInfo.dwProcessId, ATTACH_TIMEOUT);
    } else {
        DSCLI_DebugPrint("SDK CLI: CreateProcess error %u\n",GetLastError());
    }

    STRBUF_Destroy(&cmd.sb);
    return sim;
}

STATIC Bool DSCLI_StartSim(App * app)
{
    ASSERT(!app->sim);
    if (!app->sim) {
        app->sim = DSCLI_StartExe(app);
        if (app->sim) {
            /* determine the instance ID */
            if (app->sim->instID) {
                DSCLI_Output(app, "instance : %s\n", app->sim->instID);
            } else {
                DSCLI_Output(app, "instance : unknown\n");
            }
        }
    }

    return (app->sim != NULL);
}

/***************************************************************************
 * Create and initializes ConnectInfo
 ***************************************************************************/

STATIC ConnectInfo * DSCLI_CreateConnectInfo(const XRegServiceInfo * svc)
{
    ConnectInfo * info = MEM_New(ConnectInfo);
    Str def = XREG_GetServiceProp(svc, DSCLI_DEFAULT_INSTANCE_PROP);
    Str instID = XREG_GetServiceProp(svc, DSCLI_INSTANCE_ID_PROP);
    memset(info, 0, sizeof(*info));
    info->serviceID = STRING_Dup(svc->id);
    info->instID = STRING_Dup(instID);
    info->host = STRING_Dup(svc->host);
    info->port = svc->port;
    info->defaultDevice = (def && StrCaseCmp(def,"true") == 0);
    return info;
}

/***************************************************************************
 * Deallocate ConnectInfo structures
 **************************************************************************/

STATIC void DSCLI_FreeConnectInfo(ConnectInfo * info)
{
    if (info) {
        MEM_Free(info->serviceID);
        MEM_Free(info->instID);
        MEM_Free(info->host);
        MEM_Free(info);
    }
}

/***************************************************************************
 * VectorFree routine for a Vector containing ConnectInfo structures
 **************************************************************************/

STATIC void DSCLI_VectorFreeConnectInfo(VElement e)
{
    DSCLI_FreeConnectInfo(e);
}

/***************************************************************************
 * Helper for DSCLI_Find
 ***************************************************************************/

STATIC XRpcBool XREG_API DSCLI_FindCB(const XRegServiceInfo * svc, void * ctx)
{
    FindContext * find = ctx;
    if (DSCLI_IsDevice(find->app, svc)) {
        Str instID = XREG_GetServiceProp(svc, DSCLI_INSTANCE_ID_PROP);
        if (instID && StrCmp(instID, find->instID) == 0) {
            find->info = DSCLI_CreateConnectInfo(svc);
            return False;
        }
    }
    return True;
}

/***************************************************************************
 * Finds the device given its instance ID
 ***************************************************************************/

STATIC ConnectInfo * DSCLI_Find(App * app, Str instID)
{
    FindContext find;
    memset(&find, 0, sizeof(find));
    find.app = app;
    find.instID = instID;
    ASSERT(app->reg);
    XREG_List(app->reg, app->protocol, DSCLI_FindCB, &find);
    return find.info;
}

/***************************************************************************
 * Collects information about the registered XRPC services
 ***************************************************************************/

STATIC XRpcBool XREG_API DSCLI_XRpcCB(const XRegServiceInfo * svc, void * ctx)
{
    ConnectContext* conn = ctx;
    if (DSCLI_IsDevice(conn->app, svc)) {
        VECTOR_Add(&conn->info, DSCLI_CreateConnectInfo(svc));
    } else {
        DSCLI_DebugPrint("SDK CLI: ignoring service %s (%s)\n",svc->id,svc->name);
    }
    return True;
}

/***************************************************************************
 * Pushes emulator window to front
 ***************************************************************************/

STATIC Bool DSCLI_WindowToFront(App * app)
{
    Bool ok = False;
    if (app->sim) {
        XRpcCall* call;
        DSCLI_DebugPrint("SDK CLI: pushing window to front\n");
        call = XRPC_Call(app->sim->client,app->protocol,DSCLI_TO_FRONT_METHOD,NULL);
        if (call) {
            XRPC_Wait(call);
            ok = (XRPC_CallStatus(call) == XRpcStatusOK);
            XRPC_FreeCall(call);
        }
    }
    return ok;
}

/***************************************************************************
 * Respectfully asks the emulator to exit
 ***************************************************************************/

STATIC Bool XREG_API DSCLI_ExitClient(XRpcClient * client, Str protocol)
{
    Bool ok = False;
    XRpcCall* call;
    DSCLI_DebugPrint("SDK CLI: sending exit command\n");
    call = XRPC_Call(client, protocol, DSCLI_EXIT_METHOD, NULL);
    if (call) {
        XRPC_Wait(call);
        ok = (XRPC_CallStatus(call) == XRpcStatusOK);
        XRPC_FreeCall(call);
    }
    return ok;
}

STATIC Bool XREG_API DSCLI_Exit(App * app)
{
    return app->sim && DSCLI_ExitClient(app->sim->client, app->protocol);
}

/***************************************************************************
 * Converts string into a file URL if it looks like a file URL
 **************************************************************************/

STATIC char * DSCLI_ConvertToFileURL(Str str)
{
    char * file = NULL;
    Str fname = NULL;
    StrBuf nameBuf;
    StrBuf postFields;
    STRBUF_Init(&nameBuf);
    STRBUF_Init(&postFields);

    if (STRING_StartsWith(str,"file:")) {
        int pos,end,len;
        fname = str + 5;
        end = len = StrLen(fname);
        while (fname[0] == '/' && fname[1] == '/') fname++;
        pos = STRING_LastIndexOf(fname,'#');
        if (pos >= 0) end = pos;
        pos = STRING_LastIndexOf(fname,'?');
        if (pos >= 0) end = MIN(pos,end);
        STRBUF_CopyN(&nameBuf, fname, end);
        STRBUF_CopyN(&postFields, fname+end, len-end);
        if (FILE_Exist(STRBUF_Text(&nameBuf))) {
            fname = STRBUF_Text(&nameBuf);
        } else {
            if (STRBUF_CharAt(&nameBuf, 0) == '/' &&
                FILE_Exist(STRBUF_Text(&nameBuf)+1)) {
                STRBUF_Erase(&nameBuf,0,1);
                fname = STRBUF_Text(&nameBuf);
            } else {
                fname = NULL;
            }
        }
    } else if (FILE_Exist(str)) {
        fname = str;
    }

    if (fname) {
        char fullPath[MAX_PATH];
        char * filePart;
        int len = GetFullPathName(fname,COUNT(fullPath),fullPath,&filePart);
        if (len > 0) {
            StrBuf urlBuf;
            STRBUF_Init(&urlBuf);
            if (URL_EncodeChars(&urlBuf, fullPath, " \t\r\n\f\v")) {
                STRBUF_Replace(&urlBuf, '\\', '/');
                STRBUF_Insert(&urlBuf, "file://", 0);
                STRBUF_AppendBuf(&urlBuf, &postFields);
                file = STRBUF_Dup(&urlBuf);
                DSCLI_DebugPrint("SDK CLI: file URL: %s\n",file);
            }
            STRBUF_Destroy(&urlBuf);
        }
    }

    STRBUF_Destroy(&postFields);
    STRBUF_Destroy(&nameBuf);
    return file;
}

/***************************************************************************
 * Loads a URL into the emulator or handset
 ***************************************************************************/

STATIC Bool DSCLI_Load(App * app, Str url)
{
    XRpcStatus status = XRpcStatusSysErr;
    Char * fileURL = NULL;
    Str fname = NULL;
    XRpcString response = NULL;

    ASSERT(app->sim);

    /* check if this is a file URL */
    if (url) {
        fileURL = DSCLI_ConvertToFileURL(url);
        if (fileURL) url = fileURL;
    }

    /* push window to front */
    DSCLI_WindowToFront(app);

    /* send the load command to the emulator (or gateway) */
    if (url) {
        XRpcCall* call = NULL;

        /* we exit in out-of-memory situation, no need to check for NULL */
        XRpcContainer * params = XRPC_CreateContainer(XRpcTagParams);
        DSCLI_DebugPrint("SDK CLI: loading %s\n",url);

        /* encode the parameters */
        XRPC_AddStringField(params, DSCLI_LOAD_URL_PARAM, url);
        if (app->com > 0) {
            XRPC_AddIntField(params,ECMTGW_LOAD_PORT_PARAM,app->com);
        } else if (app->port) {
            XRPC_AddStringField(params,ECMTGW_LOAD_PORT_PARAM,app->port);
        }
        if (app->browser) {
            XRPC_AddIntField(params, DSCLI_LOAD_BROWSER_PARAM,
                app->browser);
        }

        /* make the call */
        call = XRPC_Call(app->sim->client, app->protocol,
            DSCLI_LOAD_METHOD, XRPC_ContainerToElement(params));

        /* delete the parameters */
        XRPC_FreeContainer(params);

        /* wait for call to complete */
        if (call) {
            XRPC_Wait(call);
            status = XRPC_CallStatus(call);
            if (status == XRpcStatusOK) {
                /* extract result string, if such exists */
                const XRpcElement * result = XRPC_CallResult(call);
                if ((result) && (XRPC_GetDataType(result) == XRpcTypeString)) {
                    response = XRPC_GetString(XRPC_IsStringElement(XRPC_IsDataElement(result)));
                }
            }

            /* produce appropriate load response. */
            switch (status) {
            case XRpcStatusOK:
                /* if the ack had result-string, print that; else print ok */
                DSCLI_Output(app, "%s : %s\n", CMD_LOAD, (response ? response : "ok"));
                break;
            case XRpcStatusNoTarget:
            case XRpcStatusNoMethod:
                DSCLI_Output(app, "%s : error unsupported\n", CMD_LOAD);
                break;
            default:
                DSCLI_Output(app, "%s : error rejected\n", CMD_LOAD);
                break;
            }

        	XRPC_FreeCall(call);
        }
    } else {
        /* nothing to do */
        return True;
    }

    MEM_Free(fileURL);
    return (status == XRpcStatusOK);
}

/***************************************************************************
 * Calls the phone software to start a phone application
 ***************************************************************************/

STATIC Bool DSCLI_RunPhoneApp(App * app, Str exe, const Vector* args)
{
    uint64_t pid = (uint64_t)0;
    XRpcCall* call = NULL;
    XRpcStatus status = XRpcStatusSysErr;
    XRpcContainer * params;

    ASSERT(app->sim);
    DSCLI_DebugPrint("SDK CLI: starting %s\n",exe);

    /* push window to front */
    DSCLI_WindowToFront(app);

    /* we exit in out-of-memory situation, no need to check for NULL */
    params = XRPC_CreateContainer(XRpcTagParams);
    XRPC_AddStringField(params, DSCLI_RUN_EXE_PARAM, exe);
    if (VECTOR_Size(args) > 0) {
        int i;
        XRpcContainer * a = XRPC_CreateArray();
        for (i=0; i<VECTOR_Size(args); i++) {
            XRPC_AddElement(a,_XRPC_CreateStringElement(VECTOR_Get(args,i)));
        }
        XRPC_AddContainerField(params, DSCLI_RUN_ARGS_PARAM, a);
    }

    /* make the call */
    call = XRPC_Call(app->sim->client, app->protocol,
        DSCLI_RUN_METHOD, XRPC_ContainerToElement(params));

    /* delete the parameters */
    XRPC_FreeContainer(params);

    /* wait for call to complete */
    if (call) {
        XRPC_Wait(call);
        status = XRPC_CallStatus(call);
        if (status == XRpcStatusOK) {
            /* extract process id from the result */
            const XRpcElement * result = XRPC_CallResult(call);
            const XRpcDataElement * data = XRPC_IsDataElement(result);
            if (data) {
                if (XRPC_IsLongElement(data)) {
                    pid = XRPC_GetLong((XRpcLongElement*)result);
                } else if (XRPC_IsIntElement(data)) {
                    pid = XRPC_GetInt((XRpcIntElement*)result);
                }
            }
        }
        XRPC_FreeCall(call);
    }

    /* produce appropriate load response */
    switch (status) {
    case XRpcStatusOK:
        DSCLI_Output(app, "%s : ok "I64U_FORMAT"\n", CMD_RUN, pid);
        break;
    case XRpcStatusNoTarget:
    case XRpcStatusNoMethod:
        DSCLI_Output(app, "%s : error unsupported\n", CMD_RUN);
        break;
    default:
        DSCLI_Output(app, "%s : error rejected\n", CMD_RUN);
        break;
    }

    return (status == XRpcStatusOK);
}

/***************************************************************************
 * Same as the above, only with one parameter
 ***************************************************************************/
STATIC Bool DSCLI_RunPhoneApp1(App * app, Str exe, Str arg)
{
    Bool ok;
    Vector args;
    VECTOR_Init(&args,1,NULL,NULL);
    VECTOR_Add(&args, arg);
    ok = DSCLI_RunPhoneApp(app, exe, &args);
    VECTOR_Destroy(&args);
    return ok;
}

#ifdef ENABLE_KILL_OPTION

/***************************************************************************
 * Calls the phone software to kill a phone application
 ***************************************************************************/

STATIC Bool DSCLI_KillPhoneApp(App* app, uint64_t pid)
{
    XRpcCall* call = NULL;
    XRpcStatus status = XRpcStatusSysErr;
    XRpcContainer * params;
    XRpcBool response = XRpcFalse;

    ASSERT(app->sim);
    DSCLI_DebugPrint("SDK CLI: killing pid %lu\n", pid);

    /* push window to front */
    DSCLI_WindowToFront(app);

    /* we exit in out-of-memory situation, no need to check for NULL */
    params = XRPC_CreateContainer(XRpcTagParams);
    XRPC_AddLongField(params, DSCLI_KILL_PID_PARAM, pid);

    /* make the call */
    call = XRPC_Call(app->sim->client, app->protocol,
        DSCLI_KILL_METHOD, XRPC_ContainerToElement(params));

    /* delete the parameters */
    XRPC_FreeContainer(params);

    /* wait for call to complete */
    if (call) {
        XRPC_Wait(call);
        status = XRPC_CallStatus(call);
        if (status == XRpcStatusOK) {
            const XRpcElement * result = XRPC_CallResult(call);
            if ((result) && (XRPC_GetDataType(result) == XRpcTypeBool)) {
                response = XRPC_GetBool(XRPC_IsBoolElement(XRPC_IsDataElement(result)));
            }
        }
        XRPC_FreeCall(call);
    }

    /* produce appropriate command response */
    switch (status) {
    case XRpcStatusOK:
        DSCLI_Output(app, "%s : "I64U_FORMAT" %s\n", CMD_KILL, pid,
                    (response == XRpcTrue ? "ok" : "failed"));
        break;
    case XRpcStatusNoTarget:
    case XRpcStatusNoMethod:
        DSCLI_Output(app, "%s : error unsupported\n", CMD_KILL);
        break;
    default:
        DSCLI_Output(app, "%s : error rejected\n", CMD_KILL);
        break;
    }

    return (status == XRpcStatusOK);
}

#endif

/***************************************************************************
 * Performs the action specified on the command line. It's assumed that
 * we are already connected to the emulator or gateway.
 ***************************************************************************/

STATIC Bool DSCLI_PerformCommand(App * app)
{
    ASSERT(app->sim);
    if (app->run) {
        return DSCLI_RunPhoneApp(app, app->run, &app->runArgs);
#ifdef ENABLE_KILL_OPTION
    } else if (app->kill) {
        return DSCLI_KillPhoneApp(app, app->kill);
#endif
    } else {
        return DSCLI_Load(app, app->url);
    }
}

/***************************************************************************
 * Connects to the emulator at the specified host/port. Optionally, loads
 * a URL into the emulator. Returns the connection to the emulator
 ***************************************************************************/

STATIC RemoteSim * DSCLI_Connect(App * app, Port port, Str instID)
{
    ASSERT(!app->sim);
    if (!app->sim) {
        app->sim = DSCLI_ConnectSim(port, instID);
        if (app->sim) {
            ASSERT(instID);
            DSCLI_Output(app, "instance : %s\n", instID ? instID : "unknown");
        }
    }
    return app->sim;
}

/***************************************************************************
 * Marks the emulator we are connected to as the default instance
 ***************************************************************************/

STATIC Bool DSCLI_MakeDefault(App * app)
{
    Bool ok = False;
    ASSERT(app->reg);
    ASSERT(app->sim);
    if (app->reg && app->sim) {
        int n;
        ConnectContext conn;
        memset(&conn, 0, sizeof(conn));
        VECTOR_Init(&conn.info, 0, NULL, DSCLI_VectorFreeConnectInfo);
        conn.app = app;

        DSCLI_DebugPrint("SDK CLI: making instance %d default\n",app->sim->instID);

        /* collect the information about running emulators */
        XREG_Lock(app->reg, app->protocol);
        XREG_List(app->reg, app->protocol, DSCLI_XRpcCB, &conn);

        /* got anything? */
        n = VECTOR_Size(&conn.info);
        if (n > 0) {
            int i;
            ConnectInfo * thisInstance = NULL;
            ConnectInfo * defaultInstance = NULL;

            /* find the default instance */
            for (i=0; (!thisInstance || !defaultInstance) && i<n; i++) {
                ConnectInfo * info = VECTOR_Get(&conn.info,i);
                if (info->defaultDevice) defaultInstance = info;
                if (info->port == app->sim->remotePort) thisInstance = info;
            }

            /* must be able to find this connection */
            ASSERT(thisInstance);
            if (thisInstance) {
                Bool updateDefaultInstance = False;
                if (defaultInstance) {
                    if (defaultInstance == thisInstance) {
                        /* nothing to do */
                        ok = True;
                    } else {
                        if (XREG_ClearProperty(app->reg,
                            defaultInstance->serviceID,
                            DSCLI_DEFAULT_INSTANCE_PROP)) {
                            updateDefaultInstance = True;
                        }
                    }
                }

                if (updateDefaultInstance) {
                    ok = XREG_SetProperty(app->reg, thisInstance->serviceID,
                            DSCLI_DEFAULT_INSTANCE_PROP,"true");
                }
            }
        }

        VECTOR_Destroy(&conn.info);
        XREG_Unlock(app->reg);
    }
    return ok;
}

/***************************************************************************
 * Loads a URL via XRPC. Returns the XRPC session to the emulator which
 * accepted the load
 ***************************************************************************/

STATIC Bool DSCLI_Attach(App * app)
{
    int n;
    ConnectContext conn;

    ASSERT(app->reg);
    if (app->sim || !app->reg) {
        return (app->sim != NULL);
    }

    memset(&conn, 0, sizeof(conn));
    VECTOR_Init(&conn.info, 0, NULL, DSCLI_VectorFreeConnectInfo);
    conn.app = app;

    /* collect the information about running emulators */
    ASSERT(!app->sim);
    XREG_Lock(app->reg, app->protocol);
    XREG_List(app->reg, app->protocol, DSCLI_XRpcCB, &conn);

    /* got anything? */
    n = VECTOR_Size(&conn.info);
    if (n > 0) {
        int i;
        Bool haveDefaultDevice = False;

        /* see if we have a default device */
        for (i=0; !haveDefaultDevice && i<n; i++) {
            ConnectInfo * info = (ConnectInfo*)VECTOR_Get(&conn.info,i);
            haveDefaultDevice = info->defaultDevice;
        }

        /* first try to attach to the default device */
        for (i=0; !app->sim && i<n; i++) {
            ConnectInfo * info = (ConnectInfo*)VECTOR_Get(&conn.info,i);
            if (!haveDefaultDevice || info->defaultDevice) {
                DSCLI_DebugPrint("SDK CLI: trying default device at port %hu\n",info->port);
                app->sim = DSCLI_Connect(app, info->port, info->instID);
                if (app->sim && !info->defaultDevice) {
                    /* make it the default device */
                    XREG_SetProperty(app->reg,info->serviceID,
                        DSCLI_DEFAULT_INSTANCE_PROP,"true");
                }
            }
        }

        /* then try non-default device */
        if (!app->sim && haveDefaultDevice) {
            for (i=0; !app->sim && i<n; i++) {

                /* we have already tried all the default devices */
                ConnectInfo * info = (ConnectInfo*)VECTOR_Get(&conn.info,i);
                if (!info->defaultDevice) {
                    DSCLI_DebugPrint("SDK CLI: trying device at port %hu\n",info->port);
                    app->sim = DSCLI_Connect(app, info->port, info->instID);
                    if (app->sim) {
                        /* make it the default device */
                        XREG_SetProperty(app->reg,info->serviceID,
                            DSCLI_DEFAULT_INSTANCE_PROP,"true");
                    }
                }
            }
        }
    }

    VECTOR_Destroy(&conn.info);
    XREG_Unlock(app->reg);
    return (app->sim != NULL);
}

/***************************************************************************
 * -exitall action
 ***************************************************************************/

STATIC XRpcBool XREG_API DSCLI_ExitCB(const XRegServiceInfo * info, void * ctx)
{
    App * app = ctx;
    if (DSCLI_IsDevice(app, info)) {
        XRpcSession* sim = XRPC_Connect(NULL, info->host, info->port);
        if (sim) {
            DSCLI_ExitClient(XRPC_GetClient(sim), app->protocol);
            XRPC_FreeSession(sim);
        }
    }
    return True;
}

/***************************************************************************
 * XRPC Registry listener
 ***************************************************************************/

STATIC void XREG_API DSCLI_Listener(const XRegEvent * event, void * ctx)
{
    App * app = ctx;
    switch (event->type) {
    case XRegEventRegisterService:
        DSCLI_DebugPrint("SDK CLI: service %s registered at port %hu\n",
            event->registerService.service.name,
            event->registerService.service.port);
        break;

    case XRegEventUnregisterService:
        DSCLI_DebugPrint("SDK CLI: service %s (port %hu) is gone\n",
            event->unregisterService.service.name,
            event->unregisterService.service.port);
        if (app->sim->remotePort == event->unregisterService.service.port) {
            DSCLI_DebugPrint("SDK CLI: emulator is gone\n");
            app->sim->gone = True;
            DSCLI_Exiting(app, True);
        }
        break;

    case XRegEventRebind:
        DSCLI_DebugPrint("SDK CLI: XRPC Registry restarted\n");
        if (XRPC_IsAlive(app->sim->client)) {
            DSCLI_DebugPrint("SDK CLI: XRPC Registry restarted\n");
        } else {
            DSCLI_DebugPrint("SDK CLI: emulator is apparently gone\n");
            app->sim->gone = True;
            DSCLI_Exiting(app, True);
        }
        break;

    case XRegEventShutdown:
        DSCLI_DebugPrint("SDK CLI: XRPC Registry gone\n");
        if (!XRPC_IsAlive(app->sim->client)) {
            DSCLI_DebugPrint("SDK CLI: emulator is gone too\n");
            app->sim->gone = True;
            DSCLI_Exiting(app, True);
        }
        break;
    }
}

/***************************************************************************
 * Interactive help
 ***************************************************************************/

#ifdef _CONSOLE
STATIC void DSCLI_PrintHelp( const Help* help, int count )
{
    File * out = FILE_AttachToFile(stdout, "stdout");
    if (out) {
        int i;
        int cols = 80;
        IO_TermSize(NULL,&cols);
        FILE_Puts(out, "Commands:\n");
        for (i=0; i<count; i++) {
            File * wrap = FILE_Wrap(out, 2, cols-1);
            if (wrap) {
                FILE_Printf(out, "\n%s%s\n",
                            help[i].command,
                            (help[i].parameter ? help[i].parameter : ""));
                WRAP_Indent(wrap, 1);
                FILE_Puts(wrap, help[i].help);
                FILE_Flush(wrap);
                FILE_Detach(wrap);
                FILE_Close(wrap);
            }
        }
        FILE_Puts(out, "\n");
        FILE_Detach(out);
        FILE_Close(out);
    }
}

STATIC void DSCLI_Help()
{
    // note, that we will print help only of documented options. There are
    // some other options, which are implemented, but there is no requirements
    // for those (or info, what is their exact purpose)
    //
    STATIC const Help help[] = {
        {
            OPT_HELP, NULL,
            "Prints this help.\n"
        },
        {
            OPT_URL, ":[url | pathname]",
            "Loads file or URL to the emulator. When loading a local file, "\
            "the emulator will try to determine the content type and the "\
            "character encoding based on the name of the file and its "\
            "contents. This will work in most but certainly not all the cases.\n"
        },
        {
            OPT_RUN, ":[executable]",
            "Runs the specified executable in the emulator. The name of the "\
            "executable is the name in the phone file system, not a PC file name.\n"
        },
        {
            OPT_QINST, NULL,
            "Queries existing target instances (emulators or devices) for the sdk.exe. "\
            "If -target -option is not also given, the default is query of emulators.\n"
        },
        {
            OPT_USE, ":[instance id]",
            "Specifies which target instance is the default target for sdk.exe. "\
            "This option is needed only when multiple emulators or device "\
            "connectivity tools are running.\n"
        },
        {
            OPT_TARGET, ":[emulator | device]",
            "Specifies the target for the sdk.exe. "\
            "If -target -option is not given, the default is emulator.\n"
        },
        {
            OPT_SHELL, NULL,
            "Starts shell-mode.\n"
        },
        {
            OPT_COM, ":[com-port number]",
            "Specifies the serial-port to be used communicating with device.\n"
        },
        {
            OPT_BROWSER, ":[uid]",
            "Specifies the UID of the browser to be launched. If not given, "\
            "the default browser will be launched.\n"
        },
        /* What are these?
        {
            OPT_NEW, NULL,
            "?\n"
        },
        {
            OPT_EXE, NULL,
            "?\n"
        },
        {
            OPT_DEVICE, NULL,
            "?\n"
        },
        */
        {
            OPT_DEBUG, NULL,
            "Prints additional debug-information.\n"
        }
#ifdef ENABLE_KILL_OPTION
        ,
        {
            OPT_KILL, ":[pid]",
            "Kills the process of the specified process id in emulator.\n"
        }
#endif
    };

    DSCLI_PrintHelp(help, COUNT(help));
}

/***************************************************************************
 * Interactive help
 ***************************************************************************/

STATIC void DSCLI_ShellHelp()
{
    STATIC const Help help[] = {
        {
            "help", NULL,
            "Prints this help. This is not a standard DSCLI command.\n"
        },
        {
            "load <file | url>", NULL,
            "Loads file or URL to the emulator. When loading a local file, "\
            "the emulator will try to determine the content type and the "\
            "character encoding based on the name of the file and its "\
            "contents. This will work in most but certainly not all the "\
            "cases.\n"
        },
        {
            "run exe [args]", NULL,
            "Runs the specified executable in the emulator. The name of the "\
            "executable is the name in the phone file system, not a PC file "\
            "name. This is not a standard DSCLI command.\n"
        },
        {
            "close [ -exit | -exitall ]", NULL,
            "Closes the connection with the emulator. If no option is "\
            "provided, the emulator will keep running. The command option "\
            "-exit will force unconditional termination of the emulator. "\
            "The command option -exitall will unconditionally terminate "\
            "all running emulators.\n"
        }
    };

    DSCLI_PrintHelp(help, COUNT(help));
}
#endif  /* _CONSOLE */

/***************************************************************************
 * Tokenizer for the shell mode. Returns number of tokens
 ***************************************************************************/

STATIC int DSCLI_Tokenize(Str line, StrBuf * buf, Vector * tokens)
{
    const Char* p = line;
    VECTOR_Clear(tokens);
    while (*p) {
        STRBUF_Clear(buf);
        while (*p && IsSpace(*p)) p++;
        if (*p == '"') {
            p++;
            while (*p && *p != '"') STRBUF_AppendChar(buf,*p++);
            if (*p) p++;
        } else {
            while (*p && !IsSpace(*p)) STRBUF_AppendChar(buf,*p++);
        }
        VECTOR_Add(tokens, STRBUF_Dup(buf));
    }
    return VECTOR_Size(tokens);
}

/***************************************************************************
 * Shell mode
 ***************************************************************************/

#ifdef _CONSOLE
#  define INTERACTIVE_PROMPT() Output("-> ")
#else   /* _CONSOLE */
/* don't litter standard output in non-interactive build */
#  define INTERACTIVE_PROMPT() NOTHING
#endif  /* _CONSOLE */

STATIC void DSCLI_Shell(App * app)
{
    Str exitMsg = NULL;
    StrBuf cmd;
    StrBuf line;
    Vector tokens;
    File * in = FILE_AttachToFile(stdin, "stdin");
    STRBUF_Init(&cmd);
    STRBUF_Init(&line);
    VECTOR_Init(&tokens, 0, NULL, vectorFreeValueProc);
    INTERACTIVE_PROMPT();
    ASSERT(app->reg);
    XREG_SetListener(app->reg, DSCLI_Listener, app);
    while (FILE_ReadLine(in, &line)) {
        Char * p = line.s;
        STRBUF_Clear(&cmd);
        while (*p && !IsSpace(*p)) STRBUF_AppendChar(&cmd,*p++);
        while (*p && IsSpace(*p)) p++;
        if (STRBUF_EqualsTo(&cmd, CMD_LOAD)) {

            /* unquote the URL */
            int len = strlen(p);
            if (len > 1 && p[0] == '"' && p[len-1] == '"') {
                p[len-1] = 0;
                p++;
            }

            /* anything left? */
            if (*p) {
                DSCLI_Load(app, p);
            } else {
                DSCLI_Output(app, "%s : error unknowntype\n", CMD_LOAD);
            }
        } else if (STRBUF_EqualsTo(&cmd, CMD_CLOSE)) {
            if (StrCmp(p,CLOSE_EXIT) == 0) {
                app->flags |= APP_FLAG_EXITING;
                DSCLI_Exit(app);
                exitMsg = "exiting";
            } else if (StrCmp(p,CLOSE_EXIT_ALL) == 0) {
                app->flags |= APP_FLAG_EXITING;
                if (app->reg) {
                    XREG_List(app->reg,app->protocol,DSCLI_ExitCB,app);
                }
                exitMsg = "exiting";
            }
            break;
        } else if (STRBUF_EqualsTo(&cmd, CMD_RUN)) {

            if (DSCLI_Tokenize(p, &cmd, &tokens) > 0) {
                /* "steal" the first token from the vector */
                Char* exe = VECTOR_Get(&tokens, 0);
                VectorFree tmp = tokens.free;
                tokens.free = vectorFreeNothingProc;
                VECTOR_Remove(&tokens, 0);
                tokens.free = tmp;
                DSCLI_RunPhoneApp(app, exe, &tokens);
                MEM_Free(exe);
                VECTOR_Clear(&tokens);
            } else {
                DSCLI_Output(app, "%s : error unknownexe\n", CMD_RUN);
            }

#ifdef _CONSOLE
        } else if (STRBUF_EqualsTo(&cmd, CMD_HELP)) {
            DSCLI_ShellHelp();
#endif /* _CONSOLE */
        } else if (STRBUF_Length(&cmd) > 0) {
            DSCLI_DebugPrint("SDK CLI: garbage \"%s\"\n", STRBUF_Text(&cmd));
            DSCLI_Output(app, "%s : unsupported\n", STRBUF_Text(&cmd));
        }
        INTERACTIVE_PROMPT();
    }
    app->flags |= APP_FLAG_EXITING;
    FILE_Detach(in);
    FILE_Close(in);
    VECTOR_Destroy(&tokens);
    STRBUF_Destroy(&cmd);
    STRBUF_Destroy(&line);
    if (!exitMsg) exitMsg = (app->sim->gone ? "exiting" : "detaching");
    DSCLI_Output(app, "%s\n", exitMsg);
    ASSERT(app->reg);
    XREG_SetListener(app->reg, NULL, NULL);
}

/***************************************************************************
 * Registry enumeration
 ***************************************************************************/

STATIC XRpcBool XREG_API DSCLI_QInstCB(const XRegServiceInfo * svc, void* ctx)
{
    QInstContext * qinst = ctx;
    if (DSCLI_IsDevice(qinst->app, svc)) {
        Str instID = XREG_GetServiceProp(svc, DSCLI_INSTANCE_ID_PROP);
        if (instID && StrCmp(instID,"0")) {
            if (STRBUF_Length(qinst->sb) > 0) STRBUF_Append(qinst->sb,",");
            STRBUF_AppendFormat(qinst->sb, "%s", instID);
        }
    }
    return True;
}

/***************************************************************************
 * Parses the browser name, returns UID
 ***************************************************************************/

STATIC I32u DSCLI_ParseBrowser(Str browser, I32u defaultUid)
{
    I32u uid = defaultUid;
    if ((!StrCaseCmp(browser,ECMTGW_BROWSER_NAME_KIMONO))
        || (!StrCaseCmp(browser,ECMTGW_BROWSER_NAME_KHTML))) {
        /* legacy branch. don't touch uid i.e. default browser is launched */
        DSCLI_DebugPrint("SDK CLI: default browser\n");
    } else {
        if (PARSE_U32(browser,&uid,16)) {
            DSCLI_DebugPrint("SDK CLI: browser %08x\n",uid);
        } else {
            DSCLI_DebugPrint("SDK CLI: invalid browser \"%s\"\n",browser);
        }
    }
    return uid;
}


/***************************************************************************
 * Memory allocator
 ***************************************************************************/

struct _MemContext {
    const MemHook * nextHook;
};

STATIC void * DSCLI_Malloc(MemContext * ctx, size_t size)
{
    void * ptr = MEM_Alloc2(ctx->nextHook, size);
    if (!ptr) {
        PRINT_Error("out of memory!\n");
        abort();
    }
    return ptr;
}

/***************************************************************************
 * Main application function
 ***************************************************************************/

int DSCLI_Run(App * app, int argc, char* argv[])
{
    int i;
    Str inst = NULL;

    /* process the command line */
    for (i=1; i<argc; i++) {
        Str arg = argv[i];

        if (STRING_StartsWith(arg, "-")) {

            /* command line options */
            if (!StrCmp(OPT_NEW, arg)) {
                if (inst || (app->flags & (OPT_FLAG_NEW | OPT_FLAG_QINST))) {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
                app->flags |= OPT_FLAG_NEW;
                continue;

            } else if (!StrCmp(OPT_DEBUG, arg)) {
                g_debug_output = True;
                continue;

            } else if (!StrCmp(OPT_QINST, arg)) {
                if (inst || (app->flags & (OPT_FLAG_NEW | OPT_FLAG_QINST))) {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
                app->flags |= OPT_FLAG_QINST;
                continue;

            } else if (!StrCmp(OPT_SHELL, arg)) {
                if (app->flags & OPT_FLAG_SHELL) {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
                app->flags |= OPT_FLAG_SHELL;
                continue;

            } else if (STRING_StartsWith(arg, OPT_USE)) {
                if (arg[COUNT(OPT_USE)-1] == ':' &&
                    arg[COUNT(OPT_USE)]) {
                    if (inst || (app->flags & (OPT_FLAG_NEW | OPT_FLAG_QINST))) {
                        DSCLI_CommandLineError(app, arg);
                        return -1;
                    }
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
                inst = arg + COUNT(OPT_USE);
                continue;

            } else if (STRING_StartsWith(arg, OPT_EXE)) {
                if (arg[COUNT(OPT_EXE)-1] == ':' &&
                    arg[COUNT(OPT_EXE)]) {
                    app->exe = arg + COUNT(OPT_EXE);
                    if (!FILE_IsFile(app->exe)) {
                        DSCLI_Error(app, "no such file: %s", app->exe);
                        return -1;
                    }
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
            } else if (STRING_StartsWith(arg, OPT_TARGET)) {
                if (arg[COUNT(OPT_TARGET)-1] == ':' &&
                    arg[COUNT(OPT_TARGET)]) {
                    Str target = arg + COUNT(OPT_TARGET);
                    if (StrCaseCmp(target,TARGET_DEVICE) == 0) {
                        app->flags |= OPT_FLAG_HANDSET;
                        app->protocol = ECMTGW_PROTOCOL;
                    } else if (StrCaseCmp(target,TARGET_EMULATOR) == 0) {
                        app->flags &= ~OPT_FLAG_HANDSET;
                        app->protocol = DSCLI_PROTOCOL;
                    } else {
                        DSCLI_Error(app, "unsupported target: %s", target);
                    }
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
            } else if (STRING_StartsWith(arg, OPT_COM)) {
                if (arg[COUNT(OPT_COM)-1] == ':' &&
                    arg[COUNT(OPT_COM)]) {
                    Str port = arg + COUNT(OPT_COM);
                    if (PARSE_Int(port, &app->com, 0) && app->com > 0) {
                        DSCLI_DebugPrint("SDK CLI: using COM%d\n",app->com);
                        app->port = NULL;
                    } else {
                        app->com = 0;
                        app->port = port;
                    }
                    app->flags |= OPT_FLAG_HANDSET;
                    app->protocol = ECMTGW_PROTOCOL;
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
            } else if (STRING_StartsWith(arg, OPT_DEVICE)) {
                if (arg[COUNT(OPT_DEVICE)-1] == ':' &&
                    arg[COUNT(OPT_DEVICE)]) {
                    app->port = arg + COUNT(OPT_DEVICE);
                    app->com = 0;
                    app->flags |= OPT_FLAG_HANDSET;
                    app->protocol = ECMTGW_PROTOCOL;
                    DSCLI_DebugPrint("SDK CLI: using port %s\n",app->port);
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
            } else if (STRING_StartsWith(arg, OPT_RUN)) {
                if (arg[COUNT(OPT_RUN)-1] == ':' &&
                    arg[COUNT(OPT_RUN)]) {
#ifdef ENABLE_KILL_OPTION
                    if ((app->flags & (OPT_FLAG_QINST|OPT_FLAG_LOAD)) || (app->kill)) {
#else
                    if (app->flags & (OPT_FLAG_QINST|OPT_FLAG_LOAD)) {
#endif
                        DSCLI_CommandLineError(app, arg);
                        return -1;
                    } else {
                        app->run = arg + COUNT(OPT_RUN);
                    }
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
#ifdef ENABLE_KILL_OPTION
            } else if (STRING_StartsWith(arg, OPT_KILL)) {
                if (arg[COUNT(OPT_KILL)-1] == ':' &&
                    arg[COUNT(OPT_KILL)]) {

                    Str pid = arg + COUNT(OPT_KILL);
                    if (PARSE_ULong64(pid, &app->kill, 0) && app->kill > 0) {
                        DSCLI_DebugPrint("SDK CLI: killing pid %d\n", app->kill);
                    }
                    if ((app->flags & (OPT_FLAG_QINST|OPT_FLAG_LOAD))
                        || (app->run) || (app->kill == 0)) {
                        DSCLI_CommandLineError(app, arg);
                        return -1;
                    }
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
#endif
            } else if (app->run) {
                VECTOR_Add(&app->runArgs, arg);
            } else if (STRING_StartsWith(arg, OPT_URL)) {
                if (arg[COUNT(OPT_URL)-1] == ':' &&
                    arg[COUNT(OPT_URL)]) {
                    app->flags |= OPT_FLAG_LOAD;
                    app->url = arg + COUNT(OPT_URL);
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
            }
            /* in 3.1 there is only one browser */
            else if (STRING_StartsWith(arg, OPT_BROWSER)) {
                if (arg[COUNT(OPT_BROWSER)-1] == ':' &&
                    arg[COUNT(OPT_BROWSER)]) {
                    Str br = arg + COUNT(OPT_BROWSER);
                    app->browser = DSCLI_ParseBrowser(br, app->browser);
                } else {
                    DSCLI_CommandLineError(app, arg);
                    return -1;
                }
            }
#ifdef _CONSOLE
            else if (!StrCmp(OPT_HELP, arg)) {
                DSCLI_Help();
                return 0;
            }
#endif
            else {
                /* silently ignore unknown command line optionis */
                DSCLI_DebugPrint("SDK CLI: ignoring %s option\n",arg);
            }

        } else {

            if (app->run) {
                /* command line argument for phone app */
                VECTOR_Add(&app->runArgs, arg);

            /* else it's a command */
            } else if (StrCmp(arg, CMD_LOAD) == 0) {
                if (app->flags & OPT_FLAG_CMD) {
                    DSCLI_Output(app, "%s : error unexpected\n",arg);
                }
                app->flags |= OPT_FLAG_LOAD;
                if ((i+1)<argc) {
                    app->url = argv[++i];
                }
            } else if (StrCmp(arg, CMD_CLOSE) == 0) {
                if (app->flags & OPT_FLAG_CMD) {
                    DSCLI_Output(app, "%s : error unexpected\n",arg);
                }
                app->flags |= OPT_FLAG_CLOSE;
                if ((i+1)<argc) {
                    Str param = argv[i+1];
                    if (StrCmp(param, CLOSE_EXIT) == 0) {
                        app->flags |= OPT_FLAG_EXIT;
                        i++;
                    } else if (StrCmp(param,CLOSE_EXIT_ALL) == 0) {
                        app->flags |= OPT_FLAG_EXITALL;
                        i++;
                    }
                }
            } else if (!app->url) {
                /* assume implicit "load" command */
                if (!(app->flags & OPT_FLAG_CMD)) app->flags |= OPT_FLAG_LOAD;
                app->url = arg;
            } else {
                /* unsupported command? */
                DSCLI_Output(app, "%s : error unsupported\n",arg);
            }
        }
    }

    /* generate the name for the gateway executable if we need one */
    if ((app->flags & OPT_FLAG_HANDSET) && !app->exe) {
        Str path;
        StrBuf256 buf;
        STRBUF_InitBufXXX(&buf);
        path = WIN32_GetModuleFileName(NULL, &buf.sb);
        if (path) {
            Str filePart;
            for (i=0; i<4 && (filePart = FILE_FilePart(path))[0] &&
                filePart > path; i++) {
                STRBUF_SetLength(&buf.sb, (filePart-path)-1);
            }
            STRBUF_Append(&buf.sb, "\\");
            STRBUF_Append(&buf.sb, GATEWAY_EXE);
            app->exe = app->defaultExe = STRBUF_Dup(&buf.sb);
            STRBUF_Destroy(&buf.sb);

#if DEBUG /* sanity check in debug build */
            if (!FILE_IsFile(app->exe)) {
                DSCLI_DebugPrint("WARNING: No such file: %s\n",app->exe);
            }
#endif /* DEBUG */

        } else {
            STRBUF_Destroy(&buf.sb);
        }
    }

    /* see what we have been asked to to */
    if (app->flags & OPT_FLAG_QINST) {
        StrBuf32 buf;
        STRBUF_InitBufXXX(&buf);
        if (DSCLI_GetRegistry(app)) {
            QInstContext qinst;
            memset(&qinst, 0, sizeof(qinst));
            qinst.app = app;
            qinst.sb = &buf.sb;
            XREG_List(app->reg, app->protocol, DSCLI_QInstCB, &qinst);
        }
        if (STRBUF_Length(&buf.sb) == 0) STRBUF_Copy(&buf.sb, "none");
        DSCLI_Output(app, "instance : %s\n", STRBUF_Text(&buf.sb));
        STRBUF_Destroy(&buf.sb);
    } else if (app->flags & OPT_FLAG_NEW) {
        if (!(app->flags & OPT_FLAG_CLOSE)) {
            if (DSCLI_StartSim(app)) {
                DSCLI_PerformCommand(app);
            }
        }
    } else if (inst) {
        ConnectInfo * info = NULL;
        if (DSCLI_GetRegistry(app)) {
            info = DSCLI_Find(app, inst);
            if (info) {
                app->sim = DSCLI_Connect(app, info->port, info->instID);
                if (app->sim) {
                    DSCLI_PerformCommand(app);
                    DSCLI_MakeDefault(app);
                }
                DSCLI_FreeConnectInfo(info);
            }
        }
        if (!app->sim) {
            DSCLI_DebugPrint("SDK CLI: no such instance (%s)\n",inst);
            DSCLI_Output(app, "instance : error nonexistent\n");
            if (app->url) DSCLI_Output(app, "%s : error rejected\n", CMD_LOAD);
            return -1;
        }
    } else {
        /* nothing particular interesting was on the command line */
        /* don't create new instance if we are given the "close" command */
        if (!(app->flags & OPT_FLAG_CLOSE)) {
            if (DSCLI_GetRegistry(app) && DSCLI_Attach(app)) {
                DSCLI_PerformCommand(app);
            } else if (DSCLI_StartSim(app)) {
                DSCLI_PerformCommand(app);
            } else {
                DSCLI_DebugPrint("SDK CLI: cannot connect to the emulator\n");
                DSCLI_Output(app, "instance : error invoke\n");
                if (app->flags & OPT_FLAG_LOAD) {
                    DSCLI_Output(app, "%s : error rejected\n", CMD_LOAD);
                }
                return -1;
            }
        }
    }

    /* handle the "close" command */
    if (app->flags & OPT_FLAG_CLOSE) {
        if (app->flags & OPT_FLAG_EXIT) {
            if (DSCLI_GetRegistry(app) && DSCLI_Attach(app)) {
                DSCLI_Exiting(app, False);
                DSCLI_Exit(app);
                DSCLI_Disconnect(app);
            }
        } else if (app->flags & OPT_FLAG_EXITALL) {
            DSCLI_Exiting(app, False);
            if (DSCLI_GetRegistry(app)) {
                if (app->sim) {
                    DSCLI_Exit(app);
                    DSCLI_Disconnect(app);
                }
                XREG_List(app->reg,app->protocol,DSCLI_ExitCB,&app);
            }
        } else {
            DSCLI_Detaching(app);
        }
    }

    /* enter the shell mode */
    if (app->sim) {
        if (app->flags & OPT_FLAG_SHELL) {
            DSCLI_DebugPrint("SDK CLI: entering the shell mode\n");
            VERIFY(DSCLI_GetRegistry(app));
            DSCLI_Shell(app);
            DSCLI_DebugPrint("SDK CLI: exiting the shell mode\n");
        } else {
            DSCLI_WindowToFront(app);
        }
    }

    return 0;
}

/***************************************************************************
 * Entry point
 ***************************************************************************/

int main(int argc, char* argv[])
{
    int ret;
    App app;
    StrBuf sb;

    /* install memory hook */
    MemProc memHook;
    MemContext memContext;
    memset(&memHook,0,sizeof(memHook));
    memHook.memAlloc = DSCLI_Malloc;
    memContext.nextHook = MEM_NextHook(MEM_Hook(&memHook,&memContext));

    XRPC_Init();

    memset(&app, 0, sizeof(app));
    MUTEX_Init(&app.outputMutex);
    VECTOR_Init(&app.runArgs, 0, NULL, NULL);
    app.logonID = XRPC_GetLogonID();
    app.protocol = DSCLI_PROTOCOL;

    /*
     * Determine the product id. If we find MIDP.TXT from epoc32\tools\ecmt\,
     * we are part of the MIDP SDK. Otherwise, we think it's a C++ SDK.
     * Note! the check assumes, that sdk.exe resides in directory
     * \epoc32\release\winscw\udeb\ (or in some other dir of same level).
     */
    STRBUF_Init(&sb);
    WIN32_GetModuleFileName(NULL, &sb);
    STRBUF_SetLength(&sb, STRBUF_LastIndexOf(&sb, '\\')+1);
    STRBUF_Append(&sb, MIDP_SDK_FILE);
    if (FILE_Exist(STRBUF_Text(&sb))) {
	     app.productId = SDK_PRODUCT_ID_MIDP;
    } else {
	     app.productId = SDK_PRODUCT_ID_CPP;
    }
#if DEBUG
    {
        int i;
        STRBUF_Clear(&sb);
        for (i=0; i<argc; i++) {
            STRBUF_Append(&sb, argv[i]);
            STRBUF_Append(&sb, " ");
        }
        MessageBox(NULL, STRBUF_Text(&sb), app.productId, 0);
    }
#endif
    STRBUF_Destroy(&sb);

    ret = DSCLI_Run(&app, argc, argv);

    DSCLI_DebugPrint("SDK CLI: product id: %s\n",app.productId);

    DSCLI_Disconnect(&app);
    XREG_FreeRegistry(app.reg);
    MEM_Free(app.sim);
    MUTEX_Destroy(&app.outputMutex);
    XRPC_Free(app.logonID);
    VECTOR_Destroy(&app.runArgs);
    MEM_Free(app.defaultExe);
    XRPC_Deinit();
    return 0;
}
