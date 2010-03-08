/*
 * $Id: s_trace.c,v 1.33 2006/03/12 08:29:40 slava Exp $
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

#include "s_buf.h"
#include "s_util.h"
#include "s_strbuf.h"

#define PRINTABLE(c) (IsPrint(c) ? (c) : '.')
#define BYTES_PER_LINE  16

#if defined(_WIN32) && !defined(_NT_KERNEL) && !defined(_WIN32_WCE)
#   include <crtdbg.h> /* _CrtDbgReport */
#endif  /* _WIN32 && !_NT_KERNEL && !_WIN32_WCE */

STATIC int PRINT_mask = DEFAULT_PRINT_MASK;

/*==========================================================================*
 *              T R A C E
 *==========================================================================*/

/**
 * Sets trace mask
 */
void PRINT_SetMask(int mask)
{
    ASSERT(!(mask & (~PRINT_ALL)));
    PRINT_mask = mask;
}

/**
 * Returns current trace mask
 */
int PRINT_GetMask()
{
    return PRINT_mask;
}

#ifndef _NT_KERNEL

/**
 * Normal non-debug output (PRINT_NORMAL)
 */
int PRINT_Output(Str format, ... )
{
    int n = 0;
    if (PRINT_mask & PRINT_NORMAL) {
        va_list va;
        va_start(va, format);
        n = PRINT_Stdout(format, va);
        va_end(va);
    }
    return n;
}

/**
 * Normal non-debug output (PRINT_NORMAL)
 */
int PRINT_OutputVa(Str format, va_list va)
{
    if (PRINT_mask & PRINT_NORMAL) {
        return PRINT_Stdout(format, va);
    } else {
        return 0;
    }
}

/**
 * Verbose trace (PRINT_VERBOSE)
 */
int PRINT_Verbose(Str format, ... )
{
    int n = 0;
    if (PRINT_mask & PRINT_VERBOSE) {
        va_list va;
        va_start(va, format);
        n = PRINT_Stdout(format, va);
        va_end(va);
    }
    return n;
}

/**
 * Verbose trace (PRINT_VERBOSE)
 */
int PRINT_VerboseVa(Str format, va_list va)
{
    if (PRINT_mask & PRINT_VERBOSE) {
        return PRINT_Stdout(format, va);
    } else {
        return 0;
    }
}

/**
 * Error trace (PRINT_ERROR)
 */
int PRINT_Error(Str format, ... )
{
    int n = 0;
    if (PRINT_mask & PRINT_ERROR) {
        va_list va;
        va_start(va, format);
        n = PRINT_Stderr(format, va);
        va_end(va);
    }
    return n;
}

/**
 * Error trace (PRINT_ERROR)
 */
int PRINT_ErrorVa(Str format, va_list va)
{
    if (PRINT_mask & PRINT_ERROR) {
        return PRINT_Stderr(format, va);
    } else {
        return 0;
    }
}

/**
 * Warning trace (PRINT_WARNING)
 */
int PRINT_Warning(Str format, ... )
{
    int n = 0;
    if (PRINT_mask & PRINT_WARNING) {
        va_list va;
        va_start(va, format);
        n = PRINT_Stderr(format, va);
        va_end(va);
    }
    return n;
}

/**
 * Warning trace (PRINT_WARNING)
 */
int PRINT_WarningVa(Str format, va_list va)
{
    if (PRINT_mask & PRINT_WARNING) {
        return PRINT_Stderr(format, va);
    } else {
        return 0;
    }
}

/**
 * Error reporting function for YACC generated parsers.
 */
void yyerror(Str s)
{
    Error(TEXT("ERROR: %s\n"),s);
}

#else /* _NT_KERNEL */

/**
 * Error trace (PRINT_ERROR)
 */
int PRINT_Error(Str format, ... )
{
    int n = 0;
    if (PRINT_mask & PRINT_ERROR) {
        StrBuf128 buf;
        va_list va;
        va_start(va, format);
        STRBUF_InitBufXXX(&buf);
        STRBUF_FormatVa(&buf.sb, format, va);
        n = STRBUF_Length(&buf.sb);
        DbgPrint("%s",STRBUF_Text(&buf.sb));
        STRBUF_Destroy(&buf.sb);
        va_end(va);
    }
    return n;
}

#endif /* _NT_KERNEL */

/**
 * Dump data into the stream 
 */
void PRINT_Dump(PrintProc p, const void* buf, size_t count, size_t off)
{
    size_t i,j;
    const unsigned char* data = (unsigned char*)buf;
    Char line[100];              /* actually, we fit into 80 characters */

    for (i=0; i<count; i += BYTES_PER_LINE) {
        Char tmp[8];
        Sprintf(line,
#ifdef __LONG_64__
                TEXT("   %04lx: "), (unsigned long)
#else  /* ! __LONG_64__ */
                TEXT("   %04x: "), (unsigned int)
#endif /* ! __LONG_64__ */
                (i+off));

        /* hex bytes */
        for (j=i; j < (i+BYTES_PER_LINE); j++) {
            if (j < count) {
                Sprintf(tmp,TEXT("%02x "),(unsigned int)data[j]);
                StrCat(line, tmp);
            } else {
                StrCat(line, TEXT("   "));
            }
        }

        /* ASCII letters */
        StrCat(line,TEXT("    "));
        for (j =i; j<(i+BYTES_PER_LINE); j++) {
            if (j<count) {
                Sprintf(tmp,TEXT("%c"),PRINTABLE(data[j]));
                StrCat(line, tmp);
            } else {
                StrCat(line,TEXT(" "));
            }
        }

        p(TEXT("%s\n"), line);
    }
}

