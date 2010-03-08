/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/* File    : U:\wpsvob\wmlenc\src\include\xmlbuf.h
   Version : \main\7
   Date    : 17-Oct-00.12:51:41
*/

/*
 * ========================================================================
 *  Name        : xmlbuf.h
 *  Part of     : 
 *  Description : 
 *  Version     : 
 *
 *  Copyright (c) 2000 - 2005 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ========================================================================
 */

/*
-----------------------------------------------------------------------------

    DESCRIPTION

    This header file contains the interface to the XML parser.

-----------------------------------------------------------------------------
*/

#ifndef XMLBUF_H
#define XMLBUF_H

#include "xmlapi.h"
#include "wmlapi.h"

/*  CONSTANTS  */

/*  MACROS  */

/*  DATA TYPES  */

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

WML_STATUS     initBufferCache
                (WML_URLLOAD         download,
                 WML_DTD_DESCRIPTOR *desc[]);

void    termBufferCache
                (void);

void    addBufferToCache
                (PString        pubId,
                 const WCHAR  * buffer,
                 UINT32         nWChar);

int     getBuffer
                (PString        pubId,
                 const WCHAR ** buffer,
                 UINT32       * nWChar);

void    releaseBuffer
                (int bufferId);

WML_STATUS  downLoadExternal
                (SExtId       *   extId,
                 const BYTE   *   loc,
                 WCHAR        **  buffer,     /* Out: Unicode buffer   */
                 UINT32       *   nChar);     /* Out: number of chars  */

#endif  /* XMLBUF_H */

/*  End of File  */
