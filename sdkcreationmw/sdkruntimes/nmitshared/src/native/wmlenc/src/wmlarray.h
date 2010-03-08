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
/* File    : U:\wpsvob\wmlenc\src\include\wmlarray.h
   Version : \main\6
   Date    : 17-Oct-00.12:50:32
*/

/*
 * ========================================================================
 *  Name        : wmlarray.h
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
-----------------------------------------------------------------------------
*/

#ifndef WMLARRAY_H
#define WMLARRAY_H

#include "xmlhost.h"

/*  CONSTANTS  */

/*  MACROS  */
#define ADD_TO_ARRAY(parr,val) {if((parr)->m_maxSize>(parr)->m_size)\
                                  (parr)->m_data[(parr)->m_size++]=((BYTE)val);\
                                else addToArray(parr,(BYTE)val);}

/*  DATA TYPES  */

/* dynamic array inforamtion structure */
typedef struct SArray
{
  BYTE  * m_data;
  UINT32  m_size;
  UINT32  m_maxSize;
}SArray;

typedef SArray *PArray;


/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

void  constrArray(PArray array);

void  destrArray
                (PArray array);

void  addToArray
                (PArray array, BYTE data);


#endif  /* WMLARRAY_H */

/*  End of File  */
