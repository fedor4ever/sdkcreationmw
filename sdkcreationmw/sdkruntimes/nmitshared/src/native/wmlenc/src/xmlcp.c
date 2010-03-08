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
/* File    : U:\wpsvob\wmlenc\src\common\xmlcp.c
   Version : \main\8
   Date    : 17-Oct-00.12:48:43
*/

/*
 * ========================================================================
 *  Name        : xmlcp.c
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

        This file contains the implementation of the 'Content Parser' of
        the XML Parser.


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

        7.1    checkCP
        7.2    leaveCP
        7.3    skipCP
        7.4    rewindCP
        7.5    matchCP
        7.6    enterCP
        7.7    nextCheck
        7.8    ignore
        7.9    noPending

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlpars.h"
#include "xmllex.h"
#include "xmlmsg.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define ENDCH(c) ( (c)=='|' || (c)=='*' || (c)=='?' || (c)=='+'\
                || (c)==',' || (c)==')' || (c)=='(')
#define IS_OPTION(c) ( (c)=='*' || (c)=='?' || (c)=='+' )

#define CUR     scp->m_cur
#define BEG     scp->m_beg
#define END     scp->m_end
#define GOOD    ((BEG<=CUR) && (CUR<END))

#define SKIP_OPTION {if(GOOD && IS_OPTION(*CUR)) CUR++;}


/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

    /* None */


/*  6.2 Local Function Prototypes  */

static void skipCP(SCP *scp);
static void rewindCP(SCP *scp);
static int matchCP(const PString elem, SCP *scp);
static int enterCP(const PString elem, SCP *scp);
static int nextCheck(const PString elem, SCP *scp);
static int ignore(SCP *scp);
static int noPending(SCP *scp);


/*  7 MODULE CODE  */


/*
 *===========================================================================
 */
/*  7.1  */
int checkCP(const PString elem,
            SCP *scp)
{
/*  Check if 'elem' is a acceptable as next CP in content model.
 *  This is the interface function to the 'Content Model Parser'.
 *  Return: 1 - found
 *          0 - otherwise
 *  Note: Restore the state of the parser in case of failure.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int retv = 0;
  const WCHAR *_saved;

/*  Code  */
#if 0
    {
      char bubu[512];
      wPrint(scp->m_end-scp->m_beg,scp->m_beg, 512, bubu);
      DBG_LOG2( (DBG_SYM,elem,"checkCP elem:") );
      DBG_LOG( (DBG_SYM, "in model: %s\n", bubu) );
    }
#endif
  if(!elem || !STR_LEN(elem) || !scp || !GOOD) {
      DBG_LOG( (DBG_SYM, "checkCP FAILED 1") );
      return 0;
  }

  switch(*BEG) {
     case 'E' : return 0;       /* EMPTY */
     case 'A' : return 1;       /* ANY  */
  }

  _saved = CUR;

  if(BEG!=CUR)
      retv = nextCheck(elem,scp);   /* a subsequent call    */
  else
      retv = enterCP(elem,scp);     /* the first call       */

  if(!retv) {                       /* restore the parser   */
      CUR = _saved;
      DBG_LOG( (DBG_SYM,"checkCP FAILED 2") );
  }

  return retv;
}

/*
 *===========================================================================
 */
/*  7.2  */
int leaveCP(SCP *scp)
{
/*  Return: 1 - no further elem required to reach the end.
 *          0 - otherwise
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int retv = 0;
  const WCHAR * saved;

/*  Code  */
  if(!scp || !GOOD)
      return 1;

  switch(*BEG) {
     case 'E' : return 1;       /* EMPTY */
     case 'A' : return 1;       /* ANY  */
  }
  saved = CUR;

  if(BEG!=CUR)
      retv = noPending(scp);     /* a subsequent call    */
  else
      retv = ignore(scp);        /* the first call       */

  CUR = saved;

  return retv;
}

/*
 *===========================================================================
 */
/*  7.3  */
static void skipCP(SCP *scp)
{
/*  Go through on current content particle, and increment CUR to
 *  next position.
 *  Return: 0 - if no match for the beginning '('.
 *          1 - otherwise
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int deepness = 0;

/*  Code  */
  if(!GOOD)
      return;

  if(*CUR=='(') {
      deepness++;
      while(CUR++ && GOOD) {
          if(*CUR=='(') {
              deepness++;
          }
          else if(*CUR==')' && !--deepness) {
              CUR++;
              return;
              
          }
      }
      return;
  }

  /* CP without surrounding () */
  while(GOOD && !ENDCH(*CUR))       /* go until delimiter */
      CUR++;

  return;
}

/*
 *===========================================================================
 */
/*  7.4  */
static void rewindCP(SCP *scp)
{
/*  Go back to the beginning of the current CP.
 *  Assumes: CUR points to the last character of the current CP.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
/*  Code  */
  if(CUR>BEG) {
      if(*CUR==')') {
          CUR--;
          while(CUR>BEG) {
              switch(*CUR) {
                 case '(' : return;
                            break;
                 case ')' : rewindCP(scp);
                            if(CUR>BEG)
                                CUR--;
                            break;
                 default  : CUR--;
                            break;
              }
          }
      }
      else {
          while(CUR>BEG && !ENDCH(*CUR))
              CUR--;
          if(ENDCH(*CUR))
              CUR++;
      }
  }
}

