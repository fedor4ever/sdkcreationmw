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
* Description:  Includes headers from NmitShared if they are available
 *
*/


#ifndef __ECMTMENUBAR_NMITSHARED_H__
#define __ECMTMENUBAR_NMITSHARED_H__ 1

#ifdef HAVE_SLIB

// We are using non-UNICODE build of SLIB
#  ifdef _UNICODE
#    define _UNICODE_WAS_DEFINED
#  endif // _UNICODE
#  undef _UNICODE
#  undef UNICODE
#  define Time STime
#  include "s_lib.h"
#  undef WIN32_LEAN_AND_MEAN // gets redefined in <emulator.h>
#  undef IsSpace
#  undef Time

#  ifdef HAVE_WLIB
#    include "w_lib.h"
#  endif // HAVE_WLIB

// Restore the UNICODE settings
#  ifdef _UNICODE_WAS_DEFINED
#    define _UNICODE
#    define UNICODE
#  endif // _UNICODE_WAS_DEFINED

// We are shipping debug winscw builds. Make sure that there's no
// debug trace in debug build for CodeWarrior. Debug trace is only
// vailable in debug Visual C build
#  undef TRACE
#  undef TRACE0
#  undef TRACE1
#  undef TRACE2
#  undef TRACE3
#  undef TRACE4
#  undef TRACE5
#  undef TRACE6
#  undef TRACE7

#  if defined(_DEBUG) && defined(NMIT_DEBUG_LIBS)
#    define TRACE(s)           DEBUG_Trace("%s",s)
#    define TRACE0(s)          DEBUG_Trace("%s",s)
#    define TRACE1(s,p1)       DEBUG_Trace(s,p1)
#    define TRACE2(s,p1,p2)    DEBUG_Trace(s,p1,p2)
#    define TRACE3(s,p1,p2,p3) DEBUG_Trace(s,p1,p2,p3)
#  else
#    define TRACE(s)           NOTHING
#    define TRACE0(s)          NOTHING
#    define TRACE1(s,p1)       NOTHING
#    define TRACE2(s,p1,p2)    NOTHING
#    define TRACE3(s,p1,p2,p3) NOTHING
#    undef ASSERT
#    undef VERIFY
#    define ASSERT(x)          NOTHING
#    define VERIFY(x)          (x)
#  endif


#else  // HAVE_SLIB
   // Useful macro defined in s_def.h
#  define COUNT(array) (sizeof(array)/sizeof(array[0]))
#  undef HAVE_WLIB  // wlib requires slib
#endif // HAVE_SLIB

#ifndef HAVE_WLIB
#  define RECT_Width(r) (((r)&&((r)->right>(r)->left))?((r)->right-(r)->left):0)
#  define RECT_Height(r) (((r)&&((r)->bottom>(r)->top))?((r)->bottom-(r)->top):0)
#endif // !HAVE_WLIB

#endif // __ECMTMENUBAR_NMITSHARED_H__
