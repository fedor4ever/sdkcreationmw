/*
 * $Id: s_def.h,v 1.46 2006/03/27 21:52:32 slava Exp $
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

#ifndef _SLAVA_DEF_H_
#define _SLAVA_DEF_H_

#ifndef DEBUG
#  ifdef _DEBUG
#    define DEBUG 1
#  else
#    define DEBUG 0
#  endif
#endif

/* OS dependent stuff */
#include "s_os.h"
#include "s_ver.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* define 8,16,32 and 64 bit integer types */
typedef int8_t    I8s;      /*  8 bit integer */
typedef uint8_t   I8u;      /*  8 bit integer */
typedef int16_t   I16s;     /* 16 bit integer */
typedef uint16_t  I16u;     /* 16 bit integer */
typedef int32_t   I32s;     /* 32 bit integer */
typedef uint32_t  I32u;     /* 32 bit integer */
typedef int64_t   I64s;     /* 64 bit integer */
typedef uint64_t  I64u;     /* 64 bit integer */

/* string type */
typedef const Char* Str;

/* const pointer */
typedef const void * ConstPtr;

/* boolean type */
typedef enum __Bool {
    False = 0,
    True  = 1,
    MayBe = 2   /* Hey, why not? */
} Bool;

/* converts any non-zero value into True, zero into False */
#define BoolValue(x) ((x) ? True : False)

/* 
 * elapsed time in milliseconds or date in milliseconds since 
 * January 1st 1970, 00:00 GMT (which is compatible with time 
 * representation used by Java)
 */
typedef I64s Time;

/*
 * TIME_C macro should be used to initialize a Time variable with a 
 * fixed value.
 */
#define TIME_C(_t) __INT64_C(_t)

#ifdef NOTHING
#undef NOTHING
#endif
#define NOTHING ((void)0)

#define TRUE_STRING TEXT("true")
#define FALSE_STRING TEXT("false")

/* is this platform dependent ? */
#define __W(x) L##x
#define _W(x) __W(x)

/* useful macros */
#ifndef MIN
#  define MIN(x,y) (((x)<(y))?(x):(y))
#endif /* MIN */
#ifndef MAX
#  define MAX(x,y) (((x)>(y))?(x):(y))
#endif /* MAX */
#ifndef ABS
#  define ABS(x)   (((x)>=0)?(x):(-(x)))
#endif /* ABS */

#define COUNT(array) ((int)(sizeof(array)/sizeof(array[0])))
#define OFFSET(type,field) ((int)(((char*)&(((type *)0)->field))-((char*)0)))
#define FIELDSIZE(type,field) ((int)sizeof(((type *)0)->field))
#define NEXTOFFSET(type,field)  (OFFSET(type,field)+FIELDSIZE(type,field))
#define UNREF(param) ((void)(param))

#define WORD_ALIGN(_x)  ((((PtrWord)(_x)) + 1) & ((PtrWord)(-2)))
#define DWORD_ALIGN(_x) ((((PtrWord)(_x)) + 3) & ((PtrWord)(-4)))
#define QUAD_ALIGN(_x)  ((((PtrWord)(_x)) + 7) & ((PtrWord)(-8)))

#define _SIGNATURE32(c1,c2,c3,c4) SIGN32(c1,c2,c3,c4)
#define SIGN32(c1,c2,c3,c4) \
  (((((I32u)(c1)) & 0xff) <<  0) | \
   ((((I32u)(c2)) & 0xff) <<  8) | \
   ((((I32u)(c3)) & 0xff) << 16) | \
   ((((I32u)(c4)) & 0xff) << 24))

/* 
 * Calculate the address of the base of the structure given its type, and an
 * address of a field within the structure. 
 */
#define CAST(address,type,field) ((type *)( \
                                  (char*)(address) - \
                                  (char*)(&((type *)0)->field)))

/* debug stuff */
#ifdef ASSERT
#undef ASSERT
#endif

