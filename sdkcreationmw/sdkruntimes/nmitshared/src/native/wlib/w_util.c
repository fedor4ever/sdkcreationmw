/*
 * $Id: w_util.c,v 1.17 2005/04/21 23:49:14 slava Exp $
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

#include "w_lib.h"

/*==========================================================================*
 * Some random numbers which is unlikely to interfere with any VER_PLATFORM
 * constant defined in WINBASE.H
 *==========================================================================*/

#define VER_PLATFORM_UNKNOWN    23524626
#define VER_PLATFORM_INVALID    56764787

/* VER_PLATFORM_WIN32_CE may be undefined */
#define  _VER_PLATFORM_WIN32_CE 3
#ifndef    VER_PLATFORM_WIN32_CE
#  define VER_PLATFORM_WIN32_CE _VER_PLATFORM_WIN32_CE
#else
/* ASSERT that our guess about the value of VER_PLATFORM_WIN32_CE was right */
COMPILE_ASSERT(VER_PLATFORM_WIN32_CE == _VER_PLATFORM_WIN32_CE)
#endif /* VER_PLATFORM_WIN32_CE */

STATIC ULONG win32_platform = VER_PLATFORM_UNKNOWN;
STATIC DWORD win32_verMajor = 0;
STATIC DWORD win32_verMinor = 0;
STATIC DWORD win32_build    = 0;

/*
 * use the A functions so that this code runs on Win9x as well as NT.
 * note that WinCE only supports W functions
 */
#ifndef _WIN32_WCE
#undef  GetWindowLong
#define GetWindowLong GetWindowLongA
#undef  SystemParametersInfo
#define SystemParametersInfo SystemParametersInfoA
#endif /* !_WIN32_WCE */

/*==========================================================================*
 *      P L A T F O R M    D E T E C T I O N
 *==========================================================================*/

/**
 * Returns the Windows platform ID. Should return one of the following:
 * VER_PLATFORM_WIN32s (very unlikely :-), VER_PLATFORM_WIN32_WINDOWS,
 * VER_PLATFORM_WIN32_NT or VER_PLATFORM_WIN32_CE
 */
ULONG WIN32_QueryPlatform()
{
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof(ver);
    ver.dwPlatformId = VER_PLATFORM_UNKNOWN;
    if (GetVersionEx(&ver)) {
        ASSERT(ver.dwPlatformId != VER_PLATFORM_UNKNOWN);

        /* keep some values returned by GetVersionEx in global variables */
        win32_verMajor = ver.dwMajorVersion;
        win32_verMinor = ver.dwMinorVersion;
        win32_platform = ver.dwPlatformId;

        /*
         * under Windows 95/98, the high-order word of OsVer.dwBuildNumber 
         * contains the major and minor version numbers which should be
         * the same as OsVer.dwMajorVersion and OsVer.dwMinorVersion. 
         * ASSERT that this is the case
         */
        if (ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
            ASSERT(HIBYTE(HIWORD(ver.dwBuildNumber))==ver.dwMajorVersion);
            ASSERT(LOBYTE(HIWORD(ver.dwBuildNumber))==ver.dwMinorVersion);
            win32_build = LOWORD(ver.dwBuildNumber);
        } else {
            win32_build = ver.dwBuildNumber;
        }

        return ver.dwPlatformId;
    }
    ASSERT(FALSE);
    return VER_PLATFORM_INVALID;
}

/**
 * Returns the Windows platform ID. Unlike WIN32_QueryPlatform(), this
 * function returns cached result because platform is unlikely to change
 * without a reboot :-)
 */
ULONG WIN32_GetPlatform() {
    if (win32_platform == VER_PLATFORM_UNKNOWN) {
        win32_platform = WIN32_QueryPlatform();
    }
    return win32_platform;
}


/**
 * Returns TRUE if we are running on Windows 95/98
 */
Bool WIN32_IsWin9x() {
    return (WIN32_GetPlatform() == VER_PLATFORM_WIN32_WINDOWS);
}

