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


#ifndef W32BEARER_DEBUG_H
#define W32BEARER_DEBUG_H

#include <e32std.h>
#include <e32debug.h>

// Usually, debug trace appears in the debug build and does not appear in
// the release build. However, we ship the debug build with the SDK, which
// essentially means that debug build is the release build, therefore we
// don't have a debug build :)
//
// You have to uncomment the following line to enable debug trace:

//#define _W32SOCKET_DEBUG

#define __S(x) _S(x)
#undef ASSERT

#ifdef _W32SOCKET_DEBUG

#  ifdef TRACE_PREFIX
#    if defined(_UNICODE)
#      define ___L(a,b) _L(a L##b)
#    else  // _UNICODE
#      define ___L(a,b) _L(a##b)
#    endif // _UNICODE
#    define __L(z) ___L(TRACE_PREFIX,z)
#  else
#    define __L(z) _L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)                  RDebug::Print(__L("%s"),_S(s))
#  define TRACE0(s)                 RDebug::Print(__L("%s"),_S(s))
#  define TRACE1(s,p1)              RDebug::Print(__L(s),p1)
#  define TRACE2(s,p1,p2)           RDebug::Print(__L(s),p1,p2)
#  define TRACE3(s,p1,p2,p3)        RDebug::Print(__L(s),p1,p2,p3)
#  define TRACE4(s,p1,p2,p3,p4)     RDebug::Print(__L(s),p1,p2,p3,p4)
#  define TRACE5(s,p1,p2,p3,p4,p5)  RDebug::Print(__L(s),p1,p2,p3,p4,p5)

#  define ASSERT(x) \
   ((void)((x)||(CW32SocketDebug::Assert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE_IF_ERROR(x) \
   CW32SocketDebug::LeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x)         ASSERT((x) == KErrNone)

#  define DEBUG_ONLY(x)             x

class CW32SocketDebug
{
public:
    static void Assert(const TText* aExpr, const TText* aFile, TInt aLine);
    static void LeaveIfError(TInt aError, const TText* aExpr,
                             const TText* aFile, TInt aLine);
};

#else  /* !_W32SOCKET_DEBUG */

#  define TRACE(s)                  ((void)0)
#  define TRACE0(s)                 ((void)0)
#  define TRACE1(s,p1)              ((void)0)
#  define TRACE2(s,p1,p2)           ((void)0)
#  define TRACE3(s,p1,p2,p3)        ((void)0)
#  define TRACE4(s,p1,p2,p3,p4)     ((void)0)
#  define TRACE5(s,p1,p2,p3,p4,p5)  ((void)0)
#  define ASSERT(x)                 ((void)0)
#  define VERIFY_SUCCESS(x)         (x)
#  define LEAVE_IF_ERROR(x)         User::LeaveIfError(x)
#  define DEBUG_ONLY(x)

#endif /* !_W32SOCKET_DEBUG */

#endif  // W32BEARER_DEBUG_H

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