/* Backward compatibility macros */
#define TRACE_Stdout PRINT_Stdout
#define TRACE_Stderr PRINT_Stderr

#if DEBUG

#ifdef _NT_KERNEL
#  define DEBUG_Trace DbgPrint
#  define DEBUG_Assert(_exp,_file,_line) RtlAssert(_exp,_file,_line,NULL)
#else /* _NT_KERNEL */

extern int  DEBUG_Trace P_((Str format, ... ));
extern void DEBUG_Assert P_((const char * msg, const char * file, long l));

#endif /* _NT_KERNEL */

extern void DEBUG_AssertFormat P_((const char * file, long l, Str fmt, ...));

#define STATIC

#define ASSERT(x)                       ((x) ? NOTHING : ASSMSG(#x))
#define VERIFY(x)                       ASSERT(x)
#define DEBUG_ONLY(x)                   x
#define DEBUG_PARAM(x)                  x,
#define DEBUG_UNREF(param)              ((void)(param))
#define RELEASE_UNREF(param)            NOTHING

#define TRACE(s)                        DEBUG_Trace(TEXT("%s"),TEXT(s))
#define TRACE0(s)                       DEBUG_Trace(TEXT("%s"),TEXT(s))
#define TRACE1(s,p1)                    DEBUG_Trace(TEXT(s),p1)
#define TRACE2(s,p1,p2)                 DEBUG_Trace(TEXT(s),p1,p2)
#define TRACE3(s,p1,p2,p3)              DEBUG_Trace(TEXT(s),p1,p2,p3)
#define TRACE4(s,p1,p2,p3,p4)           DEBUG_Trace(TEXT(s),p1,p2,p3,p4)
#define TRACE5(s,p1,p2,p3,p4,p5)        DEBUG_Trace(TEXT(s),p1,p2,p3,p4,p5)
#define TRACE6(s,p1,p2,p3,p4,p5,p6)     DEBUG_Trace(TEXT(s),p1,p2,p3,p4,p5,p6)
#define TRACE7(s,p1,p2,p3,p4,p5,p6,p7)  DEBUG_Trace(TEXT(s),p1,p2,p3,p4,p5,p6,p7)

#define ASSMSG(s)                       DEBUG_Assert(s,__FILE__,__LINE__)
#define ASSMSG1(s,p1)                   DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1)
#define ASSMSG2(s,p1,p2)                DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1,p2)
#define ASSMSG3(s,p1,p2,p3)             DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1,p2,p3)
#define ASSMSG4(s,p1,p2,p3,p4)          DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1,p2,p3,p4)
#define ASSMSG5(s,p1,p2,p3,p4,p5)       DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1,p2,p3,p4,p5)
#define ASSMSG6(s,p1,p2,p3,p4,p5,p6)    DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1,p2,p3,p4,p5,p6)
#define ASSMSG7(s,p1,p2,p3,p4,p5,p6,p7) DEBUG_AssertFormat(__FILE__,__LINE__,TEXT(s),p1,p2,p3,p4,p5,p6,p7)

#else /* DEBUG */

#undef THIS_FILE

#define STATIC          static

#define ASSMSG(s)                       NOTHING
#define ASSERT(x)                       NOTHING
#define VERIFY(x)                       (x)
#define DEBUG_ONLY(x)
#define DEBUG_PARAM(x)
#define DEBUG_UNREF(param)              NOTHING
#define RELEASE_UNREF(param)            ((void)(param))

#define TRACE(s)                        NOTHING
#define TRACE0(s)                       NOTHING
#define TRACE1(s,p1)                    NOTHING
#define TRACE2(s,p1,p2)                 NOTHING
#define TRACE3(s,p1,p2,p3)              NOTHING
#define TRACE4(s,p1,p2,p3,p4)           NOTHING
#define TRACE5(s,p1,p2,p3,p4,p5)        NOTHING
#define TRACE6(s,p1,p2,p3,p4,p5,p6)     NOTHING
#define TRACE7(s,p1,p2,p3,p4,p5,p6,p7)  NOTHING

