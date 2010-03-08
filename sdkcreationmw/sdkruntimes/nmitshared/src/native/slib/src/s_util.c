/*
 * $Id: s_util.c,v 1.88 2005/02/20 20:31:30 slava Exp $
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

#include "s_util.h"
#include "s_file.h"
#include "s_strbuf.h"
#include "s_random.h"
#include "s_mem.h"

#include <math.h>

#if !defined(_WIN32_WCE) && !defined(__KERNEL__)
#  define HAVE_ERRNO 1
#endif /* !_WIN32_WCE && !__KERNEL__ */

#ifdef __linux__
#  define PROC_STAT_CPU_COUNT "cpus detected"
#  define PROC_STAT_CPU "processor"
#  ifndef STATFILE
#  define STATFILE "/proc/cpuinfo"
#  endif /* STATFILE */
#endif /* __linux__ */

#ifdef __sun
#  include <kstat.h>
#endif /* __sun */

#if defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__)
#  include <sys/sysctl.h>
#endif /* __OpenBSD__ || __NetBSD__ || __FreeBSD__ */

/*==========================================================================*
 *              S Y S T E M    I N F O R M A T I O N
 *==========================================================================*/

/**
 * Returns number of active CPUs.
 */
int SYSTEM_CountCPU() 
{
    static int CPU_count = 0;

    /* detect number of CPUs when we first time get here */
    if (!CPU_count) {

#if defined(_NT_KERNEL)

        NTSTATUS Status;
        SYSTEM_BASIC_INFORMATION BasicInfo;
        memset(&BasicInfo, 0, sizeof(BasicInfo));
        Status = NtQuerySystemInformation(SystemBasicInformation,&BasicInfo,
            sizeof(BasicInfo),NULL);
        ASSERT(NT_SUCCESS(Status));
        if (NT_SUCCESS(Status)) {
            CPU_count = BasicInfo.NumberOfProcessors;
        }

#elif defined(_WIN32)

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        CPU_count = sysInfo.dwNumberOfProcessors;

#elif defined(__linux__) && !defined(__KERNEL__)

        File * f = FILE_Open(STATFILE, "r", PlainFile);
        if (f) {
            StrBuf64 buf;
            STRBUF_InitBufXXX(&buf);

            /* first try to find "cpus detected" field */
            while (FILE_ReadLine(f, &buf.sb)) {
                if (STRBUF_StartsWith(&buf.sb, PROC_STAT_CPU_COUNT)) {
                    int colon = STRBUF_IndexOf(&buf.sb, ':');
                    if (colon > 0) {
                        Str s = buf.sb.s + colon + 1;
                        while (*s && IsSpace(*s)) s++;
                        VERIFY(PARSE_Int(s, &CPU_count, 0));
                    }
                    break;
                }
            }
            FILE_Close(f);

            /* try to count the number of CPUs mentioned in the stat file */
            if (CPU_count <= 0) {
                f = FILE_Open(STATFILE, "r", PlainFile);
                if (f) {
                    CPU_count = 0;
                    while (FILE_ReadLine(f, &buf.sb)) {
                        if (STRBUF_StartsWith(&buf.sb, PROC_STAT_CPU)) {
                            CPU_count++;
                        }
                    }
                    FILE_Close(f);
                }
            }

            STRBUF_Destroy(&buf.sb);
        }

#elif defined(__sun)

        kstat_ctl_t * kc = kstat_open();
        if (kc) {
            static const char KS_MODULE [] = "unix";
            static const char KS_NAME [] = "system_misc";
            static const char KS_DATA [] = "ncpus";
            
            kstat_t * ks;
            
            /* copy strings to avoid compilation warnings... */
            char ks_module[COUNT(KS_MODULE)];
            char ks_name[COUNT(KS_NAME)];
            char ks_data[COUNT(KS_DATA)];
            StrCpy(ks_module, KS_MODULE);
            StrCpy(ks_name, KS_NAME);
            StrCpy(ks_data, KS_DATA);

            ks = kstat_lookup(kc, ks_module, 0, ks_name);
            if (ks) {
                if (kstat_read(kc, ks, 0) != -1) {
                    kstat_named_t * kn = kstat_data_lookup(ks, ks_data);
                    if (kn && kn->value.ui32 > 0) {
                        CPU_count = kn->value.ui32;
                    }
                }
            }
            kstat_close(kc);
        }
#elif defined (__OpenBSD__) || defined (__NetBSD__) || defined (__FreeBSD__)

        int buflen = sizeof(CPU_count);
        int mib[2];

        mib[0] = CTL_HW;
        mib[1] = HW_NCPU;
        if (sysctl(mib, COUNT(mib), &CPU_count, &buflen, NULL, 0) != 0) {
            TRACE1("SYSTEM: sysctl error ",errno);
            CPU_count = 1;
        }
#else

        /* autodetection NOT currently implemented for other platforms */ 
        TRACE("SYSTEM: auto-detection of number of CPUs not implemented!\n");
        CPU_count = 1;

#endif

        /* final sanity check */
        ASSERT(CPU_count > 0);
        if (CPU_count <= 0) CPU_count = 1;
        Verbose(TEXT("SYSTEM: %d CPU detected\n"),CPU_count);
    }
    return CPU_count;
}

