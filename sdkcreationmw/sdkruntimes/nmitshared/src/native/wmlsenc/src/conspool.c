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
/* File    : U:\wpsvob\wmlscript\work\ported\conspool.c
   Version : \main\11
   Date    : 22-Nov-00.17:24:54
*/

/*
 * ========================================================================
 *  Name        : conspool.c
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

      Validates ints, floats, hexs, octs and escape chars, converts them to 
      ints/escapes, adds them into hash table and generates byte code out of
      the constant pool.

-----------------------------------------------------------------------------
*/
#include "main.h"
#include "common.h"
#include "symtable.h"
#include "bytecode.h"
#include "conspool.h"


#define IANA_MIBENUM_UTF_8 106


/* local protos */
void    ConstPoolAppendConst(struct StorageStruct* pStorage, struct HashNode* pNode);
void    ConstPoolAddInteger(struct StorageStruct* pStorage, int iVal);
void    ConstPoolAddFloat(struct StorageStruct* pStorage, float fVal);
void    ConstPoolAddString(struct StorageStruct* pStorage, char* pcVal);
INT32   ConvertEscapeSequences(char* pcVal);
int		AddUTF8(UINT16 uiValue, BYTE* pbDest);
int		CheckZeroStr(char *pcStart);









/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/


/*********************************************************************

 FUNCTION     : DecValidate

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE 

 PARAMETERS:
        char* pcValue

 DESCRIPTION:
        Checks that integer value is between 2147483647 and 
        -2147483648.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE DecValidate(char* pcValue)
{
    int iLen;
    int iMinus=0;

    if(pcValue[0]=='-')
    {
        iMinus=1;
        pcValue++;
    }
    else if(pcValue[0]=='+')
    {
        pcValue++;
    }

    iLen=strlen(pcValue);

    /* if len is 9 numbers max -> no problemos */
    if(iLen<10)
        return 1;

    /* if len is 11+ numbers -> no changes */
    if(iLen>10)
        return 0;

    /* len is now 10 numbers */
    if(iMinus)
    {
        /* min value is -2147483648 */
        if(strcmp(pcValue,"2147483649")<0)
            return 1;
    }
    else
    {
        /* max value is 2147483647 */
        if(strcmp(pcValue,"2147483648")<0)
            return 1;
    }

    return 0;
    
}


/*********************************************************************

 FUNCTION     : FloatValidate

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE 

 PARAMETERS:
        char* pcValue

 DESCRIPTION:
        Checks that float value is between 3.40282347E+38 and 
        1.17549435E-38 or 0.0.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE FloatValidate(char* pcValue)
{
    double fValue;

    if(pcValue[0]=='-')
        pcValue++;

    fValue = atof(pcValue);
        
    if( (fValue >= 1.17549435E-38) && (fValue <= 3.40282347E+38) )
        return 1;
    else
    {
        if(fValue == 0.0)
            return 1;
        else
            return 0;   
    }
}


/*********************************************************************

 FUNCTION     : OctValidate

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE 

 PARAMETERS:
        char* pcOct

 DESCRIPTION:
        Checks that octal value is between \0 and \37777777777
            
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE OctValidate(char* pcOct)
{
    int i;
    int iLen;
    char*	pcStart = pcOct + 1; /* no sign */


    if(( pcOct[0]=='+' ) || ( pcOct[0]=='-' ))
      pcStart++;

    iLen = strlen( pcStart );

	/* If the whole string is 0, it's valid */
	if (CheckZeroStr(pcStart))
	{
		return 1;
	}

    if(iLen == 0)
      return 0;

    for( i = 0; i < iLen; i++ )
      if(( pcStart[i] < '0' ) || ( pcStart[i] > '7' ))
        return 0;

    if( iLen<11 )
        return 1;
    else
      if( iLen == 11 )
		    {
          /* 017777777777 or smaller always legal */
		      if( pcStart[0]<'2' )
			      return 1;

          /* if not this then always illegal */
          if( strcmp( pcStart,"20000000000" ))
            return 0;

          /* -040000000000 only legal */
          if( pcOct[0] == '-' )
            return 1;
        }
  
   return 0;

}


