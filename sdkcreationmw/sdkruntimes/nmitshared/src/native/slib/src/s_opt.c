/*
 * $Id: s_opt.c,v 1.27 2006/03/28 23:44:37 slava Exp $
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

#include "s_opt.h"
#include "s_mem.h"
#include "s_util.h"
#include "s_queue.h"

/*==========================================================================*
 *              C O M M A N D    L I N E
 *==========================================================================*/

/* Group of command line options */
struct _CmdOptGrp {
    QEntry entry;               /* entry in CmdLine->groups */
    Queue opts;                 /* command line options */
    Char * name;                /* the name of the option group */
};

/* Command line parser */
struct _CmdLine {
    Str app;                    /* app name, may be NULL */
    StrBuf16 appBuf;            /* buffer for app name */
    StrBuf64 buf;               /* temporary buffer */
    CmdOutProc print;           /* output function */
    void * printCtx;            /* output function context */
    Queue opts;                 /* contains command line options */
    Queue groups;               /* contains command line options */
    int minArgs;                /* minimum number of additional arguments */
    int maxArgs;                /* maximum number of additional arguments */
};

/* Supported types of command line options */
typedef enum _CmdOptType {
    OptInt = 1,                 /* option has integer parameter */
    OptI64,                     /* option has 64-bit integer parameter */
    OptDouble,                  /* option has double parameter */
    OptStr,                     /* option has string parameter */
    OptTrue,                    /* boolean option */
    OptFalse,                   /* boolean option */
    OptCustom                   /* custom callback option */
} CmdOptType;

/* Command line option */
struct _CmdOpt {
    QEntry entry;               /* entry in CmdLine->opts */
    QEntry groupQ;              /* entry in CmdOptGrp->opts */
    Vector exclude;             /* options mutually exclusive with this one */
    Vector require;             /* options required by this option */
    Char optChar;               /* short option name (one character) */
    Str optName;                /* long  option name */
    Str descr;                  /* description */
    Str param;                  /* parameter name */
    CmdOptType type;            /* type of command line option */
    Bool repeatable;            /* True if this option is repeatable */
    int count;                  /* how many times this option has been seen */
    union _CmdOptValue {
        int * i;                /* OptInt */ 
        I64s * i64;             /* OptI64 */ 
        double * d;             /* OptDouble */
        Str * s;                /* OptStr */ 
        Bool * b;               /* OptTrue/OptFalse */ 
        struct _CustomOpt {
            CmdOptProc cb;      /* callback to invoke */
            void * ctx;         /* callback's context */
        } custom;
    } value;
};

/* Context for CMDLINE_FindOpt */
typedef struct _FindOpt {
    union _FindOptName {
        Char c;
        Str s;
    } name;                     /* name of the option we are looking for */
    CmdOpt * opt;               /* the option that we have found */
} FindOpt;

/* Default representation of arguments of different types in CMDLINE_Usage */
STATIC const Str CMDLINE_NumParam = TEXT("NUMBER");
STATIC const Str CMDLINE_StrParam = TEXT("STRING");

/**
 * CmdOutProc that sends output to standard output. 
 * Context parameter is ignored.
 */
STATIC void CMDLINE_StdOutProc(void * ctx, Str str) 
{
    UNREF(ctx);
    Output(TEXT("%s"), str);
}

/**
 * CmdOutProc that sends output to a File stream. 
 * Context parameter must point to a File
 */
STATIC void CMDLINE_FileOutProc(void * ctx, Str str) 
{ 
    FILE_Puts((File*)ctx, str);
}

/**
 * Formats and prints a message. DOES NOT prepend the output with the 
 * app name. Returns number of characters written to the output stream,
 * zero if output is disabled.
 */
STATIC int CMDLINE_Printf(CmdLine * c, Str format, ...)
{
    int nchars = 0;
    if (c->print) {
        va_list va;
        va_start(va, format);
        STRBUF_FormatVa(&c->buf.sb, format, va);
        nchars = STRBUF_Length(&c->buf.sb);
        c->print(c->printCtx, STRBUF_Text(&c->buf.sb));
        va_end(va);
    }
    return nchars;
}

/**
 * Prints a string. DOES NOT prepend the output with the app name. 
 * Returns number of characters written to the output stream,
 * zero if output is disabled.
 */
STATIC int CMDLINE_Puts(CmdLine * c, Str str)
{
    int nchars = 0;
    if (c->print) {
        nchars = StrLen(str);
        c->print(c->printCtx, str);
    }
    return nchars;
}

/**
 * Formats and prints a message. Prepends the output with the app name
 * if such is defined.
 */
void CMDLINE_Msg(CmdLine * c, Str format, ...)
{
    if (c->print) {
        va_list va;
        va_start(va, format);
        STRBUF_Clear(&c->buf.sb);
        if (c->app && c->app[0]) {
            STRBUF_Append(&c->buf.sb, c->app);
            STRBUF_Append(&c->buf.sb, TEXT(": "));
        }
        STRBUF_AppendFormatVa(&c->buf.sb, format, va);
        c->print(c->printCtx, STRBUF_Text(&c->buf.sb));
        va_end(va);
    }
}

