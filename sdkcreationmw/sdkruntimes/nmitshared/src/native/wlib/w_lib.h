/*
 * $Id: w_lib.h,v 1.25 2005/04/21 23:48:22 slava Exp $
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

#ifndef __WLIB_H__
#define __WLIB_H__ 1

/*==========================================================================*
 * Make sure both UNICODE and _UNICODE are defined, or none
 *==========================================================================*/
#if defined(UNICODE) || defined(_UNICODE)
#undef   UNICODE
#undef  _UNICODE
#define  UNICODE 1
#define _UNICODE 1
#endif

/*==========================================================================*
 *      I N C L U D E S
 *==========================================================================*/

/*
 * This is useful if <s_lib.h> cannot be included just like that,
 * but has already been included in some other way, for example as 
 * <slib/include/s_lib.h>
 */
#ifndef _SLAVA_LIB_H_
#  include "../slib/include/s_lib.h"
#endif /* _SLAVA_LIB_H_ */

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*
 *      M A C R O S
 *==========================================================================*/

/* predefined registry keys */
#define HKLM HKEY_LOCAL_MACHINE
#define HKCU HKEY_CURRENT_USER
#define HKCR HKEY_CLASSES_ROOT

/* exception handling */
#define TRY_FINALLY     __try
#define TRY_EXCEPT      __try
#define TRY_CATCH       __try
#define EXCEPT(x)       __except(x)
#define FINALLY         __finally

#define CATCH           EXCEPT(EXCEPTION_EXECUTE_HANDLER)

/* miscellaneous */
#define DRIVE_INDEX(c)  ((((c)>=_T('a'))&&((c)<=_T('z')))?((c)-_T('a')):((c)-_T('A')))
#define RECT_Width(r) (((r)&&((r)->right>(r)->left))?((r)->right-(r)->left):0)
#define RECT_Height(r) (((r)&&((r)->bottom>(r)->top))?((r)->bottom-(r)->top):0)

/*==========================================================================*
 *      T Y P E S
 *==========================================================================*/

typedef enum _REG_ACTION {
    REG_CREATE_KEY,             /* create key                       */
    REG_CREATE_SUBKEY,          /* create sub key                   */
    REG_OPEN_KEY,               /* open key (don't close)           */
    REG_OPEN_SUBKEY,            /* open a subkey                    */
    REG_OPEN_CREATE_KEY,        /* create and keep open             */
    REG_OPEN_CREATE_SUBKEY,     /* create and keep open a subkey    */
    REG_DELETE_KEY,             /* delete key                       */
    REG_DELETE_SUBKEY,          /* delete a subkey                  */
    REG_SET_VALUE,              /* set value                        */
    REG_DELETE_VALUE,           /* delete values                    */
    REG_DELETE_ALL_VALUES       /* delete all values                */
} REG_ACTION;

typedef union _REG_DATA {
    struct _REG_BINARY {
        PCVOID data;            /* REG_BINARY                       */
        ULONG size;             /* size in bytes                    */
    } binary;
    struct _REG_MULTI_SZ {
        Str chars;              /* REG_MULTI_SZ                     */
        ULONG count;            /* size in characters               */
    } msz;
    Str string;                 /* REG_[EXPAND]_SZ                  */
    ULONG   dword;              /* REG_DWORD                        */
} REG_DATA;

typedef struct _REG_TRANSACTION {
    REG_ACTION action;          /* action to perform                */
    HKEY       key;             /* predefined registry key          */
    Str        name;            /* key/value name                   */
    ULONG      type;            /* value type                       */
    REG_DATA   data;            /* data                             */
} REG_TRANSACTION;

typedef enum  {    
    ATTACH_NONE = 0,            /* marks the last goemetry rule     */
    ATTACH_FORM,                /* to the border of the container   */
    ATTACH_OPPOSITE_FORM,       /* to the opposite border           */
    ATTACH_WIDGET,              /* to a sibling component           */
    ATTACH_OPPOSITE_WIDGET,     /* to the opposite side             */
    ATTACH_POSITION,            /* position within parent container */
    ATTACH_SELF,                /* to self                          */
    ATTACH_ANY,                 /* does not matter                  */
} ATTACH_METHOD;

