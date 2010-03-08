/*
 * $Id: s_strbuf.c,v 1.54 2005/10/19 21:39:58 slava Exp $
 *
 * Copyright (C) 2000-2005 by Slava Monich
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

#include "s_strbuf.h"
#include "s_util.h"
#include "s_mem.h"

/*==========================================================================*
 *              S T R I N G    B U F F E R 
 *==========================================================================*/

/**
 * Allocates StrBuf from the heap.
 */
StrBuf * STRBUF_Create()
{
    StrBuf * sb = MEM_New(StrBuf);
    if (sb) {
        STRBUF_Init(sb);
        return sb;
    }
    return NULL;
}

/**
 * Delets StrBuf previously allocated by STRBUF_Create()
 */
void STRBUF_Delete(StrBuf * sb)
{
    if (sb) {
        STRBUF_Destroy(sb);
        MEM_Free(sb);
    }
}

/**
 * Initialize the string buffer
 */
void STRBUF_Init(StrBuf * sb)
{
    STRBUF_InitBuf(sb, NULL, 0);
}

/**
 * Initialize string buffer with external buffer of fixed size (typically
 * allocated on stack) 
 */
void STRBUF_InitBuf2(StrBuf * sb, Char* buf, size_t len, size_t bufsize)
{
    ASSERT(sb);
    if (sb) {
        if (buf && bufsize > 0) {
            ASSERT(len < bufsize);
            sb->s = sb->ext = buf;
            sb->alloc = bufsize;
            sb->len = MIN(bufsize-1,len);
            sb->s[sb->len] = 0;
            ASSERT(StrLen(buf) == sb->len);
        } else {
            memset(sb, 0, sizeof(*sb));
        }
    }
}

/**
 * Initialize string buffer with external buffer of fixed size (typically
 * allocated on stack) 
 */
void STRBUF_InitBuf(StrBuf * sb, Char* buf, size_t bufsize)
{
    STRBUF_InitBuf2(sb, buf, 0, bufsize);
}

/**
 * Destroy the contents of the string buffer.
 */
void STRBUF_Destroy(StrBuf * sb)
{
    if (sb) {
        if (sb->alloc) {
            ASSERT(sb->s);
            if (sb->s != sb->ext) MEM_Free(sb->s);
            sb->alloc = 0;
        }
        sb->len = 0;
        sb->s = NULL;
    }
}

/**
 * Returns length of the string.
 */
size_t STRBUF_Length(const StrBuf * sb)
{
    ASSERT(sb);
    if (sb) {
        ASSERT(sb->len == 0 || StrLen(sb->s) == sb->len);
        return sb->len;
    }
    return 0;
}

/**
 * Makes sure that length of the string does not exceed maxlen
 */
void STRBUF_SetLength(StrBuf * sb, size_t maxlen)
{
    ASSERT(sb);
    if (sb && sb->len > maxlen) {
        sb->len = maxlen;
        sb->s[maxlen] = 0;
    }
}

/**
 * Tests if two strings are equal. If any of them is NULL, the result
 * is False.
 */
Bool STRBUF_Equals(const StrBuf * sb1, const StrBuf * sb2)
{
    if (sb1 && sb2 && sb1->s && sb2->s) {

        /* try some quick checks */
        if (sb1 == sb2) {
            return True;
        } else if (sb1->s == sb2->s) {
            return True;
        } else if (sb1->len != sb2->len) {
            return False;
        }

        /* strings may be equal; compare them */
        return BoolValue(StrCmp(sb1->s, sb2->s) == 0);
    }
    ASSMSG("STRBUF_Equals: invalid arguments!");
    return False;
}

/**
 * Tests if two strings are equal. If any of them is NULL, the result
 * is False.
 */
Bool STRBUF_EqualsTo(const StrBuf * sb, Str s)
{
    if (sb && s) {
        if (sb->len == 0) {
            return BoolValue(!s[0]);
        } else if (sb->s == s) {
            return True;
        } else {
            return BoolValue(StrCmp(sb->s, s) == 0);
        }
    }
    ASSMSG("STRBUF_EqualsTo: invalid arguments!");
    return False;
}

