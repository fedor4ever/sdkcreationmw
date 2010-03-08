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
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\other_util.c
   Version : \main\9
   Date    : 07-Feb-00.19:36:42
*/

/*
 * ========================================================================
 *  Name        : other_util.c
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
    INCLUDES
-----------------------------------------------------------------------------
*/


#include "other_util.h"

#include <limits.h>
#include <string.h>


/*
-----------------------------------------------------------------------------
    DEFINES
-----------------------------------------------------------------------------
*/

#define SEC_MIN				(60)
#define SEC_HOUR			(60 * SEC_MIN)
#define SEC_DAY				(24 * SEC_HOUR)
#define SEC_YEAR			(365 * SEC_DAY)
#define SEC_FOURYEAR		(4 * SEC_YEAR + SEC_DAY)

#define BASE_DAY			4


/*
-----------------------------------------------------------------------------
    FUNCTIONS
-----------------------------------------------------------------------------
*/


/* ----------------------------------------------------------------------- */
/* Function   : UINT32toStr

   Converts a UINT32 number to string (10 based)

   Parameters: - number: number
               - keyOut: buffer for the string result
               - maxLen: maximum length of the result (must include the
                         trailing '\0')

   Returns:    - WPS_HC_STATUS_OK : OK
               - otherwise : the port number does not fit into space given

*/


HC_STATUS UINT32toStr(UINT32 number, char*  keyOut, UINT16 maxLen)
{
    HC_STATUS status = WPS_HC_STATUS_OK;
    char   buffer[UINT32_BUFFER_LEN];
    int    i = 0;

    /* Print the port number as 10 based string */
    do {
        buffer[i++] = (char)('0' + (number % 10));
        number = number / 10;
        if (i == UINT32_BUFFER_LEN) {
            break;
        }
    } while (number != 0);

    if (i + 1 > maxLen || i == UINT32_BUFFER_LEN) {
        status = 1;

    }
	else {
        do {
            *keyOut++ = buffer[--i];
        } while (i != 0);
        *keyOut = '\0';
    }

    return status;
}


/* ----------------------------------------------------------------------- */
/* Function   : INT32toStr

   Converts an INT32 number to string (10 based)

   Parameters: - number: number
               - keyOut: buffer for the string result
               - maxLen: maximum length of the result (must include the
                         trailing '\0')

   Returns:    - WPS_HC_STATUS_OK : OK
               - otherwise : the port number does not fit into space given

*/

HC_STATUS INT32toStr(INT32 number, char* keyOut, UINT32 maxLen)
{
    UCHAR	tmp[3*sizeof(int)+1];
	UINT32	l = 0;    
    HC_STATUS status = WPS_HC_STATUS_OK;

	if( number == 0 )
	{
		*keyOut = '0';
		keyOut++;
	}
	else
	{
		if( number < 0 )
		{
			*keyOut = '-'; 
			keyOut++;
			tmp[l] = (UCHAR)(((int)'0')-(number - ((number/10)*10)));
			number = (number / 10) * (-1);
			l++;
		}
		while( number>0 )
		{
			tmp[l] = (UCHAR)('0'+(number%10));
			l++;
			number = number/10;
		}
	}

    if (l + 1 > maxLen || l == UINT32_BUFFER_LEN) {
        status = 1;
	}
	else
	{
		while( l > 0 )
		{
			*keyOut = tmp[l-1];
			keyOut++;
			l--;
		}
		*keyOut = '\0';
	}

	return status;
}


/* ----------------------------------------------------------------------- */
/* Function   : StrtoUINT32

   Converts a string ending into a delimiter to UINT32 (10 based)
   number. The buffer pointer will be moved to the delimiter.

   Parameters: - strptr (in/out): pointer to str pointer
               - intptr (out): ptr to the UINT32 buffer
               - separator: the separator which ends the string (or the '\0')

   Returns:    - WPS_HC_STATUS_OK : OK
               - otherwise: zero length string, or an illegal separator was encountered
				 or strptr is null pointer

*/

HC_STATUS StrtoUINT32(const char* strptr, UINT32* intptr, char separator)
{
    HC_STATUS status = WPS_HC_STATUS_OK;
    UINT32 number = 0;
	const char *pcTmp = strptr;
	UINT32 nPrevTotal = 0;
	BOOL bOverflowed = FALSE;

	if (pcTmp==NULL)
	{
        status = 1;
	}
	else
	{
		if (*pcTmp=='\0')
		{
	        status = 1;
		}
		else
		{
			while (*pcTmp==' '){
				pcTmp++;
			}
		
			while (*pcTmp <= '9' && *pcTmp >= '0') 
			{
				nPrevTotal = number;
				number = number * 10 + (*pcTmp - '0');
				(pcTmp)++;
		        bOverflowed = (nPrevTotal > number / 10);
			}

			if ((*pcTmp != separator) && (*pcTmp!= '\0'))
			{
		        status = 1;
			} 
			else 
			{
				*intptr = number;
			}

		    if (bOverflowed)
			{   /* Overflow - return the largest possible value */
				status = 1;
				*intptr = ULONG_MAX;
			}
		}
	}	
    return status;
}


