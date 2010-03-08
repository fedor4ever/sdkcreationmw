/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#define g_rgSCardT0Pci  _g_rgSCardT0Pci
#define g_rgSCardT1Pci  _g_rgSCardT1Pci
#define g_rgSCardRawPci _g_rgSCardRawPci
#define WINSCARDDATA
#define WINSCARDAPI

#define TRACE_PREFIX "SCARD: [WinsCard] "
#include "SdkCardDebug.h"

#include <windows.h>
#include <winioctl.h>
#undef FILE_DEVICE_SMARTCARD /* redefined in <winscard.h> */
#include <winscard.h>

#undef g_rgSCardT0Pci
#undef g_rgSCardT1Pci
#undef g_rgSCardRawPci

/**
 * We can't use TRACE macros here because some of these functions may be
 * called from within Emulator::Escape()/Reenter() block.
 */
#ifdef _REALLY_DEBUG
#  define WINSCARD_TRACE(s) OutputDebugStringA(TRACE_PREFIX s "\n")
#else 
#  define WINSCARD_TRACE(s)
#endif // _REALLY_DEBUG

/* *************************************************************************
 *  Support for WINSCARD.DLL
 *  This code will run on the systems that don't have WINSCARD.DLL installed
 * *************************************************************************/

/* handle to WINSCARD.DLL */
static HMODULE WINSCARD_handle = NULL;

/*
 * This flag is set to TRUE if we determine that WINSCARD.DLL is missing
 * or unusable (i.e. some required exports are missing), so that we don't 
 * try to load it more than once.
 */
static BOOL WINSCARD_missing = FALSE; 

static LPCSTR WINSCARD_DLL = "WINSCARD.DLL";

/* 
 * these are entry points used by this module. These are NOT all entry
 * points exported by WINSCARD.DLL, just those we need here
 */

/* SCardConnectW */
#define WINSCARD_PROC_CONNECT_W  "SCardConnectW"
#define WINSCARD_PROC_CONNECT WINSCARD_PROC_CONNECT_W
#define WINSCARD_CONNECT WINSCARD_CONNECT_W
#define WINSCARD_Connect WINSCARD_ConnectW
typedef LONG (WINAPI * WINSCARD_CONNECT_W)(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol);

/* SCardDisconnect */
#define WINSCARD_PROC_DISCONNECT "SCardDisconnect"
typedef LONG (WINAPI * WINSCARD_DISCONNECT)(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition);

/* SCardEstablishContext */
#define WINSCARD_PROC_ESTABLISH_CONTEXT "SCardEstablishContext"
typedef LONG (WINAPI * WINSCARD_ESTABLISH_CONTEXT)(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext);

/* SCardListReadersW */
#define WINSCARD_PROC_LIST_READERS_W "SCardListReadersW"
#define WINSCARD_PROC_LIST_READERS WINSCARD_PROC_LIST_READERS_W
#define WINSCARD_LIST_READERS WINSCARD_LIST_READERS_W
#define WINSCARD_ListReaders WINSCARD_ListReadersW
typedef LONG (WINAPI * WINSCARD_LIST_READERS_W)(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszGroups,
    OUT     LPWSTR mszReaders,
    IN OUT  LPDWORD pcchReaders);

/* SCardReleaseContext */
#define WINSCARD_PROC_RELEASE_CONTEXT "SCardReleaseContext"
typedef LONG (WINAPI * WINSCARD_RELEASE_CONTEXT)(
    IN      SCARDCONTEXT hContext);

/* SCardTransmit */
#define WINSCARD_PROC_TRANSMIT "SCardTransmit"
typedef LONG (WINAPI * WINSCARD_TRANSMIT)(
    IN SCARDHANDLE hCard,
    IN LPCSCARD_IO_REQUEST pioSendPci,
    IN LPCBYTE pbSendBuffer,
    IN DWORD cbSendLength,
    IN OUT LPSCARD_IO_REQUEST pioRecvPci,
    OUT LPBYTE pbRecvBuffer,
    IN OUT LPDWORD pcbRecvLength);

/* SCardStatusW */
#define WINSCARD_PROC_STATUS_W "SCardStatusW"
#define WINSCARD_PROC_STATUS WINSCARD_PROC_STATUS_W
#define WINSCARD_STATUS WINSCARD_STATUS_W
#define WINSCARD_Status WINSCARD_StatusW
typedef LONG (WINAPI * WINSCARD_STATUS_W)(
    IN SCARDHANDLE hCard,
    OUT LPWSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen);