/**
 * Clear the string buffer.
 */
void STRBUF_Clear(StrBuf * sb)
{
    ASSERT(sb);
    if (sb) {
        sb->len = 0;
        if (sb->alloc) sb->s[0] = 0;
    }
}

/**
 * Erase some characters from the string, starting from index 
 * 'from' (inclusive), ending at 'to' (exclusive)
 */
void STRBUF_Erase(StrBuf * sb, size_t from, size_t to)
{
    ASSERT(sb);
    if (sb) {
        ASSERT(from <= to);
        ASSERT(to <= sb->len);
        if (to > sb->len) to = sb->len;
        if (from < to) {
            memmove(sb->s+from, sb->s+to, sizeof(Char)*(sb->len-to+1));
            sb->len -= (to - from);
            ASSERT(!sb->s[sb->len]);
        }
    }
}

/**
 * Removes leading and trailing whitespaces. Returns True if at least one
 * whitespace has been removed, False if string didn't change.
 */
Bool STRBUF_Trim(StrBuf * sb)
{
    Bool trimmed = False;
    if (sb->len > 0) {
        size_t pos;

        /* trim trailing whitespaces */
        pos = sb->len;
        while (pos > 0 && IsSpace(sb->s[pos-1])) pos--;
        if (pos < sb->len) {
            trimmed = True;
            STRBUF_Erase(sb, pos, sb->len);
        }

        /* trim leading whitespaces */
        pos = 0;
        while (pos < sb->len && IsSpace(sb->s[pos])) pos++;
        if (pos > 0) {
            trimmed = True;
            STRBUF_Erase(sb, 0, pos);
        }
    }
    return trimmed;
}

/**
 * Returns character at specified position.
 */
Char STRBUF_CharAt(const StrBuf * sb, size_t pos)
{
    ASSERT(sb);
    if (sb) {
        ASSERT(pos <= sb->len);
        if (pos <= sb->len) {
            return sb->s[pos];
        }
    }
    return 0;
}

/**
 * Returns first character of the string, 0 if string is empty
 */
Char STRBUF_FirstChar(const StrBuf * sb)
{
    ASSERT(sb);
    if (sb && sb->len > 0) {
        return sb->s[0];
    }
    return 0;
}

/**
 * Returns last character of the string, 0 if string is empty
 */
Char STRBUF_LastChar(const StrBuf * sb)
{
    ASSERT(sb);
    if (sb && sb->len > 0) {
        return sb->s[sb->len-1];
    }
    return 0;
}

/**
 * Returns index of the specified character, -1 if character not found
 */
int STRBUF_IndexOf(const StrBuf * sb, Char ch)
{
    ASSERT(sb);
    if (sb && sb->len > 0) {
        Str found = StrChr(sb->s, ch);
        if (found) {
            int pos = found - sb->s;
            ASSERT(pos >= 0);  /* if negative, string is WAY too long */
            return pos;
        }
    }
    return -1;
}

/**
 * Returns last index of the specified character, -1 if character not found
 */
int STRBUF_LastIndexOf(const StrBuf * sb, Char ch)
{
    ASSERT(sb);
    if (sb && sb->len > 0) {
        Str found = StrrChr(sb->s, ch);
        if (found) {
            int pos = found - sb->s;
            ASSERT(pos >= 0);  /* if negative, string is WAY too long */
            return pos;
        }
    }
    return -1;
}

/**
 * Finds the first occurrence of s in sb. Returns the position where
 * the occurrence was found, or -1 if none was found, or if the string
 * buffer is empty.
 */
int STRBUF_Find(const StrBuf * sb, Str s)
{
    return STRBUF_FindFrom(sb, 0, s);
}

