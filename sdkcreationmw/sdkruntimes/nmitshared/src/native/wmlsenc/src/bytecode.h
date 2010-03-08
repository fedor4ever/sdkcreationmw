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
/* File    : U:\wpsvob\wmlscript\work\ported\bytecode.h
   Version : \main\6
   Date    : 08-Aug-00.15:27:22
*/

/*
 * ========================================================================
 *  Name        : bytecode.h
 *  Part of     : 
 *  Description : 
 *  Version     : 
 *
 *  Copyright (c) 2000 - 2005 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ========================================================================
 */




/*
-----------------------------------------------------------------------------

    DESCRIPTION

      Defintions and interface function declarations for byte code generator.

-----------------------------------------------------------------------------
*/
#if !defined(BYTECODE_H)
#define BYTECODE_H


#define RET_OK      1
#define RET_ERROR   0

#define BYTECODE_INITIAL_SIZE 5000
#define BYTECODE_INCREASE_SIZE 1000

#define CONST_POOL_INITIAL_SIZE 500
#define CONST_POOL_INCREASE_SIZE 100

/* TIR 230699
typedef unsigned char BYTE;
typedef unsigned short WORD;
*/

struct StorageStruct
{
    BYTE*       pbData;
    INT16       iIndex;
    UINT32      iCurrentlyAllocated;
    UINT32      iInitialSize;
    INT16       iIncreaseSize;
};


/* global protos */
INT16   ByteCodeInitialize(struct StorageStruct* pStorage, int iInitialSize, int iIncreaseSize);
void    ByteCodeCleanUp(struct StorageStruct* pStorage);
INT16   ByteCodeGetStatus(struct StorageStruct* pStorage);
void    ByteCodeResize(struct StorageStruct* pStorage);

void    ByteCodeAppendMBInt(UINT32 uiVal, struct StorageStruct* pStorage);
void    ByteCodeAppendByte(BYTE bVal, struct StorageStruct* pStorage);
void    ByteCodeAppendWord(WORD wVal, struct StorageStruct* pStorage);
void    ByteCodeAppendLong(INT32 lVal, struct StorageStruct* pStorage);
void    ByteCodeAppendFloat(float fVal, struct StorageStruct* pStorage);
void    ByteCodeAppendString(char* pcVal, INT32 iLen, struct StorageStruct* pStorage);
void    ByteCodeAppendBytes(BYTE* pbBytes, INT16 iSize, struct StorageStruct* pStorage);

void    ByteCodeSetByte(INT16 iIndex, BYTE bVal, struct StorageStruct* pStorage);
void    ByteCodeSetWord(INT16 iIndex, WORD wVal, struct StorageStruct* pStorage);






#endif
