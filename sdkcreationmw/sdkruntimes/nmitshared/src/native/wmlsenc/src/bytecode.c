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
/* File    : U:\wpsvob\wmlscript\work\ported\bytecode.c
   Version : \main\10
   Date    : 22-Nov-00.17:25:02
*/

/*
 * ========================================================================
 *  Name        : bytecode.c
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

      Generates bytecode out of node tree.

-----------------------------------------------------------------------------
*/
#include "main.h"
#include "common.h"
#include "bytecode.h"
#include "wps_service.h"


/* local protos */
INT16   EnsureFit(UINT32 iBytes, struct StorageStruct* pStorage);
void    ByteCodeReverseBytes(BYTE* pbBytes, INT16 iNumByte);




/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/


/*********************************************************************

 FUNCTION     : ByteCodeInitialize

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        struct StorageStruct* pStorage
        int iInitialSize
        int iIncreaseSize

 DESCRIPTION:
        Initializes the StorageStruct by allocating iInitialSize of 
        memory for it.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 ByteCodeInitialize(struct StorageStruct* pStorage, int iInitialSize, int iIncreaseSize)
{
    INT16 iRet = RET_ERROR;

    pStorage->iInitialSize = iInitialSize;
    pStorage->iIncreaseSize = iIncreaseSize;
    pStorage->pbData = (BYTE*)WMLS_malloc(pStorage->iInitialSize);

#ifdef MEMORY_OUT_TEST
    if(rand()%MEMORY_MAX < MEMORY_LIMIT)
    {
        if(pStorage->pbData)
        {
            WMLS_free(pStorage->pbData);
            pStorage->pbData = NULL;
        }
    }
#endif
    
    
    if(pStorage->pbData)
    {
        pStorage->iCurrentlyAllocated = pStorage->iInitialSize;
        pStorage->iIndex = 0;
        iRet = RET_OK;
    }

    return iRet;
}



/*********************************************************************

 FUNCTION     : ByteCodeCleanUp

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct StorageStruct* pStorage

 DESCRIPTION:
        Frees any memory reserved by pStorage.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeCleanUp(struct StorageStruct* pStorage)
{
    if(pStorage->pbData)
    {
        WMLS_free(pStorage->pbData);
        pStorage->pbData = NULL;
    }
}




/*********************************************************************

 FUNCTION     : ByteCodeResize

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct StorageStruct* pStorage

 DESCRIPTION:
        If reserved buffer is bigger than currently used, decreases
        the buffer to used size.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeResize(struct StorageStruct* pStorage)
{
    if(pStorage->pbData)
    {
        pStorage->pbData = (BYTE*)WMLS_realloc ((void*)pStorage->pbData,pStorage->iIndex, pStorage->iCurrentlyAllocated);
        pStorage->iCurrentlyAllocated = pStorage->iIndex;
    }
}




/*********************************************************************

 FUNCTION     : ByteCodeGetStatus

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        struct StorageStruct* pStorage

 DESCRIPTION:
        Returns error if there is no memory reserved.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 ByteCodeGetStatus(struct StorageStruct* pStorage)
{
    if(pStorage->pbData == NULL)
        return RET_ERROR;
    else
        return RET_OK;
}





/*********************************************************************

 FUNCTION     : ByteCodeAppendMBInt

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        UINT32 uiVal
        struct StorageStruct* pStorage

 DESCRIPTION:
        Appends uiVal as MultiByteInt.
    
 NOTE: 
        MultiByteInt is series of bytes where most significant byte
        is reserved for marking that number continues in next byte.

 CHANGES: 

*********************************************************************/
void ByteCodeAppendMBInt(UINT32 uiVal, struct StorageStruct* pStorage)
{
    INT16   iIndex=0;
    BYTE    pbTemp[8];
    BYTE    bCurrent;

    /* is there enough room for value */
    if( EnsureFit(8,pStorage) )
    {
    
        do{
            /* set the MSB to 0. */
            bCurrent = (BYTE)(uiVal & 127);

            /* if not first BYTE set MSB to 1 */
            if(iIndex)
                bCurrent += 0x80;

            uiVal = uiVal >> 7;

            pbTemp[iIndex++] = bCurrent;

        }while( uiVal && (iIndex<8) );

        if(iIndex < 8)
        {
            /* now add bytes in reverse order to byteArray */
            while(iIndex)
            {
                pStorage->pbData[(pStorage->iIndex)++] = pbTemp[--iIndex];  
            }
        }
    }
}