/**
 * Finds the first occurrence of s in sb, starting with the specified
 * position. Returns the position where the occurrence was found, or -1
 * none was found, the position is invalid or if the string buffer is 
 * empty.
 */
int STRBUF_FindFrom(const StrBuf * sb, size_t pos, Str s)
{
    if (sb->len > 0 && pos < sb->len && s) {
        if (!*s) {
            return 0;  /* like strstr does */
        } else {
            Str found = StrStr(sb->s+pos, s);
            if (found) {
                ASSERT((found - sb->s) < (int)sb->len);
                return (found - sb->s);
            }
        }
    }
    return -1;
}

/**
 * Finds the last occurrence of s in sb. Returns the position where
 * the occurrence was found, or -1 if none was found, or if either
 * string is empty.
 */
int STRBUF_FindLast(const StrBuf * sb, Str s)
{
    if (sb->len > 0 && s && *s) {
        size_t slen = StrLen(s);
        if (sb->len >= slen) {
            Str ptr = sb->s + (sb->len-slen);
            while (ptr >= sb->s) {
                Str s1 = ptr;
                Str s2 = s;
                while (*s1 && *s2 && !(*s1-*s2)) s1++, s2++;
                if (!*s2) {
                    return (ptr - sb->s);
                }
                ptr--;
            }
        }
    }
    return -1;
}

#ifndef __KERNEL__
/**
 * Finds the first occurrence of s in sb, ignoring the case. Returns the
 * position where the occurrence was found, or -1 if none was found, or if
 * the string buffer is empty. The string comparison is case-insentive.
 */
int STRBUF_FindNoCase(const StrBuf * sb, Str s)
{
    return STRBUF_FindFromNoCase(sb, 0, s);
}

/**
 * Finds the first occurrence of s in sb, starting with the specified
 * position, ignoring the case. Returns the position where the occurrence
 * was found, or -1 none was found, the position is invalid or if the
 * string buffer is empty.  The string comparison is case-insentive.
 */
