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
/* File    : U:\wpsvob\wmlscript\work\ported\wmlsyacc.c
   Version : \main\13
   Date    : 22-Nov-00.17:25:03
*/


/*  A Bison parser, made from wmlsenc.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ACCESS	258
#define	AGENT	259
#define	ANDASSIGN	260
#define	ASSIGN	261
#define	B_LEFTSHIFTASSIGN	262
#define	B_LSHIFT	263
#define	B_RSIGNEDSHIFT	264
#define	B_RSIGNEDSHIFTASSIGN	265
#define	B_RSZSHIFT	266
#define	B_RZSHIFTASSIGN	267
#define	BANG	268
#define	BIT_AND	269
#define	BIT_OR	270
#define	BREAK	271
#define	COLON	272
#define	COMMA	273
#define	CONTINUE	274
#define	DBLQ_LITERAL	275
#define	DEC_LITERAL	276
#define	DECR	277
#define	DIV	278
#define	DIVASSIGN	279
#define	DOMAIN	280
#define	DOT	281
#define	ELSE	282
#define	EQ	283
#define	EXTERN	284
#define	EQUIV	285
#define	FLT_LITERAL	286
#define	FOR	287
#define	FUNCTION	288
#define	GE	289
#define	GT	290
#define	HASH	291
#define	HEX_LITERAL	292
#define	HOOK	293
#define	HTTP	294
#define	IDENTIFIER	295
#define	IF	296
#define	INCR	297
#define	ISVALID	298
#define	LBRACE	299
#define	LE	300
#define	LPAREN	301
#define	LT	302
#define	MINUS	303
#define	MINUSASSIGN	304
#define	META	305
#define	NAME	306
#define	NE	307
#define	OCT_LITERAL	308
#define	ORASSIGN	309
#define	PATH	310
#define	PLUS	311
#define	PLUSASSIGN	312
#define	PUBLIC	313
#define	RBRACE	314
#define	REM	315
#define	REMASSIGN	316
#define	RETURN	317
#define	RPAREN	318
#define	SC_AND	319
#define	SC_OR	320
#define	SEMICOLON	321
#define	SLASH	322
#define	SLASHASSIGN	323
#define	SQ_LITERAL	324
#define	STAR	325
#define	STARASSIGN	326
#define	TILDE	327
#define	TYPEOF	328
#define	URL	329
#define	USE	330
#define	USER	331
#define	VAR	332
#define	WHILE	333
#define	XOR	334
#define	XORASSIGN	335
#define	HEADER	336
#define	DELETE	337
#define	IN	338
#define	LIB	339
#define	NEW	340
#define	THIS	341
#define	VOID	342
#define	WITH	343
#define	CASE	344
#define	CATCH	345
#define	CLASS	346
#define	CONST	347
#define	DEBUGGER	348
#define	DEFAULT	349
#define	DO	350
#define	ENUM	351
#define	EXPORT	352
#define	EXTENDS	353
#define	FINALLY	354
#define	IMPORT	355
#define	PRIVATE	356
#define	SIZEOF	357
#define	STRUCT	358
#define	SUPER	359
#define	SWITCH	360
#define	THROW	361
#define	TRY	362
#define	LBRACKET	363
#define	RBRACKET	364
#define	INVALID	365
#define	TOK_FALSE	366
#define	TOK_TRUE	367
#define	EXPONENT	368
#define	TOK_NULL	369


/*
NOKIA WIRELESS SOFTWARE SOLUTIONS
CCC Automation Oy
WMLS Encoder









                GNU Bison 1.25 WMLScript parser

                SW Include Document








File:               include/wmlsyacc.c


Copyright (c) 1999.  Nokia Wireless Software Solutions




Change History:

VERSION    : 1.0
REASON     : Created.
DESCRIPTION: -

VERSION    : 1.3                JoS/CCC 210699
REASON     : Bug fix
DESCRIPTION: include file names changed to lower case

VERSION    : 1.8                JOS/CCC 040799
REASON     : Request change
DESCRIPTION: - public access control specifier removed. JOS/CCC 040799

*/




/*
-----------------------------------------------------------------------------

    DESCRIPTION

      GNU Bison 1.25 generated WMLScript parser.

-----------------------------------------------------------------------------
*/

#undef __cplusplus
#undef __GNUC__
#include "common.h"
#include "main.h"
#include "codenode.h"
#include "memmana.h"
#include "symtable.h"
#include "pragpool.h"
#include "bytecode.h"
#include "conspool.h"
#include "funcpool.h"
#include "wmlsyacc.h"
#include "lex.h"

#define YYPARSE_PARAM (struct Environment*)env
#define YYSTYPE struct CodeNode*

void yyerror(const char*, int iError, struct Environment * env);

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		261
#define	YYFLAG		-32768
#define	YYNTBASE	115

#define YYTRANSLATE(x) ((unsigned)(x) <= 369 ? yytranslate[x] : 175)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     8,    10,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    30,    32,    36,    38,    41,    46,
    51,    53,    55,    57,    59,    61,    64,    68,    70,    74,
    76,    79,    87,    94,    96,    98,   100,   103,   104,   106,
   110,   113,   117,   119,   122,   124,   126,   128,   131,   133,
   135,   138,   141,   143,   146,   150,   154,   156,   160,   162,
   165,   167,   170,   172,   178,   180,   184,   186,   188,   190,
   192,   194,   196,   198,   200,   202,   204,   206,   208,   210,
   212,   214,   218,   220,   224,   226,   230,   232,   236,   238,
   242,   244,   248,   252,   254,   258,   262,   266,   270,   272,
   276,   280,   284,   286,   290,   294,   296,   300,   304,   308,
   312,   314,   317,   320,   323,   326,   329,   332,   335,   338,
   340,   343,   346,   348,   350,   358,   364,   370,   372,   382,
   393,   394,   396,   399,   401,   403,   406,   410,   414,   417,
   419,   421,   424,   427,   432,   435,   439,   444,   449,   455,
   460,   466
};

