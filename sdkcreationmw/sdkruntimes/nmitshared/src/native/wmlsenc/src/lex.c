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
/* File    : U:\wpsvob\wmlscript\work\ported\lex.c
   Version : \main\11
   Date    : 28-Feb-01.13:04:08
*/

/*
 * ========================================================================
 *  Name        : lex.c
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

      Reads characters from inputBuffer and passes tokens or error code to
      Bison parser.

-----------------------------------------------------------------------------
*/
#include "main.h"
#include "common.h"
#include "wmlsyacc.h"
#include "wmlsapi.h"
#include "lex.h"

#define CHAR_IDENTIFIER         0x01
#define CHAR_IDENTIFIER_FIRST   0x02
#define CHAR_INTEGER            0x04
#define CHAR_HEX                0x08
#define CHAR_OCT                0x10

#define TAB_LEN 2

/* static table for each char identifying its properties (see above */
static BYTE pbChar[257];



/* 

int main(int argc, char* argv[])
{
    char    pcText[]="var text for 567";

    struct LexStruct env;

    env.iDataLen = strlen(pcText);
    
    env.pbData = (BYTE*)malloc(strlen(pcText)+1);

    strcpy((char*)(env.pbData),pcText);

    env.iDataIndex = 0;
    env.iRow = 1;

    InitializeTables();

    for(int i=0;i<1000;i++)
    {
        env.iDataIndex = 0;
        env.iToken = 0;
        while(env.iToken != TOKEN_END)
        {
            ParseNext(&env);
        }
    }

    return 0;
}
*/

void ParseNumber(struct LexStruct* pLex);

#if 0

/*********************************************************************

 FUNCTION     : LexInitFile

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex
        char* pcFilename

 DESCRIPTION:
        Opens given file and copies it to memory buffer. 
        LexStruct.pbData is updated to point to this buffer.
        Initializes other LexStruct values.
    
 NOTE: 
        Currently 10000 bytes fixed size buffer

 CHANGES: 

*********************************************************************/
void LexInitFile(struct LexStruct* pLex, char* pcFilename)
{
    FILE *input;

    input = fopen( pcFilename, "r");

    if(input)
    {
        pLex->pbData = (BYTE*) WMLS_malloc(10000);
        pLex->iDataLen = fread((void*)pLex->pbData,1,10000,input);
        pLex->pbData[pLex->iDataLen] = 0;

        pLex->iRow = 1;
        pLex->iDataIndex = 0;
        pLex->pcTokenText = NULL;

        fclose(input);

        InitializeTables();
    }
}

#endif

/*********************************************************************

 FUNCTION     : LexInit

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
                

 DESCRIPTION:   Calls InitializeTables function.
        
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void LexInit()
{
    InitializeTables();
}





/*********************************************************************

 FUNCTION     : LexCleanUp

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Frees the memory block allocated for the script
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void LexCleanUp(struct LexStruct* pLex)
{
    if(pLex->pbData)
    {
        WMLS_free(pLex->pbData);
        pLex->pbData = NULL;
    }
}




/*********************************************************************

 FUNCTION     : InitializeTables

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:

 DESCRIPTION:
        Fills in global static table containing flag values for each
        char. Possible values are:

        #define CHAR_IDENTIFIER         0x01
        #define CHAR_IDENTIFIER_FIRST   0x02
        #define CHAR_INTEGER            0x04
        #define CHAR_HEX                0x08
        #define CHAR_OCT                0x10

    
 NOTE: 
        Call only once per GateWay lifetime

 CHANGES: 

*********************************************************************/
void InitializeTables()
{
    int     i;

    for(i=0;i<=256;i++)
        pbChar[i]=0;
    
    /* numbers */
    for(i='0';i<='9';i++)
    {
        pbChar[i] |= CHAR_IDENTIFIER;
        pbChar[i] |= CHAR_INTEGER;
        pbChar[i] |= CHAR_HEX;
        
        if(i<'8')
            pbChar[i] |= CHAR_OCT;
    }
        

    /* letters */
    for(i='A';i<='Z';i++)
    {
        pbChar[i] |= CHAR_IDENTIFIER;
        pbChar[i+32] |= CHAR_IDENTIFIER;

        pbChar[i] |= CHAR_IDENTIFIER_FIRST;
        pbChar[i+32] |= CHAR_IDENTIFIER_FIRST;

        

        if(i<'G')
        {
            pbChar[i] |= CHAR_HEX;
            pbChar[i+32] |= CHAR_HEX;
        }
    }

    /* '_' */
    pbChar['_'] |= CHAR_IDENTIFIER;
    pbChar['_'] |= CHAR_IDENTIFIER_FIRST;

}



