/*
 * $Id: s_str.c,v 1.20 2006/04/13 12:15:39 slava Exp $
 *
 * Copyright (C) 2000-2006 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "s_util.h"
#include "s_mem.h"

/*==========================================================================*
 *              S T R I N G
 *==========================================================================*/

#define MAX_DOUBLE_FRACTION_DIGITS 340

/**
 * Finds the index of the first occurrence of the character c in the string s.
 * If the string does not contain this character, returns -1
 */
int STRING_IndexOf(Str s, Char c)
{
    Str found = StrChr(s, c);
    return (found ? (found - s) : -1);
}

/**
 * Finds the index of the last occurrence of the character c in the string s.
 * If the string does not contain this character, returns -1
 */
int STRING_LastIndexOf(Str s, Char c)
{
    Str found = StrrChr(s, c);
    return (found ? (found - s) : -1);
}

/**
 * Returns hash code for a string. The algorithm is borrowed from 
 * java.lang.String implementation. 
 *
 * NOTE: hash code can be negative
 */
int STRING_HashCode(Str s)
{
    int h = 0;    
    if (s) {
        while (*s) {
            h = (h<<5) - h + *s++;
        }
    }
    return h;
}

/**
 * Returns case-insensitive hash code for a string. The algorithm is borrowed 
 * from java.lang.String implementation
 *
 * NOTE: hash code can be negative
 */
int STRING_HashCodeNoCase(Str s)
{
    int h = 0;    
    if (s) {
        while (*s) {
            Char c = *s++; /* ToUpper is a macro, may have side effects */
            h = (h<<5) - h + ToUpper(c);
        }
    }
    return h;
}

/**
 * Duplicates a single-byte string.
 */
char * STRING_Dup8(const char * s) 
{
    char * copy = NULL;
    if (s) {
        copy = (char*)MEM_Alloc(sizeof(Char)*(strlen(s) + 1));
        if (copy) {
            strcpy(copy,s);
        }
    }
    return copy;
}

/**
 * Duplicates a Unicode string.
 */
wchar_t * STRING_DupU(const wchar_t * ws) 
{
    wchar_t * copy = NULL;
    if (ws) {
        /* inline variant of wcslen... on some systems it's unavailable */
        const wchar_t * p = ws;
        while (*p++) NOTHING;
        copy = (wchar_t*)MEM_Alloc(sizeof(wchar_t)*(p-ws));
        if (copy) {
            wchar_t * dst = copy;
            while ((*dst++ = *ws++) != 0) NOTHING;
        }
    }
    return copy;
}

/**
 * Formats the string into the given buffer (most commonly allocated 
 * on caller's stack). If formatted string fits into the provided buffer,
 * returns pointer to the given buffer. If formatted string is too long 
 * to fit into the provided buffer, allocates a new one and returns pointer
 * to it. If memory allocation fails, returns pointer to the buffer that
 * contains truncated string. Only returns NULL if the first parameter 
 * is NULL.  
 */
Char * STRING_Format(Char * buf, int bufsize, Str format, ...) 
{
    int size;
    Char * buffer;

    if (buf) {
        size = bufsize;
        buffer = buf;
    } else {
        size = MAX(100,bufsize);
        buffer = (Char*)MEM_Alloc(size * sizeof(Char));
    }

    while (buffer)  {
        Char * newbuf;
        int nchars;
        va_list va;

        /* try to print in the allocated space. */
        va_start(va, format);
        nchars = Vsnprintf(buffer, size, format, va);
        va_end(va);
        
        /* if that worked, return the string. */
        if (nchars > -1 && nchars < size) {
            break;
        }

        /* else try again with more space. */
        if (nchars > -1) {
            size = nchars+1;  /* precisely what is needed */
        } else {
            size *= 2;        /* twice the old size */
        }

        newbuf = (Char*)MEM_Alloc(size * sizeof(Char));
        if (newbuf && buffer && (buffer != buf)) MEM_Free(buffer);

        /* If memory allocation fails, break the loop and return prev buffer */
        if (!newbuf) {
            break;
        }
    }
    return buffer;
}

/**
 * Returns True if string s1 starts with the sequence of characters 
 * represented by string s2. If s2 is empty, always return True.
 * The comparison is case sensitive
 */
Bool STRING_StartsWith(Str s1, Str s2)
{
    while (*s2) {
        if (*s1++ != *s2++) {
            return False;
        }
    }
    return True;
}

/**
 * Case insensitive version of STRING_StartsWith
 */
#ifndef __KERNEL__
Bool STRING_StartsWithNoCase(Str s1, Str s2)
{
    while (*s2) {
        if (*s1++ != *s2++) {
            /* switch to more expensive case-insensitive algorithm */
            size_t len1 = StrLen(s1--)+1;
            size_t len2 = StrLen(s2--)+1;
            if (len1 >= len2) {
                if (StrnCaseCmp(s1,s2,len2) == 0) {
                    return True;
                }
            }
            return False;
        }
    }
    return True;
}
#endif /* __KERNEL__ */

/**
 * Returns True if string s1 ends with the sequence of characters 
 * represented by string s2. If s2 is empty, always return True.
 * The comparison is case sensitive
 */
Bool STRING_EndsWith(Str s1, Str s2)
{
    int len2 = StrLen(s2);
    if (len2 > 0) {
        int len1 = StrLen(s1);
        if (len1 >= len2) {
            if (StrCmp(s2, s1 + len1 - len2) == 0) {
                return True;
            }
        }
        return False;
    }
    return True;
}

/**
 * Case insensitive version of STRING_EndsWith
 */