/*********************************************************************

 FUNCTION     : ByteCodeAppendFloat

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        float fVal      
        struct StorageStruct* pStorage

 DESCRIPTION:
        Appends float to pStorage. Bytes must be reversed!
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeAppendFloat(float fVal, struct StorageStruct* pStorage)
{
    BYTE*  pFloat; 
/*    BYTE*   pbBytes; */

    /* is there enough room for value */
    if( EnsureFit(sizeof(float),pStorage) )
    {

        pFloat = (BYTE*) (pStorage->pbData + pStorage->iIndex);
        /* *pFloat = fVal; */
        memcpy( pFloat,&fVal,sizeof( fVal ));

        /* float needs BYTE reversing */
        ByteCodeReverseBytes( pFloat,4 );

        pStorage->iIndex += sizeof(float);
     
    }
}


/*********************************************************************

 FUNCTION     : ByteCodeAppendByte

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        BYTE bVal
        struct StorageStruct* pStorage

 DESCRIPTION:
        Appends BYTE to pStorage.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeAppendByte(BYTE bVal, struct StorageStruct* pStorage)
{
    /* is there enough room for value */
    if( EnsureFit(1,pStorage) )
    {
        pStorage->pbData[(pStorage->iIndex)++] = bVal;
    }
}


/*********************************************************************

 FUNCTION     : ByteCodeSetByte

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        INT16 iIndex                    -in: which byte to set
        BYTE bVal                       -in: which value to set
        struct StorageStruct* pStorage

 DESCRIPTION:
        Sets existing BYTE to new value.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeSetByte(INT16 iIndex, BYTE bVal, struct StorageStruct* pStorage)
{
    /* is there enough room for value */
    if( iIndex < pStorage->iIndex )
    {
        pStorage->pbData[iIndex] = bVal;
    }
}



/*********************************************************************

 FUNCTION     : ByteCodeAppendWord

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        WORD wVal
        struct StorageStruct* pStorage

 DESCRIPTION:
        Appends WORD to pStorage.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeAppendWord(WORD wVal, struct StorageStruct* pStorage)
{
    BYTE*   pwWord;
/*    BYTE*   pbBytes; */

    /* is there enough room for value */
    if( EnsureFit(2,pStorage) )
    {   
        pwWord = (BYTE*) (pStorage->pbData + pStorage->iIndex);
        memcpy( pwWord,&wVal,sizeof( wVal ));

/*        pbBytes = (BYTE*) pwWord; */
        ByteCodeReverseBytes( pwWord,2);

        pStorage->iIndex += sizeof(WORD);
    }
}


/*********************************************************************

 FUNCTION     : ByteCodeSetWord

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        INT16 iIndex                    -in: which byte to set
        WORD wVal                       -in: which value to set
        struct StorageStruct* pStorage

 DESCRIPTION:
        Sets existing WORD to new value.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeSetWord(INT16 iIndex, WORD wVal, struct StorageStruct* pStorage)
{
    BYTE*   pwWord;
/*    BYTE*   pbBytes; */

    /* is there enough room for value */
    if( iIndex < pStorage->iIndex )
    {
        pwWord = (BYTE*) (pStorage->pbData + iIndex);
        memcpy( pwWord,&wVal,sizeof( wVal ));

/*        pbBytes = (BYTE*) pwWord; */
        ByteCodeReverseBytes( pwWord,2 );
    }
}


/*********************************************************************

 FUNCTION     : ByteCodeAppendLong

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        INT32 lVal
        struct StorageStruct* pStorage

 DESCRIPTION:
        Appends long int to pStorage.
    
 NOTE: 

 CHANGES: 
	reverse high and low word TIR 160699

*********************************************************************/
void ByteCodeAppendLong(INT32 lVal, struct StorageStruct* pStorage)
{
    BYTE*   plLong;
/*    BYTE*   pbBytes; */

    /* is there enough room for value */
    if( EnsureFit(sizeof(INT32),pStorage) )
    {
        plLong = (BYTE*) (pStorage->pbData + pStorage->iIndex);
        memcpy( plLong,&lVal,sizeof( lVal ));

		 /* 32 bit int needs BYTE reversing */
/*        pbBytes = (BYTE*) plLong; */
        ByteCodeReverseBytes( plLong,4 );


        pStorage->iIndex += sizeof(INT32);
    }
}




/*********************************************************************

 FUNCTION     : ByteCodeAppendString

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        char* pcVal
        struct StorageStruct* pStorage

 DESCRIPTION:
        Appends string to pStorage.
    
 NOTE: 

 CHANGES: It gets a new parameter, the length of the string, because
          the string can contain 0 characters: iLen

*********************************************************************/
void ByteCodeAppendString(char* pcVal, INT32 iLen, struct StorageStruct* pStorage)
{
    char*   pcChar;
    
    if(iLen) {
        /* 
         * is there enough room for value 
         */
        if( EnsureFit(iLen,pStorage) ) {
            pcChar = (char*) (pStorage->pbData + pStorage->iIndex);
            memcpy(pcChar, pcVal, iLen);

            pStorage->iIndex += iLen;
        }
    }
}



