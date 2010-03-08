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
* Description:  All Windows header files are included from here
 *
*/


#ifndef _ECMTMENUBAR_WIN32_H_
#define _ECMTMENUBAR_WIN32_H_

#ifdef __WINS__

#ifndef _UNICODE
#  define _UNICODE
#endif // _UNICODE

#ifndef UNICODE
#  define UNICODE
#endif //  UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commdlg.h>
#include <tchar.h>

#ifndef _tcsrchr
#  ifdef UNICODE
#    define _tcsrchr  wcsrchr
#  else  /* !UNICODE */
#    define _tcsrchr  strrchr
#  endif /* !UNICODE */
#endif /* _tcsrchr */

#include "EcmtNmitShared.h"
    
#endif // __WINS__

#endif // _ECMTMENUBAR_WIN32_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
