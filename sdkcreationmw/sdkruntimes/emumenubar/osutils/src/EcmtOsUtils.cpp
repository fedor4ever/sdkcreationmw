/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Windows OS specific utilities
*
*/


// INCLUDE FILES

#include <emulator.h>
#include <tchar.h>
#include "EcmtOsUtils.h"

// CODEWARRIOR 3.0/3.1 COMPATIBILITY

#ifndef _tcsrchr
#  ifdef UNICODE
#    define _tcsrchr  wcsrchr
#  else  /* !UNICODE */
#    define _tcsrchr  strrchr
#  endif /* !UNICODE */
#endif /* _tcsrchr */

// MACROS

#define COUNT(a) (sizeof(a)/sizeof((a)[0]))

// STATIC VARIABLES

/// Full Windows path of epocroot directory.
static TCHAR gEpocroot[MAX_PATH] = {0};

/// Full Windows path of epoc32\\data directory.
static TCHAR gDataDir[MAX_PATH] = {0};

/// Full Windows path of epoc32\\tools\\ecmt directory.
static TCHAR gEcmtDir[MAX_PATH] = {0};

/// Full Windows path of system's temp directory
static TCHAR gTempDir[MAX_PATH] = {0};

/// In launcher mode if TRUE.
static int gLauncherMode = -1;

/**
 * Returns full Windows path of Epocroot directory.
 */
const unsigned short* EcmtOsUtils::Epocroot( )
{
    if ( gEpocroot[0] == TCHAR(0) )
    {
        // Find the last occurrence of epoc32\ in the file name
        GetModuleFileName( NULL, gEpocroot, COUNT(gEpocroot) );
        TCHAR* epoc32 = _tcsstr( _tcslwr( gEpocroot ), TEXT( "epoc32\\" ) );
        TCHAR* p = NULL;
        while (epoc32)
        {
            p = epoc32;
            epoc32 = _tcsstr( p+7, TEXT( "epoc32\\" ) );
        }
        if (p)
        {
            *p = TCHAR(0);
        }
        else
        {
            gEpocroot[0] = '\\';
        }
    }
    return gEpocroot;
}

/**
 * Returns full path of Ecmt directory (epoc32\\tools\\ecmt).
 */
const unsigned short* EcmtOsUtils::EcmtDir( )
{
    if (!gEcmtDir[0])
    {
        _tcscpy( gEcmtDir, Epocroot() );
        _tcscat( gEcmtDir, TEXT( "epoc32\\tools\\ecmt" ) );
    }
    return gEcmtDir;
}

/**
 * Returns full path of the data directory (epoc32\\data).
 */
const unsigned short* EcmtOsUtils::DataDir( )
{
    if (!gDataDir[0])
    {
        _tcscpy( gDataDir, Epocroot() );
        _tcscat( gDataDir, TEXT( "epoc32\\data" ) );
    }
    return gDataDir;
}

/**
 * Return full Windows path of system's temp directory 
 * (c:\\docume~1\\username\\locals~1\\Temp).
 */
const unsigned short* EcmtOsUtils::TempDir( )
{
    if ( !gTempDir[0])
    {
        GetTempPath( COUNT(gTempDir), gTempDir );
    }
    return gTempDir;
}

/**
 * Launches a new native Windows process.
 * @param aCmdLine Command line that starts the new process. Cannot 
 * be const because Windows CreateProcess does not accept a const 
 * command line.
 *
 * @param aWorkDir Working directory for the new process.
 * @param aFlags Flags affecting new process.
 * @return 1 if process creation succeeded, 0 otherwise.
 */
BOOL EcmtOsUtils::Launch( TCHAR* aCmdLine, const TCHAR* aWorkDir, 
                          TEcmtLaunchFlags aFlags,
                          PROCESS_INFORMATION* aPinfo)
{
    DWORD flags = aFlags & EShowConsole ? 0 : CREATE_NO_WINDOW;

    STARTUPINFO startup;
    ZeroMemory( &startup, sizeof( startup ) );
    startup.cb = sizeof( startup );
    
    PROCESS_INFORMATION pInfo;
    BOOL retval = CreateProcess( NULL, aCmdLine, NULL, NULL, 0, flags,
                                 NULL, aWorkDir, &startup, &pInfo );
    if (retval)
    {
        if (aPinfo) *aPinfo = pInfo;
        if (!(aFlags & EKeepHandlesOpen))
        {
            // Close the process and thread handles since we do not need them
            CloseHandle( pInfo.hProcess );
            CloseHandle( pInfo.hThread );
        }
    }
    else if (aPinfo)
    {
        ZeroMemory(aPinfo, sizeof(*aPinfo));
    }
    return retval;
}

/**
 * Returns whether we are in launcher mode or normal mode.
 * In launcher mode only a small "Wait Window" is displayed, then
 * only the Ecmt and Epdt cores are started and the Preferences dialog
 * is displayed. This mode is needed for the UEI prefs.exe command.
 * @return 1 if in launcher mode, 0 otherwise.
 */
BOOL EcmtOsUtils::LauncherMode()
{
    if (gLauncherMode < 0)
    {
        TCHAR exeName[MAX_PATH+1];
        LPCTSTR fname;
        exeName[0] = 0;
        GetModuleFileName( NULL, exeName, COUNT(exeName) );
        exeName[MAX_PATH] = 0;
        fname = _tcsrchr(exeName, TEXT('\\'));
        if (fname && _tcsstr(_tcslwr(exeName), TEXT("ecmtlauncher.exe")))
        {
            gLauncherMode = TRUE;
        }
        else
        {
            gLauncherMode = FALSE;
        }
    }
    return gLauncherMode;
}

/**
 * Allocates the requested number of bytes. Sets the allocated memory
 * to zero. This memory allocator work from any context, Symbian thread
 * or not. The pointers returned by this function must be deallocated
 * with Free.
 */
void* EcmtOsUtils::Alloc(unsigned int aSize)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, aSize);
}

/**
 * Deallocates the memory block allocated by the Alloc function.
 * NULL pointer is ignored.
 */
void EcmtOsUtils::Free(void* aPtr)
{
    if (aPtr)
    {
        HeapFree(GetProcessHeap(), 0, aPtr);
    }
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
