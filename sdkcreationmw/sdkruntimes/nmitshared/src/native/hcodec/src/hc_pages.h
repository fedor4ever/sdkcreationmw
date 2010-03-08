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
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\hc_pages.h
   Version : \main\6
   Date    : 25-Feb-00.15:36:01
*/

/*
 * ========================================================================
 *  Name        : hc_pages.h
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

    Contains the Header Code Page Manager (HCPM) interface for 
    the WSP header codec.

-----------------------------------------------------------------------------
*/

#ifndef HC_Pages_h
#define HC_Pages_h

/*
-----------------------------------------------------------------------------
    Includes
-----------------------------------------------------------------------------
*/

#include "hc_util.h"

/*
-----------------------------------------------------------------------------
    Definitions
-----------------------------------------------------------------------------
*/

/* Predefined header code page numbers. */
#define WPS_HCP_DEFAULT          1  /* Default header code page. */
#define WPS_HCP_WAP_FIRST        1  /* First WAP header code page. */
#define WPS_HCP_WAP_LAST         1  /* Last WAP header code page. */

#define WPS_HCP_WAP_FIRST_RESERVED  2   /* First WAP reserved header code page. */
#define WPS_HCP_WAP_LAST_RESERVED   15  /* Last  WAP reserved header code page. */

#define WPS_HCP_SHORTCUT_MAX    31  /* Last header code page which can be referenced 
                                       by a shortcut-shift-delimiter. */

/* Predefined header field codes of the Default header code page. */
#define WPS_HFC_DEF_ACCEPT                  0x00
#define WPS_HFC_DEF_ACCEPT_CHARSET          0x01
#define WPS_HFC_DEF_ACCEPT_ENCODING         0x02
#define WPS_HFC_DEF_ACCEPT_LANGUAGE         0x03
#define WPS_HFC_DEF_ACCEPT_RANGES           0x04
#define WPS_HFC_DEF_AGE                     0x05
#define WPS_HFC_DEF_ALLOW                   0x06
#define WPS_HFC_DEF_AUTHORIZATION           0x07
#define WPS_HFC_DEF_CACHE_CONTROL           0x08
#define WPS_HFC_DEF_CONNECTION              0x09
#define WPS_HFC_DEF_CONTENT_BASE            0x0A
#define WPS_HFC_DEF_CONTENT_ENCODING        0x0B
#define WPS_HFC_DEF_CONTENT_LANGUAGE        0x0C
#define WPS_HFC_DEF_CONTENT_LENGTH          0x0D
#define WPS_HFC_DEF_CONTENT_LOCATION        0x0E
#define WPS_HFC_DEF_CONTENT_MD5             0x0F
#define WPS_HFC_DEF_CONTENT_RANGE           0x10
#define WPS_HFC_DEF_CONTENT_TYPE            0x11
#define WPS_HFC_DEF_DATE                    0x12
#define WPS_HFC_DEF_ETAG                    0x13
#define WPS_HFC_DEF_EXPIRES                 0x14
#define WPS_HFC_DEF_FROM                    0x15
#define WPS_HFC_DEF_HOST                    0x16
#define WPS_HFC_DEF_IF_MODIFIED_SINCE       0x17
#define WPS_HFC_DEF_IF_MATCH                0x18
#define WPS_HFC_DEF_IF_NONE_MATCH           0x19
#define WPS_HFC_DEF_IF_RANGE                0x1A
#define WPS_HFC_DEF_IF_UNMODIFIED_SINCE     0x1B
#define WPS_HFC_DEF_LOCATION                0x1C
#define WPS_HFC_DEF_LAST_MODIFIED           0x1D
#define WPS_HFC_DEF_MAX_FORWARDS            0x1E
#define WPS_HFC_DEF_PRAGMA                  0x1F
#define WPS_HFC_DEF_PROXY_AUTHENTICATE      0x20
#define WPS_HFC_DEF_PROXY_AUTHORIZATION     0x21
#define WPS_HFC_DEF_PUBLIC                  0x22
#define WPS_HFC_DEF_RANGE                   0x23
#define WPS_HFC_DEF_REFERER                 0x24
#define WPS_HFC_DEF_RETRY_AFTER             0x25
#define WPS_HFC_DEF_SERVER                  0x26
#define WPS_HFC_DEF_TRANSFER_ENCODING       0x27
#define WPS_HFC_DEF_UPGRADE                 0x28
#define WPS_HFC_DEF_USER_AGENT              0x29
#define WPS_HFC_DEF_VARY                    0x2A
#define WPS_HFC_DEF_VIA                     0x2B
#define WPS_HFC_DEF_WARNING                 0x2C
#define WPS_HFC_DEF_WWW_AUTHENTICATE        0x2D
#define WPS_HFC_DEF_CONTENT_DISPOSITION     0x2E

/* WAP 1.2 */
#define WPS_HFC_DEF_X_WAP_APPLICATION_ID    0x2F
#define WPS_HFC_DEF_X_WAP_CONTENT_URI       0x30
#define WPS_HFC_DEF_X_WAP_INITIATOR_URI     0x31
#define WPS_HFC_DEF_ACCEPT_APPLICATION      0x32
#define WPS_HFC_DEF_BEARER_INDICATION       0x33
#define WPS_HFC_DEF_PUSH_FLAG               0x34
#define WPS_HFC_DEF_PROFILE                 0x35
#define WPS_HFC_DEF_PROFILE_DIFF            0x36
#define WPS_HFC_DEF_PROFILE_WARNING         0x37

/* WAP 1.3 */
#define WPS_HFC_DEF_EXPECT                  0x38
#define WPS_HFC_DEF_TE                      0x39
#define WPS_HFC_DEF_TRAILER                 0x3A
#define WPS_HFC_DEF_ACCEPT_CHARSET13        0x3B
#define WPS_HFC_DEF_ACCEPT_ENCODING13       0x3C
#define WPS_HFC_DEF_CACHE_CONTROL13         0x3D
#define WPS_HFC_DEF_CONTENT_RANGE13         0x3E
#define WPS_HFC_DEF_X_WAP_TOD               0x3F
#define WPS_HFC_DEF_CONTENT_ID              0x40
#define WPS_HFC_DEF_SET_COOKIE              0x41
#define WPS_HFC_DEF_COOKIE                  0x42
#define WPS_HFC_ENCODING_VERSION            0x43

/*
-----------------------------------------------------------------------------
    Function declarations
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Looks for the header field code associated with the given field name. The 
    field code is returned in <pbValue> in case of success; 
    otherwise <pbValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_FieldName2FieldCode(
    int           nPage,    /* in:  Header code page number. */
    const char*   pcPage,   /* in:  Header code page name (used only if <nPage> is 0). */
    const char*   pcName,   /* in:  Header field name. */
    HC_WapVersion wapVer,   /* in:  WAP verson to comply with */
    BYTE*         pbValue); /* out: The corresponding header field code. */

/*
-----------------------------------------------------------------------------
    Looks for the header field name associated with the given field code.
    Returns the header field name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_FieldCode2FieldName(
    BYTE          bPage,    /* in:  Header code page number. */
    const char*   pcPage,   /* in:  Header code page name (used only if <bPage> is 0). */
    BYTE          bValue,   /* in:  Header field code. */
    HC_WapVersion wapVer);  /* in:  WAP verson to comply with */

#endif /* HC_Pages_h */
