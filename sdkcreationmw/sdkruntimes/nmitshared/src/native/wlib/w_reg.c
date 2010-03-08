/*
 * $Id: w_reg.c,v 1.26 2005/03/15 08:01:35 slava Exp $
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

#include "w_lib.h"

static TCHAR BACKSLASH = _T('\\');
#define PRINTABLE(_s) (((_s) && (_s)[0]) ? (_s) : TEXT("<default>"))

/**
 * Checks if the specified registry key exists. 
 */
Bool REG_KeyExists(HKEY hKey, LPCTSTR szSubKey)
{
    HKEY hSubKey = REG_OpenKey( hKey, szSubKey, KEY_READ );
    if (hSubKey) {
        REG_CloseKey(hSubKey);
        return True;
    }
    return False;
}

/**
 * Checks if the specified value of given type exists. If expectType is 
 * REG_NONE, then this function does not check the value type. 
 */
Bool REG_ValueExists(HKEY hKey, LPCTSTR szName, ULONG expectType)
{
    ULONG type = REG_NONE;
    ULONG size = 0;
    ULONG status = RegQueryValueEx(hKey, szName, NULL, &type, NULL, &size);
    if (status == NO_ERROR) {
        if (expectType == REG_NONE) {
            return True; 
        } else if (expectType == REG_SZ) {
            return (type == REG_SZ || type == REG_EXPAND_SZ);
        } else {
            return (expectType == type);
        }
    }
    return False;
}

/**
 * Opens existing registry key. Returns the handle to the open registry key
 * or NULL if the key does not exist
 */
HKEY REG_OpenKey(HKEY hKey, LPCTSTR szSubKey, REGSAM sam)
{
    ULONG status;
    HKEY hSubKey = NULL;

    //TRACE2("Opening registry key %08lX\\%s\n",hKey,szSubKey);
    status = RegOpenKeyEx(hKey,szSubKey, 0, sam, &hSubKey);
    if (status != NO_ERROR) {
        hSubKey = NULL;
        TRACE3("RegOpenKeyEx(%08lX,%s) failed, error %lu\n",
            hKey,szSubKey,status);
    }

    SetLastError(status);
    return hSubKey;
}

/**
 * Opens a registry key. If key does not exist, creates it.
 */
Bool REG_CreateKey(HKEY hKey, LPCTSTR szSubKey)
{
    HKEY hSubKey = REG_OpenCreateKey(hKey, szSubKey);
    if (hSubKey) {
        REG_CloseKey(hSubKey);
        return True;
    }
    return False;
}

/**
 * Opens a registry key. If key does not exist, creates it. 
 * Returns the handle to the open (or created) registry key
 */
HKEY REG_OpenCreateKey(HKEY hKey, LPCTSTR szSubKey)
{
    ULONG status = NO_ERROR;
    HKEY hParent = hKey;

    /* quick test if the key already exists */
    HKEY hSubKey = REG_OpenKey(hKey, szSubKey, KEY_ALL_ACCESS);
    if (!hSubKey) {
        StrBuf64 buf;
        STRBUF_InitBufXXX(&buf);

        /* the key does not exist, need to create it */
        TRACE2("Creating registry key %08lX\\%s\n",hKey,szSubKey);

        if (STRBUF_Append(&buf.sb, szSubKey)) {    
            LPTSTR name = buf.sb.s;
            while (name) {

                /* find next backslash and replace it with NULL */
                LPTSTR bs = _tcschr(name, BACKSLASH);
                if (bs) *bs = 0;

                /* make sure this subkey does exist */
                status = RegOpenKeyEx(hParent,name,0,KEY_ALL_ACCESS,&hSubKey);
                if (status == NO_ERROR) {
                    TRACE1("  %s exists\n",name);                
                } else {
                    /* if it does not exist, then create it */
                    ULONG disposition;
                    status = RegCreateKeyEx( hParent, name,0, NULL, 
                        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
                        &hSubKey, &disposition);

                    /* if create failed, abort the mission */
                    if (status != NO_ERROR) {
                        TRACE2("  Cannot create %s, error %d\n",name,status);
                        hSubKey = NULL;
                        break;
                    }                
                    TRACE1("  %s created\n",name);
                }

                /* switch to the next portion of the registry path */
                if (hParent != hKey) REG_CloseKey(hParent);
                hParent = hSubKey;
                name = bs;

                /* skip multiple backslashes */
                if (name) while (*(++name) == BACKSLASH) name++;
            }
        } else {
            status = ERROR_NOT_ENOUGH_MEMORY;
        }
        STRBUF_Destroy(&buf.sb);
    }

    if (hParent && hParent != hKey && hParent != hSubKey) {
        REG_CloseKey(hParent);
    }

    SetLastError(status);
    return hSubKey;
}

