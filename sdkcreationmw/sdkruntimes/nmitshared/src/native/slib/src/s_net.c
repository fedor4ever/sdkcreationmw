/*
 * $Id: s_net.c,v 1.8 2005/02/19 02:37:10 slava Exp $
 *
 * Copyright (C) 2000-2005 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "s_lib.h"

/* Unix ioctl definitions are needed by SOCKET_Block */
#ifdef _UNIX
#  ifdef __sun
#    define BSD_COMP /* to define FIONBIO on Solaris */
#  endif /* __sun */
#  include <sys/ioctl.h>
#endif /* _UNIX */

/**
 * Initializes all slib modules. Also initializes Winsock under Win32
 * which is why this function has been moved from s_lib.c into this
 * file (to avoid unnecessary linkage with winsock)
 */
void SLIB_Init()
{
#if defined(_WIN32) && ! defined(_NT_KERNEL)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1,1),&wsaData);
#endif /* _WIN32 && !_NT_KERNEL*/
    SLIB_InitModules();
}

/*==========================================================================*
 *              S O C K E T S
 *==========================================================================*/

/**
 * Creates new INET socket of specified type (datagram/stream)
 * Note that both port and IP address are assumed to be in host
 * byte order
 */
Bool SOCKET_Create(int type, IPaddr ipaddr, Port port, Socket * sock)
{
    ASSERT(sock);
    if (sock) {
        Socket s;
        *sock = INVALID_SOCKET;
        s = socket(AF_INET, type, 0);
        if (s == INVALID_SOCKET) {
            Error(TEXT("ERROR: cannot create socket, error %d\n"),
            SOCKET_GetLastError());
        } else {
            int err;
            struct sockaddr_in sa;

            /* initialize sockaddr_in */
            memset(&sa, 0, sizeof(sa));
            sa.sin_family = AF_INET;
            sa.sin_port = htons(port);
            if (ipaddr) sa.sin_addr.s_addr = htonl(ipaddr);

            /* bind to port */
            err = bind(s,(struct sockaddr*)&sa,sizeof(sa));
            if (err) {
                if (port) {
                    Error(TEXT("ERROR: cannot bind to port %hu, error %d\n"),
                          port, SOCKET_GetLastError());
                } else {
                    Error(TEXT("ERROR: cannot bind socket, error %d\n"),
                          SOCKET_GetLastError());
                }
                SOCKET_Close(s);
            } else {
                *sock = s;
                return True;
            }
        }
    }
    return False;
}

/**
 * Close the specified socket
 */
Bool SOCKET_Close(Socket sock)
{
    int err = closesocket(sock);
    if (err) {
        Warning(TEXT("WARNING: failed to close %d\n"),sock);
        return False;
    } else {
        return True;
    }
}

/**
 * Returns errno on UNIX, Win32 socket error code from Winsock
 */
int SOCKET_GetLastError()
{
#ifdef _WIN32
    return WSAGetLastError();
#else /* !_WIN32 */
    return errno;
#endif /* !_WIN32 */
}

/**
 * Connects to the specified IP address/port. 
 * Both port and IP address are in host byte order
 */
Bool SOCKET_Connect(Socket s, IPaddr ipaddr, Port port)
{
    int err;
    struct sockaddr_in sa;

    /* initialize sockaddr_in */
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(ipaddr);

    /* connect */
    err = connect(s,(struct sockaddr*)&sa,sizeof(sa));
    if (err) {
        Verbose(TEXT("ERROR: cannot connect to ") TEXT(IPADDR_FORMAT) 
                TEXT(":%hu\n"), HOST_IPADDR_FORMAT_ARG(ipaddr), port);   
        return False;
    } else {
        return True;
    }
}

/**
 * Sets blocking mode for the socket
 */
Bool SOCKET_Block(Socket s, Bool block)
{
    unsigned long arg = (!block);
    if (ioctlsocket(s, FIONBIO, &arg) >= 0) {
        return True;
    } else {
        ASSMSG1("ioctl(FIONBIO) failed, err %d",SOCKET_GetLastError());
        return False;
    }
}

/**
 * Waits for something to happen with the socket. Returns the mask
 * which indicates what really happened, zero if select() timed out
 * and -1 in case of error
 */
int SOCKET_Wait(Socket s, int mask, Time timeout)
{
    int nfd;
    struct timeval tval;
    struct timeval * tv = NULL;

    fd_set read_fd_set;
    fd_set write_fd_set;
    fd_set except_fd_set;

    fd_set * readfs = NULL;
    fd_set * writefs = NULL;
    fd_set * exceptfs = NULL;

    /* at least bit mast be set */
    ASSERT(mask);
    if (mask & SOCK_WAIT_READ) {
        readfs = &read_fd_set;
        FD_ZERO(readfs);
        FD_SET(s,readfs);
    }
    if (mask & SOCK_WAIT_WRITE) {
        writefs = &write_fd_set;
        FD_ZERO(writefs);
        FD_SET(s,writefs);
    }
    if (mask & SOCK_WAIT_EXCEPT) {
        exceptfs = &except_fd_set;
        FD_ZERO(exceptfs);
        FD_SET(s,exceptfs);
    }

    /* convert milliseconds to seconds and microseconds */
    if (timeout) {
        tval.tv_sec = (long)(timeout/1000);
        tval.tv_usec = (long)((timeout%1000)*1000);
        tv = &tval;
    }

    /* actually wait */
    nfd = select(s+1, readfs, writefs, exceptfs, tv);
    if (nfd > 0) {
        int result = 0;
        if (readfs && FD_ISSET(s,readfs)) {
            result |= SOCK_WAIT_READ;
        }
        if (writefs && FD_ISSET(s,writefs)) {
            result |= SOCK_WAIT_WRITE;
        }
        if (exceptfs && FD_ISSET(s,exceptfs)) {
            result |= SOCK_WAIT_EXCEPT;
        }

        ASSERT(result);
        return result;
    }

    return nfd;
}

/**
 * This function resolves host name or dotted IP representation into 32 bit
 * binary IP address in host byte order. This function mostly deals with the
 * stupidity of Windoze implementation of gethostbyname() which, given an
 * IP address in standard dot notation, is unable to convert it into a 
 * binary form. 
 */
Bool INET_ResolveAddr(Str s, IPaddr * addr)
{
    Bool ok = False;
    if (s) {
        while (*s && IsSpace(*s)) s++;
        if (*s) {
#ifdef UNICODE
            char * host = STRING_ToMultiByte(s);
            if (host) {
#else  /* UNICODE */
                const char * host = s;
#endif /* UNICODE */
                IPaddr tmp = inet_addr(host);
                if (tmp == INADDR_NONE) {
                    struct hostent * h = gethostbyname(host);
                    if (h && h->h_addr_list[0]) {
                        tmp = *((IPaddr*)h->h_addr_list[0]);
                        ok = True;
                    }
                } else {
                    ok = True;
                }
                if (ok && addr) *addr = ntohl(tmp);
#ifdef UNICODE
                MEM_Free(host);
            }
#endif /* UNICODE */
        }
    }
    return ok;
}