/*==========================================================================*
 *              U R L    E N C O D I N G
 *==========================================================================*/

/**
 * URL encodes a string into a string buffer. On success, returns pointer 
 * to the string buffer data. The URL encoded string is guaranteed to be a
 * 7-bit ASCII string consisting entirely of valid URL characters. If buffer
 * is empty, memory allocation fails or source string contains characters 
 * that cannot be URL encoded, returns NULL. Does not destroy the original
 * contents of the string buffer (appends the result). 
 */
Str URL_Encode(StrBuf * dest, Str src)
{
    /* The list of characters that are not encoded has been
     * determined as follows:
     *
     * RFC 2396 states:
     * -----
     * Data characters that are allowed in a URI but do not have a
     * reserved purpose are called unreserved.  These include upper
     * and lower case letters, decimal digits, and a limited set of
     * punctuation marks and symbols. 
     *
     * unreserved  = alphanum | mark
     *
     * mark        = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
     *
     * Unreserved characters can be escaped without changing the
     * semantics of the URI, but this should not be done unless the
     * URI is being used in a context that does not allow the
     * unescaped character to appear.
     * -----
     *
     * It appears that both Netscape and Internet Explorer escape
     * all special characters from this list with the exception
     * of "-", "_", ".", "*". While it is not clear why they are
     * escaping the other characters, perhaps it is safest to
     * assume that there might be contexts in which the others
     * are unsafe if not escaped. Therefore, we will use the same
     * list. It is also noteworthy that this is consistent with
     * O'Reilly's "HTML: The Definitive Guide" (page 164).
     *
     * As a last note, Intenet Explorer does not encode the "@"
     * character which is clearly not unreserved according to the
     * RFC. We are being consistent with the RFC in this matter,
     * as is Netscape.
     *
     */
    if (src) {
        Char c;
        int orig = STRBUF_Length(dest);
        while ((c = *src++) != 0) {
            switch (c) {

            /* valid URL characters */
            case 'a': case 'b': case 'c': case 'd': 
            case 'e': case 'f': case 'g': case 'h': 
            case 'i': case 'j': case 'k': case 'l': 
            case 'm': case 'n': case 'o': case 'p': 
            case 'q': case 'r': case 's': case 't': 
            case 'u': case 'v': case 'w': case 'x': 
            case 'y': case 'z': case 'A': case 'B': 
            case 'C': case 'D': case 'E': case 'F': 
            case 'G': case 'H': case 'I': case 'J': 
            case 'K': case 'L': case 'M': case 'N': 
            case 'O': case 'P': case 'Q': case 'R': 
            case 'S': case 'T': case 'U': case 'V': 
            case 'W': case 'X': case 'Y': case 'Z': 
            case '0': case '1': case '2': case '3': 
            case '4': case '5': case '6': case '7': 
            case '8': case '9':

            /*
             * (some) unreserved characters not being escaped (see the 
             * long comment above) 
             */
            case '-': case '_': case '.': case '*': 

                /* append this character */
                if (!STRBUF_AppendChar(dest,c)) {
                    STRBUF_SetLength(dest, orig);
                    return NULL;
                }
                break;

            default:
#ifdef UNICODE
                /* cannot encode wide characters */
                if ((c & (~((Char)0xff))) != 0) {
                    STRBUF_SetLength(dest, orig);
                    return NULL;
                }
                /* fall through */
#endif /* UNICODE */

            /*
             * (some) unreserved characters being escaped (see the 
             * long comment above) 
             */
            case '!':  case '~': case '(': case ')':
            case '\\': 

            /* reserved characters */
            case ';': case '/': case '?': case ':': 
            case '@': case '&': case '=': case '+': 
            case '$': case ',':
                if (!STRBUF_AppendFormat(dest,TEXT("%%%02X"),((int)c)&0xff)) {
                    STRBUF_SetLength(dest, orig);
                    return NULL;
                }
            }
        }
        return STRBUF_Text(dest);
    }
    return NULL;
}