/**
 * Deletes a single command line option
 */
STATIC void CMDOPT_Delete(CmdOpt * opt)
{
    if (opt) {
        QUEUE_RemoveEntry(&opt->entry);
        QUEUE_RemoveEntry(&opt->groupQ);
        VECTOR_Destroy(&opt->exclude);
        VECTOR_Destroy(&opt->require);
        MEM_Free(opt);
    }
}

/**
 * Creates command line object 
 */
CmdLine * CMDLINE_Create(Str app)
{
    CmdLine * c = MEM_New(CmdLine);
    if (c) {
        memset(c, 0, sizeof(*c));
        c->maxArgs = -1;   /* no limit */
        c->print = CMDLINE_StdOutProc;
        STRBUF_InitBufXXX(&c->appBuf);
        STRBUF_InitBufXXX(&c->buf);
        QUEUE_Init(&c->opts);
        QUEUE_Init(&c->groups);
        CMDLINE_SetApp(c, app);
    }
    return c;
}

/**
 * Deletes command line object 
 */
void CMDLINE_Delete(CmdLine * c)
{
    if (c) {
        QEntry * e;
        while ((e = QUEUE_RemoveHead(&c->opts)) != NULL) {
            CmdOpt * opt = QCAST(e,CmdOpt,entry);
            CMDOPT_Delete(opt);
        }
        while ((e = QUEUE_RemoveHead(&c->groups)) != NULL) {
            CmdOptGrp * g = QCAST(e,CmdOptGrp,entry);
            ASSERT(QUEUE_IsEmpty(&g->opts));
            MEM_Free(g->name);
            MEM_Free(g);
        }
        STRBUF_Destroy(&c->appBuf.sb);
        STRBUF_Destroy(&c->buf.sb);
        MEM_Free(c);
    }
}

/**
 * Returns the first option group, NULL if no options is defined
 */
STATIC CmdOptGrp * OPTGRP_First(CmdLine * c)
{
    QEntry * e = QUEUE_First(&c->groups);
    if (e) {
        return QCAST(e,CmdOptGrp,entry);
    }
    return NULL;
}

/**
 * Returns the next option group, NULL if it's the last one
 */
STATIC CmdOptGrp * OPTGRP_Next(CmdOptGrp * g)
{
    if (g) {
        QEntry * e = QUEUE_Next(&g->entry);
        if (e) {
            return QCAST(e,CmdOptGrp,entry);
        }
    }
    return NULL;
}

/**
 * Returns the first command line option, NULL if no options is defined
 */
STATIC CmdOpt * CMDOPT_First(CmdLine * c)
{
    QEntry * e = QUEUE_First(&c->opts);
    if (e) {
        return QCAST(e,CmdOpt,entry);
    }
    return NULL;
}

/**
 * Returns the next command line option, NULL if it's the last one
 */
STATIC CmdOpt * CMDOPT_Next(CmdOpt * opt)
{
    if (opt) {
        QEntry * e = QUEUE_Next(&opt->entry);
        if (e) {
            return QCAST(e,CmdOpt,entry);
        }
    }
    return NULL;
}

/**
 * Returns True if this option requires a parameter, False if not
 */
STATIC Bool CMDOPT_HasParam(CmdOpt * opt)
{
    switch (opt->type) {
    case OptInt:
    case OptI64:
    case OptStr:
    case OptDouble:
        return True;
    case OptCustom:
        return BoolValue(opt->param != NULL);
    default:
        return False;
    }
}

/** 
 * Sets app name.
 */
void CMDLINE_SetApp(CmdLine * c, Str app)
{
    if (app) {
        STRBUF_Copy(&c->appBuf.sb, FILE_FilePart(app));
#ifdef _WIN32
        if (STRBUF_Length(&c->appBuf.sb) > 4) {
            static Str exe = TEXT(".EXE");
            size_t len = STRBUF_Length(&c->appBuf.sb);
            int i, ext = len - 4;
            for (i=0; i<4; i++) {
                if (exe[i] != ToUpper(STRBUF_CharAt(&c->appBuf.sb,ext+i))) {
                    break;
                }
            }
            if (i == 4) {
                STRBUF_SetLength(&c->appBuf.sb,len-4);
            }
        } else {
            STRBUF_Copy(&c->appBuf.sb, FILE_FilePart(app));
        }
#endif /* _WIN32 */
        if (STRBUF_Length(&c->appBuf.sb) > 0) {
            c->app = STRBUF_Text(&c->appBuf.sb);
        } else {
            c->app = NULL;
        }
    } else {
        c->app = NULL;
        STRBUF_Clear(&c->appBuf.sb);
    }
}

/**
 * Returns app file name without path and .exe suffix on Windows 
 */
Str CMDLINE_GetApp(CmdLine * c)
{
    return c->app;
}

/**
 * Directs output to a File stream. NULL File disables the output
 */
void CMDLINE_SetOutFile(CmdLine * c, File * out)
{
    c->printCtx = out;
    c->print = (out ? NULL : CMDLINE_FileOutProc);
}

/**
 * Directs output to the specified function. NULL function disables the output
 */