/*********************************************************************

 FUNCTION     : NextChar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: BYTE           - next char in buffer

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Returns next char in buffer. If it is '\n' updates the
        row and column counters in LexStruct. 
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE NextChar(struct LexStruct* pLex)
{
    BYTE b1 = 0;
	
	 /* end of file */
    if(pLex->iDataIndex < (pLex->iDataLen) )  
    {
        b1 = pLex->pbData[pLex->iDataIndex++];

		/* update column */
		if(b1=='\t')
			pLex->iColumn += TAB_LEN;
		else
			pLex->iColumn++;


		if(b1=='\n')
		{
			(pLex->iRow)++;

			pLex->iColumn = 0;
		}
	}


    return b1;
}



/*********************************************************************

 FUNCTION     : CurrentChar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: BYTE 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Returns the current character. Index is not changed.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE CurrentChar(struct LexStruct* pLex)
{
	return pLex->pbData[pLex->iDataIndex-1];
}



/*********************************************************************

 FUNCTION     : PeepChar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: BYTE 

 PARAMETERS:
        struct LexStruct* pLex
        int iNext                   - in: how many chars from current

 DESCRIPTION:
        Returns iNext character from current. Index counter is not
        changed.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE PeepChar(struct LexStruct* pLex, int iNext)
{	
	BYTE b1 = 0;

 	/* end of file */
    if( (pLex->iDataIndex+iNext-1) < (pLex->iDataLen) )  
    {
		b1 = pLex->pbData[pLex->iDataIndex+iNext-1];
	}

	return b1;

}




/*********************************************************************

 FUNCTION     : SkipChar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex
        int iNumToSkip          - in: how many chars to skip

 DESCRIPTION:
        Skips over characters in buffer. Index counter is updated. 
    
 NOTE: 
        Do not skip over Linefeeds -> Row count goes invalid

 CHANGES: 

*********************************************************************/
void SkipChar(struct LexStruct* pLex,int iNumToSkip)
{
    pLex->iDataIndex += iNumToSkip;

    pLex->iColumn += iNumToSkip;
}





/*********************************************************************

 FUNCTION     : GetIdentifier

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex
        BYTE bFlag                  - in: flag that identifies chars

 DESCRIPTION:
        Points LexStruct.pcCurrentText to the start of identifier.
        Gets chars as long as their bFlag bit is set. Updates 
        LexStruct.iCurrentTextLen to correct value.
        Checks that the next char is valid token.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void GetIdentifier(struct LexStruct* pLex, BYTE bFlag)
{
    int     iLen = 0;
    BYTE    b1 = CurrentChar(pLex);

    if(bFlag == CHAR_IDENTIFIER)
    {
        /* if identifier is div or div= we must return the previous identifier */
        pLex->pcTokenTextPrev = pLex->pcTokenText;
        pLex->iTokenTextLenPrev = pLex->iTokenTextLen;
        
        /* var a = true div 4 needs reserved store too TIR 280599 */
        pLex->iReservedPrev = pLex->iReserved;
    }
    
    if(pbChar[b1] & bFlag)
    {
        iLen++;

        pLex->pcTokenText = (char*)(pLex->pbData + pLex->iDataIndex-1);

        while(pbChar[PeepChar(pLex,1)] & bFlag)
        {
            iLen++;
            
            b1=NextChar(pLex);

			if(b1==0)
			{
				pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_IDENTIFIER;
				break;
			}
        }

        pLex->iTokenTextLen = iLen;
    }
    else
        pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_IDENTIFIER;


}