/**
 * Returns TRUE if we are running on Windows NT
 */
Bool WIN32_IsWinNT() {
    return (WIN32_GetPlatform() == VER_PLATFORM_WIN32_NT);
}


/**
 * Returns TRUE if we are running on Windows CE
 */
Bool WIN32_IsWinCE() {
    return (WIN32_GetPlatform() == VER_PLATFORM_WIN32_CE);
}

/**
 * Returns TRUE if this code is running on Windows 95, and FALSE if running 
 * or Windows 98, Windows NT, etc. This function should be used in the places
 * where there are substantial differences between Windows 95 and Windows 98
 */
Bool WIN32_IsWin95() {
    if (WIN32_GetPlatform() == VER_PLATFORM_WIN32_WINDOWS) {
        return (win32_verMajor == 4 && 
                win32_verMinor == 0 && 
                win32_build < 1998);
    }                
    return False;
}

/**
 * Returns TRUE if this code is running on Windows 98, and FALSE if running 
 * or Windows 95, Windows NT, etc. This function should be used in the places
 * where there are substantial differences between Windows 95 and Windows 98
 */
Bool XcIsWin98() {
    if (WIN32_GetPlatform() == VER_PLATFORM_WIN32_WINDOWS) {
        return (win32_verMajor == 4 && 
                win32_verMinor == 10 && 
                win32_build >= 1998);
    }                
    return False;
}

/*==========================================================================*
 *      S E C U R I T Y
 *==========================================================================*/

/**
 * This function checks if current user is a member of the local 
 * Administrators group (WinNT specific)
 */
Bool WIN32_IsAdmin()
{
    Bool isAdmin = False;

#ifndef _WIN32_WCE

    static SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    HANDLE hToken = NULL;

    /* This function is specific to WinNT. Don't call it on Win9x */
    ASSERT(WIN32_IsWinNT());

    /* Open process token */
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken)) {
        PTOKEN_GROUPS buffer = NULL;
        ULONG size = 0;

        /* fet size of the token information buffer */
        GetTokenInformation( hToken, TokenGroups, NULL, 0, &size);
        ASSERT(size > 0);

        /* allocate the buffer for GetTokenInformation  */
        buffer = (PTOKEN_GROUPS)MEM_Alloc(size);
        if (buffer) {
            
            /* call GetTokenInformation to actually get the info */
            memset(buffer, 0, size);
            if (GetTokenInformation(hToken,TokenGroups,buffer,size,&size)) {

                /* allocate SID to compare with */
                PSID psidAdmins = NULL;
                if (AllocateAndInitializeSid( &ntAuthority, 2,
                    SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                    0, 0, 0, 0, 0, 0,&psidAdmins)) {

                    /* look for the Admins SID */
                    ULONG i;
                    for (i=0; i<buffer->GroupCount; i++) {
                        if (EqualSid(buffer->Groups[i].Sid, psidAdmins)) {
                            isAdmin = True;
                            break;
                        }
                    }

                    FreeSid(psidAdmins);

                } else {
                    TRACE1("AllocateAndInitializeSid failed, err %lu\n",
                            GetLastError());
                }
            } else {
                TRACE1("GetTokenInformation failed, err %lu\n",
                        GetLastError());
            }

            MEM_Free(buffer);
        } else {
            TRACE1("OpenProcessToken failed, err %lu\n",
                    GetLastError());
        }
        CloseHandle(hToken);
    }

#endif /* _WIN32_WCE */

    return isAdmin;
}