/* SCardGetStatusChangeW */
#define WINSCARD_PROC_GET_STATUS_CHANGE_W "SCardGetStatusChangeW"
#define WINSCARD_PROC_GET_STATUS_CHANGE WINSCARD_PROC_GET_STATUS_CHANGE_W
#define WINSCARD_GET_STATUS_CHANGE WINSCARD_GET_STATUS_CHANGE_W
#define WINSCARD_GetStatusChange WINSCARD_GetStatusChangeW
typedef LONG (WINAPI * WINSCARD_GET_STATUS_CHANGE_W)(
    IN SCARDCONTEXT hContext,
    IN DWORD dwTimeout,
    IN OUT LPSCARD_READERSTATE_W rgReaderStates,
    IN DWORD cReaders);

/* SCardCancel */
#define WINSCARD_PROC_CANCEL "SCardCancel"
typedef LONG (WINAPI * WINSCARD_CANCEL)(
    IN SCARDCONTEXT hContext);

/* addresses of functions */
static WINSCARD_CONNECT             WINSCARD_Connect            = NULL;
static WINSCARD_DISCONNECT          WINSCARD_Disconnect         = NULL;
static WINSCARD_ESTABLISH_CONTEXT   WINSCARD_EstablishContext   = NULL;
static WINSCARD_LIST_READERS        WINSCARD_ListReaders        = NULL;
static WINSCARD_RELEASE_CONTEXT     WINSCARD_ReleaseContext     = NULL;
static WINSCARD_TRANSMIT            WINSCARD_Transmit           = NULL;
static WINSCARD_STATUS              WINSCARD_Status             = NULL;
static WINSCARD_GET_STATUS_CHANGE   WINSCARD_GetStatusChange    = NULL;
static WINSCARD_CANCEL              WINSCARD_Cancel             = NULL;

/* and some variables, too */
static SCARD_IO_REQUEST * WINSCARD_g_rgSCardT0Pci  = NULL;
static SCARD_IO_REQUEST * WINSCARD_g_rgSCardT1Pci  = NULL;
static SCARD_IO_REQUEST * WINSCARD_g_rgSCardRawPci = NULL;

extern "C" SCARD_IO_REQUEST g_rgSCardT0Pci;
extern "C" SCARD_IO_REQUEST g_rgSCardT1Pci;
extern "C" SCARD_IO_REQUEST g_rgSCardRawPci;

SCARD_IO_REQUEST g_rgSCardT0Pci;
SCARD_IO_REQUEST g_rgSCardT1Pci;
SCARD_IO_REQUEST g_rgSCardRawPci;

static struct _WINSCARD_Export {
    const char* name;
    FARPROC * value_ptr;
} WINSCARD_exports [] = {

    /* functions */
    {WINSCARD_PROC_CONNECT,             (FARPROC*)&WINSCARD_Connect},
    {WINSCARD_PROC_DISCONNECT,          (FARPROC*)&WINSCARD_Disconnect},
    {WINSCARD_PROC_ESTABLISH_CONTEXT,   (FARPROC*)&WINSCARD_EstablishContext},
    {WINSCARD_PROC_LIST_READERS,        (FARPROC*)&WINSCARD_ListReaders},
    {WINSCARD_PROC_RELEASE_CONTEXT,     (FARPROC*)&WINSCARD_ReleaseContext},
    {WINSCARD_PROC_TRANSMIT,            (FARPROC*)&WINSCARD_Transmit},
    {WINSCARD_PROC_STATUS,              (FARPROC*)&WINSCARD_Status},
    {WINSCARD_PROC_GET_STATUS_CHANGE,   (FARPROC*)&WINSCARD_GetStatusChange},
    {WINSCARD_PROC_CANCEL,              (FARPROC*)&WINSCARD_Cancel},

    /* variables */
    {"g_rgSCardT0Pci",                  (FARPROC*)&WINSCARD_g_rgSCardT0Pci},
    {"g_rgSCardT1Pci",                  (FARPROC*)&WINSCARD_g_rgSCardT1Pci},
    {"g_rgSCardRawPci",                 (FARPROC*)&WINSCARD_g_rgSCardRawPci},
};

#define COUNT(array) (sizeof(array)/sizeof(array[0]))