void CMDLINE_SetOutProc(CmdLine * c, CmdOutProc out, void * ctx)
{
    c->printCtx = ctx;
    c->print = out;
}

/**
 * Sets parameter name for the specified option, for printing purposes. 
 * The option must have a parameter (i.e. be either string or integer option).
 * The string is NOT copied to the internal storage, so it better be allocated
 * statically.
 */
Bool CMDLINE_SetParamName(CmdOpt * opt, Str param)
{
    if (opt) {
        ASSERT(opt->type == OptCustom || CMDOPT_HasParam(opt));
        opt->param = param;
        return True;
    } else {
        return False;
    }
}

/**
 * Sets minimum number of additional argumentes (not including options
 * and their parameters) expected in the command line. 
 */
void CMDLINE_SetMinArgs(CmdLine * c, int min)
{
    c->minArgs = min;
}

/**
 * Sets maximum number of additional argumentes (not including options
 * and their parameters) expected in the command line. Negative value
 * is interpreted as no limit.
 */
void CMDLINE_SetMaxArgs(CmdLine * c, int max)
{
    c->maxArgs = max;
}

/**
 * Callback for CMDLINE_FindShortOpt
 */
STATIC Bool CMDLINE_FindShortOptCB(QEntry * e, void * ctx)
{
    CmdOpt * opt = QCAST(e,CmdOpt,entry);
    FindOpt * find = (FindOpt*)ctx;
    if (opt->optChar && opt->optChar == find->name.c) {
        find->opt = opt;
        return False;
    }
    return True;
}

/**
 * Callback for CMDLINE_FindLongOpt
 */
STATIC Bool CMDLINE_FindLongOptCB(QEntry * e, void * ctx)
{
    CmdOpt * opt = QCAST(e,CmdOpt,entry);
    FindOpt * find = (FindOpt*)ctx;
    if (opt->optName && StrCmp(opt->optName,find->name.s) == 0) {
        find->opt = opt;
        return False;
    }
    return True;
}

/* 
 * Finds short command line option. 
 */
STATIC CmdOpt * CMDLINE_FindShortOpt(CmdLine * c, Char ch)
{
    FindOpt find;
    find.name.c = ch;
    find.opt = NULL;
    QUEUE_Examine(&c->opts,CMDLINE_FindShortOptCB,&find);
    return find.opt;
}

/* 
 * Finds long command line option. 
 */
STATIC CmdOpt * CMDLINE_FindLongOpt(CmdLine * c, Str name)
{
    FindOpt find;
    find.name.s = name;
    find.opt = NULL;
    QUEUE_Examine(&c->opts,CMDLINE_FindLongOptCB,&find);
    return find.opt;
}

/* 
 * The following functions create command line options. We allow two names 
 * for the same option, a short and a long one. For example: 'p' and 
 * "pattern". Both options are absolutely interchangeable. NOTE: CmdOpt 
 * does NOT copy option names and description to the internal storage. 
 * The strings MUST be statically allocated and SHOULD never change 
 * during the lifetime of CmdOpt object. Either short or long option 
 * (but not both) may be NULL.
 */
STATIC CmdOpt * CMDLINE_Add(CmdLine * c, Char opt1, Str opt2, Str descr)
{
    CmdOpt * opt = MEM_New(CmdOpt);
    ASSERT(opt1 || opt2);
    ASSERT(!opt2 || opt2[0] != '-');  /* must not start with - */
    ASSERT(!opt1 || !CMDLINE_FindShortOpt(c,opt1));
    ASSERT(!opt2 || !CMDLINE_FindLongOpt(c,opt2));
    ASSERT(!opt2 || StrLen(opt2)>0);
    if (opt) {
        memset(opt, 0, sizeof(*opt));
        if (VECTOR_Init(&opt->exclude, 0, NULL, NULL)) {
            if (VECTOR_Init(&opt->require, 0, NULL, NULL)) {
                opt->optChar = opt1;
                opt->optName = opt2;
                opt->descr = descr;
                QUEUE_InsertTail(&c->opts, &opt->entry);
                return opt;
            }
            VECTOR_Destroy(&opt->exclude);
        }
        MEM_Free(opt);
    }
    return NULL;
}

CmdOpt * 
CMDLINE_AddIntOpt(CmdLine * c, Char opt1, Str opt2, Str descr, int * value)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        opt->param = CMDLINE_NumParam;
        opt->type = OptInt;
        opt->value.i = value;
    }
    return opt;
}

CmdOpt * 
CMDLINE_AddI64Opt(CmdLine * c, Char opt1, Str opt2, Str descr, I64s * value)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        opt->param = CMDLINE_NumParam;
        opt->type = OptI64;
        opt->value.i64 = value;
    }
    return opt;
}

CmdOpt * 
CMDLINE_AddDoubleOpt(CmdLine * c,Char opt1,Str opt2,Str descr,double * value)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        opt->param = CMDLINE_NumParam;
        opt->type = OptDouble;
        opt->value.d = value;
    }
    return opt;
}

CmdOpt * 
CMDLINE_AddTrueOpt(CmdLine * c, Char opt1, Str opt2, Str descr, Bool * value)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        opt->type = OptTrue;
        opt->value.b = value;
    }
    return opt;
}

