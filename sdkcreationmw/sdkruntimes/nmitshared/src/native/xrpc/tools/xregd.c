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


#include <xregapi.h>
#include <s_lib.h>

static const char pname [] = "xregd";
static Bool stopped = False;
static Event signalEvent;

/**
 * Signal handler
 */
static void XREGD_SignalHandler(int sig)
{
    Str name;
    Bool fatal = True;
    char temp[32];

    switch (sig) {
#ifndef _WIN32
    case SIGHUP:
        name = "hangup";
        break;
    case SIGQUIT:
        name = "quit";
        break;
    case SIGPIPE:
        name = "broken pipe";
        fatal = False;          /* ignore "broken pipe" signals on Unix */
        break;
#endif /* _WIN32 */

    case SIGINT:
        name = "interrupt";
        break;
    case SIGTERM:
        name = "termination";
        break;
    default:
        /* 
         * This case should never be reached, because we list above all
         * the signals for which we actually establish a signal handler.  
         */
        sprintf (temp, "%d", sig);
        name = temp;
        break;
    }

    if (!stopped) {
        stopped = True;
        Verbose("*** %s ***\n", name);
    }
    if (fatal) {
        EVENT_Set(&signalEvent);
    }
    signal(sig, XREGD_SignalHandler);
}

/**
 * Runs the XRPC registry
 */
static int XREGD_Run(Port p)
{
    XRpcRegistry * reg = XREG_CreateRegistry(p,XRegTypeServer);
    if (reg) {
        Verbose("%s: XRPC Registry running\n",pname);
        EVENT_Wait(&signalEvent);
        XREG_FreeRegistry(reg);
        return 0;
    } else {
        Error("%s: could not create XRPC Registry\n",pname);
        return 1;
    }
}

/**
 * The entry point for the XRPC Registry daemon program
 */
int main(int argc, char * argv[]) 
{
    int retval = 1;
    
    /* initialize libraries */
    if (XRPC_Init()) {

        /* initialize local variables */
        CmdLine * c = CMDLINE_Create(pname);
        if (c) {

            /* parse command line */
            int port = 0;
            Bool verbose = False;
            Bool help = False;
            Vector params;
            StrBuf msg;
            STRBUF_Init(&msg);
            STRBUF_Format(&msg,"registry port (default %d)",XREG_DEFAULT_PORT);
            CMDLINE_AddTrueOpt(c,'h',"help","print this help and exit",&help);
            CMDLINE_AddIntOpt(c,'p',"port",STRBUF_Text(&msg),&port);
            CMDLINE_AddTrueOpt(c,'v',"verbose","produce verbose output",&verbose);
            VECTOR_Init(&params, 0, vectorEqualsString, NULL);
            if (CMDLINE_Parse(c,argv+1,argc-1,PARSE_NO_DUP,&params) &&
                VECTOR_Size(&params) == 0 && !help) {
                CMDLINE_Delete(c);
                if (verbose) TRACE_SetMask(TRACE_ALL);

                /* verify port number */
                if (port < 0 || port > (int)USHRT_MAX) {
                    Error("%s: invalid port number %d\n",pname,port);
                } else {
                    if (EVENT_Init(&signalEvent)) {

                        /* install signal handlers */
#ifndef _WIN32
                        signal(SIGHUP,  XREGD_SignalHandler);
                        signal(SIGQUIT, XREGD_SignalHandler);
                        signal(SIGPIPE, XREGD_SignalHandler);
#endif /* _WIN32 */
                        signal(SIGINT,  XREGD_SignalHandler);
                        signal(SIGTERM, XREGD_SignalHandler);

                        /* run the registry */
                        retval = XREGD_Run((XRpcPort)port);
                        EVENT_Destroy(&signalEvent);
                    }
                }

            } else {
                CMDLINE_Usage(c,NULL,0);
                CMDLINE_Delete(c);
            }
            VECTOR_Destroy(&params);
            STRBUF_Destroy(&msg);
        }
        XRPC_Deinit();
    }
    return retval;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