typedef enum {
    ATTACH_LEFT,
    ATTACH_RIGHT,
    ATTACH_TOP,
    ATTACH_BOTTOM,
} ATTACH_TYPE;

typedef struct _GEOMETRY_RULE {
    int  nID;                   /* windows id (dialog item id)      */
    ATTACH_TYPE nType;          /* which side to attach             */
    int  nOtherID;              /* window we attach this one to     */
    ATTACH_METHOD nMethod;      /* how to attach                    */
    int  nPos;                  /* position (see ATTACH_POSITION)   */
    int  nOffset;               /* offset (dialog units or pixels)  */
} GEOMETRY_RULE;

#define BEGIN_GEOM_DESCRIPTION(name) static const GEOMETRY_RULE name[] = {
#define END_GEOM_DESCRIPTION()    {0, (ATTACH_TYPE) 0, 0, ATTACH_NONE, 0, 0}};

/*--------------------------------------------------------------------------*
 *      Macros to define registry transactions
 *--------------------------------------------------------------------------*/
#define _CREATE_KEY(key,name)     { REG_CREATE_KEY,key,name,0,0 }
#define _CREATE_SUBKEY(name)      { REG_CREATE_SUBKEY, NULL,name,0,0}
#define _OPEN_KEY(key,name)       { REG_OPEN_KEY, key, name, 0, 0}
#define _OPEN_SUBKEY(name)        { REG_OPEN_SUBKEY, NULL,name,0,0 }
#define _OPEN_CREATE_KEY(key,name){ REG_OPEN_CREATE_KEY,key,name,0,0 }
#define _OPEN_CREATE_SUBKEY(name) { REG_OPEN_CREATE_SUBKEY,NULL,name,0,0}
#define _DELETE_KEY(key,name)     { REG_DELETE_KEY, key, name,0,0}
#define _DELETE_SUBKEY(name)      { REG_DELETE_SUBKEY,NULL,name,0,0}
#define _SET_STRING(name,str)     { REG_SET_VALUE,NULL,name,REG_SZ,str}
#define _SET_DWORD(name,n)        { REG_SET_VALUE,NULL,name,REG_DWORD,(PVOID)n}
#define _DELETE_VALUE(name)       { REG_DELETE_VALUE,NULL,name,0,0}
#define _DELETE_ALL_VALUES()      { REG_DELETE_ALL_VALUES,NULL,NULL,0,0}

/*==========================================================================*
 *      F U N C T I O N S 
 *==========================================================================*/

/*--------------------------------------------------------------------------*
 *      Generic
 *--------------------------------------------------------------------------*/

ULONG WIN32_GetPlatform(void);
Bool  WIN32_IsWinNT(void);
Bool  WIN32_IsWin9x(void);
Bool  WIN32_IsWin95(void);
Bool  WIN32_IsWin98(void);
Bool  WIN32_IsWinCE(void);

Bool  WIN32_IsAdmin(void);
Bool  WIN32_LogonId(LUID * logonId);
Bool  WIN32_BrowseURL(Str url);

/*--------------------------------------------------------------------------*
 *      Resources
 *--------------------------------------------------------------------------*/

Str WIN32_LoadString (HMODULE hModule, UINT uID, StrBuf* sb);
Str WIN32_FormatMessage (HMODULE hModule, UINT uFormatID, StrBuf* sb, ...);
Str WIN32_FormatMessageVa (HMODULE hModule, UINT uID, StrBuf* sb, va_list va);

/*--------------------------------------------------------------------------*
 *      Files
 *--------------------------------------------------------------------------*/

Str WIN32_GetModuleFileName(HMODULE hModule, StrBuf * sb);

/*--------------------------------------------------------------------------*
 *      GUI
 *--------------------------------------------------------------------------*/

HBRUSH WIN32_CreateHalftoneBrush (void);
Str  WIN32_GetWindowText (HWND hwnd, StrBuf * sb);
void WIN32_Layout (HWND hwnd, const GEOMETRY_RULE * rules);
Bool WIN32_CenterWindow (HWND hwnd, HWND hwndCenter);
void WIN32_DrawTransparentBitmap (HDC dc, HBITMAP h, int x, int y, COLORREF c);