Bool WIN32_LogonId(LUID * logonId)
{

#ifndef _WIN32_WCE

    HANDLE processToken = NULL;
    memset(logonId, 0, sizeof(*logonId));
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &processToken)) {
        TOKEN_STATISTICS tokenInfo;
        ULONG returnedLength = 0;
        memset(&tokenInfo, 0, sizeof(tokenInfo));

        if (GetTokenInformation(processToken, TokenStatistics, &tokenInfo, 
                                sizeof(tokenInfo), &returnedLength)) {

            ASSERT(returnedLength == sizeof(tokenInfo));
            TRACE2("Logon id: %08lX%08lX\n",
                tokenInfo.AuthenticationId.HighPart,
                tokenInfo.AuthenticationId.LowPart);

            *logonId = tokenInfo.AuthenticationId;
            return True;
        } else {
            ASSMSG1("GetTokenInformation() failed (%lu)\n",GetLastError());
        }
    } else {
        ASSMSG1("OpenProcessToken() failed (%lu)\n",GetLastError());
    }

#endif /* _WIN32_WCE */

    return False;
}

/*==========================================================================*
 *      R E S O U R C E S
 *==========================================================================*/

/**
 * Loads a string from the resources. Returns NULL if string is not defined
 * or memory allocation fails. Uses provided string buffer for storing the
 * string. In case of success, returns pointer to the StrBuf's buffer
 */
Str WIN32_LoadString(HMODULE hModule, UINT uID, StrBuf * sb) 
{
    /* we will multiply by 2 when we enter the loop...  divide by 2 now to 
     * avoid unnecessary reallocation */
    size_t maxlen = (sb->alloc ? ((sb->alloc-1)/2) : 0);
    do {
        STRBUF_Clear(sb);
        maxlen = MAX(2*maxlen,31);
        if (!STRBUF_Alloc(sb, maxlen)) {
            /* cannot allocate the memory */
            return NULL;
        }
        sb->len = LoadString(hModule, uID, sb->s, sb->alloc);
        ASSERT(sb->len < sb->alloc);
        ASSERT(!sb->s[sb->len]);
        if (!sb->len) {
            /* no such string */
            return NULL;
        }
    } while (sb->len >= maxlen);
    return STRBUF_GetString(sb);
}

/**
 * Formats a message using Win32 FormatMessage API. Loads format string 
 * from the resources. Returns NULL if either resource ID is invalid, or
 * FormatMessage fails. NOTE: FormatMessage uses different notation than
 * sprintf (i.e. %1!d! rather than %d)
 */
Str WIN32_FormatMessageVa(HMODULE hModule, UINT uID, StrBuf * sb, va_list va)
{
    Str str = NULL;
    Str format = WIN32_LoadString(hModule, uID, sb);
    if (format) {
#ifdef _USE_EXCEPTION_HANDLING
        TRY_CATCH {
#endif /*_USE_EXCEPTION_HANDLING */
            LPTSTR lpMsgBuf = NULL;
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                              FORMAT_MESSAGE_FROM_STRING,
                              format, 0, 0, (LPTSTR) &lpMsgBuf, 0, &va)) {

                if (lpMsgBuf) {
                    if (STRBUF_Copy(sb, lpMsgBuf)) {
                        str = STRBUF_GetString(sb);
                    }
                    LocalFree(lpMsgBuf);
                }
            } 
#ifdef _USE_EXCEPTION_HANDLING
        } CATCH {
            ASSMSG1("EXCEPTION %08lX (WIN32_FormatString)",GetExceptionCode());
            str = NULL;
        }
#endif /*_USE_EXCEPTION_HANDLING */
    }

    if (str) {
        return str;
    } else {
        STRBUF_Clear(sb);
        return NULL;
    }
}

/**
 * The same as WIN32_FormatMessageVa but takes variable number of arguments
 */
Str WIN32_FormatMessage(HMODULE hModule, UINT uFormatID, StrBuf * sb, ...)
{
    va_list va;
    va_start(va,sb);
    return WIN32_FormatMessageVa(hModule, uFormatID, sb, va);
}

/*==========================================================================*
 *      F I L E    U T I L I T I E S
 *==========================================================================*/

/**
 * Returns the module file name, using the specified buffer to actually store
 * the file name. Returns NULL in case of any problem, such as wrong module
 * handle or memory allocation failure.
 */
