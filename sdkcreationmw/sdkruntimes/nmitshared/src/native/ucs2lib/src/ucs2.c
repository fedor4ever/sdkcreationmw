/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s_mem.h>
#include "ucs2lib.h"

/* *************************************************************************
 *  UCS2 utilities
 * *************************************************************************/

/**
 * UCS2_Len - returns the length of a null-terminated UCS2 string
 */
int UCS2_Len(const Ucs2 * str)
{
    const Ucs2 * eos = str;
    while (*eos++);
    return (eos - str - 1);
}

/**
 * UCS2_Cmp  - compares two UCS2 strings
 *        returning less than, equal to, or greater than
 *
 * src - string for left-hand side of comparison
 * dst - string for right-hand side of comparison
 *
 * Returns:
 *       -1 if src <  dst
 *        0 if src == dst
 *       +1 if src >  dst
 */
int UCS2_Cmp(const Ucs2 * src, const Ucs2 * dst)
{
    int ret = 0;
    while (!(ret = (*src - *dst)) && *dst) src++, dst++;
    if (ret < 0) {
        ret = -1;
    } else if (ret > 0) {
        ret = 1;
    }
    return ret;
}

/**
 * UCS2_CmpASCII  - compares UCS2 and ASCII strings
 *        returning less than, equal to, or greater than
 *
 * src - string for left-hand side of comparison
 * dst - string for right-hand side of comparison
 *
 * Returns:
 *       -1 if src <  dst
 *        0 if src == dst
 *       +1 if src >  dst
 */
int UCS2_CmpASCII(const Ucs2 * src, const char* dst)
{
    int ret = 0;
    while (!(ret = (*src - (Ucs2)*dst)) && *dst) src++, dst++;
    if (ret < 0) {
        ret = -1;
    } else if (ret > 0) {
        ret = 1;
    }
    return ret;
}

/**
 * UCS2_Ncpy - copy at most n UCS2 characters
 *             Returns the destination string
 */
Ucs2 * UCS2_Ncpy(Ucs2 * dest, const Ucs2 * src, int count)
{
    Ucs2 * start = dest;
    while (count && (*dest++ = *src++)) count--;
    if (count) *dest++ = 0;
    return start;
}

/**
 * UCS2_Cpy - copy NULL terminated UCS2 string
 *            Returns the destination string
 */
extern Ucs2 * UCS2_Cpy(Ucs2 * dest, const Ucs2 * src)
{
    Ucs2 * p = dest;
    while ((*p++ = *src++));
    return  dest;
}

/**
 * Calculates hash code for a UCS2 string
 */
int UCS2_Hash(const Ucs2 * str)
{
    int h = 0;    
    if (str) {
        while (*str) h = (h<<5) - h + *str++;
    }
    return h;
}

/**
 * UCS2_SubStr - allocates a substring of UCS2 string
 */
Ucs2 * UCS2_SubStr(const Ucs2 * src, int off, int len)
{
    Ucs2 * dest = NULL;
    if (src && len >= 0) {
        ASSERT(UCS2_Len(src) >= (off + len));
        dest = UCS2_New(len);
        if (dest) {
            UCS2_Ncpy(dest, src + off, len);
            dest[len] = 0;
        }
    }
    return dest;
}

/**
 * UCS2_Dup  - creates a copy of UCS2 string
 */
Ucs2 * UCS2_Dup(const Ucs2 * src)
{
    int len = UCS2_Len(src);
    Ucs2 * dst = UCS2_New(len);
    if (dst) memcpy(dst, src, (len+1) * sizeof(Ucs2));
    return dst;
}

/**
 * UCS2_Clone  - creates a copy of UCS2 string
 * Allows NULL parameter (unlike UCS2_Dup)
 */
Ucs2 * UCS2_Clone(const Ucs2 * src)
{
    return src ? UCS2_Dup(src) : NULL;
}

/**
 * UCS2_New - allocates a new uninitialized UCS2 string
 */