#ifndef __KERNEL__
Bool STRING_EndsWithNoCase(Str s1, Str s2)
{
    int len2 = StrLen(s2);
    if (len2 > 0) {
        int len1 = StrLen(s1);
        if (len1 >= len2) {
            if (StrCaseCmp(s2, s1 + len1 - len2) == 0) {
                return True;
            }
        }
        return False;
    }
    return True;
}
#endif /* __KERNEL__ */

/**
 * Formats a double value into the shortest possible string without
 * losing precision.
 */
#ifndef __KERNEL__
Str STRING_FormatDouble(StrBuf * sb, double d)
{
    /* Try to achieve exact representation */
    int len, precision = 2;
    for (;;) {
        Char format[16];
        Sprintf(format, TEXT("%%.%df"), precision);
        if (STRBUF_Format(sb, format, d)) {
            double tmp;
            if (PARSE_Double(STRBUF_Text(sb), &tmp)) {
                if (tmp == d || precision >= MAX_DOUBLE_FRACTION_DIGITS) {
                    break;
                } else {
                    precision *= 2;
                    continue;
                }
            }
        }
        return NULL;
    }

    /* strip trailing zeros */
    while ((len = STRBUF_Length(sb)) >= 2 &&
            STRBUF_CharAt(sb,len-1) == '0' &&
            STRBUF_CharAt(sb,len-2) != '.') {
        STRBUF_SetLength(sb, len-1);
    }
    return STRBUF_Text(sb);
}
#endif /* __KERNEL__ */

/**
 * Primitive strcasecmp substitution for Linux kernel environment 
 */
#ifdef _LINUX_KERNEL
int strcasecmp(const char * dst, const char * src)
{
    int f,l;
    do {
        if (((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z')) {
            f -= ('A' - 'a');
        }
        if (((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z')) {
            l -= ('A' - 'a');
        }
    } while (f && (f == l));
    return(f - l);
}
#else /* ! _LINUX_KERNEL */

/**
 * Converts Unicode string to multibyte. The result is allocated from 
 * the heap. The caller is responsible for deallocating the result of
 * conversion. Returns NULL is the argument is NULL or memory allocation 
 * fails.
 */
char * STRING_ToMultiByte(const wchar_t * ws)
{
    char * s = NULL;
    if (ws) {
        int n = wcstombs(NULL, ws, 0);
        if (n >= 0) {
            s = MEM_NewArray(char, n+1);
            if (s) {
                wcstombs(s, ws, n+1);
                s[n] = 0;
            }
        } else {
            for (n = 0; ws[n]; n++) NOTHING;
            s = MEM_NewArray(char, n+1);
            if (s) {
                int i;
                for (i=0; i<n; i++) {
                    ASSERT((ws[i] & (~((wchar_t)0x7f))) == 0);
                    s[i] = (char)ws[i];
                }
                s[i] = 0;
            }
        }
    }
    return s;
}

#ifdef _NO_WCHAR_H
#  define wcsncpy _slib_wcsncpy
static wchar_t * wcsncpy(wchar_t * dest, const wchar_t * src, size_t n)
{
    wchar_t * start = dest;
    while (n && ((*dest++ = *src++) != 0)) n--;
    if (n) *dest++ = 0;
    return start;
}
#endif /* _NO_WCHAR_H */

/**
 * Converts Unicode string of specified length to multibyte string. The 
 * result is allocated from the heap. The caller is responsible for 
 * deallocating the result of conversion. Returns NULL is the argument
 * is NULL or memory allocation fails.
 */
char * STRING_ToMultiByteN(const wchar_t * ws, size_t count)
{
    char * s = NULL;
    wchar_t * ws2 = MEM_NewArray(wchar_t, count+1);
    if (ws2) {
        wcsncpy(ws2, ws, count);
        ws2[count] = 0;
        s = STRING_ToMultiByte(ws2);
        MEM_Free(ws2);
    }
    return s;
}

/**
 * Converts multibyte string to Unicode. The result is allocated from 
 * the heap. The caller is responsible for deallocating the result of
 * conversion. Returns NULL is the argument is NULL or memory allocation 
 * fails.
 */
wchar_t * STRING_ToUnicode(const char * s)
{
    wchar_t * ws = NULL;
    if (s) {
        int n = mbstowcs(NULL, s, 0);
        if (n >= 0) {
            ws = MEM_NewArray(wchar_t, n+1);
            if (s) {
                mbstowcs(ws, s, n+1);
                ws[n] = 0;
            }
        } else {
            n = strlen(s);
            ws = MEM_NewArray(wchar_t, n+1);
            if (ws) {
                int i;
                for (i=0; i<n; i++) {
                    ASSERT((s[i] & (~((char)0x7f))) == 0);
                    ws[i] = s[i];
                }
                ws[i] = 0;
            }
        }
    }
    return ws;
}

#  if defined(__CW32__) && defined(UNICODE)
/**
 * CodeWarrior incorrectly defines swprintf function. This is the
 * right swprintf implementation for the CoreWarrior build.
 */
int _cw_swprintf(wchar_t * dst, const wchar_t * format, ...)
{
    int len;
    va_list va;
    StrBuf sb;
    STRBUF_InitBuf(&sb, dst, INT_MAX);
    va_start(va, format);
    STRBUF_FormatVa(&sb, format, va);
    len = STRBUF_Length(&sb);
    STRBUF_Destroy(&sb);
    va_end(va);
    return len;
}
#  endif /* !(__CW32__ && UNICODE) */

#endif /* ! _LINUX_KERNEL */