/**
 * Encodes the specified characters in the data as escape triples.
 * If the input string is NULL, memory allocation fails or source
 * string contains characters that cannot be URL encoded, returns
 * NULL. Does not destroy the original contents of the string buffer 
 * (appends the result). 
 */
Str URL_EncodeChars(StrBuf * dest, Str src, Str esc)
{
    if (src) {
        Char c;
        int origLen = STRBUF_Length(dest);
        while ((c = *src++) != 0) {

#ifdef UNICODE
            /* cannot encode wide characters */
            if ((c & (~((Char)0xff))) != 0) {
                STRBUF_SetLength(dest, origLen);
                return NULL;
            }
#endif /* UNICODE */

            if (StrChr(esc, c)) {
                /* encode this character */
                if (!STRBUF_AppendFormat(dest,TEXT("%%%02X"),((int)c)&0xff)) {
                    STRBUF_SetLength(dest, origLen);
                    return NULL;
                }
            
            } else {
                /* append this character */
                if (!STRBUF_AppendChar(dest,c)) {
                    STRBUF_SetLength(dest, origLen);
                    return NULL;
                }
            }
        }
        return STRBUF_Text(dest);
    }
    return NULL;
}

/**
 * Decodes URL encoded string into a string buffer. On success, returns 
 * pointer to string buffer data, NULL if memory allocation fails. Does 
 * not destroy the original contents of the string buffer (appends the 
 * result). 
 */
Str URL_Decode(StrBuf * dest, Str src)
{
    if (src) {
        Char c;
        int orig = STRBUF_Length(dest);
        while ((c = *src++) != 0) {
            if (c == '%' && IsXdigit(src[0]) && IsXdigit(src[1])) {
                Char x1 = src[0];
                Char x2 = src[1];
                if (x1 >= '0' && x1 <= '9') {
                    x1 = (Char)(x1 - '0');
                } else if (x1 >= 'A' && x1 <= 'F') {
                    x1 = (Char)(x1 - 'A' + 10);
                } else if (x1 >= 'a' && x1 <= 'f') {
                    x1 = (Char)(x1 - 'a' + 10);
                } else {
                    ASSMSG("Not a hex digit!");
                }
                if (x2 >= '0' && x2 <= '9') {
                    x2 = (Char)(x2 - '0');
                } else if (x2 >= 'A' && x2 <= 'F') {
                    x2 = (Char)(x2 - 'A' + 10);
                } else if (x2 >= 'a' && x2 <= 'f') {
                    x2 = (Char)(x2 - 'a' + 10);
                } else {
                    ASSMSG("Not a hex digit!");
                }
                c = (Char)(x2 | (x1 << 4));
                src += 2;
            }
            if (!STRBUF_AppendChar(dest,c)) {
                STRBUF_SetLength(dest, orig);
                return NULL;
            }
        }
        return STRBUF_Text(dest);
    }
    return NULL;
}