/*********************************************************************

 FUNCTION     : GetString

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Gets characters from stream until ending quote is reached.
        Checks that no illegal characters inside string.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void GetString(struct LexStruct* pLex)
{
    int     i,
            iLen = 2; /* one for both end quotes */
    BYTE    b1, bQuote;
	char	cFirstOcta;
	int		iLenOcta;
    
    pLex->pcTokenText = (char*)(pLex->pbData + pLex->iDataIndex-1);

    bQuote = CurrentChar(pLex);

    /* step over starting quote */
    b1 = NextChar(pLex);

    while(b1!=bQuote) 
    {
  
        /* end of line */
        if( (b1=='\r') || (b1=='\n') || (b1==0) )
        {
            pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_STRING;
            break;
        }


        /* check for illegal command chars: */
        if(b1=='\\')
        {
            iLen++;
            b1=NextChar(pLex);

            switch(b1)
            {
            case '\'':
            case '\"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                break;

            case 'x':   /* \xhh */
                for(i=0;i<2;i++)
                {
                    b1=NextChar(pLex);
                    iLen++;
                    if(!(pbChar[b1] & CHAR_HEX))
                    {
                        pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_ESCAPE;
                        break;
                    }
                }
                break;

            case 'u':   /* \uhhhh */
                for(i=0;i<4;i++)
                {
                    b1=NextChar(pLex);
                    iLen++;
                    if(!(pbChar[b1] & CHAR_HEX))
                    {
                        pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_ESCAPE;
                        break;
                    }
                }
                break;

            case '0':   /* \o , \oo , \ooo  max 377 */
            case '1':   
            case '2':   
            case '3':   
            case '4':   
            case '5':   
            case '6':   
            case '7':   
                cFirstOcta = b1;
				iLenOcta = 1;
				
				if(pbChar[PeepChar(pLex,1)] & CHAR_OCT)
				{
					iLenOcta++;
					NextChar(pLex);
					
					if(pbChar[PeepChar(pLex,1)] & CHAR_OCT)
					{
						iLenOcta++;
						NextChar(pLex);
					}
				}

				/* now validity check, if octal length is 3 then first digit must
				   be 0,1,2 or 3 */
				if(iLenOcta == 3)
				{
					if(cFirstOcta>'3')
						pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_ESCAPE;
				}

				iLen += (iLenOcta-1); 

                break;

            case '8':   
            case '9':   
                pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_ESCAPE;
                break;

            default:
                pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_ESCAPE;
                break;
            }
        }

        iLen++;
        b1=NextChar(pLex);

        if(pLex->iStatus)
            break;
    }

    pLex->iTokenTextLen = iLen;

}




/*********************************************************************

 FUNCTION     : CommentLine

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Removes // comment from stream
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void CommentLine(struct LexStruct* pLex)
{
    BYTE b1 = CurrentChar(pLex);
    
    while(1)
    {
        /* end of buffer */
        if(b1==0) 
            break;

        if( (b1=='\n') || (b1=='\r') )
        {
            /* end of comment */

            /*eliminate other part of line break if present */
            b1 = PeepChar(pLex,1);
            
            if( (b1=='\r') || (b1=='\n') )
            {
                NextChar(pLex);
            }
                
            break;
        }
        
        b1 = NextChar(pLex);
    }
}


/*********************************************************************

 FUNCTION     : CommentBlock

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Removes comment from stream
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void CommentBlock(struct LexStruct* pLex)
{
    BYTE b1 = CurrentChar(pLex);
    
    while(1)
    {
        /* end of buffer */
        if(b1==0) 
            break;

        if(b1=='*') 
        {
            if(PeepChar(pLex,1)=='/')
            {
                b1 = NextChar(pLex);
                /* end of comment */
                break;
            }
        }

        b1 = NextChar(pLex);
    }


}