/**
 * Checks if WINSCARD.DLL has been loaded; if not, attempts to load it
 * Returns TRUE if WINSCARD.DLL has been successfully loaded, FALSE 
 * it the DLL or some entry points are missing.
 */
static BOOL WINSCARD_IsAvailable()
{
    if (!WINSCARD_handle && !WINSCARD_missing)
    {
        HINSTANCE handle = LoadLibraryA(WINSCARD_DLL);
        if (handle)
        {
            int i;
            for( i=0; i<COUNT(WINSCARD_exports); i++)
            {
                LPCSTR name = WINSCARD_exports[i].name;
                FARPROC addr = GetProcAddress(handle,name);
                if (addr)
                {
                    *(WINSCARD_exports[i].value_ptr) = addr;
                }
                else
                {
                    TRACE2("ERROR: export %hs is missing from %hs\n",
                               name, WINSCARD_DLL);

                    /* bad library, don't use it */
                    WINSCARD_missing = TRUE;
                    FreeLibrary(handle);
                    handle = NULL;
                    break;
                }
            }
            g_rgSCardT0Pci = *WINSCARD_g_rgSCardT0Pci;
            g_rgSCardT1Pci = *WINSCARD_g_rgSCardT1Pci;
            g_rgSCardRawPci = *WINSCARD_g_rgSCardRawPci;
            WINSCARD_handle = handle;
        }
        else
        {
            TRACE1("ERROR: failed to load %hs\n",WINSCARD_DLL);
            WINSCARD_missing = TRUE;
        }
    }
    return (WINSCARD_handle != NULL);
}

/* *************************************************************************
 *  Functions that redirect WINSCARD calls to the real DLL if possible
 * *************************************************************************/

LONG WINAPI
SCardConnectW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol)
{
    WINSCARD_TRACE("SCardConnect");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_ConnectW(hContext,szReader,dwShareMode,
                                 dwPreferredProtocols, phCard,
                                 pdwActiveProtocol);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardDisconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition)
{
    WINSCARD_TRACE("SCardDisconnect");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_Disconnect(hCard,dwDisposition);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardEstablishContext(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext)
{
    WINSCARD_TRACE("SCardEstablishContext");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_EstablishContext(dwScope,pvReserved1,
                                         pvReserved2,phContext);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardListReadersW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszGroups,
    OUT     LPWSTR mszReaders,
    IN OUT  LPDWORD pcchReaders)
{
    WINSCARD_TRACE("SCardListReaders");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_ListReadersW(hContext,mszGroups,
                                     mszReaders,pcchReaders);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardReleaseContext(
    IN      SCARDCONTEXT hContext)
{
    WINSCARD_TRACE("SCardReleaseContext");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_ReleaseContext(hContext);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardTransmit(
    IN SCARDHANDLE hCard,
    IN LPCSCARD_IO_REQUEST pioSendPci,
    IN LPCBYTE pbSendBuffer,
    IN DWORD cbSendLength,
    IN OUT LPSCARD_IO_REQUEST pioRecvPci,
    OUT LPBYTE pbRecvBuffer,
    IN OUT LPDWORD pcbRecvLength)
{
    WINSCARD_TRACE("SCardTransmit");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_Transmit(hCard, pioSendPci, pbSendBuffer, 
                                 cbSendLength, pioRecvPci, pbRecvBuffer,
                                 pcbRecvLength);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardStatusW(
    IN SCARDHANDLE hCard,
    OUT LPWSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen)
{
    WINSCARD_TRACE("SCardStatus");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_StatusW(hCard, szReaderName, pcchReaderLen,
                                pdwState, pdwProtocol, pbAtr, pcbAtrLen);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardGetStatusChangeW(
    IN SCARDCONTEXT hContext,
    IN DWORD dwTimeout,
    IN OUT LPSCARD_READERSTATE_W rgReaderStates,
    IN DWORD cReaders)
{
    //WINSCARD_TRACE("SCardGetStatusChange");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_GetStatusChangeW(hContext, dwTimeout, rgReaderStates,
                                         cReaders);
    }
    return SCARD_E_NO_SERVICE;
}

LONG WINAPI
SCardCancel(
    IN SCARDCONTEXT hContext)
{
    WINSCARD_TRACE("SCardCancel");
    if (WINSCARD_IsAvailable())
    {
        return WINSCARD_Cancel(hContext);
    }
    return SCARD_E_NO_SERVICE;
}
