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
/* File    : U:\wpsvob\wmlenc\src\include\chenc.h
   Version : \main\7
   Date    : 17-Oct-00.12:50:22
*/

/*
 * ========================================================================
 *  Name        : chenc.h
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

    This header file is used internally by the WML encoder.

  
      Supported character sets and formats:
     
      Character set        Format      Mapping
      -------------        ------      -------
     
      ASCII                byte        no
     
      ISO-8859-(1...10)    byte        yes
      
      ISO/IEC10646-1 (UCS) UTF-8       no
                           UTF-16      no
                           UCS4        no
     
      User defined         byte/word   yes
      
      Note:
       - The UCS currently is identical with Unicode.
       - ASCII is covered by UCS/UTF-8 (default)
     
      Limitation:
       - It is an error, the document contains UCS
         character with 3 or 4 octet.
     
      The 'Character encodings'
      -----------------------------------
       (in REC-xml-19980210, Chapter 4.3.3)
     
       To specify a proper encoding we should specify the character
       set and the used format as well. However the 'character
       encoding' names implicitly specify both the format and
       the character set.
     
       The format of the character set ISO.8859-... is always one byte.
     
       The character set of the format UTF-8 or UTF-16 or UCS4 is 
       always 'UCS'.
     
       Using 'encoding names' we allow to refer to a character
       set and format with a single name. Such encoding names:
       ISO-8859-2, UTF8 etc. .
      
      Autodetection of Character Encodings
      ------------------------------------
       See: REC-xml-19980210, Appendix F.
  
-----------------------------------------------------------------------------
*/
#ifndef CHENC_H
#define CHENC_H

#include "chmap.h"
#include "wmlarray.h"

/*  CONSTANTS  */

/*  MACROS  */

/*  DATA TYPES  */
typedef struct SEncBuffers_s
{
  const SCharEncoding * m_enc;     /* source or destination */

  const BYTE   *    m_src;
  UINT32            m_srcSize;

  WCHAR        *    m_dst;
  UINT32            m_dstSize;
}SEncBuffers;

typedef struct SCCOut SCCOut;

typedef int (*FCharConverter)(SCCOut *cnv);

struct SCCOut {
    WCHAR                     m_char;
    FCharConverter            m_convert;
    PArray                    m_array;
    const SLIST(SCharNode)  * m_nodes;
    unsigned long             m_MIBenum;
};
/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

void initCharEncoding(void);
void termCharEncoding(void);

int convertToUnicode    (SEncBuffers *enc);


int constrSCCOut(SCCOut        *scc,
                 PArray         array,
                 const char    *encName);

void destrSCCOut(SCCOut *scc);

#endif  /* CHENC_H */

/*  End of File  */