static const short yyrhs[] = {    40,
     0,   116,     0,    46,   120,    63,     0,   118,     0,   119,
     0,   117,     0,   111,     0,   112,     0,    21,     0,    37,
     0,    53,     0,    31,     0,    20,     0,    69,     0,   145,
     0,   120,    18,   145,     0,   115,     0,   124,   127,     0,
   122,    36,   123,   127,     0,   125,    26,   126,   127,     0,
    40,     0,    40,     0,    40,     0,    40,     0,    40,     0,
    46,    63,     0,    46,   128,    63,     0,   145,     0,   128,
    18,   145,     0,   130,     0,   129,   130,     0,    29,    33,
   131,    46,   134,    63,   133,     0,    33,   132,    46,   134,
    63,   133,     0,    40,     0,    40,     0,   135,     0,   135,
    66,     0,     0,    40,     0,   134,    18,    40,     0,    44,
    59,     0,    44,   136,    59,     0,   137,     0,   136,   137,
     0,   135,     0,   139,     0,    66,     0,   120,    66,     0,
   162,     0,   163,     0,    19,    66,     0,    16,    66,     0,
   138,     0,    62,    66,     0,    62,   120,    66,     0,    77,
   140,    66,     0,   141,     0,   140,    18,   141,     0,   142,
     0,   142,   143,     0,    40,     0,     6,   144,     0,   148,
     0,   148,    38,   145,    17,   145,     0,   144,     0,   146,
   147,   145,     0,    40,     0,     6,     0,    71,     0,    68,
     0,    61,     0,    57,     0,    49,     0,     7,     0,    10,
     0,    12,     0,     5,     0,    54,     0,    80,     0,    24,
     0,   149,     0,   148,    65,   149,     0,   150,     0,   149,
    64,   150,     0,   151,     0,   150,    15,   151,     0,   152,
     0,   151,    79,   152,     0,   153,     0,   152,    14,   153,
     0,   154,     0,   153,    28,   154,     0,   153,    52,   154,
     0,   155,     0,   154,    47,   155,     0,   154,    35,   155,
     0,   154,    45,   155,     0,   154,    34,   155,     0,   156,
     0,   155,     8,   156,     0,   155,     9,   156,     0,   155,
    11,   156,     0,   157,     0,   156,    56,   157,     0,   156,
    48,   157,     0,   158,     0,   157,    70,   158,     0,   157,
    67,   158,     0,   157,    23,   158,     0,   157,    60,   158,
     0,   159,     0,    73,   158,     0,    43,   158,     0,    42,
    40,     0,    22,    40,     0,    56,   158,     0,    48,   158,
     0,    72,   158,     0,    13,   158,     0,   121,     0,   160,
    42,     0,   161,    22,     0,    40,     0,    40,     0,    41,
    46,   120,    63,   137,    27,   137,     0,    41,    46,   120,
    63,   137,     0,    78,    46,   120,    63,   137,     0,   164,
     0,    32,    46,   165,    66,   165,    66,   165,    63,   137,
     0,    32,    46,    77,   140,    66,   165,    66,   165,    63,
   137,     0,     0,   120,     0,   167,   129,     0,   129,     0,
   168,     0,   167,   168,     0,    75,   169,    66,     0,    74,
   170,   119,     0,     3,   171,     0,   172,     0,    40,     0,
    25,   119,     0,    55,   119,     0,    25,   119,    55,   119,
     0,    50,   173,     0,    51,   174,   174,     0,    51,   174,
   174,   174,     0,    39,    30,   174,   174,     0,    39,    30,
   174,   174,   174,     0,    76,     4,   119,   119,     0,    76,
     4,   119,   119,   119,     0,    20,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   183,   185,   186,   189,   191,   192,   195,   197,   200,   202,
   203,   204,   207,   209,   212,   214,   221,   223,   229,   237,
   246,   254,   262,   270,   278,   286,   288,   291,   293,   300,
   302,   305,   314,   323,   331,   339,   341,   344,   346,   351,
   358,   364,   367,   369,   376,   378,   379,   380,   381,   382,
   383,   384,   385,   388,   390,   393,   397,   399,   406,   408,
   418,   426,   430,   432,   439,   441,   449,   457,   460,   462,
   464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
   486,   488,   495,   497,   504,   506,   513,   515,   522,   524,
   531,   533,   538,   545,   547,   552,   557,   562,   569,   571,
   576,   581,   588,   590,   595,   602,   604,   609,   614,   619,
   626,   628,   629,   630,   637,   644,   645,   646,   647,   650,
   652,   658,   666,   674,   682,   688,   695,   701,   704,   710,
   717,   719,   722,   724,   727,   729,   732,   736,   744,   745,
   748,   756,   762,   767,   774,   778,   784,   789,   794,   799,
   804,   811
};