/*********************************************************************

 FUNCTION     : ByteCodeAppendBytes

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        BYTE* pbBytes                   -in: bytes to add
        INT16 iSize                     -in: how many bytes
        struct StorageStruct* pStorage

 DESCRIPTION:
        Adds multiple bytes at once to pStorage.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ByteCodeAppendBytes(BYTE* pbBytes, INT16 iSize, struct StorageStruct* pStorage)
{
    if( EnsureFit(iSize,pStorage) )
    {
        memcpy((void*) (pStorage->pbData + pStorage->iIndex), pbBytes, iSize);

        pStorage->iIndex += iSize;
    }
}

















/***************************************************************************
                            LOCAL FUNCTIONS
***************************************************************************/



/*********************************************************************

 FUNCTION     : EnsureFit

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        UINT32 iBytes               -in: how many bytes will be added
        struct StorageStruct* pStorage

 DESCRIPTION:
        Checks if iBytes bytes will fit to reserved buffer, if not
        reallocs buffer by increasing pStorage->iIncreaseSize bytes.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 EnsureFit(UINT32 iBytes, struct StorageStruct* pStorage)
{
    INT16 iRet = RET_OK;
    INT16 iSize;

	if (pStorage->pbData == NULL)
	{
		return RET_ERROR;
	}

	/* checking the overflowing of index */
	if ( pStorage->iIndex >= (INT32)(0x7fff - iBytes))
	{
		/* out of memory */
		WMLS_free(pStorage->pbData);
		pStorage->pbData = NULL;
		iRet = RET_ERROR;
	}
    /* is there enough space? */
    if( ((iBytes + pStorage->iIndex) > pStorage->iCurrentlyAllocated) && (iRet == RET_OK) )
    {
        /* allocate more space */
        if(pStorage->pbData)
        {
            /* do we need more than normally reallocated */
            if(iBytes < (UINT32)pStorage->iIncreaseSize)
                iSize = pStorage->iIncreaseSize;
            else
                iSize = (INT16)iBytes + (INT16)pStorage->iIncreaseSize;

			if ( pStorage->iCurrentlyAllocated >= (UINT32)(0xffffffff - iSize))
			{
				/* out of memory */
				WMLS_free(pStorage->pbData);
				pStorage->pbData = NULL;
				iRet = RET_ERROR;
			}

            pStorage->iCurrentlyAllocated += iSize;
            pStorage->pbData = (BYTE*)WMLS_realloc ((void*)pStorage->pbData,pStorage->iCurrentlyAllocated, pStorage->iCurrentlyAllocated-iSize);
        }

        if(pStorage->pbData == NULL)
        {
            iRet = RET_ERROR;
        }
    }


    return iRet;
}



/*********************************************************************

 FUNCTION     : ByteCodeReverseBytes

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        BYTE* pbBytes       -in: byte buffer
        INT16 iNumBytes     -in: how many bytes

 DESCRIPTION:
        Reverses bytes in buffer
    
 NOTE: 
        Supports only 2 or 4 bytes

 CHANGES: 

*********************************************************************/
void ByteCodeReverseBytes(BYTE* pbBytes, INT16 iNumBytes)
{
/*
   BYTE b;

    if(iNumBytes == 4)
    {
        b=pbBytes[0];
        pbBytes[0] = pbBytes[3];
        pbBytes[3] = b;

        b=pbBytes[1];
        pbBytes[1] = pbBytes[2];
        pbBytes[2] = b;
    }
    else
    {
        b=pbBytes[0];
        pbBytes[0] = pbBytes[1];
        pbBytes[1] = b;
    }
*/
  UINT16 tmp2;
  UINT32 tmp4;
 
  switch( iNumBytes )
    { case 2 :
        memcpy( &tmp2,pbBytes,2 ); /* Use of memcpy is a MUST! (instead of casting) 
                                      Data can be not aligned! */
        pbBytes[0] = (BYTE)(tmp2 >> 8 & 0xFF);
        pbBytes[1] = (BYTE)(tmp2 & 0xFF);
        break;
      case 4 :
        memcpy( &tmp4,pbBytes,4 ); /* Use of memcpy is a MUST! (instead of casting) 
                                      Data can be not aligned! */
        pbBytes[0] = (BYTE)(tmp4 >> 24 & 0xFF);
        pbBytes[1] = (BYTE)(tmp4 >> 16 & 0xFF);
        pbBytes[2] = (BYTE)(tmp4 >> 8 & 0xFF);
        pbBytes[3] = (BYTE)(tmp4 & 0xFF);
        break;      
    }
}