Str WIN32_GetModuleFileName(HMODULE hModule, StrBuf * sb)
{
    size_t maxlen = (sb->alloc ? ((sb->alloc-1)/2) : 0);
    do {
        STRBUF_Clear(sb);
        maxlen = MAX(2*maxlen,31);
        if (!STRBUF_Alloc(sb, maxlen)) {
            /* cannot allocate the memory */
            return NULL;
        }
        SetLastError(NO_ERROR);
        sb->len = GetModuleFileName(hModule, sb->s, maxlen);
        ASSERT(sb->len <= maxlen);
        sb->s[maxlen] = 0;
        ASSERT(!sb->s[sb->len]);
        if (!sb->len) {
            return NULL;    /* bad module handle, I guess */
        }
    } while (sb->len >= (maxlen-1));
    return STRBUF_GetString(sb);
}

/*==========================================================================*
 *      B R O W S E R
 *==========================================================================*/

/**
 * Opens the specified URL in the external browser.
 */
Bool WIN32_BrowseURL(Str url)
{
    Bool result = False;
    TRACE1("WLIB: request to browse %s\n",url);
    if (url) {
        HKEY hKeyRoot = NULL;
        char cmd[MAX_PATH];
        long len = COUNT(cmd);
        static const Str key = "http\\shell\\open\\command";
        int err = RegQueryValue(HKEY_CLASSES_ROOT,key,cmd,&len);
        ASSERT(err == ERROR_SUCCESS);
        if (err == NO_ERROR) {
            size_t i = 0;
            int percent;
            Bool quote = False;
            BOOL ok;
            STARTUPINFO startupInfo;
            PROCESS_INFORMATION processInfo;
            StrBuf128 cmdBuf;
            StrBuf128 exeBuf;
            STRBUF_InitBufXXX(&cmdBuf);
            STRBUF_InitBufXXX(&exeBuf);
            memset(&startupInfo, 0, sizeof(startupInfo));
            memset(&processInfo, 0, sizeof(processInfo));
            STRBUF_Copy(&cmdBuf.sb, cmd);
            TRACE1("WLIB: browser command is %s\n",STRBUF_Text(&cmdBuf.sb));
            while (i < cmdBuf.sb.len && IsSpace(cmdBuf.sb.s[i])) i++;
            if (i < cmdBuf.sb.len && STRBUF_CharAt(&cmdBuf.sb, i) == '"') {
                quote = True;
                i++;
            }
            while (i < cmdBuf.sb.len) {
                char c = STRBUF_CharAt(&cmdBuf.sb, i);
                if ((quote && c == '"') || (!quote && IsSpace(c))) {
                    break;
                }
                STRBUF_AppendChar(&exeBuf.sb, c);
                i++;
            }
            TRACE1("WLIB: browser is %s\n",STRBUF_Text(&exeBuf.sb));
            percent = STRBUF_IndexOf(&cmdBuf.sb, '%');
            if (percent > 0 && 
                percent < (int)(cmdBuf.sb.len-1) &&
                STRBUF_CharAt(&cmdBuf.sb, percent+1) == '1') {
                STRBUF_Erase(&cmdBuf.sb, percent, percent+2);
                STRBUF_Insert(&cmdBuf.sb, url, percent);
            } else {
                STRBUF_AppendChar(&cmdBuf.sb, ' ');
                STRBUF_Append(&cmdBuf.sb, url);
            }
            TRACE1("WLIB: command line is %s\n",STRBUF_Text(&cmdBuf.sb));
            ok = CreateProcess(exeBuf.sb.s, cmdBuf.sb.s, NULL, NULL, False, 0,
                NULL, NULL, &startupInfo, &processInfo);
            if (ok) {
                TRACE1("WLIB: started process %lu\n",processInfo.dwProcessId);
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
                result = True;
            } else {
                TRACE1("WLIB: CreateProcess error %u\n",GetLastError());
            }
            STRBUF_Destroy(&cmdBuf.sb);
            STRBUF_Destroy(&exeBuf.sb);
        }
    }
    return result;
}