Ucs2 * UCS2_New(int len)
{
    Ucs2 * str = MEM_NewArray(Ucs2,len+1);
    if (str) str[0] = 0;
    return str;
}

/**
 * UCS2_Free - deallocates a UCS2 string allocated by this module.
 * Ignores NULL parameter
 */
void UCS2_Free(Ucs2 * str)
{
    MEM_Free(str);
}

/**
 * UCS2_FreeASCII - deallocates an ASCII string allocated by this module
 * Ignores NULL parameter
 */
void UCS2_FreeASCII(char * str)
{
    MEM_Free(str);
}

/**
 * UCS2_CpyFromASCII - copies ASCII string into a Ucs2 string.
 * Returns number of characters copied
 */
int UCS2_CpyFromASCII(Ucs2 * dest, const char * src)
{
    Ucs2 * p = dest;
    while ((*p++ = ((Ucs2)(*src++)) & 0x00ff));
    return (p - dest - 1);
}

/**
 * UCS2_CpyToASCII - copies a UCS2 string into an ASCII string, making
 * a naive assumption that UCS2 string contains exclusively characters
 * in the ASCII range. Returns number of characters copied
 */
int UCS2_CpyToASCII(char * dest, const Ucs2 * src)
{
    char * p = dest;
    while ((*p++ = (char)(*src++))) ASSERT((src[-1] & 0xff00) == 0);
    return (p - dest - 1);
}

/**
 * UCS2_FromASCII - creates a new UCS2 string, initializes it with an 
 * ASCII string
 */
Ucs2 * UCS2_FromASCII(const char * src)
{
    int len = strlen(src);
    Ucs2 * dest = UCS2_New(len);
    if (dest) {
        UCS2_CpyFromASCII(dest, src);
    }
    return dest;
}

/**
 * UCS2_ToASCII - creates a new ASCII string, initializes it with an 
 * UCS2 string (if possible)
 */
char * UCS2_ToASCII(const Ucs2 * src)
{
    int len = UCS2_Len(src);
    char * dest = MEM_NewArray(char,len+1);
    if (dest) {
        UCS2_CpyToASCII(dest, src);
    }
    return dest;
}

/**
 * UCS2_Hex - if ch is a hex character, returns the number it represents.
 *            Otherwise, returns -1
 */
int UCS2_Hex(Ucs2 ch)
{
    if (ch >= ((Ucs2)'0') && (ch <= ((Ucs2)'9'))) {
        return ch - (Ucs2)'0';
    } else if (ch >= ((Ucs2)'A') && (ch <= ((Ucs2)'F'))) {
        return 10 + (ch - (Ucs2)'A');
    } else if (ch >= ((Ucs2)'a') && (ch <= ((Ucs2)'f'))) {
        return 10 + (ch - (Ucs2)'a');
    } else {
        return -1;
    }
}

/**
 * UCS2_Hex - if ch is a space character, returns 1.
 *            Otherwise, returns 0
 */
int UCS2_IsSpace(Ucs2 ch)
{
    switch (ch) {
    case ' ':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
        return True;
    default:
        return False;
    }
}

/**
 * UCS2_URLDecode - a utility for converting from a MIME format called 
 * x-www-form-urlencoded into a UCS2 string.
 * 
 * To convert to a string, each character is examined in turn:
 * 
 * o The ASCII characters 'a' through 'z', 'A' through 'Z', 
 *   and '0' through '9' remain the same.
 * o The plus sign '+'is converted into a space character ' '.
 * o The remaining characters are represented by 3-character strings which 
 *   begin with the percent sign, "%xy", where xy is the two-digit
 *   hexadecimal representation of the lower 8-bits of the character.
 */
