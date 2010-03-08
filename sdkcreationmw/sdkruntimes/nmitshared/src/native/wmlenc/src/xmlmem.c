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
/* File    : U:\wpsvob\wmlenc\src\common\xmlmem.c
   Version : \main\7
   Date    : 17-Oct-00.12:48:59
*/

/*
 * ========================================================================
 *  Name        : xmlmem.c
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

        Memory management routines.


    1.3 Specification/Design Reference

        doc/wmlarch.doc

    1.4 Module Test Specification Reference

        none

    1.5 Compilation Information

        See: building.txt

    1.6 Notes

        Unfortunately some CRT lib function uses the original
        malloc/free so this solution (waiting for memory) does
        not always work.

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

        7.1   xmlMalloc
        7.2   xmlFree

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlhost.h"
#include "wps_service.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#ifdef _DBGMEM

#ifdef _MALLOC
#undef _MALLOC
#endif
#ifdef _FREE
#undef _FREE
#endif

#define NR_PTR      32384
#define MAGIC       ((BYTE)0xfa)
#define _MALLOC     debugMalloc
#define _FREE       debugFree

#else

#define _MALLOC     WSL_OS_Malloc
#define _FREE       WSL_OS_Free

#endif  /* _DBGMEM */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

#ifdef _DBGMEM
typedef BYTE * PTR;

static PTR           s_buf[NR_PTR];
static unsigned      s_bufSize[NR_PTR];
static unsigned long s_hiWater    = 0;
static unsigned long s_waterLevel = 0;
static unsigned long s_nrAlloc    = 0;
static unsigned long s_nrFree     = 0;
static WSL_Mutex     s_mutex;
#endif


/*  6.2 Local Function Prototypes  */

#ifdef _DBGMEM
static void *debugMalloc(size_t size);
static void  debugFree(void *p );
static void  finish(void);
#endif


/*  7 MODULE CODE  */



/*
=============================================================================
*/
/*  7.1  */
void *xmlMalloc(UINT32 siz)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  void *        _retv;
  int           _om = 0;

/* Code */

  while( !(_retv = _MALLOC(siz)) ) { 
      if(!_om++) {
          wmlLOGs(WSL_LOG_ERROR, "Waiting for memory");
      }
      /* there is no any 'sleep()' in Service Library */
  }

  return _retv;
}

/*
=============================================================================
*/
/*  7.2  */
void  xmlFree(void *ptr)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  _FREE(ptr);
}


#ifdef _DBGMEM
int s_initialized = 0;


/**************************************************************************/
void  initMemDebug()
{
  s_initialized++;
  WSL_OS_MutexInit(&s_mutex);
}
/**************************************************************************/
void  termMemDebug()
{
  s_initialized--;
  WSL_OS_MutexDestroy(&s_mutex);
  finish();
}
/**************************************************************************/
void chkPointer(void *ptr)
{
  int i;
  BYTE *p = (BYTE*)ptr;
  for(i=0; i<NR_PTR; i++)
   {
      if(s_buf[i]
         && s_buf[i]<=p
         && (s_buf[i]+s_bufSize[i])>p) {
          return ;
      }
   }
  fprintf(stderr,"Invalid pointer:%p\n",ptr);
  exit(1999);
}
/**************************************************************************/
static void finish(void)
{
  unsigned n = 0;
  unsigned i,j;
  fprintf(stderr,"\n");
  for(i=0; i<NR_PTR; i++)
   {
      if(s_buf[i])
       {
          n++;
          fprintf(stderr,"not deleted: %p size:%u\n",s_buf[i],s_bufSize[i]);
          for(j=0; j<60; j+=2)
              fprintf(stderr,"%c",(int)((char*)s_buf[i])[j]);
       }
   }
  fprintf(stderr,"\tMax. allocated   :\t%lu \n",s_hiWater);
  fprintf(stderr,"\tCur. allocated   :\t%lu \n",s_waterLevel);
  fprintf(stderr,"\tunfreed pointers :\t%u \n",n);
  fprintf(stderr,"\tmalloc called    :\t%lu \n",s_nrAlloc);
  fprintf(stderr,"\tfree   called    :\t%lu \n",s_nrFree);
}
/**************************************************************************/
static void *debugMalloc(size_t size)
{
  unsigned char *p;
  unsigned i;

  WSL_OS_MutexLock(&s_mutex);

  if(!size) {
      fprintf(stderr,"debugMalloc: zero size\n");
      exit(1999);
  }

  p = malloc(size+8); 
  /* p = Malloc(size+8); */
  if(!p) {
      fprintf(stderr,"debugMalloc: out of memory\n");
      exit(1999);
  }

  memset(p,MAGIC,size+8);

  for(i=0; i<NR_PTR && s_buf[i]; i++)
      ;
  if(i==NR_PTR) {
      fprintf(stderr,"debugMalloc: no more room in array of pointers.\n");
      exit(1999);
  }

  s_buf[i] = p+4;
  s_bufSize[i] = size;
  s_waterLevel+=size;
  if(s_hiWater<s_waterLevel)
      s_hiWater=s_waterLevel;
  s_nrAlloc++;

  WSL_OS_MutexUnlock(&s_mutex);
  return p+4;
}
/**************************************************************************/
static void debugFree( void *p )
{
  unsigned i;
  size_t size;
  BYTE * cp=(BYTE *)p;
  
  WSL_OS_MutexLock(&s_mutex);

  if(!p) {
      fprintf(stderr,"\nNULL pointer\n");
      exit(1999);
  }
  
  for(i=0; i<NR_PTR && s_buf[i]!=p; i++)
      ;
  if(i>=NR_PTR) {
      fprintf(stderr,"Bad pointer to free:%p\n",p);
      exit(1999);
  }

  s_buf[i]     = 0;
  s_waterLevel-=s_bufSize[i];
  size         = s_bufSize[i];
  s_bufSize[i] = 0;

  cp-=4;
  if(cp[0]!=MAGIC || cp[1]!=MAGIC || cp[2]!=MAGIC || cp[3]!=MAGIC) {
      fprintf(stderr,"\nUnderflow\n");
      exit(1999);
  }
  if(cp[size+7]!=MAGIC || cp[size+6]!=MAGIC
     || cp[size+5]!=MAGIC || cp[size+4]!=MAGIC) {
      fprintf(stderr,"\nOverflow\n");
      exit(1999);
  }
  memset(cp,MAGIC,size+8);
  /* Free(cp); */
  free(cp);

  s_nrFree++;
  WSL_OS_MutexUnlock(&s_mutex);
}
#endif

/*
=============================================================================
*/


/*  End of File  */