static const char * const yytname[] = {   "$","error","$undefined.","ACCESS",
"AGENT","ANDASSIGN","ASSIGN","B_LEFTSHIFTASSIGN","B_LSHIFT","B_RSIGNEDSHIFT",
"B_RSIGNEDSHIFTASSIGN","B_RSZSHIFT","B_RZSHIFTASSIGN","BANG","BIT_AND","BIT_OR",
"BREAK","COLON","COMMA","CONTINUE","DBLQ_LITERAL","DEC_LITERAL","DECR","DIV",
"DIVASSIGN","DOMAIN","DOT","ELSE","EQ","EXTERN","EQUIV","FLT_LITERAL","FOR",
"FUNCTION","GE","GT","HASH","HEX_LITERAL","HOOK","HTTP","IDENTIFIER","IF","INCR",
"ISVALID","LBRACE","LE","LPAREN","LT","MINUS","MINUSASSIGN","META","NAME","NE",
"OCT_LITERAL","ORASSIGN","PATH","PLUS","PLUSASSIGN","PUBLIC","RBRACE","REM",
"REMASSIGN","RETURN","RPAREN","SC_AND","SC_OR","SEMICOLON","SLASH","SLASHASSIGN",
"SQ_LITERAL","STAR","STARASSIGN","TILDE","TYPEOF","URL","USE","USER","VAR","WHILE",
"XOR","XORASSIGN","HEADER","DELETE","IN","LIB","NEW","THIS","VOID","WITH","CASE",
"CATCH","CLASS","CONST","DEBUGGER","DEFAULT","DO","ENUM","EXPORT","EXTENDS",
"FINALLY","IMPORT","PRIVATE","SIZEOF","STRUCT","SUPER","SWITCH","THROW","TRY",
"LBRACKET","RBRACKET","INVALID","TOK_FALSE","TOK_TRUE","EXPONENT","TOK_NULL",
"primary_expression","literal","boolean_literal","integer_literal","string_literal",
"expression","call_expression","call_expression_external_script_identifier",
"call_expression_external_script_function_identifier","call_expression_local_function_identifier",
"call_expression_library_identifier","call_expression_library_function_identifier",
"arguments","argument_list","function_declarations","function_declaration","function_declaration_extern_identifier",
"function_declaration_local_identifier","function_declaration_block","formal_parameter_list",
"block","statement_list","statement","return_statement","variable_statement",
"variable_declaration_list","variable_declaration","variable_declaration_identifier",
"variable_initializer","conditional_expression","assignment_expression","assignment_expression_identifier",
"assignment_operator","logical_OR_expression","logical_AND_expression","bitwise_OR_expression",
"bitwise_XOR_expression","bitwise_AND_expression","equality_expression","relational_expression",
"shift_expression","additive_expression","multiplicative_expression","unary_expression",
"postfix_expression","postfix_expression_incr_identifier","postfix_expression_decr_identifier",
"if_statement","iteration_statement","for_statement","opt_expression","compilation_unit",
"pragmas","pragma","pragma_declaration","pragma_declaration_url_identifier",
"access_control_specifier","meta_pragma","meta_specifier","meta_string_literal",
""
};
#endif

static const short yyr1[] = {     0,
   115,   115,   115,   116,   116,   116,   117,   117,   118,   118,
   118,   118,   119,   119,   120,   120,   121,   121,   121,   121,
   122,   123,   124,   125,   126,   127,   127,   128,   128,   129,
   129,   130,   130,   131,   132,   133,   133,   134,   134,   134,
   135,   135,   136,   136,   137,   137,   137,   137,   137,   137,
   137,   137,   137,   138,   138,   139,   140,   140,   141,   141,
   142,   143,   144,   144,   145,   145,   146,   147,   147,   147,
   147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
   148,   148,   149,   149,   150,   150,   151,   151,   152,   152,
   153,   153,   153,   154,   154,   154,   154,   154,   155,   155,
   155,   155,   156,   156,   156,   157,   157,   157,   157,   157,
   158,   158,   158,   158,   158,   158,   158,   158,   158,   159,
   159,   159,   160,   161,   162,   162,   163,   163,   164,   164,
   165,   165,   166,   166,   167,   167,   168,   169,   169,   169,
   170,   171,   171,   171,   172,   173,   173,   173,   173,   173,
   173,   174
};

static const short yyr2[] = {     0,
     1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     3,     1,     2,     4,     4,
     1,     1,     1,     1,     1,     2,     3,     1,     3,     1,
     2,     7,     6,     1,     1,     1,     2,     0,     1,     3,
     2,     3,     1,     2,     1,     1,     1,     2,     1,     1,
     2,     2,     1,     2,     3,     3,     1,     3,     1,     2,
     1,     2,     1,     5,     1,     3,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
     1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
     3,     3,     1,     3,     3,     1,     3,     3,     3,     3,
     1,     2,     2,     2,     2,     2,     2,     2,     2,     1,
     2,     2,     1,     1,     7,     5,     5,     1,     9,    10,
     0,     1,     2,     1,     1,     2,     3,     3,     2,     1,
     1,     2,     2,     4,     2,     3,     4,     4,     5,     4,
     5,     1
};