/*********************************************************************

 FUNCTION     : HexValidate

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE 

 PARAMETERS:
        char* pcHex

 DESCRIPTION:
        Checks that octal value is between 0x0 and 0xffffffff
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE HexValidate(char* pcHex)
{
    char *tmp = pcHex + 2;
    int iLen;

   	if(( pcHex[0] == '+' ) || ( pcHex[0] == '-' ))
      tmp++;

    iLen = strlen( tmp );

    if(iLen == 0)
        return 0;

    /* Strip leading zeroes */
    while( *tmp == '0' ) { tmp++; iLen--; }

    if(iLen<8) 
      return 1;
    else
    	if(iLen==8)
	      { /* 0x7fffffff or smaller always legal */
		      if( tmp[0] < '8')
			      return 1;

          /* if not this then always illegal */
          if( strcmp( tmp,"80000000" ))
              return 0;

		      /* -0x80000000 only legal */
		      if( pcHex[0] == '-' )
			      return 1;
        }

    return 0;
}



/*********************************************************************

 FUNCTION     : OctToDec

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT32 

 PARAMETERS:
        char* pcOct

 DESCRIPTION:
        Converts octal value to integer
    
 NOTE: 

 CHANGES: 
    Sl/NWSS     23/10/1999
       Some simplifications ;)
*********************************************************************/
INT32 OctToDec(char* pcOct)
{
    INT32   iVal = 0;
    char    *tmp = pcOct + 1;
    int     i,
            j,
            iLen;
/*    INT32   piPow[]={1,8,64,512,4096,32768,262144,2097152,16777216,134217728,1073741824}; */

    if(( pcOct[0] == '+' ) || ( pcOct[0] == '-' ))
      tmp++;

    iLen = strlen( tmp );

    j=0;
/*    for(i=iLen-1;i>=0;i--) */
    for( i=0; i < iLen; i++ )
      iVal = ( iVal << 3 ) + ( tmp[i] - '0' );
/*    {
        iVal += (pcOct[i]-'0') * piPow[j++];
    } */
        
    if( pcOct[0] == '-' )
      iVal = -iVal;

    return iVal;
}


/*********************************************************************

 FUNCTION     : HexToDec

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT32 

 PARAMETERS:
        char* pcHex

 DESCRIPTION:
        Converts hex value to integer
    
 NOTE: 

 CHANGES: 
    Sl/NWSS     23/10/1999
       Some simplifications ;)
*********************************************************************/
INT32 HexToDec(char* pcHex)
{
    INT32  iVal = 0,iNum;
    char    *tmp = pcHex + 2;
    int     i,
/*            j, */
            iLen;
/*    INT32   piPow[]={1,16,256,4096,65536,1048576,16777216,268435456}; */

    if(( pcHex[0]=='+' ) || ( pcHex[0]=='-' ))
      tmp++;

    iLen = strlen( tmp );

/*    j=0; 
    for(i=iLen-1;i>=0;i--) */
    for( i = 0; i < iLen; i++ )
      {
        if( tmp[i] <= '9' ) /* number */
            iNum = tmp[i] - '0';
        else if( tmp[i] <= 'F' ) /* upper case */
            iNum = tmp[i] - 'A' + 10;
        else /* lower case */
            iNum = tmp[i] - 'a' + 10;

/*        iVal += iNum*piPow[j++]; */
        iVal = ( iVal << 4 ) + iNum;
      }

    if( pcHex[0] == '-' )
	  	iVal = -iVal;
        
    return iVal;
}