/*==========================================================================*
 *              P A R S E
 *==========================================================================*/

/**
 * Determine whether given string starts with specified non-empty character
 */
STATIC Bool startsWith(Str s, char c) 
{
    if (s) {
        ASSERT(!IsSpace(c));
        while (*s && IsSpace(*s)) s++;
        return BoolValue(*s == c);
    }
    return False;
}

/**
 * Parse a short, returning True on success.
 */
#ifdef __KERNEL__
#define BOOL_StrnCmp(_s1,_s2,_len) strncmp(_s1, _s2, _len)
#else /* __KERNEL__ */
#define BOOL_StrnCmp(_s1,_s2,_len) StrnCaseCmp(_s1, _s2, _len)
#endif /* __KERNEL__ */

Bool PARSE_Bool(Str s, Bool * b) 
{
    if (s) {
        int n, len;
        Str s1;

        while (IsSpace(*s) && *s) s++;
        n = StrLen(s);

        len = StrLen(TRUE_STRING);
        if (n >= len && BOOL_StrnCmp(s, TRUE_STRING, len) == 0) {
            for (s1 = s+len; *s1 && IsSpace(*s1); s1++) NOTHING;
            if (!*s1) {
                if (b) *b = True;
                return True;
            }
        }

        len = StrLen(FALSE_STRING);
        if (n >= len && BOOL_StrnCmp(s, FALSE_STRING, len) == 0) {
            for (s1 = s+len; *s1 && IsSpace(*s1); s1++) NOTHING;
            if (!*s1) {
                if (b) *b = False;
                return True;
            }
        }
    }
    return False;
}

/**
 * Parse a byte, returning True on success.
 */
Bool PARSE_Byte(Str s, char * n, int base) 
{
    long number;
    if (PARSE_Long(s, &number, base)) {
        if (number >= (long)CHAR_MIN && number <= (long)CHAR_MAX) {
            if (n) *n = (char)number;
            return True;
        }
    }
    return False;
}

Bool PARSE_UByte(Str s, unsigned char * n, int base) 
{
    unsigned long number;
    if (PARSE_ULong(s, &number, base)) {
        if (number <= UCHAR_MAX) {
            if (n) *n = (unsigned char)number;
            return True;
        }
    }
    return False;
}

/**
 * Parse a short, returning True on success.
 */
Bool PARSE_Short(Str s, short * n, int base) 
{
    long number;
    if (PARSE_Long(s, &number, base)) {
        if (number >= SHRT_MIN && number <= SHRT_MAX) {
            if (n) *n = (short)number;
            return True;
        }
    }
    return False;
}

Bool PARSE_UShort(Str s, unsigned short * n, int base) 
{
    unsigned long number;
    if (PARSE_ULong(s, &number, base)) {
        if (number <= USHRT_MAX) {
            if (n) *n = (unsigned short)number;
            return True;
        }
    }
    return False;
}

/**
 * Parse an integer, returning True on success.
 */
Bool PARSE_Int(Str s, int * n, int base) 
{
    long number;
    if (PARSE_Long(s, &number, base)) {
        if (number >= INT_MIN && number <= INT_MAX) {
            if (n) *n = (int)number;
            return True;
        }
    }
    return False;
}

Bool PARSE_UInt(Str s, unsigned int * n, int base) 
{
    unsigned long number;
    if (PARSE_ULong(s, &number, base)) {
        if (number <= UINT_MAX) {
            if (n) *n = (unsigned int)number;
            return True;
        }
    }
    return False;
}

/**
 * Parse a long, returning True on success.
 */