int STRBUF_FindFromNoCase(const StrBuf * sb, size_t pos, Str s)
{
    if (sb->len > 0 && pos < sb->len && s) {
        if (!*s) {
            return 0;  /* like strstr does */
        } else {
            size_t len = StrLen(s);
            if (len+pos <= sb->len) {
                size_t i, max = sb->len-len;
                for (i=pos; i<=max; i++) {
                    if (StrnCaseCmp(sb->s+i, s, len) == 0) {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

/**
 * Finds the last occurrence of s in sb. Returns the position where
 * the occurrence was found, or -1 if none was found, or if either
 * string is empty. The string comparison is case-insentive.
 */
int STRBUF_FindLastNoCase(const StrBuf * sb, Str s)
{
    if (sb->len > 0 && s && *s) {
        size_t len = StrLen(s);
        if (sb->len >= len) {
            Str ptr = sb->s + (sb->len-len);
            while (ptr >= sb->s) {
                if (StrnCaseCmp(ptr, s, len) == 0) {
                    return (ptr - sb->s);
                }
                ptr--;
            }
        }
    }
    return -1;
}

#endif /* __KERNEL__ */

/**
 * Replaces all occurrences of c1 with c2, returns number of replaced 
 * characters
 */
int STRBUF_Replace(StrBuf * sb, Char c1, Char c2)
{
    int n = 0;
    ASSERT(sb && c1 && c2);
    if (sb && c1 && c2 && (c1 != c2)) {
        size_t i;
        for (i=0; i<sb->len; i++) {
            if (sb->s[i] == c1) {
                sb->s[i] = c2;
                n++;
            }
        }
    }
    return n;
}

/**
 * Returns all occurrences of substring 'from' to 'to'. Returns
 * the number of replacements made, or -(n+1) if an error occured,
 * where n is the number of successful replacements (we don't
 * undo the damage).
 */
int STRBUF_ReplaceStr(StrBuf * sb, Str from, Str to)
{
    int n = 0;
    Char* found;
    int len1 = StrLen(from);
    int len2 = StrLen(to);
    if (len1 > len2) {
        /* start from the end to reduce the amount of data to copy */
        int pos = sb->len;
        while (pos > 0) {
            size_t savelen = sb->len;
            sb->len = pos;
            pos = STRBUF_FindLast(sb, from);
            sb->len = savelen;
            if (pos >= 0) {
                /* get rid of extra characters */
                STRBUF_Erase(sb, pos+len2, pos+len1);
                memcpy(sb->s+pos, to, sizeof(Char)*len2);
                n++;
            }
        }
    } else if (len1 < len2) {
        int pos = 0;
        while (pos >= 0) {
            pos = STRBUF_FindFrom(sb, pos, from);
            if (pos >= 0) {
                /* insert extra characters */
                STRBUF_Insert(sb, to+len1, pos+len1);
                memcpy(sb->s+pos, to, sizeof(Char)*len1);
                pos += len2;
                n++;
            }
        }
    } else if (len1) {
        size_t off = 0;
        while ((found = StrStr(sb->s+off,from)) != NULL) {
            n++;
            memcpy(found, to, sizeof(Char)*len2);
            off = (found - sb->s) + len2;
            if (off > sb->len-len1) {
                break;
            }
        }
    }
    return n;
}

/**
 * Tests if this string starts with the specified prefix.
 */
STATIC Bool STRBUF_StartsWithOffset(const StrBuf * sb, size_t off, Str s)
{
    ASSERT(sb);
    if (sb) {
        if (off <= sb->len) {
            size_t i = off;
            while (i<sb->len && *s) {
                if (sb->s[i++] != *s++) {
                    return False;
                }
            }
            return BoolValue(!(*s));
        }
    }
    return False;
}

/**
 * Tests if this string starts with the specified prefix.
 */
Bool STRBUF_StartsWith(const StrBuf * sb, Str s)
{
    return STRBUF_StartsWithOffset(sb, 0, s);
}

/**
 * Tests if this string ends with the specified suffix.
 */
Bool STRBUF_EndsWith(const StrBuf * sb, Str s)
{
    ASSERT(sb);
    if (s && sb) {
        size_t len = StrLen(s);
        if (len <= sb->len) {
            return STRBUF_StartsWithOffset(sb, sb->len - len, s);
        }
    }
    return False;
}

#ifndef __KERNEL__
/**
 * Tests if this string starts with the specified prefix. Ignores the case.
 */
STATIC Bool STRBUF_StartsWithOffsetNoCase(const StrBuf* sb, size_t off, Str s)
{
    ASSERT(sb);
    if (sb) {
        if (off <= sb->len) {
            size_t i = off;
            while (i<sb->len && *s) {
                if (sb->s[i++] != *s++) {
                    /* switch to more expensive case-insensitive algorithm */
                    size_t len1 = sb->len-(--i);
                    size_t len2 = StrLen(s--)+1;
                    if (len1 >= len2) {
                        if (StrnCaseCmp(sb->s+i,s,len2) == 0) {
                            return True;
                        }
                    }
                    return False;
                }
            }
            return BoolValue(!(*s));
        }
    }
    return False;
}

/**
 * Tests if this string starts with the specified prefix. Ignores the case.
 */
Bool STRBUF_StartsWithNoCase(const StrBuf * sb, Str s)
{
    return STRBUF_StartsWithOffsetNoCase(sb, 0, s);
}

/**
 * Tests if this string ends with the specified suffix. Ignores the case.
 */
Bool STRBUF_EndsWithNoCase(const StrBuf * sb, Str s)
{
    ASSERT(sb);
    if (s && sb) {
        size_t len = StrLen(s);
        if (len <= sb->len) {
            return STRBUF_StartsWithOffsetNoCase(sb, sb->len - len, s);
        }
    }
    return False;
}
#endif /* __KERNEL__ */

/**
 * Converts all of the characters in this string to upper case
 */
void STRBUF_ToUpperCase(StrBuf * sb)
{
    size_t i;
    for (i=0; i<sb->len; i++) {
        sb->s[i] = (Char)ToUpper(sb->s[i]);
    }
}

/**
 * Converts all of the characters in this string to lower case
 */
void STRBUF_ToLowerCase(StrBuf * sb)
{
    size_t i;
    for (i=0; i<sb->len; i++) {
        sb->s[i] = (Char)ToLower(sb->s[i]);
    }
}

/**
 * Retruns string represented by this string buffer. If no memory has ever
 * been allocated by this string buffer, return statically allocated empty
 * string.
 */
Str STRBUF_GetString(const StrBuf * sb)
{
    ASSERT(sb);
    if (sb && sb->len && sb->s) {
        return sb->s;
    } 
    return TEXT("");
}

/**
 * Reallocate the buffer if necessary, to ensure that it may contain at 
 * least a string of specified length (i.e. minlen+1 characters).
 */
Bool STRBUF_Alloc(StrBuf * sb, size_t minlen)
{
    int minsize = minlen+1;
    int oldsize = sb->alloc;
    if (minsize > oldsize) {
        Char* newbuf;
        int newsize = (oldsize * 3)/2 + 1;
        if (newsize < minsize) newsize = minsize;

        /* cannot realloc external buffer */
        if (!sb->s || (sb->s && (sb->s == sb->ext))) {
            newbuf = MEM_NewArray(Char,newsize);
            if (newbuf) {
                if (sb->len > 0) {
                    memcpy(newbuf, sb->s, sizeof(Char)*(sb->len + 1));
                } else {
                    newbuf[0] = 0;
                }
            } 
        } else {
#ifdef NO_REALLOC
            newbuf = MEM_NewArray(Char, newsize);
            if (newbuf && sb->s) {
                memcpy(newbuf, sb->s, sb->alloc * sizeof(Char));
                MEM_Free(sb->s);
            }
#else
            newbuf = (Char*)MEM_Realloc(sb->s, newsize * sizeof(Char));
#endif /* NO_REALLOC */
        }

        /* failed to allocate new buffer - bail out */
        if (!newbuf) return False;

        /* otherwise switch to the new buffer */
        sb->s = newbuf;
        sb->alloc = newsize;
    }
    return True;
}

/**
 * Allocates a NULL terminated string initialized with the current contents
 * of the string buffer. The caller is responsible for deallocating the
 * returned string with MEM_Free. Returns NULL on failure.
 */
Char * STRBUF_Dup(const StrBuf * sb)
{
    Char * copy = NULL;
    if (sb) {
        copy = MEM_NewArray(Char,sb->len+1);
        if (copy) {
            StrCpy(copy, sb->s);
            ASSERT(StrLen(copy) == sb->len);
        }
    }
    return copy;
}

/**
 * Copies a string into the buffer
 */
Bool STRBUF_Copy(StrBuf * sb, Str s)
{
    ASSERT(s);
    if (s) {
        STRBUF_Clear(sb);
        return STRBUF_Append(sb, s);
    }
    return False;
}

/**
 * Replace contents of the string buffer with the specified string.
 */
Bool STRBUF_CopyN(StrBuf * sb, Str s, size_t size)
{
    ASSERT(s);
    if (s) {
        STRBUF_Clear(sb);
        return STRBUF_AppendN(sb, s, size);
    }
    return False;
}

/**
 * Append a string to the buffer
 */
Bool STRBUF_Append(StrBuf * sb, Str s)
{
    if (s) {
        int len = StrLen(s);
        if (STRBUF_Alloc(sb, sb->len + len)) {
            StrCpy(sb->s + sb->len, s);
            sb->len += len;
            ASSERT(StrLen(sb->s) == sb->len);
            return True;
        }
    }
    return False;
}

/**
 * Append the contents of sb2 to sb1. This is faster than STRBUF_Append
 * because it does not need to calculate the length of the second string.
 * Also, memcpy is probably slightly faster than strcpy.
 */
Bool STRBUF_AppendBuf(StrBuf * sb1, const StrBuf * sb2)
{
    if (sb2) {
        /* allocate necessary amount of space */
        if (STRBUF_Alloc(sb1, sb1->len + sb2->len)) {
            memcpy(sb1->s + sb1->len, sb2->s, sizeof(Char)*sb2->len);
            sb1->len += sb2->len;
            sb1->s[sb1->len] = 0;
            ASSERT(StrLen(sb1->s) == sb1->len);
            return True;
        }
    }
    return False;
}

/**
 * Append no more than N characters to the buffer
 */
Bool STRBUF_AppendN(StrBuf * sb, Str s, size_t max)
{
    if (s && max > 0) {

        /* calculate the number of bytes we are about to copy... */
        size_t n = 0;
        Str sp = s;
        while (n < max && *sp++) n++;

        /* allocate necessary amount of space */
        if (STRBUF_Alloc(sb, sb->len + n)) {
            memcpy(sb->s + sb->len, s, sizeof(Char)*n);
            sb->len += n;
            sb->s[sb->len] = 0;
            ASSERT(StrLen(sb->s) == sb->len);
            return True;
        }
    }

    /* always return True if appending 0 characters */
    return BoolValue(max == 0);
}

/**
 * Append an integer (string representation of it) to the buffer
 */
Bool STRBUF_AppendInt(StrBuf * sb, int i)
{
    Char buf[32];
    Char* s = STRING_Format(buf, COUNT(buf), TEXT("%d"), i);
    Bool success = STRBUF_Append(sb, s);
    if (s != buf) MEM_Free(s);
    return success;
}

/**
 * Append a character to the buffer.
 */
Bool STRBUF_AppendChar(StrBuf * sb, Char c)
{
    if (STRBUF_Alloc(sb, sb->len+1)) {
        sb->s[sb->len++] = c;
        sb->s[sb->len] = 0;
        return True;
    }
    return False;
}

/**
 * Append an boolean (string representation of it) to the buffer
 */
Bool STRBUF_AppendBool(StrBuf * sb, Bool b)
{
    Str s = (b ? TRUE_STRING : FALSE_STRING);
    return STRBUF_Append(sb, s);
}

/**
 * Append an double (string representation of it) to the buffer
 */
#ifndef __KERNEL__
Bool STRBUF_AppendDouble(StrBuf * sb, double d)
{
    Char buf[64];
    Char* s = STRING_Format(buf, COUNT(buf), TEXT("%f"), d);
    Bool success = STRBUF_Append(sb, s);
    if (s != buf) MEM_Free(s);
    return success;
}

/**
 * Append time to the buffer in the form "Wed Jun 30 21:49:08 1993 EST"
 */
Bool STRBUF_AppendTime(StrBuf * sb, Time t)
{
    Str s = TIME_ToString(t);
    if (s) {
        if (STRBUF_Append(sb, s)) {
            Bool success = True;
#if !defined(_WIN32_WCE) && !defined(__SYMBIAN32__)
            static Bool tzisset = False;
            if (!tzisset) {
                tzisset = True;
                tzset();
            }
            if (tzname[0]) {
                if (STRBUF_AppendChar(sb, ' ')) {
#  ifdef UNICODE
                    Char * tmp = STRING_ToUnicode(tzname[0]);
                    if (tmp) {
                        success = STRBUF_Append(sb, tmp);
                        MEM_Free(tmp);
                    }
#  else  /* !UNICODE */
                    success = STRBUF_Append(sb, tzname[0]);
#  endif /* !UNICODE */
                }
            }
#endif /* !_WIN32_WCE */
            return success;
        }
    }
    return False;
}

/**
 * Formats time in the form "Wed Jun 30 21:49:08 1993 EST"
 */
Bool STRBUF_FormatTime(StrBuf * sb, Time t)
{
    STRBUF_Clear(sb);
    return STRBUF_AppendTime(sb, t);
}
#endif /* !__KERNEL__ */

/**
 * Appends the specified character to the string until its length
 * reaches specified minimum length. If len parameter is less than 
 * the string length, the string is not modified (including the case 
 * if len is negative) 
 */
Bool STRBUF_Inflate(StrBuf * sb, size_t len, Char fill)
{
    Bool success = True;
    if (sb->len < len) {
        success = STRBUF_Alloc(sb, sb->len + len);
        if (success) {
            memset(sb->s + sb->len, fill, sizeof(Char)*(len - sb->len));
            sb->s[len] = 0;
            sb->len = len;
        }
    }
    return success;
}

/**
 * Inserts string at specified position. The length of the string is
 * given as a parameter.
 */
Bool STRBUF_InsertN(StrBuf * sb, size_t pos, Str s, size_t len)
{
    ASSERT(pos <= sb->len);
    if (pos <= sb->len) {
        if (s && len > 0) {
            DEBUG_ONLY(size_t i; for (i=0; i<len; i++) ASSERT(s[i]));
            if (STRBUF_Alloc(sb, sb->len + len)) {
                memmove(sb->s+pos+len,sb->s+pos,sizeof(Char)*(sb->len-pos+1));
                memmove(sb->s+pos, s, sizeof(Char)*len);
                sb->len += len;
            } else {
                return False;
            }
        }
        return True;
    }
    return False;
}

/**
 * Inserts string at specified position.
 */
Bool STRBUF_Insert(StrBuf * sb, Str s, size_t pos)
{
    return STRBUF_InsertN(sb, pos, s, StrLen(s));
}

/**
 * Inserts character at specified position. Inserrting NULL character has
 * no effect.
 */
Bool STRBUF_InsertChar(StrBuf * sb, Char c, size_t pos)
{
    ASSERT(c);  /* should not insert NULL character */
    return STRBUF_InsertN(sb, pos, &c, 1);
}

/**
 * Store formatted message in the buffer. Appends the formatted string to
 * the current contents of the buffer.
 */
Bool STRBUF_AppendFormatVa(StrBuf * sb, Str format, va_list va)
{
    if (!sb->s || (sb->alloc <= (sb->len+1))) {
        if (!STRBUF_Alloc(sb, sb->len + 100)) {
            return False;
        }
    }

    for (;;)  {
        size_t avail = sb->alloc - sb->len;
        size_t newsize = sb->alloc;

        /* try to print in the allocated space. */
        int nchars = Vsnprintf(sb->s + sb->len, avail, format, va);
        
        /* if that worked, we are done. */
        if (nchars > -1 && nchars < (int)avail) {
            sb->len += nchars;
            break;
        }

        /* else try again with more space. */
        if (nchars > -1) {
            newsize = sb->len + nchars;      /* precisely what is needed */
        } else {
            newsize *= 2;                    /* twice the old size */
        }

        /* if memory allocation fails, break the loop */
        if (!STRBUF_Alloc(sb, newsize)) {
            sb->s[sb->len] = 0;
            return False;
        }
    }

    ASSERT(sb->len == StrLen(sb->s));
    return True;
}

/**
 * Store formatted message in the buffer.
 */
Bool STRBUF_FormatVa(StrBuf * sb, Str format, va_list va)
{
    STRBUF_Clear(sb);
    return STRBUF_AppendFormatVa(sb, format, va);
}

/**
 * Store formatted message in the buffer.
 */
Bool STRBUF_Format(StrBuf * sb, Str format, ...)
{
    Bool ok;
    va_list va;
    va_start(va, format);
    ok = STRBUF_FormatVa(sb, format, va);
    va_end(va);
    return ok;
}

/**
 * Append formatted message in the buffer.
 */
Bool STRBUF_AppendFormat(StrBuf * sb, Str format, ...)
{
    Bool ok;
    va_list va;
    va_start(va, format);
    ok = STRBUF_AppendFormatVa(sb, format, va);
    va_end(va);
    return ok;
}