/*********************************************************************

 FUNCTION     : ConstTableAddValue

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct HashTable* pTable
        char* pcValue

 DESCRIPTION:
        Appends int or float value to contant table.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int ConstTableAddValue(struct HashTable* pTable, char* pcValue)
{
    int iRet = CONST_NOT_FOUND;
    struct HashNode* p;

    /* first check if any of the internally supported values */
    switch(pcValue[0])
    {
    case '1':
        if(pcValue[1]==0)
            iRet = CONST_TYPE_1;
        break;
    
    case '0':
        if(pcValue[1]==0)
            iRet = CONST_TYPE_0;
        break;
    
    case '-':
        if( (pcValue[1]=='1') && (pcValue[2]==0) )
            iRet = CONST_TYPE_M1;
        break;
    
    }

    if(iRet == CONST_NOT_FOUND)
    {
        /* VARTYPE_INT32 is used ONLY to differentiate from string */
        p = HashTableStore(pTable,pcValue,CONST_VALUE,STORE_MODE_MAY_EXIST);

        if(p)
            iRet = p->iIndex;
    
    }

    return iRet;
}


/*********************************************************************

 FUNCTION     : ConstTableAddString

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct HashTable* pTable
        char* pcValue
		BOOL bCheckInternal

 DESCRIPTION:
        Appends string to conatant table. If bCheckInternal == TRUE
		then checks first if string is internally supported.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int ConstTableAddString(struct HashTable* pTable, char* pcValue, BOOL bCheckInternal)
{
    int iRet = CONST_NOT_FOUND;
    struct HashNode* p;

    if(bCheckInternal)
	{
		/* first check if any of the internally supported values */
		switch(pcValue[0])
		{
		case 'f':
			if(strcmp(pcValue,"false")==0)
				iRet = CONST_TYPE_FALSE;
			break;

		case 'i':
			if(strcmp(pcValue,"invalid")==0)
				iRet = CONST_TYPE_INVALID;
			break;

		case 't':
			if(strcmp(pcValue,"true")==0)
				iRet = CONST_TYPE_TRUE;
			break;

		case 0: /* empty string */
			iRet = CONST_TYPE_ES;
			break;
		}
	}
    
    
    if(iRet == CONST_NOT_FOUND)
    {
        p = HashTableStore(pTable,pcValue,CONST_STRING,STORE_MODE_MAY_EXIST);
        
        if(p)
            iRet = p->iIndex;
    }

    return iRet;
}




/*********************************************************************

 FUNCTION     : ConstTableGenerateBytecode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct HashTable* pTable
        struct StorageStruct* pStorage

 DESCRIPTION:
        Build constant table to given pStorage.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int ConstTableGenerateBytecode(struct HashTable* pTable, struct StorageStruct* pStorage)
{
    int                 i;
    INT16               iNumItems;
    struct HashNode**   pConstants;
    

    /* Initialize ConstPool */
    if(ByteCodeInitialize(pStorage,500,100) == RET_OK)
    {
        /* add number of constants */
        ByteCodeAppendMBInt((unsigned int)pTable->iNumItems, pStorage);
        
        /* add character set identifier */
        ByteCodeAppendMBInt(IANA_MIBENUM_UTF_8, pStorage);
        
        if(pTable->iNumItems)
        {
            pConstants = GetItemsInOrder(pTable,&iNumItems);

            if(pConstants)
            {
                /* add constants one by one */
                for(i=0;i<iNumItems;i++)
                {
                    ConstPoolAppendConst(pStorage, pConstants[i]);
                }
            
                WMLS_free(pConstants);
            }
            else
            {
                return RET_ERROR;
            }
        }

        ByteCodeResize(pStorage);
    }


    return ByteCodeGetStatus(pStorage);
}



















/***************************************************************************
                            LOCAL FUNCTIONS
***************************************************************************/





/*********************************************************************

 FUNCTION     : ConstPoolAppendConst

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct StorageStruct* pStorage
        struct HashNode* pNode

 DESCRIPTION:
        Appends constant value to pStorage.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ConstPoolAppendConst(struct StorageStruct* pStorage, struct HashNode* pNode)
{
    /* VARTYPE_INT32 signifies that this is NOT a string constant */
    if(pNode->iExtraKey == CONST_VALUE)
    {
        if(  strchr(pNode->pcKey,'.') ||  strchr(pNode->pcKey,'e') ||  strchr(pNode->pcKey,'E') )
            ConstPoolAddFloat(pStorage, (float) atof(pNode->pcKey) );
        else
            ConstPoolAddInteger(pStorage, atoi(pNode->pcKey) );
    }
    else
    {
        ConstPoolAddString(pStorage, pNode->pcKey);
    }

}