Bool PARSE_Long(Str s, long * n, int base) 
{
    if (s) {
        Char * endptr = NULL;
        long number;
        while (*s && IsSpace(*s)) s++;
        number = StrTol(s, &endptr, base);
        if (endptr && endptr != s) {
            if ((number!=LONG_MAX && number!=LONG_MIN)
#ifdef HAVE_ERRNO
                || (errno!=ERANGE)
#endif /* HAVE_ERRNO */
                ) {
                while (*endptr && IsSpace(*endptr)) endptr++;
                if (!*endptr) {
                    if (n) *n = number;
                    return True;
                }
            }
        }
    }
    return False;
}

Bool PARSE_ULong(Str s, unsigned long * n, int base) 
{
    if (s && !startsWith(s,'-')) {
        Char * endptr = NULL;
        unsigned long number;
        while (*s && IsSpace(*s)) s++;
        number = StrToul(s, &endptr, base);
        if (endptr && endptr != s) {
            if ((number != ULONG_MAX)
#ifdef HAVE_ERRNO
                || (errno!=ERANGE)
#endif /* HAVE_ERRNO */
                ) {
                while (*endptr && IsSpace(*endptr)) endptr++;
                if (!*endptr) {
                    if (n) *n = number;
                    return True;
                }
            }
        }
    }
    return False;
}

/**
 * Parse a double, returning True on success.
 */
#ifndef __KERNEL__
Bool PARSE_Double(Str s, double * d) 
{
    if (s) {
        Char * endptr = NULL;
        double value;
        while (*s && IsSpace(*s)) s++;
        value = StrTod(s, &endptr);
        if (endptr && endptr != s) {
            if ((value != HUGE_VAL)
#ifdef HAVE_ERRNO
                || (errno!=ERANGE)
#endif /* HAVE_ERRNO */
                ) {
                while (*endptr && IsSpace(*endptr)) endptr++;
                if (!*endptr) {
                    if (d) *d = value;
                    return True;
                }
            }
        }
    }
    return False;
}
#endif /* __KERNEL__ */

#if !defined(__LONG_64__) && !defined(__SYMBIAN32__)

#if (! defined(HAVE_STRTOLL) || ! defined(HAVE_STRTOULL))
#define MINUS_INT64_MIN (((uint64_t)INT64_MAX)+1)

/* flag values */
#define FL_UNSIGNED   1       /* strtoul called */
#define FL_NEG        2       /* negative sign found */
#define FL_OVERFLOW   4       /* overflow occured */
#define FL_READDIGIT  8       /* we've read at least one correct digit */

