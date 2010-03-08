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
/* File    : U:\wpsvob\wmlenc\src\common\xmlcomm.c
   Version : \main\6
   Date    : 17-Oct-00.12:48:39
*/

/*
 * ========================================================================
 *  Name        : xmlcomm.c
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

/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlstruc.h"

/*  4.2 External Data Structures  */

    /* None */

/*  4.3 External Function Prototypes  */

    /* None */

/*  5  LOCAL CONSTANTS AND MACROS  */

#ifdef INVARIANTS
#define INVARIANT_FUNC(func)    ((void(*)(void*))func)
#else
#define INVARIANT_FUNC(ignore)  0
#endif /* INVARIANTS */

#ifdef WML_DEBUG
#define DEBUG_FUNC(func)    ((void(*)(void*))func)
#else
#define DEBUG_FUNC(ignore)          0
#endif

/*  6  MODULE DATA STRUCTURES  */

    /* None */

/*  6.1 Local Data Structures  */

    /* None */

/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */

/*
 *===========================================================================
 */
/*  7.1  */
void constrCommonData(PCommon cd)
{
/*  Functional Description
 *
 *  Initialize pools, and symbol tables.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  /* Pools */
  constrPool(&(cd->m_strPool),  sizeof(struct String_s));
  constrPool(&(cd->m_snodePool),sizeof(StrNode));
  constrPool(&(cd->m_attrPool), sizeof(SAttribute));
 
  /* Symbol tables */
  constrSymTbl(&cd->m_GEntities,
               STRPOOL(cd),
               cd,
               (FConstructor)constrGEntity,
               (FDestructor)destrGEntity,
               INVARIANT_FUNC(invSGEntity),
               DEBUG_FUNC(dbgSGEntity),
               sizeof(SGEntity));
  constrSymTbl(&cd->m_Notations,
               STRPOOL(cd),
               cd,
               (FConstructor)constrNotation,
               (FDestructor)destrNotation,
               INVARIANT_FUNC(invSNotation),
               DEBUG_FUNC(dbgSNotation),
               sizeof(SNotation));
  constrSymTbl(&cd->m_PEntities,
               STRPOOL(cd),
               cd,
               (FConstructor)constrPEntity,
               (FDestructor)destrPEntity,
               INVARIANT_FUNC(invSPEntity),
               DEBUG_FUNC(dbgSPEntity),
               sizeof(SPEntity));
  constrSymTbl(&cd->m_Elements,
               STRPOOL(cd),
               cd,
               (FConstructor)constrElement,
               (FDestructor)destrElement,
               INVARIANT_FUNC(invSElement),
               DEBUG_FUNC(dbgSElement),
               sizeof(SElement));
  constrSymTbl(&cd->m_IDs,
               STRPOOL(cd),
               cd,
               (FConstructor)0,
               (FDestructor)0,
               0,
               0,
               0);
}

/*
 *===========================================================================
 */
/*  7.2  */
void destrCommonData(PCommon cd)
{
/*  Functional Description
 *
 *  Deinitializes common data structures.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  /* Symbol tables */
#ifdef INVARIANTS 
  invSymbolTable(&cd->m_GEntities);
  invSymbolTable(&cd->m_Notations);
  invSymbolTable(&cd->m_PEntities);
  invSymbolTable(&cd->m_Elements);
#endif  /* INVARIANTS */

#ifdef WML_DEBUG 
      DBG_LOG( (DBG_SYM,"\n ***** ID-s             *****\n\n") );
      dbgSymbolTable(&cd->m_IDs);

      DBG_LOG( (DBG_SYM,"\n ***** General entities *****\n\n") );
      dbgSymbolTable(&cd->m_GEntities);

      DBG_LOG( (DBG_SYM,"\n ***** Notations        *****\n\n") );
      dbgSymbolTable(&cd->m_Notations);

      DBG_LOG( (DBG_SYM,"\n***** Parameter entities *****\n\n") );
      dbgSymbolTable(&cd->m_PEntities);

      DBG_LOG( (DBG_SYM,"\n***** Elements *****\n\n") );
      dbgSymbolTable(&cd->m_Elements);
#endif  /* WML_DEBUG */

  destrSymTbl(&cd->m_IDs);
  destrSymTbl(&cd->m_Elements);
  destrSymTbl(&cd->m_PEntities);
  destrSymTbl(&cd->m_Notations);
  destrSymTbl(&cd->m_GEntities);

  /* Pools */
  destrPool(&cd->m_attrPool);
  destrPool(&cd->m_snodePool);
  destrPool(&cd->m_strPool);
}

/*
=============================================================================
*/


/*  End of File  */