/*********************************************************************

 FUNCTION     : ConstPoolAddInteger

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct StorageStruct* pStorage
        int iVal

 DESCRIPTION:
        Appends integer to pStorage
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ConstPoolAddInteger(struct StorageStruct* pStorage, int iVal)
{   
    if( (iVal >= -128) && (iVal < 128) )
	{
        ByteCodeAppendByte( (BYTE) VARTYPE_INT8, pStorage);
        ByteCodeAppendByte( (INT8) iVal, pStorage);		
	}
    else if( (iVal >= -32768) && (iVal < 32768) )
    {
        ByteCodeAppendByte( (BYTE) VARTYPE_INT16, pStorage);
        ByteCodeAppendWord( (INT16) iVal, pStorage);
    }
    else 
    {
        ByteCodeAppendByte( (BYTE) VARTYPE_INT32, pStorage);
        ByteCodeAppendLong( (INT32) iVal, pStorage);
    }
}


/*********************************************************************

 FUNCTION     : ConstPoolAddFloat

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct StorageStruct* pStorage
        float fVal

 DESCRIPTION:
        Appends float to pStorage

 NOTE: 

 CHANGES: 

*********************************************************************/
void ConstPoolAddFloat(struct StorageStruct* pStorage, float fVal)
{   
    ByteCodeAppendByte( (BYTE) VARTYPE_FLOAT, pStorage);
    ByteCodeAppendFloat( fVal, pStorage);
}





/*********************************************************************

 FUNCTION     : ConstPoolAddString

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct StorageStruct* pStorage
        char* pcVal

 DESCRIPTION:
        Appends string to pStorage

    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ConstPoolAddString(struct StorageStruct* pStorage, char* pcVal)
{   
    INT32 iLen;

    if(pcVal[0])
    {
        iLen = ConvertEscapeSequences(pcVal);

        ByteCodeAppendByte( (BYTE) VARTYPE_STRING, pStorage);
        ByteCodeAppendMBInt( iLen, pStorage);
        ByteCodeAppendString( pcVal, iLen, pStorage);
    }
    else
    {
        /* empty string */
        ByteCodeAppendByte( (BYTE) VARTYPE_STRING_EMPTY, pStorage);
    }
}



