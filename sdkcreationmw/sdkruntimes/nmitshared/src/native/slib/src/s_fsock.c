/*
 * $Id: s_fsock.c,v 1.9 2005/02/20 20:31:29 slava Exp $
 *
 * Copyright (C) 2001-2005 by Slava Monich
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

#include "s_util.h"
#include "s_fio.h"
#include "s_mem.h"

/*==========================================================================*
 *              P L A I N     S O C K E T    I O
 *==========================================================================*/

#undef SocketFile
typedef struct _SocketFile {
    File file;      /* shared File structure */
    Socket sock;    /* the network socket */
    Bool eof;       /* True if we know that connection down */
} SocketFile;

STATIC SocketFile * SocketFileCast(File * f)
{
    ASSERT(f);
    if (f) {
        ASSERT(f->io == &SocketIO);
        if (f->io == &SocketIO) {
            return CAST(f,SocketFile,file);
        }
    }
    return NULL;
}

/** NOTE: both IP address and port are in host byte order */
STATIC File * SocketOpen2(IPaddr addr, Port port)
{
    Socket sock = INVALID_SOCKET;
    if (SOCKET_GetTcp(0,&sock)) {
        if (SOCKET_Connect(sock, addr, port)) {
            SocketFile * s = MEM_New(SocketFile);
            if (s) {
                Bool ok;
                StrBuf32 nameBuf;
                STRBUF_InitBufXXX(&nameBuf);
                STRBUF_Format(&nameBuf.sb, TEXT(IPADDR_FORMAT)TEXT_(":%hu"),
                    HOST_IPADDR_FORMAT_ARG(addr),port);

                memset(s, 0, sizeof(*s));
                ok = FILE_Init(&s->file, nameBuf.sb.s, False, &SocketIO);
                STRBUF_Destroy(&nameBuf.sb);

                if (ok) {
                    s->sock = sock;
                    s->eof = False;
                    return &s->file;
                }
            }            
            shutdown(sock, SHUT_RDWR);
        }
        closesocket(sock);
    }
    return NULL;
}

STATIC File * SocketOpen(Str hostname, const char * port)
{
    /* 
     * parse the port number and resolve the hostname and then call the
     * basic handler which deal with numeric port and address.
     */
    IPaddr addr;
    int p = atoi(port);
    if (p > 0 && p <= USHRT_MAX && INET_ResolveAddr(hostname,&addr)) {
        return SocketOpen2(addr, (Port)p);
    }
    return NULL;
}

STATIC Bool SocketReopen(File * f, Str hostname, const char * port)
{
    SocketFile * s  = SocketFileCast(f);
    if (s) {
        File * f2;
        shutdown(s->sock, SHUT_RDWR);
        closesocket(s->sock);
        f2 = SocketOpen(hostname, port);
        if (f2) {
            File save = *f;
            SocketFile * s2 = SocketFileCast(f2);
            *s = *s2;
            *f = save;
            MEM_Free(s2);
            return True;
        }
    }
    return False;
}

STATIC int SocketRead(File * f, void * buf, int len)
{
    int nbytes = -1;
    SocketFile * s  = SocketFileCast(f);
    if (s) {
        nbytes = recv(s->sock, (char*)buf, len, 0);
        if (nbytes < 0) s->eof = True;
    }
    return nbytes;
}

STATIC int SocketWrite(File * f, const void * buf, int len)
{
    int nbytes = -1;
    SocketFile * s  = SocketFileCast(f);
    if (s) {
        nbytes = send(s->sock, (char*)buf, len, 0);
        if (nbytes < 0) s->eof = True;
    }
    return nbytes;
}

STATIC Bool SocketEof(File * f)
{
    SocketFile * s  = SocketFileCast(f);
    return (s ? s->eof : True);
}

STATIC Bool SocketFlush(File * f)
{
    return BoolValue(!SocketEof(f));
}

STATIC void SocketDetach(File * f)
{
    SocketFile * s  = SocketFileCast(f);
    if (s) s->sock = INVALID_SOCKET;
}

STATIC void SocketClose(File * f)
{
    SocketFile * s  = SocketFileCast(f);
    if (s) {
        shutdown(s->sock, SHUT_RDWR);
        closesocket(s->sock);
        s->sock = INVALID_SOCKET;
    }
}

STATIC void SocketFree(File * f)
{
    SocketFile * s  = SocketFileCast(f);
    if (s) {
        ASSERT(!(f->flags & FILE_IS_OPEN));
        MEM_Free(s);
    }
}

/**
 * This is an exported function that associates a "socket file" with an
 * open socket. F this call fails, the caller is responsible for closing 
 * the socket. If this cal succeeds, then the socket will be closed by 
 * FILE_Close() 
 */
File * FILE_AttachToSocket(Socket sock)
{
    SocketFile * sf = MEM_New(SocketFile);
    if (sf) {
        memset(sf, 0, sizeof(*sf));
        sf->sock = sock;
        sf->eof = False;
        if (FILE_Init(&sf->file, TEXT("socket"), True, &SocketIO)) {
            return &sf->file;
        }
        MEM_Free(sf);
    }
    return NULL;
}

/**
 * Connects to the specified IP address 
 * NOTE: both IP address and port are in host byte order 
 */
File * FILE_Connect(IPaddr addr, Port port)
{
    return SocketOpen2(addr, port);
}

/*
 * A set of handlers that perform file I/O
 */
const FileIO SocketIO = {
    False               /* fileIO   */,
    SocketOpen          /* open     */,
    SocketReopen        /* reopen   */,
    NULL                /* setparam */,
    SocketRead          /* read     */,
    SocketWrite         /* write    */,
    SocketFlush         /* flush    */,
    SocketEof           /* eof      */,
    NULL                /* target   */,
    SocketDetach        /* detach   */,
    SocketClose         /* close    */,
    SocketFree          /* free     */
};