CmdOpt * 
CMDLINE_AddFalseOpt(CmdLine * c, Char opt1, Str opt2, Str descr, Bool * value)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        opt->type = OptFalse;
        opt->value.b = value;
    }
    return opt;
}

CmdOpt * 
CMDLINE_AddStrOpt(CmdLine * c, Char opt1, Str opt2, Str descr, Str * value)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        opt->param = CMDLINE_StrParam;
        opt->type = OptStr;
        opt->value.s = value;
    }
    return opt;
}

/**
 * Creates a custom option. If param is not NULL, then this option is assumed
 * to have a parameter. The callback is invoked every time this option is seen
 * on the command line. If the callback function returns False, parsing is
 * stopped and CMDLINE_Parse return False.
 */
CmdOpt * CMDLINE_AddOpt(CmdLine * c, Char opt1, Str opt2, Str descr,
                        CmdOptProc cb, void * ctx, Str param)
{
    CmdOpt * opt = CMDLINE_Add(c, opt1, opt2, descr);
    if (opt) {
        ASSERT(cb);
        opt->param = param;
        opt->type = OptCustom;
        opt->value.custom.cb = cb;
        opt->value.custom.ctx = ctx;
    }
    return opt;
}

/**
 * Make option repeatable. By default options are not repeatable.
 * Repeatable options can be repeated on the command line even if
 * PARSE_NO_DUP flag is passed to CMDLINE_Parse
 */
Bool CMDLINE_SetRepeatable(CmdOpt * opt)
{
    if (opt) {
        opt->repeatable = True;
        return True;
    } else {
        return False;
    }
}

/**
 * Returns how many times this option has been used on the command line.
 * Normally, returns either 0 (option was not used) or 1 (option was used)
 * but may return a numebr greater than 1 if the option is repeatable or
 * PARSE_NO_DUP flag wasn't set
 */
int CMDLINE_GetCount(CmdOpt * opt)
{
    return opt ? opt->count : 0;
}

/**
 * Defines mutually exclusive options. If opt2 is NULL, makes opt1
 * mutually exclusive with all other (currently defined) options.
 */
Bool CMDLINE_Exclude(CmdOpt * opt1, CmdOpt * opt2)
{
    ASSERT(opt1);
    ASSERT(opt1 != opt2);
    if (opt1 && opt1 != opt2) {
        if (opt2) {
            return BoolValue((VECTOR_Contains(&opt1->exclude, opt2) ||
                              VECTOR_Add(&opt1->exclude, opt2)) &&
                             (VECTOR_Contains(&opt2->exclude, opt1) ||
                              VECTOR_Add(&opt2->exclude, opt1)));
        } else {
            CmdLine * c = CAST_QUEUE(opt1->entry.queue,CmdLine,opts);
            if (VECTOR_EnsureCapacity(&opt1->exclude, c->opts.size-1)) {
                Bool ok = True;
                CmdOpt * opt = CMDOPT_First(c);
                while (opt) {
                    if (opt != opt1) {
                        if (!CMDLINE_Exclude(opt1,opt)) {
                            break;
                        }
                    }
                    opt = CMDOPT_Next(opt);
                }
                return ok;
            }
        }
    }
    return False;
}

/**
 * Defines dependent options. opt1 requires (depends on) opt2 
 */
Bool CMDLINE_Require(CmdOpt * opt1, CmdOpt * opt2)
{
    ASSERT(opt1 && opt2);
    ASSERT(opt1 != opt2);
    if (opt1 && opt2 && opt1 != opt2) {
        if (VECTOR_Contains(&opt1->require, opt2)) {
            return True;
        } else {
            return VECTOR_Add(&opt1->require, opt2);
        }
    }
    return False;
}

/*
 * Defines an "option group". Option groups exist strictly for formatting
 * purposes. The affect the output produced by CMDLINE_Usage function
 */
CmdOptGrp * CMDLINE_CreateOptGrp(CmdLine * c, Str name)
{
    CmdOptGrp * g = MEM_New(CmdOptGrp);
    if (g) {
        memset(g, 0, sizeof(*g));
        g->name = STRING_Dup(name);
        if (name) {
            QUEUE_Init(&g->opts);
            QUEUE_InsertTail(&c->groups, &g->entry);
            return g;
        }
        MEM_Free(g);
    }
    return NULL;
}

/**
 * Adds option to the option group
 */
void CMDLINE_AddToGroup(CmdOptGrp * group, CmdOpt * opt)
{
    ASSERT(group);
    if (group) {
        QUEUE_RemoveEntry(&opt->groupQ);
        QUEUE_InsertTail(&group->opts, &opt->groupQ);
    }
}

/**
 * Formats the options, like "-x (--exit)"
 */
STATIC Str CMDLINE_FormatOption(const CmdOpt * opt, StrBuf * sb)
{
    STRBUF_Clear(sb);
    if (opt->optChar) {
        STRBUF_Copy(sb, TEXT("-"));
        STRBUF_AppendChar(sb, opt->optChar);
        if (opt->optName) {
            STRBUF_Append(sb, TEXT(" (--"));
            STRBUF_Append(sb, opt->optName);
            STRBUF_Append(sb, TEXT(")"));
            
        }
    } else {
        ASSERT(opt->optName);
        STRBUF_Copy(sb, TEXT("--"));
        STRBUF_Append(sb, opt->optName);
    }

    return STRBUF_Text(sb);
}

