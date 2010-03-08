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
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#  include <winsock2.h>
#else
#  include <unistd.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  define closesocket close
#endif

/* 
 * These constants describe the protocol. In the real life they should be
 * in a header file shared by both server and client.
 */
#define PROTOCOL            "Test"
#define METHOD_ECHO         "echo"
#define ECHO_PARAM          "s"
#define METHOD_STRLEN       "strlen"
#define STRLEN_PARAM        "s"

/*
 * Server socket is global so that we can shutdown the server from 
 * the signal handler
 */
static int SERVER_socket = -1;

/**
 * Signal handler
 */
static void SERVER_Interrupt(int sig)
{
    switch(sig) {
#ifndef _WIN32
    case SIGPIPE:
        break;
#endif
    case SIGINT:
    default:
        if (SERVER_socket >= 0) {
            int sock = SERVER_socket;
            SERVER_socket = -1;
            shutdown(sock,3);
            closesocket(sock);
        }
    }
    signal(sig, SERVER_Interrupt);
}

/**
 * Handler for the "echo" call. Prints the string passed in as a parameter.
 */
static 
XRpcElement* 
XRPC_API 
SERVER_Echo(
    void* context,              /* method specific context, not used */
    const XRpcContainer* arg)   /* parameters of the call */
{
    const XRpcStringElement* s = XRPC_GetStringElementByName(arg,ECHO_PARAM);
    if (s) {
        /* print the string */
        printf("%s\n",XRPC_GetString(s));
    }

    /* nothing to return */
    return NULL;
}

/**
 * Handler for the "strlen" call. Calculates the length of the string
 * passed in as a parameter.
 */
static 
XRpcElement* 
XRPC_API 
SERVER_Strlen(
    void* context,              /* method specific context, not used */
    const XRpcContainer* arg)   /* parameters of the call */
{
    const XRpcStringElement* s = XRPC_GetStringElementByName(arg,STRLEN_PARAM);
    if (s) {
        /* calculate the length */
        int len = strlen(XRPC_GetString(s));
        return _XRPC_CreateIntElement(len);
    } else {
        /* some kind of error */
        return NULL;
    }
}

/**
 * Returns the handler for the specified remote call
 */
XRpcBool 
XRPC_API 
SERVER_GetMethod(
    void* context,          /* protocol specific context (not used) */
    XRpcString  name,       /* method name */
    XRpcMethodInfo* info)   /* receives handler and context */
{
    info->synchronous = XRpcTrue;
    if (strcmp(name, METHOD_ECHO) == 0) {
        info->handler = SERVER_Echo;
        return XRpcTrue;
    } else if (strcmp(name, METHOD_STRLEN) == 0) {
        info->handler = SERVER_Strlen;
        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

/**
 * Creates a TCP/IP socket. Returns the socket descriptor.
 */
static int SERVER_GetServerSocket(XRpcPort* port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock >= 0) {
        struct sockaddr_in sa;

        /* initialize sockaddr_in */
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;

        /* bind to port */
        if (bind(sock,(struct sockaddr*)&sa,sizeof(sa)) == 0) {
            
            /* get our port number */
            int namelen = sizeof(sa);
            getsockname(sock,(struct sockaddr*)&sa, &namelen);
            *port = htons(sa.sin_port);
            return sock;
        }

        closesocket(sock);
    }
    return -1;
}

/**
 * Program entry point 
 */
int main(int argc, char * argv[])
{
    XRpcPort port;

    /* Initialize the XRPC library */
    XRPC_Init();

    /* Install signal handlers */
#ifndef _WIN32
    signal(SIGPIPE, SERVER_Interrupt);
#endif /* _WIN32 */
    signal(SIGINT, SERVER_Interrupt);

    /* Create the socket */
    SERVER_socket = SERVER_GetServerSocket(&port);
    if (SERVER_socket >= 0) {

        /* Start the registry if it's not running yet */
        XRpcRegistry* reg = XREG_CreateRegistry(0, XRegTypeAny);
        if (reg) {

            /* Initialize the XRPC server runtime */
            XRpcServer * server = XRPC_CreateServer(NULL);
            if (server) {
                char* id;

                /* Register the protocol with the XRPC runtime */
                XRpcProtocol p;
                memset(&p, 0, sizeof(p));
                p.getmethod = SERVER_GetMethod;
                XRPC_RegisterProtocol(server, PROTOCOL, 
                    "Test Protocol", "slava", 1, 0, &p,NULL);

                /* Register our port number with the XRPC registry */
                id = XREG_RegisterService(reg, PROTOCOL, NULL, port, NULL, 0);
                if (id) {
                    printf("SERVER: XRPC Registry id=%s\n",id);

                    /* 
                     * Start the server. XRPC_ServerListenSocket will block 
                     * until the socket is closed. In our case, it blocks
                     * forever.
                     */
                    printf("SERVER: listening on port %hu\n",port);
                    XRPC_ServerListenSocket(server, SERVER_socket, XRpcFalse);
                    XREG_Free(id);
                }
                XRPC_FreeServer(server);
            }
            XREG_FreeRegistry(reg);
        } else {
            printf("SERVER: cannot create registry\n");
        }
    } else {
        printf("SERVER: cannot create socket\n");
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