static const short yydefact[] = {     0,
     0,     0,     0,   134,    30,     0,   135,     0,    35,     0,
     0,     0,     0,     0,   140,    31,   133,   136,    34,     0,
    38,     0,     0,   139,     0,     0,     0,   145,   141,     0,
   137,    38,    39,     0,    13,    14,   142,   143,     0,   152,
     0,     0,   138,     0,     0,     0,     0,     0,   146,     0,
     0,    40,     0,    33,    36,   144,   148,   147,   150,    32,
     0,     0,     0,     9,     0,    12,     0,    10,     1,     0,
     0,     0,     0,     0,    11,     0,    41,     0,    47,     0,
     0,     0,     0,     7,     8,    17,     2,     6,     4,     5,
     0,   120,     0,     0,     0,    45,     0,    43,    53,    46,
    65,    15,     0,    63,    81,    83,    85,    87,    89,    91,
    94,    99,   103,   106,   111,     0,     0,    49,    50,   128,
    37,   149,   151,     1,   119,    52,    51,   115,   131,     0,
   114,   113,     0,   117,   116,    54,     0,   118,   112,    61,
     0,    57,    59,     0,     0,    48,     0,     0,    18,     0,
    42,    44,    77,    68,    74,    75,    76,    80,    73,    78,
    72,    71,    70,    69,    79,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   121,   122,     0,
   132,     0,     0,     3,    55,     0,    56,     0,    60,     0,
    16,    22,     0,    26,     0,    28,    25,     0,    66,     0,
    82,    84,    86,    88,    90,    92,    93,    98,    96,    97,
    95,   100,   101,   102,   105,   104,   109,   110,   108,   107,
     0,   131,     0,    58,    62,     0,    19,     0,    27,    20,
     0,   131,     0,   126,   127,    29,    64,     0,   131,     0,
   131,     0,   125,     0,     0,     0,   129,   130,     0,     0,
     0
};

static const short yydefgoto[] = {    86,
    87,    88,    89,    90,    91,    92,    93,   203,    94,    95,
   208,   149,   205,     4,     5,    20,    10,    54,    34,    96,
    97,    98,    99,   100,   141,   142,   143,   199,   101,   102,
   103,   166,   104,   105,   106,   107,   108,   109,   110,   111,
   112,   113,   114,   115,   116,   117,   118,   119,   120,   192,
   259,     6,     7,    14,    30,    24,    15,    28,    41
};

static const short yypact[] = {     7,
   -26,   -17,     3,    79,-32768,     7,-32768,    -7,-32768,   -29,
   -21,    34,    -1,    -6,-32768,-32768,    79,-32768,-32768,    18,
    30,   -11,   -11,-32768,    37,    80,   112,-32768,-32768,   -11,
-32768,    30,-32768,    -8,-32768,-32768,    65,-32768,    80,-32768,
    80,   -11,-32768,    13,    83,   102,   -11,    80,    80,   -11,
   102,-32768,     6,-32768,    76,-32768,    80,-32768,   -11,-32768,
   412,   100,   101,-32768,   114,-32768,   124,-32768,   505,   139,
   146,   412,   449,   412,-32768,   412,-32768,   258,-32768,   412,
   412,   148,   143,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   -15,-32768,   155,   149,   166,-32768,   131,-32768,-32768,-32768,
-32768,-32768,   532,    31,   130,   181,   119,   185,    46,    98,
   128,   -24,    21,-32768,-32768,   159,   180,-32768,-32768,-32768,
-32768,-32768,-32768,    67,-32768,-32768,-32768,-32768,   312,   449,
-32768,-32768,    17,-32768,-32768,-32768,    -5,-32768,-32768,-32768,
    -3,-32768,   199,   449,   449,-32768,   167,   366,-32768,   170,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   449,   449,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,-32768,-32768,   148,
   188,   145,    23,-32768,-32768,   148,-32768,   412,-32768,    24,
-32768,-32768,   149,-32768,    27,-32768,-32768,   149,-32768,   195,
   130,   181,   119,   185,    46,    98,    98,   128,   128,   128,
   128,   -24,   -24,   -24,    21,    21,-32768,-32768,-32768,-32768,
     0,   449,   204,-32768,-32768,   204,-32768,   449,-32768,-32768,
   449,   449,   147,   187,-32768,-32768,-32768,   150,   449,   204,
   449,   152,-32768,   156,   204,   204,-32768,-32768,   218,   221,
-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,    72,   -73,-32768,-32768,-32768,-32768,-32768,
-32768,   -74,-32768,   216,    88,-32768,-32768,   176,   196,    89,
-32768,   -95,-32768,-32768,    39,    35,-32768,-32768,    32,  -137,
-32768,-32768,-32768,    64,    68,    63,    69,    62,   -25,     5,
   -23,   -18,   -60,-32768,-32768,-32768,-32768,-32768,-32768,  -121,
-32768,-32768,   232,-32768,-32768,-32768,-32768,-32768,    58
};


#define	YYLAST		612