/*********************************************************************

 FUNCTION     : GetNumber

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.04.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:   Recognises the type of the number literal.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void GetNumber(struct LexStruct* pLex)
{
    BYTE b1;
    int iLen = 0, iDot = 0;
    
    pLex->iToken = DEC_LITERAL;

    pLex->pcTokenText = (char*)(pLex->pbData + pLex->iDataIndex-1);

    b1 = CurrentChar(pLex);

    while(pbChar[b1] & CHAR_INTEGER)
    {
        iLen++;
        b1=NextChar(pLex);
    }

    if(b1 == '.' )
    {
        iDot = 1;

        pLex->iToken = FLT_LITERAL;

        b1 = NextChar(pLex);
        iLen++;

        while(pbChar[b1] & CHAR_INTEGER)
        {
            iLen++;
            b1=NextChar(pLex);
        }
    }

    if(b1 == 'e' || b1 == 'E')
    {
        pLex->iToken = FLT_LITERAL;

        /* catch illegal float ".e " */
        if(iLen == 1 && iDot == 1)
            pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_LITERAL;
        else
        {
            iLen++;
            b1 = NextChar(pLex);

            if(b1 == '+' || b1 == '-')
            {
                iLen++;
                b1 = NextChar(pLex);
            }

            if(pbChar[b1] & CHAR_INTEGER)
            {
                while(pbChar[b1] & CHAR_INTEGER)
                {
                    iLen++;
                    b1=NextChar(pLex);
                }
            }
            else
                pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_LITERAL;

        }
    }

    pLex->iDataIndex--;
    pLex->iTokenTextLen = iLen;
}



/*********************************************************************

 FUNCTION     : ParseSignedNumber

**********************************************************************

 AUTHOR:        Jouni Sipola / CCC Companies
 DATE:          04.05.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION: 
        Parser has encountered a sign - or +. This may part of a
        signed number (int or float) or part of binary operation.
        Find out which it is. If Part of the binary operation
        just return, otherwise parse signed number.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ParseSignedNumber(struct LexStruct* pLex)
{
    BYTE b2 = PeepChar(pLex, 1);
	BYTE sign = PeepChar(pLex, 0);
	INT32	nSkippedChars = 0;

	/* Set the sign to space */
	pLex->pbData[pLex->iDataIndex-1] = ' ';

	/* skip white chars */
    while( (b2==' ') || (b2=='\t') || (b2=='\r') || (b2=='\n') || (b2=='\v') || (b2=='\f'))
    {
		nSkippedChars++;
        b2 = PeepChar(pLex, nSkippedChars + 1 );
    }


    /* next char must be a integer or dot to this be a number */
    if((pbChar[b2] & CHAR_INTEGER) || b2 == '.')
    {

		SkipChar(pLex, nSkippedChars);

        /* if previous token is literal or identifier or function call func()
            then this is part of a binary operation */
        if( pLex->iPrevToken != HEX_LITERAL &&
        pLex->iPrevToken != OCT_LITERAL &&
        pLex->iPrevToken != DEC_LITERAL &&
        pLex->iPrevToken != FLT_LITERAL &&
        pLex->iPrevToken != RPAREN &&           /* TIR 280599 */
        pLex->iPrevToken != IDENTIFIER &&
        pLex->iPrevToken != DBLQ_LITERAL &&
        pLex->iPrevToken != INCR &&
        pLex->iPrevToken != DECR)
        {
            /* signed number */
            SkipChar(pLex, 1);
            ParseNumber(pLex);
    
            /* add + or - to the beginning of number */
            pLex->pcTokenText--;
            pLex->iTokenTextLen++;
			pLex->pcTokenText[0] = sign;
        }
		else
		{	/* if the previous token was literal or identifier or ... */
			pLex->pbData[pLex->iDataIndex-1] = sign;		
		}
    }
	else
	{	/* if number isn't found */
		pLex->pbData[pLex->iDataIndex-1] = sign;
	}
}


