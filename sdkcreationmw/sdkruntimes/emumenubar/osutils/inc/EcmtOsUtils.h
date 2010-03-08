/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  OS specific utilities
*
*/


#ifndef ECMTOSUTILS_H
#define ECMTOSUTILS_H

#ifdef __WINS__

/**
 * Let's keep this file platform-neutral. EcmtMenubar includes it from
 * the the files that don't include any Symbian headers. Define IMPORT_C 
 * and EXPORT_C macros here the same way as they are defined in e32def.h 
 */
#ifndef IMPORT_C
#  define IMPORT_C __declspec(dllexport)
#endif // IMPORT_C

#ifndef EXPORT_C
#  define EXPORT_C __declspec(dllexport)
#endif // EXPORT_C

/**
* Enum of flags for EcmtOsUtils::Launch member.
*/
enum TEcmtLaunchFlags
{
    /// Default value.
    ENoFlags = 0x0,
    /// Create and display the console window for the new process.
    EShowConsole = 0x1,
    /// Do not close the process handle so that it can be used in the
    /// Windows wait functions.
    EKeepHandlesOpen = 0x2
};

/**
 * Static class that offers to Symbian code some utility functions 
 * of the native OS.
 */
class EcmtOsUtils
{
public:
    // Use "unsigned short" for charactes and "int" for booleans
    // since they are defined in both Windows and Symbian 

    /**
     * Returns full path of Epocroot directory.
     */
    static IMPORT_C const unsigned short* Epocroot();

    /**
     * Returns full path of tyhe data directory (epoc32\\data).
     */
    static IMPORT_C const unsigned short* DataDir();

    /**
     * Returns full path of Ecmt directory (epoc32\\tools\\ecmt).
     */
    static IMPORT_C const unsigned short* EcmtDir();

    /**
     * Return full path of system's temp directory 
     * (c:\\docume~1\\username\\locals~1\\Temp).		
     */
    static IMPORT_C const unsigned short* TempDir();

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
    static IMPORT_C int Launch( unsigned short* aCmdLine, 
                                const unsigned short* aWorkDir, 
                                TEcmtLaunchFlags aFlags = ENoFlags,
                                struct _PROCESS_INFORMATION* aPinfo = NULL);

    /**
     * Returns whether we are in launcher mode or normal mode.
     * In launcher mode only a small "Wait Window" is displayed, then
     * only the Ecmt and Epdt cores are started and the Preferences dialog
     * is displayed. This mode is needed for the UEI prefs.exe command.
     * @return 1 if in launcher mode, 0 otherwise.
     */
    static IMPORT_C int LauncherMode();

    /**
     * Allocates the requested number of bytes. Sets the allocated memory
     * to zero. This memory allocator work from any context, Symbian thread
     * or not. The pointers returned by this function must be deallocated
     * with Free.
     */
    static IMPORT_C void* Alloc(unsigned int aSize);

    /**
     * Deallocates the memory block allocated by the Alloc function.
     * NULL pointer is ignored.
     */
    static IMPORT_C void Free(void* aPtr);
};
    
#endif	// __WINS__
#endif	// ECMTOSUTILS_H