/**
 * Handles a single option. Returns True of the option has been processed,
 * False if an error occured.
 *
 * opt     the option being processed
 * arg     the current argument (leading '-' characters are stripped)
 * nextArg is the next argument in the command line, NULL if there's no 
 *         next argument. If option requires an argument, but there's no 
 *         next argument, this function returns False
 * flags   flags passed to CMDLINE_Parse
 * argList array of previously seen arguments (strings) 
 * optList array of previously seen options (CmdOpt) 
 */
STATIC Bool 
CMDLINE_HandleOption(CmdOpt * opt, Str arg, Str nextArg, 
                     int flags, Vector * argList, Vector * optList)
{
    CmdLine * c = CAST_QUEUE(opt->entry.queue,CmdLine,opts);
    int i, n = VECTOR_Size(argList);
    Str prefix = ((StrLen(arg) == 1) ? TEXT("-") : TEXT("--"));

    /* check for duplicate options? */
    if (!opt->repeatable && (flags & PARSE_NO_DUP)) {
        if (VECTOR_Contains(argList,arg) || VECTOR_Contains(optList,opt)) {
            if (!(flags & PARSE_SILENT)) {
                StrBuf16 buf;
                STRBUF_InitBufXXX(&buf);
                CMDLINE_Msg(c,TEXT("duplicate option %s\n"),
                    CMDLINE_FormatOption(opt, &buf.sb));
                STRBUF_Destroy(&buf.sb);
            }
            return False;
        }
    }

    /* check for mutually exclusive options */
    if (!VECTOR_IsEmpty(&opt->exclude)) {
        for (i=0; i<n; i++) {
            CmdOpt * prevOpt = (CmdOpt*)VECTOR_Get(optList, i);
            if (VECTOR_Contains(&opt->exclude,prevOpt)) {
                if (!(flags & PARSE_SILENT)) {
                    StrBuf16 buf1;
                    StrBuf16 buf2;
                    STRBUF_InitBufXXX(&buf1);
                    STRBUF_InitBufXXX(&buf2);
                    CMDLINE_Msg(c,TEXT("option %s cannot be used with %s\n"),
                        CMDLINE_FormatOption(prevOpt, &buf1.sb), 
                        CMDLINE_FormatOption(opt, &buf2.sb));
                    STRBUF_Destroy(&buf1.sb);
                    STRBUF_Destroy(&buf2.sb);
                }
                return False;
            }
        }
    }

    /* does this option require an argument? boolean options don't */
    if (opt->type == OptTrue) {
        if (opt->value.b) (*opt->value.b) = True;
    } else if (opt->type == OptFalse) {
        if (opt->value.b) (*opt->value.b) = False;
    } else {
        if (!nextArg) {
            if (!(flags & PARSE_SILENT)) {
                Str what = opt->param;
                if (!what) what = TEXT("argument");
                CMDLINE_Msg(c,TEXT("expecting %s after %s%s\n"),
                    what,prefix,arg);
            }
            return False;
        }

        /* try to interpret the argument */
        switch (opt->type) {
        case OptInt:
            if (!PARSE_Int(nextArg, opt->value.i, 0)) {
                if (!(flags & PARSE_SILENT)) {
                    Str what = opt->param;
                    if (!what) what = TEXT("number");
                    CMDLINE_Msg(c,TEXT("expecting %s after %s%s option,")
                        TEXT_(" not '%s'\n"),what,prefix,arg,nextArg);
                }
                return False;
            }
            break;
        case OptI64:
            if (!PARSE_I64(nextArg, opt->value.i64, 0)) {
                if (!(flags & PARSE_SILENT)) {
                    Str what = opt->param;
                    if (!what) what = TEXT("number");
                    CMDLINE_Msg(c,TEXT("expecting %s after %s%s option,")
                        TEXT_(" not '%s'\n"),what,prefix,arg,nextArg);
                }
                return False;
            }
            break;
        case OptDouble:
            if (!PARSE_Double(nextArg, opt->value.d)) {
                if (!(flags & PARSE_SILENT)) {
                    Str what = opt->param;
                    if (!what) what = TEXT("number");
                    CMDLINE_Msg(c,TEXT("expecting %s after %s%s option,")
                        TEXT_(" not '%s'\n"),what,prefix,arg,nextArg);
                }
                return False;
            }
            break;
        case OptCustom:
            if (!opt->value.custom.cb(
                CAST_QUEUE(opt->entry.queue,CmdLine,opts),
                nextArg, opt->value.custom.ctx)) {
                return False;
            }
            break;
        default:
            ASSERT(opt->type == OptStr);
            (*opt->value.s) = nextArg;
        }
    }

    /* store this option */
    VECTOR_Add(argList,STRING_Dup(arg));
    VECTOR_Add(optList,opt);
    opt->count++;
    return True;
}

