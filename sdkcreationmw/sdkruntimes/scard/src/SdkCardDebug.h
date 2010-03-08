/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef _SDK_CARDDEBUG_H_
#define _SDK_CARDDEBUG_H_

// Defines RDebug:
#include <e32svr.h>

// files that redefine ASSERT macro
#include <e32def.h>
#include <vwsdef.h>
#include <mda/common/base.h>

#define N(x) ((TInt)(sizeof(x)/sizeof((x)[0])))

//===========================================================================
// Debug trace
//===========================================================================

#ifdef _REALLY_DEBUG

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

#  define DEBUG_ONLY(x)             x

#else  /* !_REALLY_DEBUG */

#  define TRACE(s)                  ((void)0)
#  define TRACE0(s)                 ((void)0)
#  define TRACE1(s,p1)              ((void)0)
#  define TRACE2(s,p1,p2)           ((void)0)
#  define TRACE3(s,p1,p2,p3)        ((void)0)
#  define TRACE4(s,p1,p2,p3,p4)     ((void)0)
#  define TRACE5(s,p1,p2,p3,p4,p5)  ((void)0)

#  define DEBUG_ONLY(x)

#endif /* !_REALLY_DEBUG */

//===========================================================================
// Debug assert
//===========================================================================

#undef ASSERT
#ifdef _REALLY_DEBUG
void SdkCardAssert(const TText* s,const TText* file,TInt line);
TInt SdkCardAssertSuccess(TInt err,const TText* s,const TText* file,TInt line);
void SdkCardLeave(TInt err,const TText* s,const TText* file,TInt line);
void SdkCardLeaveIfNull(void*,const TText* s,const TText* file,TInt line);
void SdkCardLeaveIfError(TInt err,const TText* s,const TText* file,TInt line);
#  define __S(x) _S(x)
#  define ASSERT(x) ((void)((x)||(SdkCardAssert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE(x) SdkCardLeave(x,_S(#x),__S(__FILE__),__LINE__)
#  define LEAVE_IF_NULL(x) SdkCardLeaveIfNull(x,_S(#x),__S(__FILE__),__LINE__)
#  define LEAVE_IF_ERROR(x) SdkCardLeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define ASSERT_SUCCESS(x) SdkCardAssertSuccess(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x) ASSERT_SUCCESS(x)
#else  // _REALLY_DEBUG
#  define ASSERT(x)         ((void)0)
#  define LEAVE(x)          User::Leave(x)
#  define LEAVE_IF_NULL(x)  User::LeaveIfNull(x)
#  define LEAVE_IF_ERROR(x) User::LeaveIfError(x)
#  define ASSERT_SUCCESS(x) ((void)0)
#  define VERIFY_SUCCESS(x) (x)
#endif // _REALLY_DEBUG

#endif // _SDK_CARDDEBUG_H_
