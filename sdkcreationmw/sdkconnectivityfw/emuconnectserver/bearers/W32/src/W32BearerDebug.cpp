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
* Description:  Provides access to Win32 sockets from Symbian OS
 *
*/


#include "W32BearerDebug.h"

#ifdef _W32SOCKET_DEBUG

#ifdef EKA2
#  define BEGIN_WIN32() Emulator::Escape()
#  define END_WIN32() Emulator::Reenter()
#  include <emulator.h>
#else
#  define BEGIN_WIN32() ((void)0)
#  define END_WIN32() ((void)0)
#endif // EKA2

extern "C" int __stdcall IsDebuggerPresent(void);
void CW32SocketDebug::Assert(const TText* aExpr,const TText* aFile,TInt aLine)
{
    TRACE1("ASSERT FAILED: %s", aExpr);
    TRACE1("  File: %s", aFile);
    TRACE1("  Line: %d", aLine);
    BEGIN_WIN32();
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
    END_WIN32();
}

void CW32SocketDebug::LeaveIfError(TInt aError, const TText* aExpr,
                                   const TText* aFile, TInt aLine)
{
    if (aError != KErrNone)
    {
        TRACE1("LEAVE: %s", aExpr);
        TRACE1("  Error: %d", aError);
        TRACE1("  File: %s", aFile);
        TRACE1("  Line: %d", aLine);
        BEGIN_WIN32();
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
        END_WIN32();
        User::Leave(aError);
    }
}

#endif // _W32SOCKET_DEBUG

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