/*********************************************************************

 FUNCTION     : IsKeyWord

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void 

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Checks if given string identifier is keyword. If found then
        LexStruct.iToken is updated according the key word.
    
 NOTE: 

 CHANGES: 
		public keyword is now future reserved word	JOS / CCC   040799

*********************************************************************/
void IsKeyWord(struct LexStruct* pLex)
{
    char*   pcIdentifier = (char*) pLex->pcTokenText;
    BYTE    bNext = PeepChar(pLex,1);
    BYTE    bEnd = pLex->pcTokenText[pLex->iTokenTextLen];
    INT16   iEnd = pLex->iTokenTextLen;

    /* change char after token temporarily to null */
    pcIdentifier[iEnd] = 0;

    pLex->iReserved = RESERVED_NONE;

    switch(pcIdentifier[0])
    {
    case 'a':
        if(strcmp(pcIdentifier,"access")==0)
            pLex->iToken = ACCESS;
        else if(strcmp(pcIdentifier,"agent")==0)
            pLex->iToken = AGENT;
        break;

    case 'b':
        if(strcmp(pcIdentifier,"break")==0)
            pLex->iToken = BREAK;
        break;

    case 'c':
        if(strcmp(pcIdentifier,"continue")==0)
            pLex->iToken = CONTINUE;
        else if(strcmp(pcIdentifier,"case")==0)
        {
            pLex->iToken = CASE;
            pLex->iReserved = RESERVED_FUTURE;
        }
        else if(strcmp(pcIdentifier,"catch")==0)
        {
            pLex->iToken = CATCH;
            pLex->iReserved = RESERVED_FUTURE;
        }
        else if(strcmp(pcIdentifier,"class")==0)
        {
            pLex->iToken = CLASS;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"const")==0)
        {
            pLex->iToken = CONST;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        break;

    case 'd':
        if(strcmp(pcIdentifier,"div")==0)
        {
            if(bNext=='=')
            {
                pLex->iToken = DIVASSIGN;
                SkipChar(pLex,1);
            }
            else
            {
                pLex->iToken = DIV;
            }

            /* return the previous identifier */
            pLex->pcTokenText = pLex->pcTokenTextPrev;
            pLex->iTokenTextLen = pLex->iTokenTextLenPrev;
            pLex->iReserved = pLex->iReservedPrev;
        }
        else if(strcmp(pcIdentifier,"domain")==0)
            pLex->iToken = DOMAIN;
        else if(strcmp(pcIdentifier,"debugger")==0)
        {
            pLex->iToken = DEBUGGER;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"default")==0)
        {
            pLex->iToken = DEFAULT;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"do")==0)
        {
            pLex->iToken = DO;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"delete")==0)
        {
            pLex->iToken = DELETE;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;
    
    
    case 'e':
        if(strcmp(pcIdentifier,"else")==0)
            pLex->iToken = ELSE;
        else if(strcmp(pcIdentifier,"extern")==0)
            pLex->iToken = EXTERN;
        else if(strcmp(pcIdentifier,"equiv")==0)
            pLex->iToken = EQUIV;
        else if(strcmp(pcIdentifier,"enum")==0)
        {
            pLex->iToken = ENUM;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"export")==0)
        {
            pLex->iToken = EXPORT;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"extends")==0)
        {
            pLex->iToken = EXTENDS;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;

    
    case 'f':
        if(strcmp(pcIdentifier,"for")==0)
            pLex->iToken = FOR;
        else if(strcmp(pcIdentifier,"false")==0)
            pLex->iReserved = RESERVED_FALSE;
        else if(strcmp(pcIdentifier,"function")==0)
            pLex->iToken = FUNCTION;
        else if(strcmp(pcIdentifier,"finally")==0)
        {
            pLex->iToken = FINALLY;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 'h':
        if(strcmp(pcIdentifier,"header")==0)
            pLex->iToken = HEADER;
        else if(strcmp(pcIdentifier,"http")==0)
            pLex->iToken = HTTP;

        break;


    case 'i':
        if(strcmp(pcIdentifier,"if")==0)
            pLex->iToken = IF;
        else if(strcmp(pcIdentifier,"isvalid")==0)
            pLex->iToken = ISVALID;
        else if(strcmp(pcIdentifier,"invalid")==0)
            pLex->iReserved = RESERVED_INVALID;
        else if(strcmp(pcIdentifier,"import")==0)
        {
            pLex->iToken = IMPORT;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"in")==0)
        {
            pLex->iToken = IN;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 'l':
        if(strcmp(pcIdentifier,"lib")==0)
        {
            pLex->iToken = LIB;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;

    case 'm':
        if(strcmp(pcIdentifier,"meta")==0)
            pLex->iToken = META;

        break;


    case 'n':
        if(strcmp(pcIdentifier,"name")==0)
            pLex->iToken = NAME;
        else if(strcmp(pcIdentifier,"new")==0)
        {
            pLex->iToken = NEW;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"null")==0)
        {
            pLex->iToken = TOK_NULL;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 'p':
        if(strcmp(pcIdentifier,"path")==0)
            pLex->iToken = PATH;
        else if(strcmp(pcIdentifier,"public")==0)
        {
            pLex->iToken = PUBLIC;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"private")==0)
        {
            pLex->iToken = PRIVATE;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 'r':
        if(strcmp(pcIdentifier,"return")==0)
            pLex->iToken = RETURN;

        break;


    case 's':
        if(strcmp(pcIdentifier,"sizeof")==0)
        {
            pLex->iToken = SIZEOF;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"struct")==0)
        {
            pLex->iToken = STRUCT;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"super")==0)
        {
            pLex->iToken = SUPER;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"switch")==0)
        {
            pLex->iToken = SWITCH;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 't':
        if(strcmp(pcIdentifier,"typeof")==0)
            pLex->iToken = TYPEOF;
        else if(strcmp(pcIdentifier,"true")==0)
            pLex->iReserved = RESERVED_TRUE;
        else if(strcmp(pcIdentifier,"throw")==0)
        {
            pLex->iToken = THROW;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"try")==0)
        {
            pLex->iToken = TRY;
            pLex->iReserved = RESERVED_FUTURE;
        }   
        else if(strcmp(pcIdentifier,"this")==0)
        {
            pLex->iToken = THIS;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 'u':
        if(strcmp(pcIdentifier,"use")==0)
            pLex->iToken = USE;
        else if(strcmp(pcIdentifier,"url")==0)
            pLex->iToken = URL;
        else if(strcmp(pcIdentifier,"user")==0)
            pLex->iToken = USER;

        break;


    case 'v':
        if(strcmp(pcIdentifier,"var")==0)
            pLex->iToken = VAR;
        else if(strcmp(pcIdentifier,"void")==0)
        {
            pLex->iToken = VOID;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    case 'w':
        if(strcmp(pcIdentifier,"while")==0)
            pLex->iToken = WHILE;
        else if(strcmp(pcIdentifier,"with")==0)
        {
            pLex->iToken = WITH;
            pLex->iReserved = RESERVED_FUTURE;
        }   

        break;


    }

    /* return the char after token temporarily changed null */
    pcIdentifier[iEnd] = bEnd;

    return;
}







