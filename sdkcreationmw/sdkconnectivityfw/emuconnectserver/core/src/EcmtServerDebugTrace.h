/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug trace for Ecmt server. Internal header file.
 *
*/

#ifndef _ECMT_SERVER_DEBUG_TRACE_H_
#define _ECMT_SERVER_DEBUG_TRACE_H_

#include "EcmtServerDebug.h"

#undef ASSERT

#ifdef ECMT_RDEBUG
#  include <f32file.h>
#  include <e32base.h>
#  include <e32debug.h>

#  define __S(x) _S(x)

#  if !defined(ECMT_TRACE_PREFIX) && !defined(ECMT_NO_TRACE_PREFIX)
#    define ECMT_TRACE_PREFIX "EcmtServer: "
#  endif // ECMT_TRACE_PREFIX

#  ifdef ECMT_TRACE_PREFIX
#    if defined(_UNICODE)
#      define ____L(x,y) (const TText*) L##x L##y
#    else  // _UNICODE
#      define ____L(x,y) x##y
#    endif // _UNICODE
#    define ___L(x,y) ____L(x,y)
#    define __L(z) ___L(ECMT_TRACE_PREFIX,z)
#  else
#    if defined(_UNICODE)
#      define ___L(x)  (const TText*) L##x
#    else  // _UNICODE
#      define ___L(x) x
#    endif // _UNICODE
#    define __L(z) ___L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)             CEcmtServerDebugTrace::Print(__L("%s"),_S(s))
#  define TRACE0(s)            CEcmtServerDebugTrace::Print(__L("%s"),_S(s))
#  define TRACE1(s,a)          CEcmtServerDebugTrace::Print(__L(s),a)
#  define TRACE2(s,a,b)        CEcmtServerDebugTrace::Print(__L(s),a,b)
#  define TRACE3(s,a,b,c)      CEcmtServerDebugTrace::Print(__L(s),a,b,c)
#  define TRACE4(s,a,b,c,d)    CEcmtServerDebugTrace::Print(__L(s),a,b,c,d)
#  define TRACE5(s,a,b,c,d,e)  CEcmtServerDebugTrace::Print(__L(s),a,b,c,d,e)
#  define TRACE_DEINIT()       CEcmtServerDebugTrace::Deinit()

#  define ASSERT(x) \
   ((void)((x)||(CEcmtServerDebugTrace::Assert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE(x) \
   CEcmtServerDebugTrace::Leave(x,_S(#x),__S(__FILE__),__LINE__)
#  define LEAVE_IF_ERROR(x) \
   CEcmtServerDebugTrace::LeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x)    ASSERT((x) == KErrNone)
#  define DEBUG_ONLY(x)        x

class CEcmtServerDebugTrace: public CBase
{
    class Truncate;
private:
    RFs iFs;
    RFile iLogFile;
    HBufC8* iBuf8;
    TPtr8 iBufPtr8;
    TInt iFlags;
    enum _TFlags {
        EFsConnected = 1,
        ELogFileOpen = 2
    };
private:
    CEcmtServerDebugTrace();
    ~CEcmtServerDebugTrace();
    void LogToFile(const TDesC* aString);
public:
    static void Deinit();
    static void Print(const TText* aFormat, ...);
    static void Assert(const TText* aExpr, const TText* aFile, TInt aLine);
    static void Leave(TInt err,const TText* s,const TText* aFile,TInt aLine);
    static void LeaveIfError(TInt err,const TText* s,const TText* f,TInt l);
};

#else  /* !ECMT_RDEBUG */

#  define TRACE(s)             ((void)0)
#  define TRACE0(s)            ((void)0)
#  define TRACE1(s,a)          ((void)0)
#  define TRACE2(s,a,b)        ((void)0)
#  define TRACE3(s,a,b,c)      ((void)0)
#  define TRACE4(s,a,b,c,d)    ((void)0)
#  define TRACE5(s,a,b,c,d,e)  ((void)0)
#  define ASSERT(x)            ((void)0)
#  define LEAVE(x)             User::Leave(x)
#  define LEAVE_IF_ERROR(x)    User::LeaveIfError(x)
#  define VERIFY_SUCCESS(x)    (x)
#  define DEBUG_ONLY(x)
#  define TRACE_DEINIT()       ((void)0)

#endif /* !ECMT_RDEBUG */


#endif // _ECMT_SERVER_DEBUG_TRACE_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