/**
 * Copies a single registry value from one key to another
 */
Bool REG_CopyValue(HKEY hKeyDest, HKEY hKeySrc, LPCTSTR name) 
{
    DWORD type = REG_NONE;
    ULONG status;
    DWORD size = 0;

    /* query the size of the value and its type */
    status = RegQueryValueEx(hKeySrc, name, NULL, &type, NULL, &size);
    if (status == NO_ERROR) {

        /* try to use small buffer on stack */
        BYTE localBuffer[32];
        BYTE * buf;
        if (size <= sizeof(localBuffer)) {
            buf = localBuffer;
        } else {
            /* allocate the buffer */
            buf = MEM_NewArray(BYTE, size);
        }

        if (buf) {

            /* actually query the value */
            status = RegQueryValueEx(hKeySrc, name, NULL, &type, buf, &size);
            ASSERT(status == NO_ERROR);
            if (status == NO_ERROR) {
                status = RegSetValueEx(hKeyDest, name, 0, type, buf, size);
            }

            /* deallocate dynamically allocated buffer */
            if (buf != localBuffer) {
                MEM_Free(buf);
            }
        } else {
            status = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    SetLastError(status);
    return (status == NO_ERROR);
}

/**
 * Copies all registry values from one key to another. If the 'recurse' 
 * parameter is True, then also copies all subkeys.
 */
Bool REG_CopyKey(HKEY hKeyDest, HKEY hKeySrc, Bool recurse) 
{
    Bool ok = False;
    if (hKeyDest && hKeySrc) {
        int index;
        StrBuf64 buf;
        STRBUF_InitBufXXX(&buf);

        /* always copy the values */
        ok = True;
        index = 0;
        while (ok && REG_GetNextValue(hKeySrc, index++, &buf.sb)) {
            if (!REG_CopyValue(hKeyDest, hKeySrc, buf.sb.s)) {
                ok = False;
            }
        }

        /* if 'recurse' is True, also copy the subkeys */
        if (recurse && ok) {
            index = 0;
            while (ok && REG_GetNextKey(hKeySrc, index++, &buf.sb)) {
                HKEY from = REG_OpenKey(hKeySrc, buf.sb.s, KEY_READ);
                HKEY to = REG_OpenCreateKey(hKeyDest, buf.sb.s);
                if (to && from) {
                    if (!REG_CopyKey(to, from, recurse)) {
                        ok = False;
                    }
                } else {
                    ok = False;
                }
                REG_CloseKey(from);
                REG_CloseKey(to);
            }
        }

        STRBUF_Destroy(&buf.sb);
    }
    return ok;
}

/**
 * Copies a named subkey of one key into another key.
 */
Bool REG_CopySubKey(HKEY hDest, HKEY hSrc, LPCTSTR szSubKey, Bool recurse)
{
    Bool ok = False;
    if (hDest && hSrc) {
        HKEY from = REG_OpenKey(hSrc, szSubKey, KEY_READ);
        if (from) {
            HKEY to = REG_OpenCreateKey(hDest, szSubKey);
            if (to) {
                ok = REG_CopyKey(to, from, recurse);
                REG_CloseKey(to);
            }
            REG_CloseKey(from);
        }
    }
    return ok;
}

/**
 * Returns the name of the n'th subkey of the specified registry key.
 * This is a wrapper around RegEnumKeyEx. Returns NULL and clears the 
 * string buffer if index is invalid or memory allocation fails. Upon 
 * success, the key name is placed into the the string buffer provided 
 * by the caller and pointer to the string is returned.
 */
Str REG_GetNextKey(HKEY hKey, int n, StrBuf * sb) 
{
    FILETIME junk;
    TCHAR szKey[MAX_PATH+1];
    ULONG nSize = COUNT(szKey);
    ULONG status = RegEnumKeyEx(hKey,n,szKey,&nSize,NULL,NULL,NULL,&junk);
    STRBUF_Clear(sb);
    if (status == NO_ERROR) {
        if (STRBUF_Copy(sb, szKey)) {
            return sb->s;
        }
    }
    return NULL;
}

/**
 * Returns the name of the n'th value of the specified registry key.
 * This is a wrapper around RegEnumValueEx. Returns NULL and clears the 
 * string buffer if index is invalid or memory allocation fails. Upon 
 * success, the value name is placed into the the string buffer provided 
 * by the caller and pointer to the string is returned.
 */
Str REG_GetNextValue(HKEY hKey, int n, StrBuf * sb) 
{
    TCHAR szName[MAX_PATH+1];
    ULONG nSize = COUNT(szName);        
    ULONG status = RegEnumValue(hKey,n,szName,&nSize,NULL,NULL,NULL,NULL);
    STRBUF_Clear(sb);
    if (status == NO_ERROR) {
        if (STRBUF_Copy(sb, szName)) {
            return sb->s;
        }
    }
    return NULL;
}

/**
 * Deletes a registry key. Takes care of deleting all the subkeys so that 
 * it works on both WinNT and Win95 (on WinNT RegDeleteKey cannot delete 
 * a key that has subkeys)
 */
Bool REG_DeleteKey(HKEY hKey, LPCTSTR szSubKey)
{
    ULONG status = NO_ERROR;
    HKEY hSubKey = NULL;

    TRACE2("Deleting registry key %08lX\\%s\n",hKey,szSubKey);
    hSubKey = REG_OpenKey(hKey,szSubKey,KEY_ALL_ACCESS);
    if (hSubKey) {

        /* delete the subkey first */
        REG_DeleteAllSubKeys(hSubKey);
        REG_CloseKey(hSubKey);

        /* finally, delete this key */
        status = RegDeleteKey(hKey,szSubKey);
        if (status != NO_ERROR) {
            TRACE3("RegDeleteKey(%08lX,%s) failed, error %lu\n",hKey,
                szSubKey, status);

            /* 
             * if we have failed to delete the key, then try to delete 
             * at least the values 
             */
            hSubKey = REG_OpenKey(hKey,szSubKey,KEY_ALL_ACCESS);
            if (hSubKey) {
                REG_DeleteAllValues(hSubKey);
                REG_CloseKey(hSubKey);
                status = RegDeleteKey(hKey,szSubKey);
            }
        }
    } else {
        status = GetLastError();
        if (status == ERROR_FILE_NOT_FOUND) {
            TRACE2("%08lX\\%s does not exist\n",hKey,szSubKey);
            status = NO_ERROR;
        } else {
            TRACE3("RegOpenKeyEx(%08lX,%s) failed, error %lu\n",
                hKey, szSubKey, status);
        }
    }

    SetLastError(status);
    return (status == NO_ERROR);
}

/**
 * Deletes value from the registry key. 
 */
Bool REG_DeleteValue(HKEY hKey, LPCTSTR szSubKey)
{
    ULONG status = RegDeleteValue(hKey, szSubKey);
    SetLastError( status );
    if (status != NO_ERROR) {
        TRACE3("RegDeleteValue(%04lX,%s) failed, error %lu\n",hKey,
            szSubKey,status);
        return False;
    }
    return True;
}

/**
 * Deletes all values from the registry key.
 */
Bool REG_DeleteAllValues(HKEY hKey)
{
    ULONG status;
    do {
        TCHAR szName[MAX_PATH+1];
        ULONG nSize = COUNT(szName);
        status = RegEnumValue(hKey,0,szName,&nSize,NULL,NULL,NULL,NULL);
        if (status == NO_ERROR) {
            if (!REG_DeleteValue(hKey,szName)) {
                break;
            }
        } else if (status !=  ERROR_NO_MORE_ITEMS) {
            TRACE2("RegEnumValue(%04lX) failed, error %lu\n",hKey,status);
        }
    } while ( status == NO_ERROR );
    SetLastError((status ==  ERROR_NO_MORE_ITEMS) ? NO_ERROR : status);
    return (status);
}

/**
 * Deletes all subkeys of the specified registry key.
 */
Bool REG_DeleteAllSubKeys(HKEY hKey)
{
    int i = 0;
    Bool ok = True;
    ULONG enumStatus;
    ULONG deleteStatus = NO_ERROR;
    do {
        FILETIME junk;
        TCHAR szKey[MAX_PATH+1];
        ULONG nSize = COUNT(szKey);
        enumStatus = RegEnumKeyEx(hKey,i,szKey,&nSize,NULL,NULL,NULL,&junk);
        if (enumStatus == NO_ERROR) {
            if (!REG_DeleteKey(hKey,szKey)) {
                deleteStatus = GetLastError();
                i++;
            }
        } else if (enumStatus !=  ERROR_NO_MORE_ITEMS) {
            TRACE2("RegEnumKeyEx(%04lX) failed, error %lu\n",hKey,enumStatus);
            ok = False;
        }
    } while (ok && enumStatus != ERROR_NO_MORE_ITEMS);

    /* set appropriate last error */
    if (deleteStatus != NO_ERROR) {
        SetLastError(deleteStatus);
    } else if (enumStatus ==  ERROR_NO_MORE_ITEMS) {
        SetLastError(NO_ERROR);
    } else {
        SetLastError(enumStatus);
    }

    return ok;
}

/**
 * Closes the registry key. Ignores NULL handles. Catches the exceptions.
 * Exception occur on some versions of WinNT when wrong handle is passed 
 * into RegCloseKey.
 */
Bool REG_CloseKey(HKEY hKey)
{
    ULONG status = NO_ERROR;
    if (hKey) {
#ifdef _USE_EXCEPTION_HANDLING
        TRY_CATCH {
#endif /*_USE_EXCEPTION_HANDLING */
            status = RegCloseKey(hKey);
            if (status != NO_ERROR) {
                TRACE2("RegCloseKey(%04lX) failed, error %lu\n",hKey,status);
            }
#ifdef _USE_EXCEPTION_HANDLING
        } CATCH {
            status = GetExceptionCode();
            ASSMSG1("EXCEPTION %08lX in REG_CloseKey!\n",status);
        }
#endif /*_USE_EXCEPTION_HANDLING */
    }
    return (status == NO_ERROR);
}

/**
 * Performs one registry transaction. Returns new current registry key.
 */
HKEY REG_Transaction(HKEY hKey, const REG_TRANSACTION * trans)
{
    HKEY hResult = hKey;
    switch ( trans->action ) {
    case REG_CREATE_KEY:
        REG_CreateKey(trans->key, trans->name);
        break;

    case REG_CREATE_SUBKEY:
        if (hKey) REG_CreateKey(hKey, trans->name);
        break;

    case REG_OPEN_KEY:
        hResult = REG_OpenKey(trans->key, trans->name, KEY_ALL_ACCESS);
        REG_CloseKey(hKey);
        break;

    case REG_OPEN_SUBKEY:
        if (hKey) hResult = REG_OpenKey(hKey, trans->name, KEY_ALL_ACCESS);
        REG_CloseKey(hKey);
        break;

    case REG_OPEN_CREATE_KEY:
        hResult = REG_OpenCreateKey(trans->key, trans->name);
        REG_CloseKey(hKey);
        break;

    case REG_OPEN_CREATE_SUBKEY:
        if (hKey) hResult = REG_OpenCreateKey(hKey, trans->name);
        REG_CloseKey( hKey );
        break;

    case REG_DELETE_KEY:
        REG_DeleteKey(trans->key, trans->name);
        break;

    case REG_DELETE_SUBKEY:
        if (hKey) REG_DeleteKey(hKey, trans->name);
        break;

    case REG_SET_VALUE:
        if (hKey) REG_SetValue(hKey, trans->name, trans->type, &trans->data);
        break;

    case REG_DELETE_VALUE:
        if (hKey) REG_DeleteValue(hKey, trans->name);
        break;

    case REG_DELETE_ALL_VALUES:
        if (hKey) REG_DeleteAllValues(hKey);
        break;

    default: 
        ASSERT(FALSE);
        break;
    }

    return hResult;
}

/**
 * Performs a set of registry transactions
 */
void REG_UpdateRegistry(const REG_TRANSACTION trans[], int nTrans)
{
    int i;
    HKEY hKey = NULL;
    for (i=0; i<nTrans; i++) {
        hKey = REG_Transaction(hKey, &trans[i]);
    }
    REG_CloseKey(hKey);
}

/*==========================================================================*
 *      M U L T I - S T R I N G    A P I
 *==========================================================================*/

static const Char EmptyMsz [2] = {0, 0};
struct _RegMsz {
    int alloc;          /* allocated size, in characters */
    int inuse;          /* not including last NULL terminator */
    Char * buffer;      /* allocated buffer */
};

/**
 * Creates a multi-string
 */
RegMsz * REGMSZ_Create() 
{
    RegMsz * msz = MEM_New(RegMsz);
    if (msz) memset(msz, 0, sizeof(*msz));
    return msz;
}

/**
 * Deletes the multi-string
 */
void REGMSZ_Delete(RegMsz * msz)
{
    if (msz) {
        if (msz->buffer) MEM_Free(msz->buffer);
        MEM_Free(msz);
    }
}

/**
 * Clears the multi-string
 */
void REGMSZ_Clear(RegMsz * msz)
{
    msz->inuse = 0;
}

/**
 * Returns number of entries in the multi-string
 */
int REGMSZ_Count(const RegMsz * msz)
{
    int count = 0;
    Str p = msz->buffer;
    Str end = p + msz->inuse;
    while (p < end) {
        while (p < end && *p) p++;
        count++;
        p++;
    }
    return count;
}

/**
 * Adds another non-empty string to the multi-string
 */
Bool REGMSZ_Add(RegMsz * msz, Str s)
{
    ASSERT(s && s[0]);
    if (s && s[0]) {
        int len = StrLen(s);
        int newsize = msz->inuse + len + 2;
        if (newsize > msz->alloc) {
            int size = MAX(msz->alloc*2,newsize);
            Char * newbuf = (Char*)MEM_Realloc(msz->buffer,sizeof(Char)*size);
            if (!newbuf) {
                return False;
            }
            msz->alloc = size;
            msz->buffer = newbuf;
        }
        StrCpy(msz->buffer + msz->inuse, s);
        msz->inuse += len + 1;
        msz->buffer[msz->inuse] = 0;
        return True;
    }
    return False;
}

/**
 * Returns the string at specified index
 */
Str REGMSZ_Get(const RegMsz * msz, int index)
{
    int i = 0;
    Str p = msz->buffer;
    Str end = p + msz->inuse;
    while (p < end) {
        if (i == index) return p;
        while (p < end && *p) p++;
        i++;
        p++;
    }
    ASSMSG1("Invalid index %d",index);
    return NULL;
}

/**
 * Returns the first string in the multi-string, NULL if the multi-string 
 * is mepty
 */
Str REGMSZ_First(const RegMsz * msz)
{
    return ((msz->inuse > 0) ? msz->buffer : NULL);
}

/**
 * Returns the next string in the multi-string, NULL if none. The input 
 * string must be returned by REGMSZ_First and REGMSZ_Next
 */
Str REGMSZ_Next(const RegMsz * msz, Str s)
{
    ASSERT(s >= msz->buffer && s < msz->buffer + msz->inuse);
    s += StrLen(s) + 1;
    if (s < msz->buffer + msz->inuse) {
        ASSERT(s + StrLen(s) < msz->buffer + msz->inuse);
        return s;
    } else {
        return NULL;
    }
}

/**
 * Sets buffer
 */
STATIC void REGMSZ_SetBuffer(RegMsz * msz, Char * s, int size)
{
    int addzeros = 2;
    MEM_Free(msz->buffer);
    msz->buffer = s;
    msz->alloc = size;
    if (size > 0 && !msz->buffer[size-1]) {
        addzeros--;
        if (size > 1 && !msz->buffer[size-2]) {
            addzeros--;
        }
    }
    if (addzeros) {
        Char* newbuf = (Char*)MEM_Realloc(s,sizeof(Char)*(size+addzeros));
        if (newbuf) {
            msz->alloc += addzeros;
            msz->buffer = newbuf;
            msz->buffer[msz->alloc-1] = 0;
            msz->buffer[msz->alloc-2] = 0;
            msz->inuse = msz->alloc-1;
        } else {
            msz->inuse = size;
        }
    } else {
        msz->inuse = size-1;
    }
}

/*==========================================================================*
 *
 * Bool REG_Query(HKEY hKey, LPCTSTR name, ULONG * type, Buffer * data); 
 * Bool REG_QueryDword (HKEY hKey, LPCTSTR name, DWORD * value); 
 * Bool REG_QueryBool(HKEY hKey, LPCTSTR name, Bool * value); 
 * Bool REG_QueryChar(HKEY hKey, LPCTSTR name, char * value); 
 * Str  REG_QueryStr(HKEY hKey, LPCTSTR name, StrBuf * value); 
 *
 * Bool REG_SetValue (HKEY hKey, LPCTSTR name, ULONG type, const REG_DATA *);
 * Bool REG_SetBinary (HKEY hKey, LPCTSTR name, PCVOID data, ULONG size);
 * Bool REG_SetDword (HKEY hKey, LPCTSTR name, DWORD value); 
 * Bool REG_SetBool (HKEY hKey, LPCTSTR name, Bool value); 
 * Bool REG_SetChar (HKEY hKey, LPCTSTR name, char value); 
 * Bool REG_SetString (HKEY hKey, LPCTSTR name, LPCTSTR value); 
 * 
 * Get/Set values of different types
 * 
 *==========================================================================*/

Bool REG_Query(HKEY hKey, LPCTSTR name, ULONG * type, Buffer * data)
{
    Bool ok = False;
    ULONG typeBuf, status, size = 0;
    if (!type) type = &typeBuf;
    *type = REG_NONE;
    status = RegQueryValueEx(hKey,name,NULL,type,NULL,&size);
    if (status == NO_ERROR) {
        if (data) {
            BUFFER_Clear(data);
            if (BUFFER_EnsureCapacity(data, size, False)) {
                void * p = BUFFER_Access(data);
                if (size > 0) {
                    ASSERT(p);
                    size = data->alloc;
                    status = RegQueryValueEx(hKey,name,NULL,type,p,&size);
                    if (status == NO_ERROR) {

                        /* 
                         * Update buffer size by the amount of data written 
                         * to the buffer
                         */
                        data->end = data->start + size;
                        ASSERT(data->end <= data->alloc);
                        if (data->end == data->alloc) {

                            /* this isn't really necessary, but won't hurt */
                            data->flags |= BUFFER_FULL;
                        }
                        ok = True;
                    }
                } else {
                    ok = True;
                }
            } else {
                status = ERROR_NOT_ENOUGH_MEMORY;
            }
        } else {
            ok = True;
        }
    } else if (data) {
        BUFFER_Clear(data);
    }
    SetLastError(status);
    return ok;
}

Bool REG_QueryBin(HKEY hKey, LPCTSTR name, Buffer * data)
{
    // do not allocate memory for data of wrong type:
    ULONG type;
    if (REG_Query(hKey, name, &type, NULL)) {
        if (type == REG_BINARY) {
            if (data) {
                if (REG_Query(hKey, name, &type, data)) {
                    // protection against extremely unlikely race condition.
                    // registry value may have been replaced with a value of 
                    // different type between the two REG_Query calls
                    if (type == REG_BINARY) {
                        return True;
                    } else {
                        BUFFER_Clear(data);
                        return False;
                    }
                }
            } else {
                return True;
            }
        }
    }
    return False;
}

Bool REG_QueryDword(HKEY hKey, LPCTSTR name, DWORD * value)
{
    ULONG type = REG_NONE;
    ULONG dword = 0;
    ULONG size = sizeof(dword);
    ULONG status = RegQueryValueEx(hKey,name,NULL,&type,(LPBYTE)&dword,&size);
    SetLastError(status);
    if (status == NO_ERROR && type == REG_DWORD && size == sizeof(dword)) {
        *value = dword;
        TRACE2("%s: %08lX\n",name,dword);
        return True;
    }
    TRACE3("RegQueryValueEx(%04lX,%s) failed, error %lu\n",hKey,name,status);
    return False;
}

Bool REG_QueryBool(HKEY hKey, LPCTSTR name, Bool * value) 
{
    ULONG data;
    if (REG_QueryDword(hKey, name, &data)) {
        *value = (data != 0);
        return True;
    }
    return False;
}

Bool REG_QueryChar(HKEY hKey, LPCTSTR name, char * value)
{
    ULONG data;
    if (REG_QueryDword(hKey, name, &data)) {
        *value = (char)data;
        return True;
    }
    return False;
}

Str REG_QueryStr(HKEY hKey, LPCTSTR name, StrBuf * sb)
{
    Str value = NULL;
    DWORD type = REG_NONE;
    ULONG status;
    DWORD size = 0;

    /* clear the buffer */
    STRBUF_Clear(sb);

    /* query the size of the string and its type */
    status = RegQueryValueEx(hKey, name, NULL, &type, NULL, &size);

    /* check status and type */
    if (status == NO_ERROR && (type == REG_SZ || type == REG_EXPAND_SZ)) {

        /* allocate the buffer */
        int n = size/sizeof(TCHAR) + 1; /* one more just in case */
        if (STRBUF_Alloc(sb, n)) {
            LPBYTE buffer = (LPBYTE)sb->s;
            size = n * sizeof(TCHAR);

            /* actually query the value */
            status = RegQueryValueEx(hKey, name, NULL, &type, buffer, &size);
            ASSERT(status == NO_ERROR);
            if (status == NO_ERROR) {

                /* update the string length */
                sb->len = _tcslen(sb->s);
                ASSERT(sb->len < sb->alloc);
                value = sb->s;
                /* success */
            }
        }
    }

    return value;
}

RegMsz * REG_QueryMultiSz(HKEY hKey, LPCTSTR name)
{
    DWORD type = REG_NONE;
    ULONG status;
    DWORD size = 0;

    /* query the size of the string and its type */
    status = RegQueryValueEx(hKey, name, NULL, &type, NULL, &size);

    /* check status and type */
    if (status == NO_ERROR && type == REG_MULTI_SZ) {

        /* allocate the buffer, one character for safety */
        ULONG allocSize = (size+sizeof(Char)-1)/sizeof(Char) + 1;
        Char * buf = MEM_NewArray(Char, allocSize);
        if (buf) {

            /* actually query the value */
            status = RegQueryValueEx(hKey,name,NULL,&type,(BYTE*)buf,&size);
            ASSERT(status == NO_ERROR);
            if (status == NO_ERROR) {
                RegMsz * msz = REGMSZ_Create();
                if (msz) {
                    if (allocSize > 1 && !buf[allocSize-2]) {
                        /* take out the extra character */
                        allocSize--;
                    } else {
                        buf[allocSize-1] = 0;
                    }
                    REGMSZ_SetBuffer(msz, buf, allocSize);
                    return msz;
                }
            }
            MEM_Free(buf);
        }
    }
    return NULL;
}

Bool REG_SetBinary(HKEY hKey, LPCTSTR name, PCVOID ptr, ULONG size)
{
    REG_DATA data;
    data.binary.data = ptr;
    data.binary.size = size;
    return REG_SetValue(hKey, name, REG_BINARY, &data);
}

Bool REG_SetDword(HKEY hKey, LPCTSTR name, DWORD value) 
{
    REG_DATA data;
    data.dword = value;
    return REG_SetValue(hKey, name, REG_DWORD, &data);
}

Bool REG_SetBool(HKEY hKey, LPCTSTR name, Bool value) 
{
    ULONG dword = value;
    return REG_SetDword(hKey, name, dword);
}

Bool REG_SetChar(HKEY hKey, LPCTSTR name, char value)
{
    ULONG dword = value;
    return REG_SetDword(hKey, name, dword);
}

Bool REG_SetString(HKEY hKey, LPCTSTR name, LPCTSTR value)
{
    REG_DATA data;
    data.string = value;
    return REG_SetValue(hKey, name, REG_SZ, &data);
}

Bool REG_SetStrBuf (HKEY hKey, LPCTSTR name, const StrBuf * value)
{
    return REG_SetString(hKey, name, STRBUF_Text(value));
}

Bool REG_SetMsz (HKEY hKey, LPCTSTR name, const RegMsz * value)
{
    LPCTSTR buffer = (value->inuse ? value->buffer : EmptyMsz);
    return REG_SetMultiSz(hKey, name, buffer);
}

Bool REG_SetMultiSz (HKEY hKey, LPCTSTR name, LPCTSTR value)
{
    REG_DATA data;
    data.msz.chars = value;
    data.msz.count = 0;
    if (value) {
        LPCTSTR s = value;
        while (*s) {
            int len = StrLen(s);
            s += (len + 1);
            data.msz.count += (len + 1);
        }
        data.msz.count++;
    }
    return REG_SetValue(hKey, name, REG_MULTI_SZ, &data);
}

/**
 * Sets registry value. Not all value types are supported, only those we 
 * really need.
 */
Bool REG_SetValue(HKEY hKey, LPCTSTR name, ULONG type, const REG_DATA * data)
{
    ULONG  nDataSize = 0;
    LPBYTE pData = NULL;
    ULONG  status;

    /* make sure it's a value type we support and set the data size */
    ASSERT(data);
    switch (type) {
    case REG_DWORD: 
        TRACE2("  %s = 0x%08lX\n",PRINTABLE(name),data->dword);
        nDataSize = sizeof(DWORD);   
        pData = (LPBYTE)&data->dword;
        break;

    case REG_SZ:
    case REG_EXPAND_SZ:
        if (data->string) {
            TRACE2("  %s = %s\n",PRINTABLE(name),data->string);
            nDataSize = (_tcslen(data->string)+1) * sizeof(TCHAR);  
            pData = (LPBYTE)data->string;
        }
        break;

    case REG_BINARY:
        nDataSize = data->binary.size;
        pData = (LPBYTE)data->binary.data;
#if DEBUG
        {   ULONG i;
            StrBuf128 b;
            STRBUF_InitBufXXX(&b);
            STRBUF_Format(&b.sb,TEXT("  %s = "),PRINTABLE(name));
            for (i=0; i<nDataSize; i++) {
                STRBUF_AppendFormat(&b.sb,TEXT("%02X "),((int)pData[i])&0xff);
            }
            TRACE1("%s\n",STRBUF_Text(&b.sb));
            STRBUF_Destroy(&b.sb);
        }
#endif /* DEBUG */
        break;

    case REG_MULTI_SZ:
        pData = (LPBYTE)data->msz.chars;
        nDataSize = data->msz.count * sizeof(TCHAR);
#if DEBUG
        if (data->string) {
            LPCTSTR s = data->string;
            TRACE1("  %s =",PRINTABLE(name));
            while (*s) {
                int len = StrLen(s);
                TRACE1(" \"%s\"",s);
                s += (len + 1);
            }
            TRACE("\n");
        } else {
            TRACE1("  %s = \n",PRINTABLE(name));
        }
#endif /* DEBUG */
        break;

    default:
        ASSMSG1("REG_SetValue: unsupported data type %d",type);
        SetLastError(ERROR_INVALID_PARAMETER);
        return False;
    }

    /* actually set the value */
    status = RegSetValueEx(hKey, name, 0, type, pData, nDataSize);
    SetLastError(status);
    if (status != NO_ERROR) {
        TRACE3("RegSetValueEx(%04lX,%s) failed, error %lu\n",hKey,
            PRINTABLE(name),status);
        return False;
    }
    return True;
}