#define ASSMSG(s)                       NOTHING
#define ASSMSG1(s,p1)                   NOTHING
#define ASSMSG2(s,p1,p2)                NOTHING
#define ASSMSG3(s,p1,p2,p3)             NOTHING
#define ASSMSG4(s,p1,p2,p3,p4)          NOTHING
#define ASSMSG5(s,p1,p2,p3,p4,p5)       NOTHING
#define ASSMSG6(s,p1,p2,p3,p4,p5,p6)    NOTHING
#define ASSMSG7(s,p1,p2,p3,p4,p5,p6,p7) NOTHING

#endif /* !DEBUG */

/* some platform specific ASSERTs */

#ifdef __linux__
#  define LINUX_ASSERT(expr) ASSERT(expr) 
#else
#  define LINUX_ASSERT(expr) NOTHING
#endif /* __linux__ */

#ifdef _UNIX
#  define UNIX_ASSERT(expr) ASSERT(expr) 
#else
#  define UNIX_ASSERT(expr) NOTHING
#endif /* _UNIX */

#ifdef _WIN32
#  define WIN32_ASSERT(expr) ASSERT(expr)
#else
#  define WIN32_ASSERT(expr) NOTHING
#endif /* _WIN32 */

/* 
 * Unicode mapping for prototypes that use Str or Char types. If the UNICODE
 * settings of the application that uses the library don't match the UNICODE
 * settings used by the library when it was compiled, the application won't
 * link. That's better than debug the problem at run time.
 */
#ifdef _UNICODE

/* s_base32.h */
#  define BASE32_Encode             BASE32_EncodeU
#  define BASE32_EncodeStr          BASE32_EncodeStrU
#  define BASE32_Decode             BASE32_DecodeU
#  define BASE32_StrictDecode       BASE32_StrictDecodeU

/* s_base64.h */
#  define BASE64_Encode             BASE64_EncodeU
#  define BASE64_EncodeStr          BASE64_EncodeStrU
#  define BASE64_Decode             BASE64_DecodeU
#  define BASE64_StdDecode          BASE64_StdDecodeU
#  define BASE64_SafeDecode         BASE64_SafeDecodeU

/* s_dom.h */
#  define DOM_FindFirst             DOM_FindFirstU
#  define DOM_FindLast              DOM_FindLastU
#  define DOM_FindNext              DOM_FindNextU
#  define DOM_FindPrev              DOM_FindPrevU
#  define DOM_CharData              DOM_CharDataU
#  define DOM_TagName               DOM_TagNameU
#  define DOM_RootCB                DOM_RootCBU
#  define DOM_TagsCB                DOM_TagsCBU
#  define DOM_Load                  DOM_LoadU
#  define DOM_LoadTags              DOM_LoadTagsU

/* s_file.h */
#  define FILE_Open                 FILE_OpenU
#  define FILE_Reopen               FILE_ReopenU
#  define FILE_SetParam             FILE_SetParamU
#  define FILE_Printf               FILE_PrintfU
#  define FILE_VaPrintf             FILE_VaPrintfU
#  define FILE_Puts                 FILE_PutsU
#  define FILE_Gets                 FILE_GetsU
#  define FILE_Putc                 FILE_PutcU
#  define FILE_Getc                 FILE_GetcU
#  define FILE_Ungetc               FILE_UngetcU
#  define FILE_AttachToFile         FILE_AttachToFileU
#  define FILE_Split                FILE_SplitU
#  define FILE_Split2               FILE_Split2U
#  define FILE_IsFileIO             FILE_IsFileIOU
#  define FILE_ReadLine             FILE_ReadLineU

/* s_hash.h */
#  define hashCompareStringKey      hashCompareStringKeyU
#  define hashCaseCompareStringKey  hashCaseCompareStringKeyU
#  define stringHashProc            stringHashProcU
#  define stringCaseHashProc        stringCaseHashProcU

/* s_hist */
#  define HIST1D_Create             HIST1D_CreateU
#  define HIST1D_Init               HIST1D_InitU