/*********************************************************************

 FUNCTION     : yylex

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: int        TOKEN

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Gets next token from script. If identifier or literal then
        LexStruct.pcCurrentText and LexStruct.iCurrentTextLen are
        updated.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int yylex(struct LexStruct* pLex)
{
    
    BYTE    b1,b2,b3,b4;

    /* initialize error to no error */
    pLex->iStatus = WMLS_STATUS_OK;

    if(pLex->iDataIndex >= pLex->iDataLen)
    {
        if(pLex->iDataLen == 0)
            pLex->iStatus = WMLS_STATUS_ERROR_INPUT_BUFFER_EMPTY;
        
        return TOKEN_END;
    }
    
whitespace:
    b1=NextChar(pLex);

    /* skip white char */
    while( (b1==' ') || (b1=='\t') || (b1=='\r') || (b1=='\n') || (b1=='\v') || (b1=='\f'))
    {
        b1=NextChar(pLex);
    }

    b2=PeepChar(pLex,1);

    /* skip comments */
    if(b1=='/')
    {
        if(b2=='/')
        {
            SkipChar(pLex,1);
            CommentLine(pLex);
            goto whitespace;
        }
        else if(b2=='*')
        {
            SkipChar(pLex,1);
            CommentBlock(pLex);
            goto whitespace;
        }
    }

    /* save column here +1 to change column 0 to column 1*/
    pLex->iTokenColumn = pLex->iColumn + 1;


    if(pbChar[b1] & CHAR_IDENTIFIER_FIRST)
    {
        GetIdentifier(pLex, CHAR_IDENTIFIER);
        pLex->iToken = IDENTIFIER;

        IsKeyWord(pLex);
    }
    else
    {
        switch(b1)
        {

        case 0:
            pLex->iToken = TOKEN_END;
            break;
            

        case '\"':
            /* "xxxx" */
            GetString(pLex);
            pLex->iToken = DBLQ_LITERAL;
            break;
            
        case '\'':
            /* 'xxx' */
            GetString(pLex);
            pLex->iToken = SQ_LITERAL;
                        
            break;
            

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            ParseNumber( pLex );
            break;

        case '=':
            if(b2=='=')
            {
                pLex->iToken = EQ;  
                SkipChar(pLex,1);
            }
            else
                pLex->iToken = ASSIGN;

            break;


        case '+':
            switch(b2)
            {
            case '+':
                pLex->iToken = INCR;
                SkipChar(pLex,1);
                break;
            case '=':
                pLex->iToken = PLUSASSIGN;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = PLUS;
                ParseSignedNumber(pLex);
            }
            break;

        case '-':
            switch(b2)
            {
            case '-':
                pLex->iToken = DECR;
                SkipChar(pLex,1);
                break;
            case '=':
                pLex->iToken = MINUSASSIGN;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = MINUS;
                ParseSignedNumber(pLex);
            }
            break;

        case '*':
            switch(b2)
            {
            case '=':
                pLex->iToken = STARASSIGN;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = STAR;
            }
            break;

        case '/':
            switch(b2)
            {
            case '=':
                pLex->iToken = SLASHASSIGN;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = SLASH;
            }
            break;

        case '%':
            switch(b2)
            {
            case '=':
                pLex->iToken = REMASSIGN;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = REM;
            }
            break;


        case '<':
            switch(b2)
            {
            case '=': /* <= */
                pLex->iToken = LE;
                SkipChar(pLex,1);
                break;
            case '<':
                b3=PeepChar(pLex,2);
                if(b3=='=') /* <<= */
                {
                    pLex->iToken = B_LEFTSHIFTASSIGN;
                    SkipChar(pLex,2);
                }
                else /* << */
                {
                    pLex->iToken = B_LSHIFT;
                    SkipChar(pLex,1);
                }

                break;
            default: /* < */
                pLex->iToken = LT;
            }
            break;


        case '>':
            switch(b2)
            {
            case '=':
                pLex->iToken = GE;
                SkipChar(pLex,1);
                break;
            case '>':
                b3=PeepChar(pLex,2);

                if(b3=='=') /* >>= */
                {
                    pLex->iToken = B_RSIGNEDSHIFTASSIGN;
                    SkipChar(pLex,2);
                }
                else if(b3=='>')
                {
                    b4=PeepChar(pLex,3);
                    if(b4=='=') /* >>>= */
                    {
                        pLex->iToken = B_RZSHIFTASSIGN; 
                        SkipChar(pLex,3);
                    }
                    else /* >>> */
                    {
                        pLex->iToken = B_RSZSHIFT;
                        SkipChar(pLex,2);
                    }
                }
                else /* >> */
                {
                    pLex->iToken = B_RSIGNEDSHIFT;
                    SkipChar(pLex,1);
                }

                break;
            default:
                pLex->iToken = GT;
            }
            break;

        case '&':
            switch(b2)
            {
            case '=':
                pLex->iToken = ANDASSIGN;
                SkipChar(pLex,1);
                break;
            case '&':
                pLex->iToken = SC_AND;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = BIT_AND;
            }
            break;

            
        case '|':
            switch(b2)
            {
            case '=':
                pLex->iToken = ORASSIGN;
                SkipChar(pLex,1);
                break;
            case '|':
                pLex->iToken = SC_OR;
                SkipChar(pLex,1);
                break;
            default:
                pLex->iToken = BIT_OR;
            }
            break;

        
        case '^':
            if(b2=='=')
            {
                pLex->iToken = XORASSIGN;
                SkipChar(pLex,1);
            }
            else
            {
                pLex->iToken = XOR;
            }
            break;

        case '!':
            if(b2=='=')
            {
                pLex->iToken = NE;
                SkipChar(pLex,1);
            }
            else
            {
                pLex->iToken = BANG;
            }
            break;

        case ',':
            pLex->iToken = COMMA;
            break;

        case ';':
            pLex->iToken = SEMICOLON;
            break;
        
        case ':':
            pLex->iToken = COLON;
            break;

        case '~':
            pLex->iToken = TILDE;
            break;

        case '#':
            pLex->iToken = HASH;
            break;

        case '?':
            pLex->iToken = HOOK;
            break;

        case '{':
            pLex->iToken = LBRACE;
            break;

        case '}':
            pLex->iToken = RBRACE;
            break;

        case '(':
            pLex->iToken = LPAREN;
            break;

        case ')':
            pLex->iToken = RPAREN;
            break;

        case '[':
            pLex->iToken = LBRACKET;
            break;

        case ']':
            pLex->iToken = RBRACKET;
            break;

        default:
            pLex->iStatus = WMLS_STATUS_ERROR_ILLEGAL_CHAR;
            pLex->pcTokenText = (char*)(pLex->pbData + pLex->iDataIndex-1);
            pLex->iTokenTextLen = 1;
        }

    }

    pLex->iPrevToken = pLex->iToken;

    if(pLex->iReserved == RESERVED_FUTURE)
        pLex->iStatus = WMLS_STATUS_ERROR_RESERVED_KEYWORD;
    
    
    if(pLex->iStatus == WMLS_STATUS_OK)
        return pLex->iToken;
    else
        return TOKEN_END;

}

