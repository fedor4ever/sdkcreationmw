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

/* The program name */
static const char pname [] = "xls";

/**
 * The callback invoked by XREG_List on each registered XRPC service.
 * Just prints the service information and increments the counter.
 */
static XRpcBool XREG_API XLS_ListCB(const XRegServiceInfo * info, void * ctx)
{
    /* Increment the counter */
    int * count = (int*)ctx;
    (*count)++;

    /* Check if we are in verbose mode */
    if (TRACE_GetMask() & TRACE_VERBOSE) {

        /* Verbose output */
        int i;
        int plen;
        char prefix[16];

        /* Current service count and the service id */
        sprintf(prefix,"%d. ",(*count));
        plen = strlen(prefix);
        TRACE_Output("\n%s%s\n",prefix,info->id);

        /* Fill the prefix with spaces */
        for (i=0; i<plen; i++) prefix[i] = ' ';

        /* The protocol (target) name */
        TRACE_Output("%sName: %s\n",prefix,info->name);

        /* Host name (if present) */
        if (info->host) TRACE_Output("%sHost: %s\n",prefix,info->host);

        /* TCP port number the service is listening on */
        TRACE_Output("%sPort: %hu\n",prefix,info->port);

        /* Service properties */
        for (i=0; i<info->nprops; i++) {
            const XRegProperty * p = info->props + i;
            if (i == 0) TRACE_Output("\n");
            TRACE_Output("%s  %s = %s\n",prefix,p->name,p->value);
        }
    } else {

        /* Compact output */
        Str host = info->host;
        if (!host) host = "localhost";
        TRACE_Output("%s:%hu  %s\n",host,info->port,info->name);
    }
    return True;
}

/**
 * The program entry point.
 */
int main(int argc, char * argv[]) 
{
    CmdLine * c;
    Str host = NULL;
    Vector params;
    int port = 0;
    Bool help = False;
    Bool verbose = False;
 
    /* Initialize the XRPC library */
    XRPC_Init();

    /* Parse the comment line */
    c = CMDLINE_Create(pname);
    CMDLINE_AddTrueOpt(c,'h',"help","print this help and exit",&help);
    CMDLINE_AddStrOpt(c,'a',"host","remote registry host",&host);
    CMDLINE_AddIntOpt(c,'p',"port","remote registry port",&port);
    CMDLINE_AddTrueOpt(c,'v',"verbose","produce verbose output",&verbose);
    VECTOR_Init(&params, 0, vectorEqualsString, NULL);
    if (CMDLINE_Parse(c,argv+1,argc-1,PARSE_NO_DUP,&params) &&
        VECTOR_Size(&params) == 0 && !help) {

        /* Set TRACE_ALL flags if verbose mode is selected */
        if (verbose) TRACE_SetMask(TRACE_ALL);

        /* Validate the port number */
        if (port < 0 || port > (int)USHRT_MAX) {
            Error("%s: invalid port number %d\n",pname,port);
        } else {

            /* Connect to the XRPC Registry */
            XRpcRegistry * reg = XREG_ConnectRegistry(host,(Port)port);
            if (reg) {

                /* 
                 * List the contents of the registry. On exit, count
                 * contains the number of services. The count variable
                 * is incremented by the XLS_ListCB callback
                 */
                int count = 0;
                XREG_List(reg, NULL, XLS_ListCB, &count);

                /* Print the number of service we have found */
                if (count > 0) {
                    TRACE_Verbose("\n%s: total %d service(s)\n",pname,count);
                } else {
                    TRACE_Verbose("%s: no XRPC services\n",pname);
                }

                /* Deallocate the XRPC Registry object */
                XREG_FreeRegistry(reg);
            } else {
                TRACE_Output("%s: XRPC Registry is not running\n",pname);
            }
        }
    } else {

        /* 
         * Print the usage summary. Zero maxcol parameter allows us to use
         * the full width of the terminal window for output
         */
        CMDLINE_Usage(c, NULL, 0);
    }


    /* Cleanup */
    VECTOR_Destroy(&params);
    CMDLINE_Delete(c);
    XRPC_Deinit();
    return 0;
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