/* ----------------------------------------------------------------------- */
/* Function   : StrtoINT32

   Converts a string ending into a delimiter to INT32 (10 based)
   number. The buffer pointer will be moved to the delimiter.

   Parameters: - strptr (in/out): pointer to str pointer
               - intptr (out): ptr to the UINT32 buffer
               - separator: the separator which ends the string

   Returns:    - WPS_HC_STATUS_OK : OK
               - otherwise : zero length string, or an illegal separator was encountered
				 or strptr is null pointer

*/

HC_STATUS StrtoINT32(const char* strptr, INT32* intptr, char separator)
{
    HC_STATUS status = WPS_HC_STATUS_OK;
    UINT32 number = 0;
	BOOL	neg=FALSE;
	const char *pcTmp = strptr;

	if (pcTmp==NULL)
	{
        status = 1;
	}

	if (*pcTmp=='\0')
	{
        status = 1;
	}

	while ((*pcTmp ==' ')&&(status==WPS_HC_STATUS_OK)){
        (pcTmp )++;
	}
	
	if ((*pcTmp =='-')&&(status==WPS_HC_STATUS_OK)){
        (pcTmp )++;
		neg=TRUE;
	} else if ((*pcTmp =='+')&&(status==WPS_HC_STATUS_OK)){
        (pcTmp)++;
	}

    while ((*pcTmp  <= '9' && *pcTmp  >= '0') &&(status==WPS_HC_STATUS_OK)){
        number = number * 10 + (*pcTmp  - '0');
        (pcTmp)++;
    }
    if ((*pcTmp  != separator) &&(status==WPS_HC_STATUS_OK) && (*pcTmp != '\0') ){
        status = 1;
    } else {
		if ((neg)&&(status==WPS_HC_STATUS_OK)){
			number = -1*number;
		}
        *intptr = number;
    }
    
	return status;
}


/* ----------------------------------------------------------------------- */
/* Function   : SectoDate

   Converts the given second (since 1970.01.01. 00:00:00) into a date structure.

   Parameters: - secs : since 1970.01.01. 00:00:00 seconds
			   - result : (out) parameter. It returns the pointer to the filled date 
			   struct.
   Returns:    - WPS_HC_STATUS_OK : it was succesfull
			   - otherwise : it wasn't succesfull.
   
*/

HC_STATUS SectoDate (UINT32 secs, DateStruct* result)
{
	
	HC_STATUS status = WPS_HC_STATUS_OK;
	unsigned int seconds, secs2;
	static int days[] = {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};
	int years4, year, month, year_day, lpyear, i;
	BOOL bLeapYear, bFound;


	seconds = secs;
	/* if it's less then 0 (means before 1970.01.01 ...) */
	if (seconds < 0)
	{
		status = 1;
	}


	/* year */
	years4 = seconds / SEC_FOURYEAR * 4 + 70;		/* 4 yrs intervals */
	seconds = seconds % SEC_FOURYEAR;				/* seconds since last 4years */

	year = years4 + 1900;
	bLeapYear = FALSE;
	
	secs2 = 0;
	i=0;
	lpyear = 0;
	while (secs2<=seconds)
	{
		if ((year+i)%4==0)
		{
			secs2+=(SEC_YEAR+SEC_DAY);
			lpyear = year+i;
		}else
		{
			secs2+=(SEC_YEAR);
		}
		i++;
	}

	year+=(i-1901);										/* year is only two digit */

	bLeapYear = FALSE;
	if ((lpyear-1900) == year)
	{
		bLeapYear = TRUE;
	}

	result->year = year;								/* year is determined */

	if (lpyear>0 && !bLeapYear)							/* if there ws a leap year and it is not the current */
	{
		seconds-= ((i-1)*SEC_YEAR+SEC_DAY);
	}
	else
	{
		seconds-= ((i-1)*SEC_YEAR);
	}
	
	year_day = seconds / SEC_DAY;
	seconds = seconds % SEC_DAY;


	/* month */
    if ( bLeapYear )
	{
		month = 1;
		bFound = FALSE;
		while ( !bFound )
		{
			if (month<2)
			{
				if (year_day<=days[month])
					bFound = TRUE;
			}else
			{
				if (year_day<=days[month]+1)
					bFound = TRUE;
			}
			month++;
		}
	}else
	{
		month = 1;
		bFound = FALSE;
		while ( !bFound )
		{
			if (year_day<=days[month])
				bFound = TRUE;
			month++;
		}
	}
	month--;
    result->mon = month-1;

	/* days in this month */
    if ( bLeapYear )
	{
		if (month > 2)
		{
			result->day = year_day - days[month-1]-1;
		}
		else
		{
			result->day = year_day - days[month-1];
		}
	}else
	{
		result->day = year_day - days[month-1];
	}

	/* days in this week */
	result->week_day = ((secs / SEC_DAY) + BASE_DAY) % 7;

    /* hours*/
	result->hour = (seconds / SEC_HOUR);
	seconds = seconds % SEC_HOUR;
	/* minutes */
	result->min = seconds / SEC_MIN;
	/* seconds */
	result->sec = seconds % SEC_MIN;

	return (status);
}


