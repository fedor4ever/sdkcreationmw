/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug trace for MIDP Debug plugin
 *
*/

#ifndef _ECMT_MIDP_DEBUG_TRACE_H_
#define _ECMT_MIDP_DEBUG_TRACE_H_

#include <e32base.h>
#include <e32debug.h>

// For now, always enabled the trace, even on the handset
// Only compile release WINS build without the trace, just to make sure that
// there are compilation errors/warnings when trace is off
#if !defined(__WINS__) || defined(_DEBUG)
//#if defined(__WINS__) && defined(_DEBUG)
#  define _MIDPDEBUG_TRACE
#endif

#define __S(x) _S(x)
#undef ASSERT

#ifdef _MIDPDEBUG_TRACE

#  ifdef TRACE_PREFIX
#    if defined(_UNICODE)
#      define ____L(x,y) (const TText*) L##x##L##y
#    else  // _UNICODE
#      define ____L(x,y) x##y
#    endif // _UNICODE
#    define ___L(x,y) ____L(x,y)
#    define __L(z) ___L(TRACE_PREFIX,z)
#  else
#    if defined(_UNICODE)
#      define ___L(x)  (const TText*) L##x
#    else  // _UNICODE
#      define ___L(x) x
#    endif // _UNICODE
#    define __L(z) ___L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)             CEcmtMidpDebugTrace::Print(__L("%s"),_S(s))
#  define TRACE0(s)            CEcmtMidpDebugTrace::Print(__L("%s"),_S(s))
#  define TRACE1(s,a)          CEcmtMidpDebugTrace::Print(__L(s),a)
#  define TRACE2(s,a,b)        CEcmtMidpDebugTrace::Print(__L(s),a,b)
#  define TRACE3(s,a,b,c)      CEcmtMidpDebugTrace::Print(__L(s),a,b,c)
#  define TRACE4(s,a,b,c,d)    CEcmtMidpDebugTrace::Print(__L(s),a,b,c,d)
#  define TRACE5(s,a,b,c,d,e)  CEcmtMidpDebugTrace::Print(__L(s),a,b,c,d,e)
#  define TRACE_INIT(m)        CEcmtMidpDebugTrace::Init(m)
#  define TRACE_DEINIT()       CEcmtMidpDebugTrace::Deinit()

#  define ASSERT(x) \
   ((void)((x)||(CEcmtMidpDebugTrace::Assert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE_IF_ERROR(x) \
   CEcmtMidpDebugTrace::LeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x)    ASSERT((x) == KErrNone)

#  define DEBUG_ONLY(x)        x

class MEcmtMessaging;
class CEcmtMidpDebugTrace: public CBase
{
    class Truncate;
private:
    MEcmtMessaging* iMessaging;
private:
    CEcmtMidpDebugTrace(MEcmtMessaging* aMessaging);
    void EcmtPrint(const TDesC* aString);
public:
    static void Init(MEcmtMessaging* aMessaging);
    static void Deinit();
    static void Print(const TText* aFormat, ...);
    static void Assert(const TText* aExpr, const TText* aFile, TInt aLine);
    static void LeaveIfError(TInt aError, const TText* aExpr,
                             const TText* aFile, TInt aLine);
};

#else  /* !_MIDPDEBUG_TRACE */

#  define TRACE(s)             ((void)0)
#  define TRACE0(s)            ((void)0)
#  define TRACE1(s,a)          ((void)0)
#  define TRACE2(s,a,b)        ((void)0)
#  define TRACE3(s,a,b,c)      ((void)0)
#  define TRACE4(s,a,b,c,d)    ((void)0)
#  define TRACE5(s,a,b,c,d,e)  ((void)0)
#  define ASSERT(x)            ((void)0)
#  define VERIFY_SUCCESS(x)    (x)
#  define LEAVE_IF_ERROR(x)    User::LeaveIfError(x)
#  define DEBUG_ONLY(x)
#  define TRACE_INIT(m)        ((void)0)
#  define TRACE_DEINIT()       ((void)0)

#endif /* !_MIDPDEBUG_TRACE */


#endif // _ECMT_MIDP_DEBUG_TRACE_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