/* s_math.h */
#  define BIGINT_Format             BIGINT_FormatU
#  define BIGINT_Parse              BIGINT_ParseU

/* s_md.h */
#  define DIGEST_Name               DIGEST_NameU

/* s_mem.h */
#  define MEM_DebugStat             MEM_DebugStatU
#  define MEM_DebugDump             MEM_DebugDumpU

/* s_opt.h */
#  define CMDLINE_Create            CMDLINE_CreateU
#  define CMDLINE_SetApp            CMDLINE_SetAppU
#  define CMDLINE_SetOutProc        CMDLINE_SetOutProcU
#  define CMDLINE_SetParamName      CMDLINE_SetParamNameU
#  define CMDLINE_GetApp            CMDLINE_GetAppU
#  define CMDLINE_Msg               CMDLINE_MsgU
#  define CMDLINE_AddIntOpt         CMDLINE_AddIntOptU
#  define CMDLINE_AddI64Opt         CMDLINE_AddI64OptU
#  define CMDLINE_AddDoubleOpt      CMDLINE_AddDoubleOptU
#  define CMDLINE_AddTrueOpt        CMDLINE_AddTrueOptU
#  define CMDLINE_AddFalseOpt       CMDLINE_AddFalseOptU
#  define CMDLINE_AddStrOpt         CMDLINE_AddStrOptU
#  define CMDLINE_AddOpt            CMDLINE_AddOptU
#  define CMDLINE_CreateOptGrp      CMDLINE_CreateOptGrpU
#  define CMDLINE_Parse             CMDLINE_ParseU
#  define CMDLINE_Parse1            CMDLINE_Parse1U
#  define CMDLINE_Usage             CMDLINE_UsageU

/* s_prop.h */
#  define PROP_Load                 PROP_LoadU
#  define PROP_Save                 PROP_SaveU
#  define PROP_LoadXML              PROP_LoadXMLU
#  define PROP_SaveXML              PROP_SaveXMLU
#  define PROP_WriteXML             PROP_WriteXMLU
#  define PROP_RootCB               PROP_RootCBU
#  define PROP_Get                  PROP_GetU
#  define PROP_GetInt               PROP_GetIntU
#  define PROP_GetUInt              PROP_GetUIntU
#  define PROP_GetLong              PROP_GetLongU
#  define PROP_GetULong             PROP_GetULongU
#  define PROP_GetBool              PROP_GetBoolU
#  define PROP_GetIntBool           PROP_GetIntBoolU
#  define PROP_GetDouble            PROP_GetDoubleU
#  define PROP_GetComment           PROP_GetCommentU
#  define PROP_Set                  PROP_SetU
#  define PROP_SetInt               PROP_SetIntU
#  define PROP_SetUInt              PROP_SetUIntU
#  define PROP_SetLong              PROP_SetLongU
#  define PROP_SetULong             PROP_SetULongU
#  define PROP_SetBool              PROP_SetBoolU
#  define PROP_SetDouble            PROP_SetDoubleU
#  define PROP_SetComment           PROP_SetCommentU
#  define PROP_Comment              PROP_CommentU
#  define PROP_GetLong64            PROP_GetLong64U
#  define PROP_SetLong64            PROP_SetLong64U
#  define PROP_Remove               PROP_RemoveU
#  define PROP_Examine              PROP_ExamineU
#  define PROP_ExamineAll           PROP_ExamineAllU
#  define PROP_Clear                PROP_ClearU
#  define PROP_Copy                 PROP_CopyU
#  define PROP_CopyAll              PROP_CopyAllU
#  define PROP_Merge                PROP_MergeU
#  define PROP_Extract              PROP_ExtractU
#  define PROP_Keys                 PROP_KeysU
#  define PROP_Values               PROP_ValuesU
#  define PROP_Data                 PROP_DataU

