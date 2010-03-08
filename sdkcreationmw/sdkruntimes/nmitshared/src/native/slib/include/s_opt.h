/*
 * $Id: s_opt.h,v 1.12 2006/03/28 23:44:36 slava Exp $
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

#ifndef _SLAVA_CMDLINE_OPT_H_
#define _SLAVA_CMDLINE_OPT_H_

#include "s_def.h"
#include "s_file.h"
#include "s_vector.h"
#include "s_strbuf.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Parser for command line options
 */

/* Opaque structures */
typedef struct _CmdLine    CmdLine;
typedef struct _CmdOpt     CmdOpt;
typedef struct _CmdOptGrp  CmdOptGrp;

/* Function that receives the output */
typedef void (*CmdOutProc) P_((void * ctx, Str str));

/* callback for a custom option. Returns False to stop parsing */
typedef Bool (*CmdOptProc) P_((CmdLine * c, Str param, void * ctx));

/* Create/destroy command line object */
extern CmdLine * CMDLINE_Create P_((Str app));
extern void CMDLINE_Delete P_((CmdLine * c));

/* Set command line parser options */
extern void CMDLINE_SetApp P_((CmdLine * c, Str app));
extern void CMDLINE_SetMinArgs P_((CmdLine * c, int min));
extern void CMDLINE_SetMaxArgs P_((CmdLine * c, int max));
extern void CMDLINE_SetOutFile P_((CmdLine * c, File * out));
extern void CMDLINE_SetOutProc P_((CmdLine * c, CmdOutProc out, void * ctx));
extern Bool CMDLINE_SetParamName P_((CmdOpt * opt, Str param));

/* Utilities */
extern Str  CMDLINE_GetApp P_((CmdLine * c));
extern void CMDLINE_Msg P_((CmdLine * c, Str format, ...));

/* 
 * Command line options. We allow two names for the same option, a short 
 * and a long one. For example: 'p' and "pattern". Both options are 
 * absolutely interchangeable. NOTE: CmdOpt does NOT copy option names
 * and description to the internal storage. The strings MUST be statically
 * allocated and SHOULD never change during the lifetime of CmdOpt object.
 * Either short or long option (but not both) may be NULL.
 *
 * Boolean options take no arguments. The different between CMDLINE_AddTrueOpt
 * and CMDLINE_AddFalseOpt is that the option defined by CMDLINE_AddTrueOpt
 * sets the value to True, while CMDLINE_AddFalseOpt set it to False, as
 * you might have guessed.
 *
 * The same option may appear several time in the same command line. The
 * default behavior is that the value of the next option overrides the 
 * previous one. You can override this behavior by specifying PARSE_NO_DUP
 * flag when you call CMDLINE_Parse.
 */
extern CmdOpt * CMDLINE_AddIntOpt P_((CmdLine * c, Char optChar, Str longOpt,
                                      Str descr, int * value));
extern CmdOpt * CMDLINE_AddI64Opt P_((CmdLine * c, Char optChar, Str longOpt,
                                      Str descr, I64s * value));
extern CmdOpt * CMDLINE_AddDoubleOpt P_((CmdLine * c,Char optChar,Str longOpt,
                                      Str descr, double * value));
extern CmdOpt * CMDLINE_AddTrueOpt P_((CmdLine * c, Char optChar, Str longOpt, 
                                       Str descr, Bool * value));
extern CmdOpt * CMDLINE_AddFalseOpt P_((CmdLine * c, Char optChar,Str longOpt,
                                        Str descr, Bool * value));
extern CmdOpt * CMDLINE_AddStrOpt P_((CmdLine * c, Char optChar, Str longOpt, 
                                      Str descr, Str * value));
extern CmdOpt * CMDLINE_AddOpt P_((CmdLine * c, Char optChar, Str longOpt, 
                                   Str descr, CmdOptProc cb, void * ctx,
                                   Str param));

/* By default options are not repeatable */
extern Bool CMDLINE_SetRepeatable P_((CmdOpt * opt));
extern int  CMDLINE_GetCount P_((CmdOpt * opt));

/* Defines mutually exclusive options. */
extern Bool CMDLINE_Exclude P_((CmdOpt * opt1, CmdOpt * opt2));

/* Defines dependent options. opt1 requires (depends on) opt2 */
extern Bool CMDLINE_Require P_((CmdOpt * opt1, CmdOpt * opt2));

/*
 * Defines an "option group". Option groups exist strictly for formatting
 * purposes. The affect the output produced by CMDLINE_Usage function
 */
extern CmdOptGrp * CMDLINE_CreateOptGrp P_((CmdLine * c, Str name));
extern void CMDLINE_AddToGroup P_((CmdOptGrp * group, CmdOpt * opt));

/* 
 * Parses command line. You can use CMDLINE_SetOutFile or CMDLINE_SetOutProc
 * to forward error messages anywhere you like. The 'unused' vector (if not 
 * NULL), receives the additional command line argumented that were not
 * interpreted as options or their parameters. If PARSE_NO_DUP is set, then
 * only repeatable options are allowed on the command line.
 */
extern Bool CMDLINE_Parse P_((CmdLine * c, Char * args [], int nargs, 
                              int flags, Vector * unused));
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
extern Bool CMDLINE_Parse1 P_((CmdLine * c, Char * args [], int nargs, 
                              int flags, Str * arg));

/* Flags for CMDLINE_Parse */
#define PARSE_NO_DUP    0x0001  /* disallow duplicate options */
#define PARSE_SILENT    0x0002  /* don't print error messages */

/* 
 * Prints usage. Zero or negative maxcol means no wrapping. You can use 
 * CMDLINE_SetOutFile or CMDLINE_SetOutProc to forward the output anywhere 
 * you like.
 */
extern void CMDLINE_Usage P_((CmdLine * c, Str extra, int maxcol));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_CMDLINE_OPT_H_ */