Ucs2 * UCS2_URLDecode(const Ucs2 * src)
{
    int len = UCS2_Len(src);
    Ucs2 * dst = UCS2_New(len);
    if (dst) {
        Ucs2 * to = dst;
        const Ucs2 * from = src;
        while (*from) {
            if (*from == (Ucs2)'+') {
                *to++ = (Ucs2)' ';
                from++;
            } else if (*from == (Ucs2)'%') {
                int n1 = UCS2_Hex(from[1]);
                if (n1 >= 0) {
                    int n2 = UCS2_Hex(from[2]);
                    if (n2 >= 0) {
                        *to++ = (Ucs2)(16*n1+n2);
                        from += 3;
                        continue;
                    }
                }
                /* invalid encoding - store it "as is" */
                *to++ = *from++;
            } else {
                *to++ = *from++;
            }
        }
        *to = 0;
    }
    return dst;
}

/**
 * UCS2_Equal - returns True if both UCS2 strings are NULL or both are 
 * non-NULL and equal
 */
int UCS2_Equal(const Ucs2 * str1, const Ucs2 * str2)
{
    if (str1 == str2) {
        return True;
    } else if (str1 && str2) {
        return (UCS2_Cmp(str1,str2) == 0);
    } else {
        return False;
    }
}

/**
 * UCS2_EqASCII - returns True if both UCS2 and ASCII strings are NULL 
 * or both are non-NULL and equal
 */
int UCS2_EqASCII(const Ucs2 * str1, const char * str2)
{
    if (str1 && str2) {
        return (UCS2_CmpASCII(str1,str2) == 0);
    } else if (!str1 && !str2) {
        return True;
    } else {
        return False;
    }
}

/**
 * UCS2_Chr - searches a string for a character
 * Returns pointer to the first occurence of ch in the given string.
 * Returns NULL if ch does not occur in the string
 */
UStr UCS2_Chr(const Ucs2 * str, Ucs2 ch)
{
    while (*str) {
        if (*str == ch) return str;
        str++;
    }
    return NULL;
}

/**
 * UCS2_Rchr - finds last occurrence of ch in the string
 * Returns pointer to the last occurence of ch in the given string.
 * Returns NULL if ch does not occur in the string
 */
UStr UCS2_Rchr(const Ucs2 * str, Ucs2 ch)
{
    UStr start = str;
    while (*str++);             /* find end of string */
    while (--str != start) {    /* search towards front */
        if (*str == ch) return str;
    }
    return NULL;
}

/**
 * UCS2_IndexOf - searches a string for a character
 * Returns index of the first occurence of ch in the string.
 * Returns -1 if ch does not occur in the string
 */
int UCS2_IndexOf(const Ucs2 * str, Ucs2 ch)
{
    UStr start = str;
    while (*str) {
        if (*str == ch) return (str - start);
        str++;
    }
    return (-1);
}

/**
 * UCS2_LastIndexOf - finds last occurrence of ch in the string
 * Returns index of the last occurence of ch in the given string.
 * Returns NULL if ch does not occur in the string
 */
int UCS2_LastIndexOf(const Ucs2 * str, Ucs2 ch)
{
    UStr start = str;
    while (*str++);             /* find end of string */
    while (--str != start) {    /* search towards front */
        if (*str == ch) return (str - start);
    }
    return (-1);
}

/**
 * UCS2_StartsWith - returns True if string str starts with the sequence 
 * of characters represented by string substr. If substr is empty, always 
 * returns True. The comparison is case sensitive.
 */
int UCS2_StartsWith(const Ucs2 * str, const Ucs2 * substr)
{
    while (*substr) {
        if (*str++ != *substr++) {
            return False;
        }
    }
    return True;
}

/**
 * UCS2_EndsWith - returns True if string str ends with the sequence 
 * of characters represented by string substr. If substr is empty, always 
 * returns True. The comparison is case sensitive.
 */
int UCS2_EndsWith(const Ucs2 * str, const Ucs2 * substr)
{
    int sublen = UCS2_Len(substr);
    if (sublen > 0) {
        int len = UCS2_Len(str);
        if (len >= sublen) {
            return (UCS2_Cmp(substr, str + len - sublen) == 0);
        }
        return False;
    }
    return True;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