/*
 *===========================================================================
 */
/*  7.5  */
static int matchCP(const PString elem,
                   SCP *scp)
{
/*  Return: 1 - if the substring (from CUR to delimiter) is identical with elem.
 *          0 - otherwise
 *  Side effect: Leave CUR on the delimiter, regardless to the success.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *p   = CUR;
  const WCHAR *e;

/*  Code  */
  for(; p<END && !ENDCH(*p); p++)        /* walk till separator */
      ;

  if((p-CUR)==STR_LEN(elem)) {
      e = STR_DATA(elem);
      for(; CUR<p && *CUR==*e; CUR++, e++)
          ;
      if(CUR==p)
          return 1;
  }
  CUR = p;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.6  */
static int enterCP(const PString elem,
                   SCP *scp)
{
/*  Go into current 'Content Particle' and search for 'elem'.
 *  Return: 1 - found
 *          0 - otherwise
 *  Remark: in case the search was failed, restore the CUR pointer.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *saved = CUR;

/*  Code  */
  if(GOOD && *CUR=='(') {
      CUR++;
  }
  while(GOOD) {
      /* Go through the current CP  */
      if( *CUR=='(') {
          if(enterCP(elem,scp))
              return 1;         /* found    */
          skipCP(scp);
      }
      else if(matchCP(elem,scp) )
          return 1;             /* found    */

      /* Not found until here, so may we skip the previous ? */
      if(GOOD && (*CUR=='?' || *CUR=='*' || *CUR=='|')) {
          CUR++;
      }
      else {
          CUR = saved;
          return 0;             /* not  */
      }
      if(GOOD && (*CUR==',' || *CUR=='|')) {
          CUR++;
      }
      /* let us see the next */
  }
  CUR = saved;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.7  */
static int nextCheck(const PString elem,
                     SCP *scp)
{
/*  Check if 'elem' is a acceptable as next CP in content model.
 *  Return: 1 - found
 *          0 - otherwise
 *  Assume: the CUR pointer was left by a successful match() function.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR * prev;
  unsigned safety_counter = 512;
  
/*  Code  */
  while(--safety_counter && GOOD) {
      prev = CUR;

      /* May I try it again ? */
      if( GOOD && (*CUR=='+' || *CUR=='*')) {
          CUR--;
          rewindCP(scp);              /* yes  */
          if(enterCP(elem,scp))
              return 1;
          CUR = ++prev;             /* but no match, skip the option char   */
      }

      if(GOOD && *CUR=='?')
          CUR++;                    /* it was optional, go ahead            */

      /* sequence ? */
      while(GOOD && *CUR==',') {
          CUR++;
          if(enterCP(elem,scp))
              return 1;

          skipCP(scp);
          if( GOOD && (*CUR!='?' && *CUR!='*')) {
              return 0;             /* cannot skip CP in sequence   */
          }
      }

      /* choice ? */
      if(GOOD && *CUR=='|') {
          while(++CUR<END && *CUR!=')')
              ;
      }

      if(GOOD && *CUR==')') {
          CUR++;                    /* leave this CP */
      }
  }

  return 0;
}

/*
 *===========================================================================
 */
/*  7.8  */
static int ignore(SCP *scp)
{
/*  
 *  Return: 1 - ignored     CUR points behind the option
 *          0 - otherwise   CUR pointer was restored
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *first = CUR;
  const WCHAR *last;

/*  Code  */

  skipCP(scp);
  if(GOOD && (*CUR=='?' || *CUR=='*')) {
      CUR++;
      return 1;
  }
  SKIP_OPTION;
  last = CUR;
  CUR = first;

  if(GOOD && *CUR=='(') {
      while(++CUR<END) {
          if(ignore(scp)) {
              if(!GOOD || *CUR!=',')
                  return (CUR=last)!=0;
          }
          else {
              skipCP(scp);
              SKIP_OPTION;
              if(!GOOD || *CUR!='|')
                  break;
          }
      }
      CUR = first;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.9  */
static int noPending(SCP *scp)
{
/*  Check if 'elem' is a acceptable as next CP in content model.
 *  Return: 1 - found
 *          0 - otherwise
 *  Assume: the CUR pointer was left by a successful match() function.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR * prev;
  unsigned safety_counter = 512;
  
/*  Code  */
  while(--safety_counter && GOOD) {
      prev = CUR;

      SKIP_OPTION;

      /* sequence ? */
      while(GOOD && *CUR==',') {
          CUR++;
          if(!ignore(scp))
              return 0;
      }

      /* choice ? */
      if(GOOD && *CUR=='|') {
          while(++CUR<END && *CUR!=')')
              ;
      }

      if(GOOD && *CUR==')') {
          CUR++;                    /* leave this CP */
      }
  }

  return 1;
}


/*
=============================================================================
*/


/*  End of File  */