/**
 * Resets option use count. This is done every time before parsing the
 * command line.
 */
STATIC Bool CMDLINE_ResetOptCountCB(QEntry * e, void * ctx)
{
    CmdOpt * opt = QCAST(e,CmdOpt,entry);
    UNREF(ctx);
    opt->count = 0;
    return True;
}

/** 
 * Parses command line. You can use CMDLINE_SetOutFile or CMDLINE_SetOutProc
 * to forward error messages anywhere you like. The 'unused' vector (if not 
 * NULL), receives the additional command line arguments that were not
 * interpreted as options or their parameters.
 *
 * The same option may appear several time in the same command line. The
 * default behavior is that the value of the next option overrides the 
 * previous one. You can override this behavior by specifying PARSE_NO_DUP
 * flag. Repeatable options are always allowed more than once regardless
 * of PARSE_NO_DUP flag.
 */
Bool 
CMDLINE_Parse(CmdLine * c, Char * args[], int n, int flg, Vector * unused)
{
    int i;
    int unusedArgs = 0;
    Bool ok = True;
    Vector argList, optList;
    VECTOR_Init(&argList, 0, vectorEqualsString, vectorFreeValueProc);
    VECTOR_Init(&optList, 0, NULL, NULL);
    QUEUE_Examine(&c->opts, CMDLINE_ResetOptCountCB, NULL);
    for (i=0; i<n && ok; i++) {
        Str arg = args[i];
        CmdOpt * opt = NULL;

        /* if it does not start with -, then it's not an option */
        if (arg[0] == '-') {

            /* if it starts with --, then it better be a long option */
            if (arg[1] == '-') {
                opt = CMDLINE_FindLongOpt(c, arg + 2);
                if (opt) {
                    arg += 2;
                } else {
                    if (!(flg & PARSE_SILENT)) {
                        CMDLINE_Msg(c,TEXT("unknown option %s\n"),arg);
                    }
                    ok = False;
                    break;
                }
            } else {

                /* try to handle this as a bunch of short options */
                int j = 1;
                Char s[2];
                s[1] = 0;
                while (arg[j]) {
                    opt = CMDLINE_FindShortOpt(c, arg[j]);
                    if (opt) {
                        Str next = NULL;
                        s[0] = arg[j];
                        if (!arg[j+1] && (i+1)<n) next = args[i+1];
                        ok = CMDLINE_HandleOption(opt,s,next,flg,
                            &argList,&optList);
                        if (ok) {
                            j++; /* switch to the next letter */
                            if (CMDOPT_HasParam(opt)) {
                                ASSERT(!arg[j]); /* must be last letter */
                                i++;
                            }
                        } else {
                            break;
                        }
                    } else {
                        if (!(flg & PARSE_SILENT)) {
                            CMDLINE_Msg(c,TEXT("unknown option -%c\n"),arg[j]);
                        }
                        ok = False;
                        break;
                    }
                }

                /* will break the loop if ok isn't True */
                continue;
            }
        }

        if (opt) {
            Str nextArg = NULL;
            if ((i+1)<n) nextArg = args[i+1];
            ok = CMDLINE_HandleOption(opt,arg,nextArg,flg,&argList,&optList);
            if (ok && CMDOPT_HasParam(opt)) i++;
        } else if (arg[0] == '-') {
            if (!(flg & PARSE_SILENT)) {
                CMDLINE_Msg(c,TEXT("unknown option %s\n"),arg);
            }
            ok = False;
            break;
        } else {
            if (c->maxArgs >= 0 && unusedArgs == c->maxArgs) {
                if (!(flg & PARSE_SILENT)) {
                    if (c->maxArgs == 0) {
                        CMDLINE_Msg(c,TEXT("unexpected argument %s\n"),arg);
                    } else {
                        CMDLINE_Msg(c,TEXT("too many command line parameters\n"));
                    }
                }
                ok = False;
                break;
            }
            if (unused) VECTOR_Add(unused,arg);
            unusedArgs++;
        }
    }
    if (ok && unusedArgs < c->minArgs) {
        if (!(flg & PARSE_SILENT)) {
            CMDLINE_Msg(c,TEXT("missing command line parameters\n"));
        }
        ok = False;
    }

    /* check the dependencies between the options */
    if (ok) {
        for (i=0; i<VECTOR_Size(&optList); i++) {
            /* don't do the same thing twice */
            CmdOpt * opt = (CmdOpt*)VECTOR_Get(&optList,i);
            if (VECTOR_IndexOf(&optList,opt) == i) {
                if (!VECTOR_ContainsAll(&optList,&opt->require)) {
                    /* take the first one and print it */
                    CmdOpt * req = (CmdOpt*)VECTOR_Get(&opt->require,0);
                    Str badArg = (Str)VECTOR_Get(&argList,i);
                    Str prefix = ((StrLen(badArg)==1)?TEXT("-"):TEXT("--"));
                    if (!(flg & PARSE_SILENT)) {
                        if (req->optChar) {
                            CMDLINE_Msg(c,TEXT("option %s%s requires -%c\n"),
                                prefix, badArg, req->optChar);
                        } else {
                            CMDLINE_Msg(c,TEXT("option %s%s requires --%s\n"),
                                prefix, badArg, req->optName);
                        }
                    }
                    ok = False;
                    break;
                }
            }
        }
    }

    VECTOR_Destroy(&argList);
    VECTOR_Destroy(&optList);
    return ok;
}

