/*
 * $Id: s_ntk.h,v 1.28 2005/02/25 02:53:39 slava Exp $
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

#ifndef _SLAVA_NTK_H_
#define _SLAVA_NTK_H_ 1

#if !defined(_WIN32) || !defined(_NT_KERNEL) || !defined(_SLAVA_OS_H_)
#  error "Do not include s_ntk.h directly. Include s_os.h"
#endif

#define __KERNEL__ 1
#define _TLS_SUPPORT 0 /* No TLS support in kernel mode */

#ifdef DBG
#  undef DEBUG
#  define DEBUG DBG
#  if DEBUG
#    ifndef _DEBUG
#      define _DEBUG
#    endif 
#    undef NDEBUG
#  else
#    ifndef NDEBUG
#      define NDEBUG
#    endif 
#    undef _DEBUG
#  endif 
#else
#   define DBG DEBUG
#endif /* DBG */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

#ifdef _USE_WDM_H_
#  include <wdm.h>
#elif defined(_USE_NTDDK_H_)
#  include <ntddk.h>
#else  /* _USE_NTDDK_H_ */
#  include <ntifs.h>
#  define _NTDDK_ /* prevent ntddk.h from being included */
#endif /* _USE_NTDDK_H_ */

#  ifndef OBJ_KERNEL_HANDLE
     /* this flag did't exist until Win2K */
#    define OBJ_KERNEL_HANDLE 0x00000200L
#  endif /* OBJ_KERNEL_HANDLE */

/* character type */
typedef char Char;

/* string manipulation functions */
#define StrLen      strlen
#define StrCmp      strcmp
#define StrCpy      strcpy
#define StrnCmp     strncmp
#define StrCat      strcat
#define StrChr      strchr
#define StrStr      strstr
#define StrrChr     strrchr
#define StrnCpy     strncpy
#define StrTod      strtod
#define StrTol      strtol
#define StrToul     strtoul
#define Sprintf     sprintf
#define Vsnprintf   _vsnprintf
#define StrCaseCmp  stricmp
#define StrnCaseCmp strnicmp

/* character classification functions */
#define IsUpper     isupper
#define IsLower     islower
#define IsPrint     isprint
#define IsDigit     isdigit
#define IsXdigit    isxdigit
#define IsSpace     isspace
#define ToUpper     toupper
#define ToLower     tolower

#undef isupper
#undef islower
#undef isdigit
#undef isxdigit
#undef isspace
#undef isprint

int __cdecl isupper(int);
int __cdecl islower(int);
int __cdecl isdigit(int);
int __cdecl isxdigit(int);
int __cdecl isspace(int);
int __cdecl isprint(int);

#define TEXT_(x) TEXT(x)
#define IsAlpha(_c) (\
   ((_c) >= 'a' && (_c) <= 'z') || \
   ((_c) >= 'A' && (_c) <= 'Z'))

/* define these types the way they are defined on Unix platforms */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

/* and their windows equivalents... */
typedef int8_t   INT8,   * PINT8;
typedef uint8_t  UINT8,  * PUINT8;
typedef int16_t  INT16,  * PINT16;
typedef uint16_t UINT16, * PUINT16;
typedef int32_t  INT32,  * PINT32;
typedef uint32_t UINT32, * PUINT32;
typedef int64_t  INT64,  * PINT64;
typedef uint64_t UINT64, * PUINT64;

typedef unsigned long  DWORD, * PDWORD;
typedef unsigned char  BYTE,  * PBYTE;
typedef unsigned short WORD,  * PWORD;

#define INT8_MIN   _I8_MIN
#define INT8_MAX   _I8_MAX
#define UINT8_MAX  _UI8_MAX

#define INT16_MIN   _I16_MIN
#define INT16_MAX   _I16_MAX
#define UINT16_MAX  _UI16_MAX

#define INT32_MIN   _I32_MIN
#define INT32_MAX   _I32_MAX
#define UINT32_MAX  _UI32_MAX

#define __INT64_C(c)  c ## i64 
#define __UINT64_C(c) c ## ui64
#define INT64_MIN  _I64_MIN
#define INT64_MAX  _I64_MAX
#define UINT64_MAX _UI64_MAX

#define LONG_HEX_FORMAT "%I64X"
#define LONG_LONG_FORMAT "%I64d"
#define LONG_ULONG_FORMAT "%I64u"