/*--------------------------------------------------------------------------*
 *      Multi-string
 *--------------------------------------------------------------------------*/

typedef struct _RegMsz RegMsz;
RegMsz * REGMSZ_Create (void);
void REGMSZ_Delete(RegMsz * msz);
void REGMSZ_Clear (RegMsz * msz);
Bool REGMSZ_Add (RegMsz * msz, Str s);
int  REGMSZ_Count (const RegMsz * msz);
Str  REGMSZ_Get (const RegMsz * msz, int i);
Str  REGMSZ_First (const RegMsz * msz);
Str  REGMSZ_Next (const RegMsz * msz, Str s);

/*--------------------------------------------------------------------------*
 *      Registry
 *--------------------------------------------------------------------------*/

Bool REG_KeyExists (HKEY hKey, Str szSubKey);
Bool REG_ValueExists (HKEY hKey, Str szName, ULONG expectType);
Bool REG_CreateKey (HKEY hKey, Str szSubKey);
HKEY REG_OpenKey(HKEY hKey, Str szSubKey, REGSAM sam);
HKEY REG_OpenCreateKey (HKEY hKey, Str szSubKey);
Bool REG_CopyValue (HKEY hDest, HKEY hSrc, Str szName);
Bool REG_CopyKey (HKEY hDest, HKEY hSrc, Bool recurse);
Bool REG_CopySubKey (HKEY hDest, HKEY hSrc, Str szSubKey, Bool recurse);
Str  REG_GetNextKey (HKEY hKey, int n, StrBuf * sb);
Str  REG_GetNextValue (HKEY hKey, int n, StrBuf * sb);

Bool REG_DeleteKey (HKEY hKey, Str szSubKey);
Bool REG_DeleteValue (HKEY hKey, Str szSubKey);
Bool REG_DeleteAllSubKeys (HKEY hKey);
Bool REG_DeleteAllValues (HKEY hKey);
Bool REG_CloseKey (HKEY hKey);

Bool REG_Query (HKEY hKey, Str name, ULONG * type, Buffer * data); 
Bool REG_QueryBin (HKEY hKey, Str name, Buffer * data); 
Bool REG_QueryDword (HKEY hKey, Str name, DWORD * value); 
Bool REG_QueryBool (HKEY hKey, Str name, Bool * value); 
Bool REG_QueryChar (HKEY hKey, Str name, char * value); 
Str  REG_QueryStr (HKEY hKey, Str name, StrBuf * value); 
RegMsz * REG_QueryMultiSz (HKEY hKey, Str name); 

Bool REG_SetValue (HKEY hKey, Str name, ULONG type, PCVOID data);
Bool REG_SetBinary (HKEY hKey, Str name, PCVOID data, ULONG size);
Bool REG_SetDword (HKEY hKey, Str name, DWORD value); 
Bool REG_SetBool (HKEY hKey, Str name, Bool value); 
Bool REG_SetChar (HKEY hKey, Str name, char value); 
Bool REG_SetString (HKEY hKey, Str name, Str value); 
Bool REG_SetStrBuf (HKEY hKey, Str name, const StrBuf * value); 
Bool REG_SetMsz (HKEY hKey, Str name, const RegMsz * value);
Bool REG_SetMultiSz (HKEY hKey, Str name, Str value);

HKEY REG_Transaction (HKEY hKey, const REG_TRANSACTION * trans);
void REG_UpdateRegistry (const REG_TRANSACTION trans[], int nTrans);

/*--------------------------------------------------------------------------*
 *      Service Control Manager (NT specific)
 *
 * Note that all functions take NULL as a handle to the service control
 * manager, in which case they open a temporary connection to the SCM
 * and close it. The caller only needs to maintain its own connection if 
 * it does many service control operations that may reuse the same handle.
 * Maintaining single connection across multiple service control calls may
 * improve the performance.
 *--------------------------------------------------------------------------*/

