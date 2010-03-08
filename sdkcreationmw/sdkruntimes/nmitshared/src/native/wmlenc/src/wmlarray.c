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
/* File    : U:\wpsvob\wmlenc\src\common\wmlarray.c
   Version : \main\7
   Date    : 17-Oct-00.12:46:51
*/

/*
 * ========================================================================
 *  Name        : wmlarray.c
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

    1  ABSTRACT
        

    1.1 Module Type

    type  : subroutines

    1.2 Functional Description

        This module called as a last phase of WML encoding to create the
        binary output.


    1.3 Specification/Design Reference

        doc/wmlarch.doc

    1.4 Module Test Specification Reference

        none

    1.5 Compilation Information

        See: building.txt

    1.6 Notes

        None

*/



/*  2  CONTENTS

        1  ABSTRACT

        2  CONTENTS

        3  GLOSSARY

        4  EXTERNAL RESOURCES
        4.1  Include Files
        4.2  External Data Structures
        4.3  External Function Prototypes

        5  LOCAL CONSTANTS AND MACROS

        6  MODULE DATA STRUCTURES
        6.1  Local Data Structures
        6.2  Local Function Prototypes

        7  MODULE CODE
        7.1   constrArray
        7.2   destrArray
        7.3   addToArray

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "wmlarray.h"


/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

    /* None */


/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */



/*
=============================================================================
*/
/*  7.1  */
void constrArray(PArray array)
{
/*  Functional Description
 *
 *  Description: Creates the initial memory for a dynamic array, and sets
 *               the maximum size to its default. (4096 elements)
 *
 *  Output:
 *
 *  Returns:     array - points to the initialized array
 *                              
 
*---------------------------------------------------------------------------
 */
/*  Data Structures  */
/*  Code  */

  array->m_size = 0;
  array->m_maxSize = 4096;
  array->m_data = XML_ALLOC(array->m_maxSize);
}

/*
=============================================================================
*/
/*  7.2  */
void destrArray(PArray array)
{
/*  Functional Description
 *
 *  Description: Deletes a dynamic array.
 *
 *  Input:       array - points to an array to be deleted.
 *                              
 
*---------------------------------------------------------------------------
 */
/*  Data Structures  */
/*  Code  */
  if (array)
  {
    XML_FREE(array->m_data);
  }
}

/*
=============================================================================
*/
/*  7.3  */
void addToArray(PArray array, BYTE value)
{
/*  Functional Description
 *
 *  Description: Adds a new element into the array, and if it reaches its
 *               maximum size, reallocates the memory. The maximum size
 *               will be doubled.
 *
 *  Inputs:      array - points to the array
 *               data  - value to be stored
 *                              
 
*---------------------------------------------------------------------------
 */
/*  Data Structures  */

/*  Code  */
  if (array->m_size == array->m_maxSize)
  {
    BYTE *p = XML_ALLOC(array->m_maxSize *= 2);
    memcpy(p, array->m_data, array->m_size);
    XML_FREE(array->m_data);
    array->m_data = p;
  }

  array->m_data[array->m_size++] = value;
}

/*
=============================================================================
*/


/*  End of File  */
