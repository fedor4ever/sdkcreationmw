/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NMIT_LIB_H_
#define _NMIT_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Browser UIDs */
#define BROWSER_UID_NOKIA    0x10008D39
#define BROWSER_UID_KHTML    0x1020724D

/* Basic typedefs */
typedef struct _NmitInterface NmitInterface;
typedef unsigned short NmitUcs2;
typedef const NmitUcs2 * NmitStr;
typedef enum _NmitBool {
    NmitFalse = 0,
    NmitTrue = 1
} NmitBool;

/* These are platform/compiler dependent */
typedef unsigned __int64 NmitProcessId;
#define NMIT_API __cdecl

/* 
 * Load URL callback. The parameter is supposed to be a well-formed URL.
 * It's not necessarily an HTTP URL. It can be a file: URL. Note that
 * file: URLs must contain complete fully qualified file names, because
 * client and server may have different current directories, and 
 * therefore relative file names would mean different things to them.
 */
typedef NmitBool (
NMIT_API * 
NmitLoad)(
    void * aContext,        /* context passed to NMIT_Create */
    NmitStr aURL,           /* the URL to load */
    unsigned int aBrowser   /* UID of the browser, zero to use default */
    );

/**
 * Request to push the emulator window to front or otherwise draw the
 * user's attention.
 */
typedef void (
NMIT_API * 
NmitToFront)(
    void * aContext         /* context passed to NMIT_Create */
    );

/**
 * Runs an executable in the emulator.
 */
typedef NmitBool (
NMIT_API * 
NmitRun)(
    void * aContext,        /* context passed to NMIT_Create */
    NmitStr aExe,           /* the executable to run */
    NmitStr * aArgs,        /* command line arguments */
    int aArgCount,          /* number of command line arguments */
    NmitProcessId * aPid    /* process id of the created process */
    );

/**
 * Request to exit the emulator.
 */
typedef void (
NMIT_API * 
NmitExit)(
    void * aContext         /* context passed to NMIT_Create */
    );

/**
 * Initializes the NMIT emulator-side DSCLI context. Returns NULL
 * on failure.
 */
extern 
NmitInterface *
NMIT_API 
NMIT_Create(
    const char * aPid,      /* the product id string */
    NmitLoad aLoad,         /* the load callback */
    NmitRun aRun,           /* the run callback */
    NmitToFront aToFront,   /* the toFront callback */
    NmitExit aExit,         /* the exit callback */
    void* aContext          /* opaque pointer */
    );

/**
 * Deallocates the NMIT DSCLI context.
 */
extern 
void 
NMIT_API 
NMIT_Delete(
    NmitInterface * aNmit   /* the pointer returned by NMIT_Create */
    );

#ifdef __cplusplus
} /* and of extern "C" */
#endif /* __cplusplus */

#endif /* _NMIT_LIB_H_ */
