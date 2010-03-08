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
* Description:  XRPC_GetLogonID, XRPC_Malloc and XRPC_Free functions
 *
*/


#include "x_int.h"

/* *************************************************************************
//
//  Sometimes, XRPC_GetLogonID is used by the executables that don't need
//  full XRPC functionality. For example, if XRPC is implemented in Java
//  code, it may need logon id, which is not accessible from Java. In that
//  case, it's passed in as a system option from the native launcher code.
//  The only two functions such a launcher needs from the XRPC library are
//  XRPC_GetLogonID and XRPC_Free. Moving these functions into a separate
//  file cuts unnecessary dependencies.
//
// *************************************************************************/

/**
 * Allocates memory using XRPC memory allocator.
 */
void * XRPC_Malloc(size_t size)
{
    return MEM_Alloc(size);
}

/**
 * Deallocates memory allocated with XRPC_Malloc
 * This functions ignores NULL pointers.
 */
void XRPC_Free(void * ptr)
{
    MEM_Free(ptr);
}

#ifdef _WIN32
/**
 * Obtains current logon id. These numbers will be different for different
 * login session on a Windows Terminal Server
 */
STATIC Bool XRPC_Win32LogonId(LUID * logonId)
{
    OSVERSIONINFO osVersion;    
    if (logonId) memset(logonId, 0, sizeof(*logonId));
    osVersion.dwOSVersionInfoSize = sizeof(osVersion);
    if (GetVersionEx(&osVersion)) {
        HANDLE processToken = NULL;

        /* if this is Win9x, return zero */
        if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
            return True;
        }
    
        /* now we know we are running on Windows NT, get down to business */
        if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&processToken)) {
            TOKEN_STATISTICS tokenInfo;
            ULONG returnedLength = 0;
            memset(&tokenInfo, 0, sizeof(tokenInfo));
        
            if (GetTokenInformation(processToken, TokenStatistics, &tokenInfo, 
                sizeof(tokenInfo), &returnedLength)) {

                ASSERT(returnedLength == sizeof(tokenInfo));
                TRACE2("logon id: %08lX%08lX\n",
                    tokenInfo.AuthenticationId.HighPart,
                    tokenInfo.AuthenticationId.LowPart);
            
                /* copy logon id to the caller's buffer */
                if (logonId) *logonId = tokenInfo.AuthenticationId;
                return True;
            } else {
                ASSMSG1("GetTokenInformation err %d\n",GetLastError());
            }
        } else {
            ASSMSG1("OpenProcessToken err %d\n",GetLastError());
        }
    }
    return False;
}
#endif /* _WIN32 */

/**
 * Returns a string that more or less uniquely identifies a user's logon
 * session. This function is not being used by the XRPC library itself,
 * but it's the recommended way for the parties communicating via XRPC
 * to find out whether they are running in context of the same login 
 * session (whether they are running on the same machine, can be 
 * determined by comparing local/remote addresses of the socket).
 * The returned string has to be deallocated with XRPC_Free when 
 * it's no longer needed. The contents of the returned string is
 * very platform specific.
 */
char * XRPC_API XRPC_GetLogonID()
{
    StrBuf32 buf;
    char * id;

#ifdef _WIN32
    
    /* use logon ID on Windows NT/2k/XP */
    LUID logonId;
    STRBUF_InitBufXXX(&buf);
    if (XRPC_Win32LogonId(&logonId)) {
        STRBUF_Format(&buf.sb,"%08lX%08lX",logonId.HighPart,logonId.LowPart);
    }

#else /* assume Unix */

    /* use a combination of tty name and the DISPLAY environment variable */
    char * tty;
    char * display = getenv("DISPLAY");
    STRBUF_InitBufXXX(&buf);
    tty = ttyname(fileno(stdin));
    if (!tty) tty = ttyname(fileno(stdout));
    STRBUF_Format(&buf.sb, "%u",(unsigned int)getuid());
    if (tty) {
        STRBUF_Append(&buf.sb, ":");
        STRBUF_Append(&buf.sb, tty);
    }
    if (display) {
        STRBUF_Append(&buf.sb, ":");
        STRBUF_Append(&buf.sb, display);
    }

#endif /* !_WIN32 */

    /* copy the string */
    if (STRBUF_Length(&buf.sb) > 0) {
        id = STRING_Dup(STRBUF_Text(&buf.sb));
    } else {
        id = STRING_Dup("");
    }
    STRBUF_Destroy(&buf.sb);
    return id;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