static const short yytable[] = {   133,
   125,   152,   145,    22,   137,    11,     8,   201,    35,    45,
   206,   132,   145,   134,   196,   135,    21,   196,    61,   138,
   139,    62,     9,   182,    63,    35,    64,    65,   209,   210,
    45,   183,    19,    23,   145,     1,    66,    67,    29,     2,
   145,   145,    68,   184,   238,    69,    70,    71,    72,    53,
   146,    73,    12,    74,    46,   191,   193,    36,    75,    31,
   195,    76,   197,    32,    77,   242,    39,    78,   167,    33,
   200,    79,    25,   173,    36,    51,    13,    80,    81,   194,
   185,     3,    82,    83,    26,   233,   236,   186,  -124,   239,
   187,    16,   -24,    37,    38,   168,    48,   174,    49,    40,
   246,    43,   -21,   247,    16,    57,    58,     1,  -123,    27,
   243,     2,   -23,    50,   122,    42,    84,    85,    56,    47,
   248,    59,    52,   227,   228,   229,   230,   252,   237,   254,
   123,   175,   176,   240,    55,   179,   180,   244,   181,    55,
   245,   121,   177,    61,   178,    53,    62,   216,   217,    63,
    35,    64,    65,   128,   253,   222,   223,   224,   191,   257,
   258,    66,    67,   225,   226,   126,   127,    68,   191,   129,
    69,    70,    71,    72,    53,   191,    73,   191,    74,   218,
   219,   220,   221,    75,   130,   131,    76,   140,   144,   151,
   147,   150,    78,   169,   148,   170,    79,   171,   172,    36,
   188,   189,    80,    81,   198,   145,   202,    82,    83,   207,
   232,   241,   249,   250,   255,   251,    61,   260,   256,    62,
   261,    17,    63,    35,    64,    65,    60,    44,   231,   235,
   234,   211,   213,   215,    66,    67,   212,    18,     0,   214,
    68,    84,    85,    69,    70,    71,    72,    53,     0,    73,
     0,    74,     0,     0,     0,     0,    75,     0,     0,    76,
     0,     0,     0,     0,     0,    78,     0,     0,     0,    79,
    61,     0,    36,     0,     0,    80,    81,    35,    64,    65,
    82,    83,     0,     0,     0,     0,     0,     0,    66,     0,
     0,     0,     0,     0,    68,     0,     0,    69,     0,    71,
    72,     0,     0,    73,     0,    74,     0,     0,     0,     0,
    75,     0,     0,    76,    84,    85,     0,     0,     0,     0,
     0,     0,     0,   136,    61,     0,    36,     0,     0,    80,
    81,    35,    64,    65,     0,     0,     0,     0,     0,     0,
     0,     0,    66,     0,     0,     0,     0,     0,    68,     0,
     0,    69,     0,    71,    72,     0,     0,    73,     0,    74,
     0,     0,     0,     0,    75,     0,     0,    76,    84,    85,
     0,     0,     0,     0,     0,     0,     0,     0,    61,     0,
    36,     0,     0,    80,    81,    35,    64,    65,   190,     0,
     0,     0,     0,     0,     0,     0,    66,     0,     0,     0,
     0,     0,    68,     0,     0,    69,     0,    71,    72,     0,
     0,    73,     0,    74,     0,     0,     0,     0,    75,     0,
     0,    76,    84,    85,    61,     0,     0,     0,   204,     0,
     0,    35,    64,    65,    36,     0,     0,    80,    81,     0,
     0,     0,    66,     0,     0,     0,     0,     0,    68,     0,
     0,   124,     0,    71,    72,     0,     0,    73,     0,    74,
     0,    61,     0,     0,    75,     0,     0,    76,    35,    64,
    65,     0,     0,     0,     0,     0,    84,    85,     0,    66,
    36,     0,     0,    80,    81,    68,     0,     0,    69,     0,
    71,    72,     0,     0,    73,     0,    74,     0,     0,     0,
     0,    75,     0,     0,    76,     0,     0,     0,     0,   -67,
   -67,   -67,     0,     0,   -67,     0,   -67,    36,     0,     0,
    80,    81,    84,    85,     0,     0,  -124,     0,   -67,     0,
   -24,     0,     0,     0,     0,     0,   153,   154,   155,     0,
   -21,   156,     0,   157,     0,     0,  -123,     0,     0,     0,
   -23,     0,     0,   -67,     0,   158,     0,     0,   -67,    84,
    85,   -67,     0,     0,     0,   -67,     0,     0,     0,     0,
     0,     0,   -67,     0,     0,   -67,     0,     0,     0,     0,
   159,     0,     0,     0,   -67,   160,     0,     0,   161,     0,
     0,     0,   162,     0,     0,     0,     0,     0,     0,   163,
     0,     0,   164,     0,     0,     0,     0,     0,     0,     0,
     0,   165
};