/* s_strbuf.h */
#  define STRBUF_Create             STRBUF_CreateU
#  define STRBUF_Delete             STRBUF_DeleteU
#  define STRBUF_Init               STRBUF_InitU
#  define STRBUF_InitBuf            STRBUF_InitBufU
#  define STRBUF_InitBuf2           STRBUF_InitBuf2U
#  define STRBUF_Destroy            STRBUF_DestroyU
#  define STRBUF_Length             STRBUF_LengthU
#  define STRBUF_SetLength          STRBUF_SetLengthU
#  define STRBUF_Equals             STRBUF_EqualsU
#  define STRBUF_EqualsTo           STRBUF_EqualsToU
#  define STRBUF_CharAt             STRBUF_CharAtU
#  define STRBUF_FirstChar          STRBUF_FirstCharU
#  define STRBUF_LastChar           STRBUF_LastCharU
#  define STRBUF_IndexOf            STRBUF_IndexOfU
#  define STRBUF_LastIndexOf        STRBUF_LastIndexOfU
#  define STRBUF_Find               STRBUF_FindU
#  define STRBUF_FindNoCase         STRBUF_FindNoCaseU
#  define STRBUF_FindLast           STRBUF_FindLastU
#  define STRBUF_FindLastNoCase     STRBUF_FindLastNoCaseU
#  define STRBUF_FindFrom           STRBUF_FindFromU
#  define STRBUF_FindFromNoCase     STRBUF_FindFromNoCaseU
#  define STRBUF_Replace            STRBUF_ReplaceU
#  define STRBUF_ReplaceStr         STRBUF_ReplaceStrU
#  define STRBUF_GetString          STRBUF_GetStringU
#  define STRBUF_StartsWith         STRBUF_StartsWithU
#  define STRBUF_EndsWith           STRBUF_EndsWithU
#  define STRBUF_EndsWithNoCase     STRBUF_EndsWithNoCaseU
#  define STRBUF_StartsWithNoCase   STRBUF_StartsWithNoCaseU
#  define STRBUF_ToUpperCase        STRBUF_ToUpperCaseU
#  define STRBUF_ToLowerCase        STRBUF_ToLowerCaseU
#  define STRBUF_Alloc              STRBUF_AllocU
#  define STRBUF_Clear              STRBUF_ClearU
#  define STRBUF_Erase              STRBUF_EraseU
#  define STRBUF_Trim               STRBUF_TrimU
#  define STRBUF_Copy               STRBUF_CopyU
#  define STRBUF_CopyN              STRBUF_CopyNU
#  define STRBUF_Append             STRBUF_AppendU
#  define STRBUF_AppendN            STRBUF_AppendNU
#  define STRBUF_AppendInt          STRBUF_AppendIntU
#  define STRBUF_AppendChar         STRBUF_AppendCharU
#  define STRBUF_AppendBool         STRBUF_AppendBoolU
#  define STRBUF_AppendDouble       STRBUF_AppendDoubleU
#  define STRBUF_AppendFormat       STRBUF_AppendFormatU
#  define STRBUF_AppendFormatVa     STRBUF_AppendFormatVaU
#  define STRBUF_Inflate            STRBUF_InflateU
#  define STRBUF_Insert             STRBUF_InsertU
#  define STRBUF_InsertN            STRBUF_InsertNU
#  define STRBUF_InsertChar         STRBUF_InsertCharU
#  define STRBUF_Format             STRBUF_FormatU
#  define STRBUF_FormatVa           STRBUF_FormatVaU
#  define STRBUF_AppendTime         STRBUF_AppendTimeU
#  define STRBUF_FormatTime         STRBUF_FormatTimeU