/*==========================================================================*
 *              D E B U G
 *==========================================================================*/

#if DEBUG

#ifndef _NT_KERNEL

/**
 * Debug trace
 */
int DEBUG_Trace(Str format, ... )
{
    int n = 0;
    if (PRINT_mask & PRINT_DEBUG) {
        va_list va;
        va_start(va, format);
        n = PRINT_Stderr(format, va);
        va_end(va);
    }
    return 0;
}

/**
 * Assertion handler.
 */
void DEBUG_Assert(const char * msg, const char * file, long line)
{
#ifdef _WIN32
#  ifdef _WIN32_WCE
    static long ass = 0;            /* counter of active ASSERT boxes */
#  endif /* _WIN32_WCE */
    ULONG lastError = GetLastError();

    Char pname[MAX_PATH];
    pname[0] = 0;
    GetModuleFileName(NULL,pname,COUNT(pname)-1);
    pname[COUNT(pname)-1] = 0;

    TRACE1("ASSERTION FAILED: %hs\n",msg);
    TRACE1("PROGRAM: %s\n",pname);
    TRACE1("FILE: %hs\n",file);
    TRACE1("LINE: %lu\n",line);

#  ifdef _WIN32_WCE
    if (InterlockedIncrement(&ass) > 1) {
        /* nested ASSERTs? */
        DebugBreak();
    } else {
        int n;
        StrBuf128 buf;
        STRBUF_InitBufXXX(&buf);
        STRBUF_AppendFormat(&buf.sb,TEXT("ASSERTION FAILED: %hs\n"),msg);
        STRBUF_AppendFormat(&buf.sb,TEXT("PROGRAM: %s\n"),pname);
        STRBUF_AppendFormat(&buf.sb,TEXT("FILE: %hs\n"),file);
        STRBUF_AppendFormat(&buf.sb,TEXT("LINE: %lu\n"),line);
        __try {
            Str msg = buf.sb.s;
            UINT flags = MB_OKCANCEL | MB_ICONHAND | MB_TOPMOST;
            n = MessageBox(NULL,msg,TEXT("Debug?"),flags);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            /* don't try to debug break, just log the event */
            TRACE1("MessageBox exception %08lX\n",GetExceptionCode());
            n = IDCANCEL;
        }
        STRBUF_Destroy(&buf.sb);
        if (n == IDOK) {
            DebugBreak();
        }
    }
    InterlockedDecrement(&ass);
#  else
    if (_CrtDbgReport(_CRT_ASSERT, file, line, NULL, msg) == 1) {
        _CrtDbgBreak(); 
    } 
#  endif /* _WIN32_WCE */
    SetLastError(lastError);
#else /* _WIN32 */
    TRACE1("ASSERTION FAILED: %s\n",msg);
    TRACE1("FILE: %s\n",file);
    TRACE1("LINE: %lu\n",line);
#endif /* !_WIN32 */
}

#endif /* _NT_KERNEL */

/**
 * Yet another assertion handler.
 */
void DEBUG_AssertFormat(const char * file, long line, Str format, ...)
{
    char msg[512];
#ifdef UNICODE
    Char msg2[COUNT(msg)];
#else /* !UNICODE */
#  define msg2 msg
#endif /* UNICODE */

#if defined(_WIN32) && !defined(_NT_KERNEL)
    ULONG lastError = GetLastError();
#  ifdef _USE_EXCEPTION_HANDLING
    __try {
#  endif /* _USE_EXCEPTION_HANDLING */
#endif /* _WIN32) && !_NT_KERNEL */
        int n;
        va_list va;
        va_start(va, format);

        /* format the message */
        n = Vsnprintf(msg2,COUNT(msg2),format,va);
#ifdef UNICODE
        msg[0] = 0;
        msg2[COUNT(msg2)-1] = 0;
        wcstombs(msg, msg2, COUNT(msg));
        msg[COUNT(msg)-1] = 0;
#endif /* UNICODE */

        /* if the output string is REALLY long, terminate it with ... */
        if (n >= COUNT(msg)-1 || n < 0) {
            n = COUNT(msg)-1;
            msg[n--] = 0;
            msg[n--] = '.'; 
            msg[n--] = '.'; 
            msg[n--] = '.';
            msg[n--] = ' ';
        } else {
            msg[COUNT(msg)-1] = 0;
        }

        va_end(va);
#if defined(_WIN32) && !defined(_NT_KERNEL)
#  ifdef _USE_EXCEPTION_HANDLING
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        /* NOTE: ASSMSG1 macro would recursively invoke this function */
        TRACE1("EXCEPTION %08lX in DEBUG_AssertFormat\n",GetExceptionCode());
        ASSMSG("EXCEPTION in DEBUG_AssertFormat");
    }
#  endif /* _USE_EXCEPTION_HANDLING */
    DEBUG_Assert(msg, file, line);
    SetLastError(lastError);
#elif defined(_NT_KERNEL)
    DEBUG_Assert(msg, (Char*)file, line);
#else
    DEBUG_Assert(msg, file, line);
#endif /* _WIN32) && !_NT_KERNEL */
}

#endif /* DEBUG */