/* ----------------------------------------------------------------------- */
/* Function   : DatetoSec

   Converts the given date struct into secs (since 1970.01.01. 00:00:00)

   Parameters: - ds : date struct
   
   Returns:    - The secs since 1970.01.01. 00:00:00
   
*/

UINT32 DatetoSec(DateStruct ds)
{
    static int days[] = 
          { -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364 };

	UINT32 result;

    UINT32 year;
	UINT32 year_day;
	UINT32 s70_day;

	year = ds.year;

    /* Calculate number of passed days during the current year */
    year_day = days[ds.mon];
/*    if ( !(year & 3) && ( year % 100 > 0 ) && (ds.mon > 1) )*/
    if ( !(year & 3) && ( (year % 100 > 0) || (year%400==100) ) && (ds.mon > 1) )
		year_day++;

    /* Number of days since 01.01.1970 till the beginning of the year */
    s70_day = ( year - 70 ) * 365 + (( year - 1 ) >> 2 ) - 17;

    /* Number of days since 01.01.1970 */
    s70_day = s70_day + year_day;

    result = s70_day + ds.day;

    /* Number of hours */
    result = result * 24 + ds.hour;

    /* Number of minutes */
    result = result * 60 + ds.min;

    /* Number of seconds */
    result = result * 60 + ds.sec;

	return result;

}


/* ----------------------------------------------------------------------- */
/* Function   : FindChars

   Counts how long the given string contains the geiven characters

   Parameters: - string : given string
			   - strCharSet : the given characters
   
   Returns:    - length of string whics contains characters from strCharSet
   
*/

size_t FindChars(const char *string, const char *strCharSet )
{
	size_t result=0;
	UINT32 i = 0;
	UINT32 j = 0;
	BOOL bfound = TRUE;

	while (i<strlen(string) && (bfound))
	{
		bfound = FALSE;
		for (j=0;((j<strlen(strCharSet)) && (!bfound));j++)
		{
			if (string[i]==strCharSet[j])
			{
				bfound = TRUE;
			}
		}
		i++;
	}

	if (!bfound)
	{
		result = i-1;
	}
	else
	{
		result = 0;
	}

	return (result);
}


/* ----------------------------------------------------------------------- */
/* Function   : StringCopy

   Copies the given source string into the destionation string while not end_char
   or end of string reached.

   Parameters: - dest : destination string
			   - source : source string
   
   Returns:    - the pointer where the function exites.
   
*/

const char *StringCopy(char *dest, const char *source, char end_char)
{
	const char *result = source;
	int i = 0;
	while ((*result!='\0') && (*result!=end_char))
	{
		dest[i]=*result;
		i++;
		result++;
	}
	dest[i] = '\0';
	return (result);
}

/* ----------------------------------------------------------------------- */
/* Function   : N_StrtoUINT32

   Converts a string to UINT32 (10 based) number.

   Parameters: - pStr      (in): pointer to str pointer
               - uiStrLen  (in): The length of the number
               - pInt     (out): ptr to the UINT32 buffer

   Returns:    - WPS_HC_STATUS_OK : OK

*/

HC_STATUS N_StrtoUINT32 (const char* pStr, UINT32 uiStrLen, UINT32* pInt)
{
    const char* pEnd = pStr+uiStrLen;

    /* Sanity check */
    if (pStr==NULL) {
        return 1;
    }

    /* Initialize the return value */
    *pInt = 0;

    /* Do the conversion */
    while (pStr <= pEnd && *pStr <= '9' && *pStr >= '0')  {
        *pInt = (*pInt * 10) + (*pStr - '0');
        pStr++;
    }

    /* Check the next character */
    if (pStr != pEnd) {
        return 1;
    }

    return 0;
}