static const short yycheck[] = {    73,
    61,    97,    18,    25,    78,     3,    33,   145,    20,    18,
   148,    72,    18,    74,    18,    76,    46,    18,    13,    80,
    81,    16,    40,    48,    19,    20,    21,    22,   166,   167,
    18,    56,    40,    55,    18,    29,    31,    32,    40,    33,
    18,    18,    37,    23,    18,    40,    41,    42,    43,    44,
    66,    46,    50,    48,    63,   129,   130,    69,    53,    66,
    66,    56,    66,    46,    59,    66,    30,    62,    38,    40,
   144,    66,    39,    28,    69,    63,    74,    72,    73,    63,
    60,    75,    77,    78,    51,    63,    63,    67,    22,    63,
    70,     4,    26,    22,    23,    65,    39,    52,    41,    20,
   238,    30,    36,   241,    17,    48,    49,    29,    42,    76,
   232,    33,    46,    42,    57,     4,   111,   112,    47,    55,
   242,    50,    40,   184,   185,   186,   187,   249,   203,   251,
    59,    34,    35,   208,    46,     8,     9,   233,    11,    51,
   236,    66,    45,    13,    47,    44,    16,   173,   174,    19,
    20,    21,    22,    40,   250,   179,   180,   181,   232,   255,
   256,    31,    32,   182,   183,    66,    66,    37,   242,    46,
    40,    41,    42,    43,    44,   249,    46,   251,    48,   175,
   176,   177,   178,    53,    46,    40,    56,    40,    46,    59,
    36,    26,    62,    64,    46,    15,    66,    79,    14,    69,
    42,    22,    72,    73,     6,    18,    40,    77,    78,    40,
    66,    17,    66,    27,    63,    66,    13,     0,    63,    16,
     0,     6,    19,    20,    21,    22,    51,    32,   190,   198,
   196,   168,   170,   172,    31,    32,   169,     6,    -1,   171,
    37,   111,   112,    40,    41,    42,    43,    44,    -1,    46,
    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,
    -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,    66,
    13,    -1,    69,    -1,    -1,    72,    73,    20,    21,    22,
    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
    -1,    -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    42,
    43,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,
    53,    -1,    -1,    56,   111,   112,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    66,    13,    -1,    69,    -1,    -1,    72,
    73,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    37,    -1,
    -1,    40,    -1,    42,    43,    -1,    -1,    46,    -1,    48,
    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,   111,   112,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,
    69,    -1,    -1,    72,    73,    20,    21,    22,    77,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,
    -1,    -1,    37,    -1,    -1,    40,    -1,    42,    43,    -1,
    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,
    -1,    56,   111,   112,    13,    -1,    -1,    -1,    63,    -1,
    -1,    20,    21,    22,    69,    -1,    -1,    72,    73,    -1,
    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    37,    -1,
    -1,    40,    -1,    42,    43,    -1,    -1,    46,    -1,    48,
    -1,    13,    -1,    -1,    53,    -1,    -1,    56,    20,    21,
    22,    -1,    -1,    -1,    -1,    -1,   111,   112,    -1,    31,
    69,    -1,    -1,    72,    73,    37,    -1,    -1,    40,    -1,
    42,    43,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,
    -1,    53,    -1,    -1,    56,    -1,    -1,    -1,    -1,     5,
     6,     7,    -1,    -1,    10,    -1,    12,    69,    -1,    -1,
    72,    73,   111,   112,    -1,    -1,    22,    -1,    24,    -1,
    26,    -1,    -1,    -1,    -1,    -1,     5,     6,     7,    -1,
    36,    10,    -1,    12,    -1,    -1,    42,    -1,    -1,    -1,
    46,    -1,    -1,    49,    -1,    24,    -1,    -1,    54,   111,
   112,    57,    -1,    -1,    -1,    61,    -1,    -1,    -1,    -1,
    -1,    -1,    68,    -1,    -1,    71,    -1,    -1,    -1,    -1,
    49,    -1,    -1,    -1,    80,    54,    -1,    -1,    57,    -1,
    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,    68,
    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    80
};
#define YYPURE 1


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {/* MODIFIED BY CCC */  yyerror ("syntax error: cannot back up", WMLS_STATUS_ERROR_SYNTAX, env); YYERROR; } \
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif


/*          COMMENTED BY CCC 
#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else 
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif 
#endif
*/
/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/


YYSTYPE	yylval = NULL;			/*  the semantic value of the		*/ /* MODIFIED BY CCC */
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