/* 
 * Equivalent to CMDLINE_Parse, assumes that there may be no more than
 * one argument on the command line. If the argument has been specified,
 * then arg points to the specified argument when this function returns.
 * Otherwise, arg is set to NULL. Whether or not this argument is requred
 * is set by CMDLINE_SetMinArgs call. Set min args to 1 if parameter is
 * required, to zero if it's optional. CMDLINE_SetMaxArgs calls made prior
 * to this call are ignored. Max args is assumed to be 1 for the duration
 * of this call.
 */
Bool CMDLINE_Parse1(CmdLine* c, Char* args [], int nargs, int flags, Str* arg)
{
    Bool ok;
    Vector params;
    int maxArgs, minArgs;

    if (arg) *arg = NULL;
    VECTOR_Init(&params, 0, NULL, NULL);

    /* why would you want to set minArgs or maxArgs to more than one? */
    ASSERT(c->maxArgs <= 1); 
    ASSERT(c->minArgs <= 1); 
    maxArgs = c->maxArgs;
    minArgs = c->minArgs;
    if (c->minArgs > 1) c->minArgs = 1;
    c->maxArgs = 1;

    /* run the parser */
    ok = CMDLINE_Parse(c, args, nargs, flags, &params);
    if (ok && !VECTOR_IsEmpty(&params) && arg) {
        /* copy the result */
        *arg = (Str)VECTOR_Get(&params,0);
    }

    /* restore minArgs and maxArgs, just in case */
    c->minArgs = minArgs;
    c->maxArgs = maxArgs;
    VECTOR_Destroy(&params);
    return ok;
}

/**
 * Prints the usage statement for a single option
 */
STATIC void 
USAGE_Opt(
    CmdLine * c, 
    CmdOpt * opt, 
    StrBuf * sb, 
    Str optPrefix, 
    Bool shortOpt, 
    int colWidth, 
    int maxcol)
{
    int pos;
    STRBUF_Copy(sb, optPrefix);
    if (opt->optChar && opt->optName) {
        STRBUF_AppendChar(sb,'-');
        STRBUF_AppendChar(sb,opt->optChar);
        STRBUF_Append(sb,TEXT(", --"));
        STRBUF_Append(sb,opt->optName);
    } else if (opt->optChar) {
        STRBUF_AppendChar(sb,'-');
        STRBUF_AppendChar(sb,opt->optChar);
    } else {
        if (shortOpt) STRBUF_Append(sb,TEXT("    "));
        STRBUF_Append(sb,TEXT("--"));
        STRBUF_Append(sb,opt->optName);
    }
    if (opt->param) {
        STRBUF_AppendChar(sb,' ');
        STRBUF_Append(sb,opt->param);
    }
    STRBUF_Inflate(sb,colWidth,' ');
    pos = CMDLINE_Puts(c,STRBUF_Text(sb));

    if (opt->descr) {
        int len;
        Str p = opt->descr;
        while (*p && IsSpace(*p)) p++;
        len = StrLen(p);

        /* can just print the whole thing? */
        if (maxcol <= 0 || (pos+len) <= maxcol) {
            CMDLINE_Printf(c,TEXT("%s\n"),p);
        } else {
            int nwords = 0;
            while (*p) {
                STRBUF_Clear(sb);
                if (!nwords) {
                    /* looking for the first word */
                    while(*p && IsSpace(*p)) p++;
                    while(*p && !IsSpace(*p))STRBUF_AppendChar(sb,*p++);
                    pos += CMDLINE_Puts(c,STRBUF_Text(sb));
                    nwords++;
                } else {
                    /* scan next word */
                    Str save = p;
                    while(*p && IsSpace(*p))STRBUF_AppendChar(sb,*p++);
                    while(*p && !IsSpace(*p))STRBUF_AppendChar(sb,*p++);
                    if ((pos + (int)STRBUF_Length(sb)) < maxcol) {
                        pos += CMDLINE_Puts(c,STRBUF_Text(sb));
                    } else {
                        CMDLINE_Printf(c,TEXT("\n"));
                        STRBUF_Clear(sb);
                        STRBUF_Inflate(sb, colWidth,' ');
                        pos = CMDLINE_Printf(c,STRBUF_Text(sb));
                        nwords = 0;
                        p = save;

                        /* can just print the rest? */
                        while(*p && IsSpace(*p)) p++;
                        len = StrLen(p);
                        if ((pos+len) <= maxcol) {
                            CMDLINE_Puts(c,p);
                            break;
                        }
                    }
                }
            }
            /* terminate the last line */
            CMDLINE_Printf(c,TEXT("\n"));
        }
    }
}

/* 
 * Prints usage. If the maxcol parameter is zero, the text will be wrapped
 * to fit into the terminal window. Negative maxcol turns off the wrapping. 
 * You can use CMDLINE_SetOutFile or CMDLINE_SetOutProc to forward the 
 * output anywhere you like.
 */