static uint64_t strtoxll (
        Str nptr,
        Str* endptr,
        int ibase,
        int flags )
{
    Str p;
    Char c;
    uint64_t number;
    uint64_t maxval;
    unsigned digval;
    
    p = nptr;                       /* p is our scanning pointer */
    number = 0;                     /* start with zero */
    
    c = *p++;                       /* read char */
    while ( IsSpace(c) ) {
        c = *p++;                   /* skip whitespace */
    }

    if (c == '-') {
        flags |= FL_NEG;            /* remember minus sign */
        c = *p++;
    } else if (c == '+') {
        c = *p++;                   /* skip sign */
    }

    if (ibase < 0 || ibase == 1 || ibase > 36) {
        /* bad base! */
        if (endptr)
            /* store beginning of string in endptr */
            *endptr = nptr;
        return 0;                  /* return 0 */

    } else if (ibase == 0) {
        /* determine base free-lance, based on first two chars of
           string */
        if (c != '0')
            ibase = 10;
        else if (*p == 'x' || *p == 'X')
            ibase = 16;
        else
            ibase = 8;
    }
    
    if (ibase == 16) {
        /* we might have 0x in front of number; remove if there */
        if (c == '0' && (*p == 'x' || *p == 'X')) {
            ++p;
            c = *p++;       /* advance past prefix */
        }
    }

    /* if our number exceeds this, we will overflow on multiply */
    maxval = UINT64_MAX / ibase;

    
    for (;;) {      /* exit in middle of loop */
        /* convert c to value */
        if ( IsDigit((unsigned char)c) ) {
            digval = c - '0';
        } else if ( IsAlpha((unsigned char)c) ) {
            digval = ToUpper(c) - 'A' + 10;
        } else {
            break;
        }
         
        if (digval >= (unsigned)ibase) {
            break;          /* exit loop if bad digit found */
        }

        /* record the fact we have read one digit */
        flags |= FL_READDIGIT;
        
        /* we now need to compute number = number * base + digval,
           but we need to know if overflow occured.  This requires
           a tricky pre-check. */
        
        if (number < maxval || (number == maxval &&
           (uint64_t)digval <= (UINT64_MAX % ibase))) {
            /* we won't overflow, go ahead and multiply */
            number = number * ibase + digval;
        } else {
            /* we would have overflowed -- set the overflow flag */
            flags |= FL_OVERFLOW;
        }
        
        c = *p++;               /* read next digit */
    }

    --p;                            /* point to place that stopped scan */

    if (!(flags & FL_READDIGIT)) {
        /* no number there; return 0 and point to beginning of
           string */
        if (endptr) {
            /* store beginning of string in endptr later on */
            p = nptr;
        }
        number = 0;            /* return 0 */
    } else if ( (flags & FL_OVERFLOW) ||
                ( !(flags & FL_UNSIGNED) &&
                  (((flags & FL_NEG) && (number > MINUS_INT64_MIN)) ||
                    ( !(flags & FL_NEG) && (number > INT64_MAX))))) {
        /* overflow or signed overflow occurred */
#ifdef HAVE_ERRNO
        errno = ERANGE;
#endif /* HAVE_ERRNO */
        if ( flags & FL_UNSIGNED ) {
            number = UINT64_MAX;
        } else if ( flags & FL_NEG ) {
            number = MINUS_INT64_MIN;
        } else {
            number = INT64_MAX;
        }
    }

    if (endptr != NULL) {
        /* store pointer to char that stopped the scan */
        *endptr = p;
    }
    
    if (flags & FL_NEG)
        /* negate result if there was a neg sign */
        number = (int64_t)(-(int64_t)number);
    
    return number;                  /* done. */
}

#endif /* (! defined(HAVE_STRTOLL) || ! defined(HAVE_STRTOULL)) */

#ifndef HAVE_STRTOLL
#define strtoll(s,endptr,base) \
    ((int64_t)strtoxll(s,(Str*)(endptr), base, 0))
#endif /* HAVE_STRTOLL */

#ifndef HAVE_STRTOULL
#define strtoull(s,endptr,base) \
   strtoxll(s,(Str*)(endptr),base,FL_UNSIGNED)
#endif /* HAVE_STRTOULL */

/**
 * Parse a 64-bit long, returning True on success.
 */
Bool PARSE_Long64(Str s, int64_t * n, int base)
{
    if (s) {
        Char * endptr = NULL;
        int64_t number = strtoll(s, &endptr, base);
        if (endptr && endptr != s) {
            if ((number!=INT64_MAX && number!=INT64_MIN)
#ifdef HAVE_ERRNO
                || (errno!=ERANGE)
#endif /* HAVE_ERRNO */
                ) {
                while (*endptr && IsSpace(*endptr)) endptr++;
                if (!*endptr) {
                    if (n) *n = number;
                    return True;
                }
            }
        }
    }
    return False;
}

Bool PARSE_ULong64(Str s, uint64_t * n, int base)
{
    if (s && !startsWith(s,'-')) {
        Char * endptr = NULL;
        uint64_t number = strtoull(s, &endptr, base);
        if (endptr && endptr != s) {
            if ((number != UINT64_MAX)
#ifdef HAVE_ERRNO
                || (errno!=ERANGE)
#endif /* HAVE_ERRNO */
                ) {
                while (*endptr && IsSpace(*endptr)) endptr++;
                if (!*endptr) {
                    if (n) *n = number;
                    return True;
                }
            }
        }
    }
    return False;
}

#endif /* !__LONG_64__ && !__SYMBIAN32__ */