/* MODIFIED BY CCC ==> */
int yyparse(struct Environment *env)
{
  char* yytext = env->lex.pcTokenText;
  BYTE bBackup;    
/* <== MODIFIED BY CCC */

  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval = NULL; /* MODIFIED BY CCC */
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval = NULL;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
    {	/* MODIFIED BY CCC */
      yyerror("parser stack overflow", WMLS_STATUS_ERROR_PARSER_STACK_OVERFLOW, env);   /* MODIFIED BY CCC */
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = yylex(&(env->lex));  /* MODIFIED BY CCC */
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      /* ADDED BY CCC */
      if(env->lex.iStatus != WMLS_STATUS_OK)
      {
          yyerror("lex error", env->lex.iStatus, env);
          YYERROR;
      }


#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  /* ADDED BY CCC ---> */
  yytext = env->lex.pcTokenText;
  bBackup = yytext[env->lex.iTokenTextLen];
  yytext[env->lex.iTokenTextLen] = 0;
  /* <--- ADDED BY CCC */

  switch (yyn) {

case 1:
{ yyval = GetVariable(env, yytext); if(yyval == NULL) YYERROR;;
    break;}
case 2:
{ yyval = yyvsp[0];;
    break;}
case 3:
{ yyval = yyvsp[-1];;
    break;}
case 7:
{ yyval = GetLiteral(env, yytext, NOQUOTE_STRING_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 8:
{ yyval = GetLiteral(env, yytext, NOQUOTE_STRING_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 9:
{ yyval = GetLiteral(env, yytext, DEC_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 10:
{ yyval = GetLiteral(env, yytext, HEX_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 11:
{ yyval = GetLiteral(env, yytext, OCT_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 12:
{ yyval = GetLiteral(env, yytext, FLT_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 13:
{ yyval = GetLiteral(env, yytext, DBLQ_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 14:
{ yyval = GetLiteral(env, yytext, SQ_LITERAL); if(yyval == NULL) YYERROR;;
    break;}
case 16:
{ 
            yyval = AddNodeToExpressionList(env, yyvsp[-2], yyvsp[0]); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 18:
{
            yyval = GetCallLocal(env, yyvsp[-1], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 19:
{
            yyval = GetCallExternal(env, yyvsp[-3], yyvsp[-1], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 20:
{
            yyval = GetCallLibrary(env, yyvsp[-3], yyvsp[-1], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 21:
{
            yyval = GetLiteral(env, yytext, NOQUOTE_STRING_LITERAL); 
            if(yyval == NULL) YYERROR; 
        ;
    break;}
case 22:
{
            yyval = GetLiteral(env, yytext, NOQUOTE_STRING_LITERAL); 
            if(yyval == NULL) YYERROR; 
        ;
    break;}
case 23:
{ 
            yyval = GetIdFunction(env, yytext); 
            if(yyval == NULL) YYERROR; 
        ;
    break;}
case 24:
{ 
            yyval = GetIdLibrary(env, yytext);
            if(yyval == NULL) YYERROR; 
        ;
    break;}
case 25:
{ 
            yyval = GetIdLibraryFunction(env, yytext);
            if(yyval == NULL) YYERROR; 
        ;
    break;}
case 26:
{ yyval = NULL; ;
    break;}
case 27:
{ yyval = yyvsp[-1]; ;
    break;}
case 29:
{ 
            yyval = AddNodeToArgumentList(env, yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 32:
{
            yyval = SetFunctionBody(env, yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 33:
{
            yyval = SetFunctionBody(env, yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 34:
{
            yyval = (YYSTYPE)AddFunction(env, yytext, 1);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 35:
{
            yyval = (YYSTYPE)AddFunction(env, yytext, 0);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 37:
{ yyval = yyvsp[-1];;
    break;}
case 39:
{
            yyval = (YYSTYPE)AddFormalParameter(env, yytext);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 40:
{
            yyval = (YYSTYPE)AddFormalParameter(env, yytext);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 41:
{ 
            yyval = GetEmptyNode(env); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 42:
{ yyval = yyvsp[-1];;
    break;}
case 44:
{
            yyval = AddNodeToStatementList(env, yyvsp[-1], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 47:
{ yyval = GetEmptyNode(env); if(yyval == NULL) YYERROR;
    break;}
case 48:
{ yyval = yyvsp[-1]; ;
    break;}
case 51:
{ yyval = GetStatementContinue(env); if(yyval == NULL) YYERROR;;
    break;}
case 52:
{ yyval = GetStatementBreak(env); if(yyval == NULL) YYERROR;;
    break;}
case 53:
{ yyval = GetStatementReturn(env, yyvsp[0]); if(yyval == NULL) YYERROR;;
    break;}
case 54:
{ yyval = NULL;;
    break;}
case 55:
{ yyval = yyvsp[-1]; ;
    break;}
case 56:
{ yyval = yyvsp[-1]; ;
    break;}
case 57:
{ yyval = yyvsp[0];;
    break;}
case 58:
{   
            yyval = AddNodeToList(env, yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 60:
{   
            yyval = GetAssignmentOperator(env, ASSIGN);
            if(yyval == NULL) YYERROR;
            yyval = GetAssignment(env, yyval, yyvsp[-1], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 61:
{   
            yyval = GetVariableDeclaration(env, yytext); 
            if(yyval == NULL) YYERROR; 
        ;
    break;}
case 62:
{ yyval = yyvsp[0]; ;
    break;}
case 64:
{
            yyval = GetStatementCondExpress(env, yyvsp[-4], yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 66:
{   
            yyval = GetAssignment(env, yyvsp[-1], yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 67:
{   
            yyval = GetVariable(env, yytext); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 68:
{   yyval = GetAssignmentOperator(env, ASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 69:
{   yyval = GetAssignmentOperator(env, STARASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 70:
{   yyval = GetAssignmentOperator(env, SLASHASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 71:
{   yyval = GetAssignmentOperator(env, REMASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 72:
{   yyval = GetAssignmentOperator(env, PLUSASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 73:
{   yyval = GetAssignmentOperator(env, MINUSASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 74:
{   yyval = GetAssignmentOperator(env, B_LEFTSHIFTASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 75:
{   yyval = GetAssignmentOperator(env, B_RSIGNEDSHIFTASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 76:
{   yyval = GetAssignmentOperator(env, B_RZSHIFTASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 77:
{   yyval = GetAssignmentOperator(env, ANDASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 78:
{   yyval = GetAssignmentOperator(env, ORASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 79:
{   yyval = GetAssignmentOperator(env, XORASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 80:
{   yyval = GetAssignmentOperator(env, DIVASSIGN); if(yyval == NULL) YYERROR; ;
    break;}
case 82:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], SC_OR);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 84:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], SC_AND);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 86:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], BIT_OR);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 88:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], XOR);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 90:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], BIT_AND);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 92:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], EQ);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 93:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], NE);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 95:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], LT);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 96:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], GT);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 97:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], LE);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 98:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], GE);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 100:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], B_LSHIFT);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 101:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], B_RSIGNEDSHIFT);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 102:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], B_RSZSHIFT);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 104:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], PLUS);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 105:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], MINUS);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 107:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], STAR);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 108:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], SLASH);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 109:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], DIV);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 110:
{   
            yyval = GetBinaryOperation(env, yyvsp[-2], yyvsp[0], REM);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 112:
{ yyval = GetUnaryOperation(env, yyvsp[0], TYPEOF); if(yyval == NULL) YYERROR;;
    break;}
case 113:
{ yyval = GetUnaryOperation(env, yyvsp[0], ISVALID); if(yyval == NULL) YYERROR;;
    break;}
case 114:
{ 
            yyval = GetVariable(env, yytext);
            if(yyval == NULL) YYERROR;
            yyval = GetPrefix(env, yyval, INCR); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 115:
{ 
            yyval = GetVariable(env, yytext);
            if(yyval == NULL) YYERROR;
            yyval = GetPrefix(env, yyval, DECR); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 116:
{ yyval = GetUnaryOperation(env, yyvsp[0], PLUS); if(yyval == NULL) YYERROR;;
    break;}
case 117:
{ yyval = GetUnaryOperation(env, yyvsp[0], MINUS); if(yyval == NULL) YYERROR;;
    break;}
case 118:
{ yyval = GetUnaryOperation(env, yyvsp[0], TILDE); if(yyval == NULL) YYERROR;;
    break;}
case 119:
{ yyval = GetUnaryOperation(env, yyvsp[0], BANG); if(yyval == NULL) YYERROR;;
    break;}
case 121:
{ 
            yyval = GetPostfix(env, yyvsp[-1], INCR); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 122:
{ 
            yyval = GetPostfix(env, yyvsp[-1], DECR); 
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 123:
{
            yyval = GetVariable(env, yytext);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 124:
{
            yyval = GetVariable(env, yytext);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 125:
{
            yyval = GetStatementIf(env, yyvsp[-4], yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 126:
{
            yyval = GetStatementIf(env, yyvsp[-2], yyvsp[0], NULL);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 127:
{
            yyval = GetStatementWhile(env, yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 129:
{
            yyval = GetStatementFor(env, yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 130:
{
            yyval = GetStatementFor(env, yyvsp[-6], yyvsp[-4], yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 131:
{ yyval = NULL; ;
    break;}
case 138:
{
            yyval = (YYSTYPE)1;
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 141:
{
            yyval = GetLiteral(env, yytext, NOQUOTE_STRING_LITERAL);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 142:
{
            yyval = (YYSTYPE)PragmaAccessControl(env, yyvsp[0], NULL);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 143:
{
            yyval = (YYSTYPE)PragmaAccessControl(env, NULL, yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 144:
{
            yyval = (YYSTYPE)PragmaAccessControl(env, yyvsp[-2], yyvsp[0]);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 146:
{
            yyval = (YYSTYPE)PragmaMeta(env, yyvsp[-1], yyvsp[0], NULL, NAME);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 147:
{
            yyval = (YYSTYPE)PragmaMeta(env, yyvsp[-2], yyvsp[-1], yyvsp[0], NAME);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 148:
{
            yyval = (YYSTYPE)PragmaMeta(env, yyvsp[-1], yyvsp[0], NULL, HTTP);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 149:
{
            yyval = (YYSTYPE)PragmaMeta(env, yyvsp[-2], yyvsp[-1], yyvsp[0], HTTP);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 150:
{
            yyval = (YYSTYPE)PragmaMeta(env, yyvsp[-1], yyvsp[0], NULL, USER);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 151:
{
            yyval = (YYSTYPE)PragmaMeta(env, yyvsp[-2], yyvsp[-1], yyvsp[0], USER);
            if(yyval == NULL) YYERROR;
        ;
    break;}
case 152:
{ yyval = GetLiteralMeta(env, yytext); if(yyval == NULL) YYERROR;;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */

  yytext[env->lex.iTokenTextLen] = bBackup;   /* ADDED BY CCC */

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  
	  msg = (char *) WMLS_malloc(size + 15); /* Modified by CCC */
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
          yyerror(msg, WMLS_STATUS_ERROR_PARSE, env); /* MODIFIED BY CCC */
          WMLS_free(msg);	/* MODIFIED BY CCC */
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded", WMLS_STATUS_ERROR_PARSE_VIRTUAL_MEMORY, env); /* MODIFIED BY CCC */
	}
      else
#endif /* YYERROR_VERBOSE */
    yyerror("parse error", WMLS_STATUS_ERROR_PARSE, env);   /* MODIFIED BY CCC */
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  /* MODIFIED BY CCC */
  if (yyssp == yyss)
  {
      yyerror("", WMLS_STATUS_ERROR_PARSE, env);      
      YYABORT;
  }

  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}


/*********************************************************************

 FUNCTION     : yyerror

**********************************************************************

 AUTHOR:        Timo Rautiala, Jouni Sipola / CCC Companies
 DATE:          19.05.1999.

 RETURN: void 

 PARAMETERS:
        char* s
        int iError
        struct Environment * env

 DESCRIPTION: 
        Modified yyerror function. Copies status, error line number,
        and error info into Environment struct if errors were found
        during lexical scan. Otherwise Environment struct already 
        holds the error occurred during the grammar check operation.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
#undef const
void yyerror(const char* s, int iError, struct Environment * env) /* MODIFIED BY CCC */
{
    if(env->status == WMLS_STATUS_OK)
    {
        if(env->lex.iStatus != WMLS_STATUS_OK)
            env->status = env->lex.iStatus;
        else
            env->status = iError;
    }
        
    if(env->iErrorLineNumber == 0)
        env->iErrorLineNumber = env->lex.iRow;

    env->iErrorColumnNumber = env->lex.iTokenColumn;

    if(env->lex.iTokenTextLen > ERROR_INFO_LEN-1)
        env->lex.iTokenTextLen = ERROR_INFO_LEN-1;

    if(env->lex.iTokenTextLen > 0)
    {
        strncpy(env->pcErrorInfo, env->lex.pcTokenText, env->lex.iTokenTextLen);
        env->pcErrorInfo[env->lex.iTokenTextLen] = 0;
    }
    else
    {
        env->pcErrorInfo[0]=0;
    }
}


/*********************************************************************

 FUNCTION     : yywrap

**********************************************************************

 AUTHOR:        Jouni Sipola / CCC Companies
 DATE:          19.05.1999.

 RETURN: int 
        

 PARAMETERS:

 DESCRIPTION: 
        Lexer calls this function when it founds the EOF. 
        Return value 1 means that there is no more inputBuffers to 
        scan and the execution stops.
    
 NOTE: 
        Since flex 2.4 will change the default yywrap() from a macro 
        to a library routine, so scanners that do not define their 
        own yywrap() function must be linked with the library.

 CHANGES: 

*********************************************************************/
int yywrap()
{
    return 1;
}