void CMDLINE_Usage(CmdLine * c, Str extra, int maxcol)
{
    if (c->print && !QUEUE_IsEmpty(&c->opts)) {
        Str optPrefix = TEXT("  ");
        CmdOpt * opt;
        CmdOptGrp * grp;
        StrBuf64 buf;
        Bool shortOpt = False, defaultGroup = False, otherGroup = False;
        int colWidth = 0;

        STRBUF_InitBufXXX(&buf);

        /* use full screen width? */
        if (maxcol == 0) {
            maxcol = 80;
            IO_TermSize(NULL, &maxcol);
            maxcol--;
        }

        /* calculate the length of the longest option */
        opt = CMDOPT_First(c);
        while (opt && !shortOpt) {
            if (opt->optChar) shortOpt = True;
            opt = CMDOPT_Next(opt);
        }
        opt = CMDOPT_First(c);
        while (opt) {
            int len = 0;
            if (shortOpt) {
                len += 2;
            }
            if (opt->optName) {
                len += StrLen(opt->optName) + 2;
                if (shortOpt) len += 2;
            }
            if (opt->param) {
                len += StrLen(opt->param) + 1;
            }
            if (len > colWidth) {
                colWidth = len;
            }
            opt = CMDOPT_Next(opt);
        }
        colWidth += StrLen(optPrefix) + 2;

        /* write Usage: stuff if we have app name */
        if (c->app) {
            StrBuf64 optBuf;
            Str prefix;
            int pos, optCount = 0;
            static const Str usage = TEXT("Usage: ");
            int prefixLen = StrLen(usage)+STRBUF_Length(&c->appBuf.sb)+1;
            
            STRBUF_InitBufXXX(&optBuf);
            STRBUF_Inflate(&buf.sb, prefixLen,' ');
            prefix = STRBUF_Text(&buf.sb);
            pos = CMDLINE_Printf(c,TEXT("%s%s "),usage,c->app);

            /* print options */
            opt = CMDOPT_First(c);
            while (opt) {
                /* compose a string that represents this option */
                StrBuf * sb = &optBuf.sb;
                STRBUF_Copy(sb,TEXT("["));
                if (opt->optChar && opt->optName) {
                    STRBUF_AppendChar(sb,'-');
                    STRBUF_AppendChar(sb,opt->optChar);
                    STRBUF_Append(sb,TEXT(", --"));
                    STRBUF_Append(sb,opt->optName);
                } else if (opt->optChar) {
                    STRBUF_AppendChar(sb,'-');
                    STRBUF_AppendChar(sb,opt->optChar);
                } else {
                    STRBUF_Append(sb,TEXT("--"));
                    STRBUF_Append(sb,opt->optName);
                }
                if (opt->param) {
                    STRBUF_AppendChar(sb,' ');
                    STRBUF_Append(sb,opt->param);
                }
                STRBUF_Append(sb,TEXT("] "));

                /* break the line is necessary */
                if ((maxcol > 0) && (optCount > 0) && 
                   ((pos + (int)STRBUF_Length(sb)) >= maxcol)) {
                    CMDLINE_Printf(c,TEXT("\n"));
                    pos = CMDLINE_Printf(c,prefix);
                    optCount = 0;
                }

                /* print this option */
                pos += CMDLINE_Printf(c,STRBUF_Text(sb));
                optCount++;

                /* switch to the next option */
                opt = CMDOPT_Next(opt);
            }

            /* print extra arguments */
            if (extra) {

                /* break the line is necessary */
                if ((maxcol > 0) && (optCount > 0) && 
                   ((pos + (int)StrLen(extra)) >= maxcol)) {
                    CMDLINE_Printf(c,TEXT("\n%s"),prefix);
                }

                /* print extra arguments */
                CMDLINE_Puts(c,extra);
            }

            /* terminate the last line */
            CMDLINE_Printf(c,TEXT("\n"));
            STRBUF_Destroy(&optBuf.sb);
        }

        /* first describe the options not associated with any group */
        opt = CMDOPT_First(c);
        while (opt) {
            if (!opt->groupQ.queue) {
                if (!defaultGroup) {
                    CMDLINE_Printf(c,TEXT("Options:\n"));
                    defaultGroup = True;
                }
                USAGE_Opt(c,opt,&buf.sb,optPrefix,shortOpt,colWidth,maxcol);
            }
            opt = CMDOPT_Next(opt);
        }

        /* then print other option group */
        grp = OPTGRP_First(c);
        while (grp) {
            QEntry * e = QUEUE_First(&grp->opts);
            if (e) {
                if (defaultGroup || otherGroup) CMDLINE_Printf(c,TEXT("\n"));
                CMDLINE_Printf(c,TEXT("%s:\n"),grp->name);
                otherGroup = True;
            }
            while (e) {
                opt = QCAST(e,CmdOpt,groupQ);
                USAGE_Opt(c,opt,&buf.sb,optPrefix,shortOpt,colWidth,maxcol);
                e = QUEUE_Next(e);
            }
            grp = OPTGRP_Next(grp);
        }

        STRBUF_Destroy(&buf.sb);
    }
}