typedef struct _NT_SERVICE {
    Str lpServiceName;          /* name of service                  */
    Str lpDisplayName;          /* name to display                  */
    Str lpExeName;              /* exe file name                    */
    Str lpDependencies;         /* dependencies list                */
    ULONG   dwServiceType;      /* service type                     */
    ULONG   dwStartType;        /* start type                       */
} NT_SERVICE;

#ifndef _WIN32_WCE

Bool  SERVICE_Install (SC_HANDLE scm, const NT_SERVICE * svc);
Bool  SERVICE_Delete (SC_HANDLE scm, Str szSvcName);
Bool  SERVICE_Create (SC_HANDLE scm, const NT_SERVICE * svc);
Bool  SERVICE_Configure (SC_HANDLE scm, const NT_SERVICE * svc);
Bool  SERVICE_Start (SC_HANDLE scm, Str szSvcName);
Bool  SERVICE_Stop (SC_HANDLE scm, Str szSvcName);
Bool  SERVICE_Control (SC_HANDLE scm, Str szSvcName, ULONG nCode);
ULONG SERVICE_State(SC_HANDLE hScMgr, Str szSvcName);

#endif /* !_WIN32_WCE */

/* 
 * Unicode mapping for prototypes that use Str or Char types. If the UNICODE
 * settings of the application that uses the library don't match the UNICODE
 * settings used by the library when it was compiled, the application won't
 * link. That's better than debug the problem at run time.
 */
#ifdef _UNICODE

#define WIN32_LoadString        WIN32_LoadStringU
#define WIN32_FormatMessage     WIN32_FormatMessageU
#define WIN32_FormatMessageVa   WIN32_FormatMessageVaU
#define WIN32_GetModuleFileName WIN32_GetModuleFileNameU
#define WIN32_GetWindowText     WIN32_GetWindowTextU
#define WIN32_BrowseURL         WIN32_BrowseURLU
#define REGMSZ_Add              REGMSZ_AddU
#define REGMSZ_Get              REGMSZ_GetU
#define REGMSZ_First            REGMSZ_FirstU
#define REGMSZ_Next             REGMSZ_NextU
#define REG_KeyExists           REG_KeyExistsU
#define REG_ValueExists         REG_ValueExistsU
#define REG_CreateKey           REG_CreateKeyU
#define REG_OpenKey             REG_OpenKeyU
#define REG_OpenCreateKey       REG_OpenCreateKeyU
#define REG_CopyValue           REG_CopyValueU
#define REG_CopySubKey          REG_CopySubKeyU
#define REG_GetNextKey          REG_GetNextKeyU
#define REG_GetNextValue        REG_GetNextValueU
#define REG_DeleteKey           REG_DeleteKeyU
#define REG_DeleteValue         REG_DeleteValueU
#define REG_Query               REG_QueryU
#define REG_QueryBin            REG_QueryBinU
#define REG_QueryDword          REG_QueryDwordU
#define REG_QueryBool           REG_QueryBoolU
#define REG_QueryChar           REG_QueryCharU
#define REG_QueryStr            REG_QueryStrU
#define REG_QueryMultiSz        REG_QueryMultiSzU
#define REG_SetValue            REG_SetValueU
#define REG_SetBinary           REG_SetBinaryU
#define REG_SetDword            REG_SetDwordU
#define REG_SetBool             REG_SetBoolU
#define REG_SetChar             REG_SetCharU
#define REG_SetString           REG_SetStringU
#define REG_SetStrBuf           REG_SetStrBufU
#define REG_SetMsz              REG_SetMszU
#define REG_SetMultiSz          REG_SetMultiSzU
#define REG_Transaction         REG_TransactionU
#define REG_UpdateRegistry      REG_UpdateRegistryU
#define SERVICE_Install         SERVICE_InstallU
#define SERVICE_Delete          SERVICE_DeleteU
#define SERVICE_Create          SERVICE_CreateU
#define SERVICE_Configure       SERVICE_ConfigureU
#define SERVICE_Start           SERVICE_StartU
#define SERVICE_Stop            SERVICE_StopU
#define SERVICE_Control         SERVICE_ControlU
#define SERVICE_State           SERVICE_StateU

#endif /* _UNICODE */

#ifdef __cplusplus
}
#endif

#endif // __WLIB_H__