#define I64X_FORMAT LONG_HEX_FORMAT
#define I64S_FORMAT LONG_LONG_FORMAT
#define I64U_FORMAT LONG_ULONG_FORMAT

#define snprintf          _snprintf
#define vsnprintf         _vsnprintf
#define strcasecmp        _stricmp
#define strncasecmp       _strnicmp

/* 
 * no sockets in kernel mode, define it only to avoid too many conditional 
 * statements in the other include files 
 */
typedef int Socket;

/* 
 * Definitions for byte order, according to significance of bytes, from low
 * addresses to high addresses.  The value is what you get by putting '4'
 * in the most significant byte, '3' in the second most significant byte,
 * '2' in the second least significant byte, and '1' in the least
 * significant byte.  
 */

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412

#define LITTLE_ENDIAN   __LITTLE_ENDIAN
#define BIG_ENDIAN      __BIG_ENDIAN
#define PDP_ENDIAN      __PDP_ENDIAN
#define BYTE_ORDER      __BYTE_ORDER

/* if byte order is not defined, default to little endian, like x86 */
#if defined(_X86_) || !defined (__BYTE_ORDER)
#  define __BYTE_ORDER __LITTLE_ENDIAN
#endif 

/* the word that has same size as a pointer. */
typedef long PtrWord;

/* (non-paged) memory allocation primitives provided by the system */
#define OS_MemAlloc(_size) ExAllocatePoolWithTag(NonPagedPool, _size, 'bilS')
#define OS_MemFree(_ptr) ((_ptr) ? ExFreePool(_ptr) : ((void)0))

#define NO_REALLOC

/*==========================================================================*
 *              A D D I T I O N A L     A P I
 *==========================================================================*/

/* NtQuerySystemInformation */
typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    /* there are more, of course ... */
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    ULONG NumberOfPhysicalPages;
    ULONG LowestPhysicalPageNumber;
    ULONG HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG MinimumUserModeAddress;
    ULONG MaximumUserModeAddress;
    KAFFINITY ActiveProcessorsAffinityMask;
    CCHAR NumberOfProcessors;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
NtQuerySystemInformation (
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

/* some things are defined only in ntifs.h */
#if defined(_USE_NTDDK_H_) || defined(_USE_WDM_H_)
extern POBJECT_TYPE *PsThreadType;
NTKERNELAPI
NTSTATUS
ObQueryNameString (
    IN PVOID Object,
    OUT POBJECT_NAME_INFORMATION ObjectNameInfo,
    IN ULONG Length,
    OUT PULONG ReturnLength
    );
NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForSingleObject(
    IN HANDLE Handle,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );
#endif /* _USE_NTDDK_H_ */

/*==========================================================================*
 *              U T I L I T I E S
 *==========================================================================*/

/* Unicode strings */
PUNICODE_STRING 
NT_AllocUnicodeString( 
    IN USHORT MaxLength 
    );

PUNICODE_STRING 
NT_DupUnicodeString( 
    IN PUNICODE_STRING String, 
    IN USHORT MaxLength
    );

PUNICODE_STRING 
NT_ConcatUnicodeStrings( 
    IN PWSTR s1,
    IN PWSTR s2,
    IN USHORT MaxLength 
    );

PUNICODE_STRING 
NT_AllocDeviceName( 
    IN PWSTR Name 
    ); 

PUNICODE_STRING 
NT_AllocSymLinkName( 
    IN PWSTR Name 
    ); 

VOID
NT_FreeUnicodeString( 
    IN PUNICODE_STRING s
    );

/* Object name */
OBJECT_NAME_INFORMATION * 
NT_QueryObjectName(
    PVOID obj
    );

/* Device object manipulations */
BOOLEAN 
NT_CreateDeviceObject( 
    IN  PDRIVER_OBJECT   DriverObject, 
    IN  PWSTR            Name, 
    IN  ULONG            DeviceExtensionSize,
    IN  BOOLEAN          Exclusive,
    IN  DEVICE_TYPE      DeviceType,
    OUT PDEVICE_OBJECT * DeviceObject,
    OUT NTSTATUS       * Status
    );

VOID 
NT_DeleteDeviceObject( 
    IN PDEVICE_OBJECT DeviceObject
    );

#endif /* _SLAVA_NTK_H_ */
