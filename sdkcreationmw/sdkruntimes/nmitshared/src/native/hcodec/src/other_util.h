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
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\other_util.h
   Version : \main\5
   Date    : 07-Feb-00.19:37:53
*/

/*
 * ========================================================================
 *  Name        : other_util.h
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
    Includes
-----------------------------------------------------------------------------
*/

#ifndef _OTHER_UTIL_H
#define _OTHER_UTIL_H

#include "hc_util.h"

/*
-----------------------------------------------------------------------------
    Defines
-----------------------------------------------------------------------------
*/

#define UINT32_BUFFER_LEN 11

#ifdef UNDER_CE

#define isdigit(c) ((c>=48)&&(c<=57)) ? TRUE:FALSE
#define isspace(c) (c==32) ? TRUE:FALSE

#endif

/*
-----------------------------------------------------------------------------
    Typedefinitions
-----------------------------------------------------------------------------
*/

typedef struct DS
{
	int sec;
	int min;
	int hour;
	int day;
	int mon;
	int year;
	int week_day;
}DateStruct;

/*
-----------------------------------------------------------------------------
    Functions prototypes
-----------------------------------------------------------------------------
*/


/* ----------------------------------------------------------------------- */
/* Function   : UINT32toStr

   Converts a UINT32 number to string (10 based)

   Parameters: - number: number
               - keyOut: buffer for the string result
               - maxLen: maximum length of the result (must include the
                         trailing '\0')

   Returns:    - WPS_NO_ERROR : OK
               - WPS_ERROR_BUFFER_TOO_SMALL : the port number does not fit into space given

*/

HC_STATUS UINT32toStr(UINT32 number, char*  keyOut, UINT16 maxLen);


/* ----------------------------------------------------------------------- */
/* Function   : INT32toStr

   Converts an INT32 number to string (10 based)

   Parameters: - number: number
               - keyOut: buffer for the string result
               - maxLen: maximum length of the result (must include the
                         trailing '\0')

   Returns:    - WPS_NO_ERROR : OK
               - WPS_ERROR_BUFFER_TOO_SMALL : the port number does not fit into space given

*/
						  
HC_STATUS INT32toStr(INT32 number, char* keyOut, UINT32 maxLen);


/* ----------------------------------------------------------------------- */
/* Function   : INT32toStr

   Converts an INT32 number to string (10 based)

   Parameters: - number: number
               - keyOut: buffer for the string result
               - maxLen: maximum length of the result (must include the
                         trailing '\0')

   Returns:    - WPS_NO_ERROR : OK
               - WPS_ERROR_BUFFER_TOO_SMALL : the port number does not fit into space given

*/

HC_STATUS StrtoUINT32(const char* strptr, UINT32* intptr, char separator);


/* ----------------------------------------------------------------------- */
/* Function   : INT32toStr

   Converts an INT32 number to string (10 based)

   Parameters: - number: number
               - keyOut: buffer for the string result
               - maxLen: maximum length of the result (must include the
                         trailing '\0')

   Returns:    - WPS_NO_ERROR : OK
               - WPS_ERROR_BUFFER_TOO_SMALL : the port number does not fit into space given

*/

HC_STATUS StrtoINT32(const char* strptr, INT32* intptr, char separator);



/* ----------------------------------------------------------------------- */
/* Function   : SectoDate

   Converts the given second (since 1970.01.01. 00:00:00) into a date structure.

   Parameters: - secs : since 1970.01.01. 00:00:00 seconds
			   - result : (out) parameter. It returns the pointer to the filled date 
			   struct.
   Returns:    - WPS_NO_ERROR : it was succesfull
			   - otherwise : it wasn't succesfull.
   
*/

HC_STATUS SectoDate (UINT32 secs, DateStruct* result);



/* ----------------------------------------------------------------------- */
/* Function   : DatetoSec

   Converts the given date struct into secs (since 1970.01.01. 00:00:00).

   Parameters: - ds : date struct
   
   Returns:    - The secs since 1970.01.01. 00:00:00
   
*/

UINT32 DatetoSec(DateStruct ds);



/* ----------------------------------------------------------------------- */
/* Function   : DatetoSec

   Converts the given date struct into secs (since 1970.01.01. 00:00:00).

   Parameters: - ds : date struct
   
   Returns:    - The secs since 1970.01.01. 00:00:00
   
*/

size_t FindChars(const char *string, const char *strCharSet );



/* ----------------------------------------------------------------------- */
/* Function   : StringCopy

   Copies the given source string into the destionation string while not end_char
   or end of string reached.

   Parameters: - dest : destination string
			   - source : source string
   
   Returns:    - the pointer where the function exites.
   
*/

const char *StringCopy(char *dest, const char *source, char end_char);

/* ----------------------------------------------------------------------- */
/* Function   : N_StrtoUINT32

   Converts a string to UINT32 (10 based) number.

   Parameters: - pStr      (in): pointer to str pointer
               - uiStrLen  (in): The length of the number
               - pInt     (out): ptr to the UINT32 buffer

   Returns:    - WPS_NO_ERROR : OK

*/

HC_STATUS N_StrtoUINT32 (const char* pStr, UINT32 uiStrLen, UINT32* pInt);

#endif /* _OTHER_UTIL_H */