/* s_util.h */
#  define PRINT_Error               PRINT_ErrorU
#  define PRINT_ErrorVa             PRINT_ErrorVaU
#  define PRINT_Warning             PRINT_WarningU
#  define PRINT_WarningVa           PRINT_WarningVaU
#  define PRINT_Output              PRINT_OutputU
#  define PRINT_OutputVa            PRINT_OutputVaU
#  define PRINT_Verbose             PRINT_VerboseU
#  define PRINT_VerboseVa           PRINT_VerboseVaU
#  define PRINT_Stdout              PRINT_StdoutU
#  define PRINT_Stderr              PRINT_StderrU
#  define yyerror                   yyerrorU
#  define TIME_ToString             TIME_ToStringU
#  define FILE_DirName              FILE_DirNameU
#  define FILE_TempName             FILE_TempNameU
#  define FILE_IsFileSeparator      FILE_IsFileSeparatorU
#  define FILE_Save                 FILE_SaveU
#  define FILE_FilePart             FILE_FilePartU
#  define FILE_CanOpen              FILE_CanOpenU
#  define FILE_Exist                FILE_ExistU
#  define FILE_NonExist             FILE_NonExistU
#  define FILE_IsFile               FILE_IsFileU
#  define FILE_IsDir                FILE_IsDirU
#  define FILE_MakeUnique           FILE_MakeUniqueU
#  define FILE_MkDir                FILE_MkDirU
#  define FILE_RmDir                FILE_RmDirU
#  define FILE_List                 FILE_ListU
#  define URL_Encode                URL_EncodeU
#  define URL_Decode                URL_DecodeU
#  define STRING_Format             STRING_FormatU
#  define STRING_StartsWith         STRING_StartsWithU
#  define STRING_EndsWith           STRING_EndsWithU
#  define STRING_IndexOf            STRING_IndexOfU
#  define STRING_LastIndexOf        STRING_LastIndexOfU
#  define STRING_HashCode           STRING_HashCodeU
#  define STRING_HashCodeNoCase     STRING_HashCodeNoCaseU
#  define STRING_FormatDouble       STRING_FormatDoubleU
#  define PARSE_Bool                PARSE_BoolU
#  define PARSE_Byte                PARSE_ByteU
#  define PARSE_UByte               PARSE_UByteU
#  define PARSE_Short               PARSE_ShortU
#  define PARSE_UShort              PARSE_UShortU
#  define PARSE_Int                 PARSE_IntU
#  define PARSE_UInt                PARSE_UIntU
#  define PARSE_Long                PARSE_LongU
#  define PARSE_ULong               PARSE_ULongU
#  define PARSE_Double              PARSE_DoubleU
#  define PARSE_Long64              PARSE_Long64U
#  define PARSE_ULong64             PARSE_ULong64U

/* s_vector.h */
#  define vectorCompareString       vectorCompareStringU
#  define vectorCompareStringNoCase vectorCompareStringNoCaseU
#  define vectorEqualsString        vectorEqualsStringU
#  define vectorEqualsStringNoCase  vectorEqualsStringNoCaseU

/* s_xml.h */
#  define xmlEmptyAttr              xmlEmptyAttrU
#  define XML_AttrCount             XML_AttrCountU
#  define XML_AttrValue             XML_AttrValueU
#  define XML_AttrValueAt           XML_AttrValueAtU
#  define XML_AttrNameAt            XML_AttrNameAtU
#  define XML_ParseStream           XML_ParseStreamU
#  define XML_ParseFile             XML_ParseFileU
#  define XML_Handle                XML_HandleU
#  define XML_Escape                XML_EscapeU
#  define XML_OpenTag               XML_OpenTagU
#  define XML_CloseTag              XML_CloseTagU
#  define XML_StartTag              XML_StartTagU
#  define XML_EndTag                XML_EndTagU
#  define XML_WriteAttr             XML_WriteAttrU
#  define XML_WriteDoubleAttr       XML_WriteDoubleAttrU
#  define XML_WriteIntAttr          XML_WriteIntAttrU
#  define XML_WriteI64Attr          XML_WriteI64AttrU
#  define XML_WriteU64Attr          XML_WriteU64AttrU
#  define XML_WriteUIntAttr         XML_WriteUIntAttrU
#  define XML_WriteHexAttr          XML_WriteHexAttrU
#  define XML_MatchAttr             XML_MatchAttrU

#endif /* _UNICODE */

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_DEF_H_ */