/*********************************************************************

 FUNCTION     : HexChar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        char cHex

 DESCRIPTION:
        Returns value 0-15 for hex digit
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int HexChar(char cHex)
{
    if(cHex<='9')
        return (cHex-'0');

    if(cHex<'G')
        return (cHex - 'A' + 10);

    return (cHex - 'a' + 10);
}




/*********************************************************************

 FUNCTION     : ConvertEscapeSequences

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        char* pcVal

 DESCRIPTION:
        Convert any escape sequences inside string to corresponding
        char. 
    
 NOTE: 

 CHANGES: It returns the length of the converted string, because it 
          can contain 0 characters.

*********************************************************************/
INT32 ConvertEscapeSequences(char* pcVal)
{
    char    *pcRead,
            *pcWrite;
	UINT16	uiValue;

    pcWrite = pcRead = pcVal;

    while(*pcRead)
    {

        if(*pcRead != '\\')
        {
            *pcWrite++ = *pcRead++; 
        }
        else
        {
            pcRead++;

            switch(*pcRead)
            {
            case '\'':
            case '\"':
            case '\\':
            case '/':
                *pcWrite++ = *pcRead++;
                break;

            case 'b': /* back space */
                *pcWrite++ = 8;
                pcRead++;
                break;

            case 'f': /* form feed */
                *pcWrite++ = 12;
                pcRead++;
                break;

            case 'n': /* line feed */
                *pcWrite++ = 10;
                pcRead++;
                break;

            case 'r': /* carriage return */
                *pcWrite++ = 13;
                pcRead++;
                break;

            case 't': /* horizontal tab */
                *pcWrite++ = 9;
                pcRead++;
                break;
            

            case 'x':   /* \xhh */
                pcRead++;
				uiValue = HexChar(pcRead[0]) * 16 + HexChar(pcRead[1]);
				pcRead +=2;
				
				pcWrite += AddUTF8(uiValue,(BYTE*)pcWrite);
                break;

            case 'u':   /* \uhhhh */
				pcRead++;
				uiValue  = (HexChar(pcRead[0]) * 4096);
				uiValue += (HexChar(pcRead[1]) * 256);
				uiValue += (HexChar(pcRead[2]) * 16);
				uiValue += (HexChar(pcRead[3]) );
                pcRead +=4;

				pcWrite += AddUTF8(uiValue,(BYTE*)pcWrite);
                break;

            case '0':   /* \ooo max 377 */
            case '1':   
            case '2':   
            case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				/* may be \7 or \77 or \377 */
                uiValue = (pcRead[0]-'0');
				pcRead++;

				if( (*pcRead >= '0') && (*pcRead <'8') )
				{
					uiValue *= 8;
					uiValue += (pcRead[0]-'0');
					pcRead++;	
					
					if( (*pcRead >= '0') && (*pcRead <'8') )
					{
						uiValue *= 8;
						uiValue += (pcRead[0]-'0');
						pcRead++;	
					}
				}

				pcWrite += AddUTF8(uiValue,(BYTE*)pcWrite);
                break;

            }
        }
    }

    *pcWrite = 0;
    return (INT32) (pcWrite - pcVal);
}




/*********************************************************************

 FUNCTION     : AddUTF8

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          4.8.1999.

 RETURN: int		number of bytes added 

 PARAMETERS:
        UINT16 uiValue
        BYTE* pbDest

 DESCRIPTION:
	Appends given value (0-65535) as UTF-8 string to given destination
	buffer.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
int AddUTF8(UINT16 uiValue, BYTE* pbDest)
{
	BYTE	b1,b2,b3;
	int		iBytes = 1;

	if(uiValue < 0x0080)	/* standard ASCII */
	{
		*pbDest = (BYTE)uiValue;
	}
	else if(uiValue < 0x0800)
	{
		/* 110xxxxx 10xxxxxx */
		
		b1 = (BYTE) (uiValue >> 6);
		b1 += 0xc0;	/* 110xxxxx */
		
		b2 = 0x3f & (BYTE) uiValue;
		b2 += 0x80;	/* 10xxxxxx */

		iBytes = 2;

		pbDest[0] = b1;
		pbDest[1] = b2;
		
	}
	else
	{
		/* 1110xxxx 10xxxxxx 10xxxxxx */
		b1 = (BYTE) (uiValue >> 12);
		b1 += 0xe0;	/* 1110xxxx */
				
		b2 = 0x3f & (BYTE) (uiValue >> 6);
		b2 += 0x80;	/* 10xxxxxx */
		
		b3 = 0x3f & (BYTE) uiValue;
		b3 += 0x80;	/* 10xxxxxx */

		iBytes = 3;
		
		pbDest[0] = b1;
		pbDest[1] = b2;
		pbDest[2] = b3;
	}

	return iBytes;
}


/*********************************************************************

 FUNCTION     : CheckZeroStr

**********************************************************************

 AUTHOR:        Gabor Valyi Nagy
 DATE:          29-May-2000

 RETURN: int	1 if it is a string which contains only '0'
				0 otherwise

 PARAMETERS:
		char *pcStart '\0' ended string

 DESCRIPTION:
	Returns 1 if it is a string which contains only '0',
	0 otherwise.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
int CheckZeroStr(char *pcStart)
{
	int status	= 1;
	int len		= strlen(pcStart);
	int i;
	
	for(i = 0; i < len; i++)
	{
		if (pcStart[i] != '0')
		{
			status = 0;
			break;
		}
	}

	return status;
}

/* end of file */