/*********************************************************************

 FUNCTION     : ParseNumber

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          30.4.1999.

 RETURN: void        

 PARAMETERS:
        struct LexStruct* pLex

 DESCRIPTION:
        Parses all legal number formats from stream and updates
		LexStruct variables.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void ParseNumber(struct LexStruct* pLex)
{

    BYTE    b1,b2;

    b1 = CurrentChar(pLex);
    b2 = PeepChar(pLex,1);

    switch(b1)
	    {
	      case '0':
          /* hex */
          if( (b2=='x') || (b2=='X') )
            { SkipChar(pLex,2);
              GetIdentifier(pLex, CHAR_HEX);
            
              /* add 0x to the beginning of text */
              pLex->pcTokenText -= 2;
              pLex->iTokenTextLen += 2;

              pLex->iToken = HEX_LITERAL;

              break;
            }
		        /* oct 07676 */
          else if( pbChar[b2] & CHAR_INTEGER ) /* CHAR_INTEGER to detect wrong OCT literals later! */
            {	SkipChar(pLex,1);
              GetIdentifier( pLex, CHAR_INTEGER );
              pLex->iToken = OCT_LITERAL;
        
              /* add \ to the beginning of text */
              pLex->pcTokenText--;
              pLex->iTokenTextLen++;
              break;
            }

          /* no break */

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          /* decimal or float */
          GetNumber(pLex);
          break;
        
        case '.':
          /* if float starts with . then next char MUST be a number */
          if(pbChar[b2] & CHAR_INTEGER)
            GetNumber( pLex );
          else
            pLex->iToken = DOT;
  }
}
