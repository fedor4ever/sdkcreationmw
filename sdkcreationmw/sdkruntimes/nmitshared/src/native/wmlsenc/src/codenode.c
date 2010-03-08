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
/* File    : U:\wpsvob\wmlscript\work\ported\codenode.c
   Version : \main\19
   Date    : 22-Nov-00.17:24:53
*/

/*
 * ========================================================================
 *  Name        : codenode.c
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

      Creates, initialises, links etc. tree nodes according the Bison parser.

-----------------------------------------------------------------------------
*/

#include "wps_service.h"

#include "main.h"
#include "common.h"
#include "codenode.h"
#include "memmana.h"
#include "symtable.h"
#include "opcodes.h"
#include "library.h"
#include "funcpool.h"
#include "conspool.h"
#include "wmlsyacc.h"
#include "pragpool.h"

#include <assert.h>
#include <limits.h>

/* local proto types */
struct CodeNode* GetIfNode(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pTrue,  struct CodeNode* pElse);
struct CodeNode* GetBlockNode(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pRight);
struct CodeNode* GetLoopNode(struct Environment* pEnv, struct CodeNode* pInit, struct CodeNode* pExpression, struct CodeNode* pBody,  struct CodeNode* pPostStatement);












/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/




/*********************************************************************

 FUNCTION     : GetEmptyNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv

 DESCRIPTION:
        Create and initialize new node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetEmptyNode(struct Environment* pEnv)
{

    struct CodeNode* pNode = (struct CodeNode*) MemoryAlloc(pEnv->pMemTemporary,sizeof(struct CodeNode));

    if(pNode)
    {
        pNode->bAlive = NODE_ALIVE;
        pNode->bOpCode = 0;
        pNode->pLeft = NULL;
        pNode->pRight = NULL;
        pNode->pParent = NULL;
        pNode->bNodeType = NODE_TYPE_EMPTY;
        pNode->bStatus = 0;
        pNode->iSize = 0;
        pNode->iIndex = -1;
        pNode->iLine = pEnv->lex.iRow;
    }
    else
    {
        pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
    }

    return pNode;
}


/*********************************************************************

 FUNCTION     : KillNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode

 DESCRIPTION:
        Sets nodes bAlive byte to zero
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void KillNode(struct CodeNode* pNode)
{
    pNode->bAlive = 0;
}




/*********************************************************************

 FUNCTION     : LinkLeft

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pParent
        struct CodeNode* pNode

 DESCRIPTION:
        Links given tode to parent and vice versa
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void LinkLeft(struct CodeNode* pParent, struct CodeNode* pNode)
{
    pParent->pLeft = pNode;
    
    if(pNode)
    {
        pNode->pParent = pParent;
        pParent->iSize += pNode->iSize;
    }
}




/*********************************************************************

 FUNCTION     : LinkRight

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pParent
        struct CodeNode* pNode

 DESCRIPTION:
        Links given tode to parent and vice versa   

 NOTE: 

 CHANGES: 

*********************************************************************/
void LinkRight(struct CodeNode* pParent, struct CodeNode* pNode)
{
    pParent->pRight = pNode;
    
    if(pNode)
    {
        pNode->pParent = pParent;
        pParent->iSize += pNode->iSize;
    }
}



/*********************************************************************

 FUNCTION     : CreateAndLink

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pLeft
        struct CodeNode* pRight
        NodeType type

 DESCRIPTION:
        Creates new node and links given nodes to it.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* CreateAndLink(struct Environment* pEnv, struct CodeNode* pLeft, struct CodeNode* pRight, NodeType type)
{
    struct CodeNode* p = GetEmptyNode(pEnv);

    if(p)
    {
        p->iSize = 0;
        p->bNodeType = type;

        LinkLeft(p,pLeft);
        LinkRight(p,pRight);
    }

    return p;
}




/*********************************************************************

 FUNCTION     : GetIdFunction

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcName

 DESCRIPTION:
        Returns the function index for given function.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetIdFunction(struct Environment* pEnv, char* pcName)
{
    struct CodeNode* pNode = GetEmptyNode(pEnv);
    struct HashNode* pFunction;

	if ( strlen (pcName) > FUNCTION_NAME_MAXLENGTH )
	{	/* if the length is more than 255, the remainder should be ignored */
        pcName[FUNCTION_NAME_MAXLENGTH] = '\0';
	}
    
    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_ID;

        /* add variable Index to operator index field */
        pFunction = HashTableFind(&(pEnv->Functions),pcName,0);

        if(pFunction)
        {
            pNode->iIndex = pFunction->iIndex;
        }
        else
        {
            /* not declared function */
            pFunction = HashTableStore(&(pEnv->Functions),pcName,0,STORE_MODE_MAY_NOT_EXIST);
            
            if(pFunction)
            {
                pNode->iIndex = pFunction->iIndex;
                pFunction->bNumArguments = 0;
                pFunction->iExtra = FUNCTION_NOT_DECLARED;
                
                /* store first line function called */
                if(pFunction->iCodeSize == 0)
                    pFunction->iCodeSize = pEnv->lex.iRow;
            }
        }
    }
    
    return pNode;
}


/*********************************************************************

 FUNCTION     : GetIdLibrary

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcName

 DESCRIPTION:
        Returns the library index for given library
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetIdLibrary(struct Environment* pEnv, char* pcName) 
{
    INT32 iIndex = LibraryGetIDLibID(pcName);
    struct CodeNode* pNode = NULL;
    
    if(iIndex != UNIDENTIFIED)
    {
        pNode = GetEmptyNode(pEnv);
        
        if(pNode)
        {
            pNode->bNodeType = NODE_TYPE_ID;
            pNode->iIndex = iIndex;

        }
        
        /* save it to the environment struct, needed in GetIdLibraryFunction */
        pEnv->pTemp = pNode;
    }
    else
    {
        pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_LIBRARY_CALL;
    }
    
    return pNode;
}



/*********************************************************************

 FUNCTION     : GetIdLibraryFunction

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcName

 DESCRIPTION:
        Returns the library function index for given function. Library
        is stored in pEnv->pTemp.

        Saves the number of arguments to bStatus field.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetIdLibraryFunction(struct Environment* pEnv, char* pcName)
{
    BYTE bNumArgs;
    INT32 iIndex = LibraryGetFuncID((UINT16)pEnv->pTemp->iIndex,pcName,&bNumArgs);

    struct CodeNode* pNode = NULL;
    
    if(iIndex != UNIDENTIFIED)
    {
        pNode = GetEmptyNode(pEnv);
        
        if(pNode)
        {
            pNode->bNodeType = NODE_TYPE_ID;
            pNode->iIndex = iIndex;

            /* store num of arguments, check later when call is made */
            pNode->bStatus = bNumArgs; 
        }
    }
    else
    {
        pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_LIBRARY_CALL;
    }
    
    return pNode;
}





/*********************************************************************

 FUNCTION     : AddNodeToList

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pFirst
        struct CodeNode* pLast

 DESCRIPTION:
        Creates list nodes and links given nodes to them OR adds
        new node to existing list.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* AddNodeToList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast)
{
    return CreateAndLink(pEnv,pFirst,pLast,NODE_TYPE_LIST);
}


/*********************************************************************

 FUNCTION     : AddNodeToExpressionList

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pFirst
        struct CodeNode* pLast

 DESCRIPTION:
        Creates list nodes and links given nodes to them OR adds
        new node to existing list.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* AddNodeToExpressionList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast)
{
    struct CodeNode* pNode = AddNodeToList(pEnv,pFirst,pLast);

    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_EXPRESSION_LIST;
    }

    return pNode;
}




/*********************************************************************

 FUNCTION     : AddNodeToArgumentList

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pFirst
        struct CodeNode* pLast

 DESCRIPTION:
        Creates list nodes and links given nodes to them OR adds
        new node to existing list.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* AddNodeToArgumentList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast)
{
    struct CodeNode* pNode = AddNodeToList(pEnv,pFirst,pLast);

    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_ARGUMENT_LIST;
    }

    return pNode;
}


/*********************************************************************

 FUNCTION     : AddNodeToStatementList

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pFirst
        struct CodeNode* pLast

 DESCRIPTION:
        Creates list nodes and links given nodes to them OR adds
        new node to existing list.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* AddNodeToStatementList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast)
{
    struct CodeNode* pNode = AddNodeToList(pEnv,pFirst,pLast);

    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_STATEMENT_LIST;
    }

    return pNode;
}




/*********************************************************************

 FUNCTION     : GetVariableDeclaration

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcVal

 DESCRIPTION: Checks that the variable has not already been declared
              and if not creates the node and initialises it.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetVariableDeclaration(struct Environment* pEnv, char* pcVal)
{
    struct CodeNode* pNode = NULL;
    struct HashNode* pVar = NULL;
    
    /* false, true and invalid */
    if(pEnv->lex.iReserved > RESERVED_FUTURE)
    {
        pEnv->status = WMLS_STATUS_ERROR_RESERVED_KEYWORD;
        pEnv->iErrorLineNumber = pEnv->lex.iRow;
        return NULL;
    }

    /* variable must not be already declared */
    pVar = HashTableStore(&(pEnv->Variables),pcVal,0,STORE_MODE_MAY_NOT_EXIST);
    
    if(pVar)
    {
        
        if( pEnv->pCurrentFunction->bNumVariables < 255 )
          { (pEnv->pCurrentFunction->bNumVariables)++;

            pNode = GetEmptyNode(pEnv);

            if(pNode)
              { pNode->bNodeType = NODE_TYPE_VARIABLE_DECLARATION;
                pNode->iIndex = pVar->iIndex;
            
                pNode->iSize = 1;

                if(pNode->iIndex > 31)
                  pNode->iSize += 1;
              }
          }
        else
          { pEnv->status = WMLS_STATUS_ERROR_TOO_MANY_LOCAL_VARIABLES;
            pEnv->iErrorLineNumber = pEnv->lex.iRow;
          }
    }
    else
    {
        if(pEnv->Variables.iLastError == HASH_TABLE_STORE_MODE_ERROR)
            pEnv->status = WMLS_STATUS_ERROR_VARIABLE_ALREADY_DECLARED;
        else
            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

        pEnv->iErrorLineNumber = pEnv->lex.iRow;
        pNode = NULL;
    }

    return pNode;
}



/*********************************************************************

 FUNCTION     : GetVariable

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcVal

 DESCRIPTION:   Searches declared variables from hash table, creates an
                empty node and sets the type, index and size of the node.
    
 NOTE: 
        Literals false, true and invalid comes here from lex -> handle
        separately

 CHANGES: 

*********************************************************************/
struct CodeNode* GetVariable(struct Environment* pEnv, char* pcVal)
{
    struct CodeNode* pNode = NULL;
    struct HashNode* pVar = NULL;
    
    /* false, true and invalid */
    if(pEnv->lex.iReserved > RESERVED_FUTURE)
    {
        return GetLiteral(pEnv,pcVal,NOQUOTE_STRING_LITERAL);
    }


    /* variable must be already declared */
    pVar = HashTableFind(&(pEnv->Variables),pcVal,0);
    
    if(pVar)
    {
        pNode = GetEmptyNode(pEnv);

        if(pNode)
        {
            pNode->bNodeType = NODE_TYPE_VARIABLE;
            pNode->iIndex = pVar->iIndex;

            pNode->iSize = 1;
            if(pNode->iIndex > 31)
                pNode->iSize += 1;
        }
    }
    else
    {
        pEnv->status = WMLS_STATUS_ERROR_VARIABLE_NOT_DECLARED;
        pEnv->iErrorLineNumber = pEnv->lex.iRow;
        pNode = NULL;
    }

    
    return pNode;
}




/*********************************************************************

 FUNCTION     : GetLiteral

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcVal
        INT16 iID

 DESCRIPTION:   Creates an empty node, and adds it into the appropriate
                pool according the type of the literal.
    
 NOTE: 

 CHANGES: 
            210699 JoS sprintf is used instead of ltoa.

*********************************************************************/
struct CodeNode* GetLiteral(struct Environment* pEnv, char* pcVal, INT16 iID)
{
    struct CodeNode* pNode = GetEmptyNode(pEnv);
    char   pcTemp[30]; /* enough to fit big numbers */
	char   cBackUp;
	int	   iLen; 

    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_LITERAL;

        if( pEnv->Constants.iNumItems == SHRT_MAX )
          { pEnv->iErrorLineNumber = pNode->iLine;
            pNode = NULL;
            pEnv->status = WMLS_STATUS_ERROR_TOO_MANY_CONSTANTS;
            return pNode;
          }

        switch(iID)
        {

        case DEC_LITERAL: /* decimal */
            if(DecValidate(pcVal)) 
                pNode->iIndex = ConstTableAddValue(&(pEnv->Constants),pcVal);
            else
                pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_LITERAL;
            break;

        case FLT_LITERAL: /* decimal */
            if(FloatValidate(pcVal))
                pNode->iIndex = ConstTableAddValue(&(pEnv->Constants),pcVal);
            else
                pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_LITERAL;
            break;

        case HEX_LITERAL: /* hex */
            if(HexValidate(pcVal)) /* 0xhhh... */
            {
                /* 210699 JoS ltoa -> sprintf */
                sprintf(pcTemp,"%d", HexToDec(pcVal));
                /*ltoa(HexToDec(pcVal+2),pcTemp,10);*/

                pNode->iIndex = ConstTableAddValue(&(pEnv->Constants),pcTemp);
            }
            else
            {
                pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_LITERAL;
            }   
            break;
        case OCT_LITERAL: /* oct */
            
            if(OctValidate(pcVal)) /* 0xxxxxx */
            {
                /* 210699 JoS ltoa -> sprintf */
                sprintf(pcTemp,"%d", OctToDec(pcVal));
                /*ltoa(OctToDec(pcVal+1),pcTemp,10);*/

                pNode->iIndex = ConstTableAddValue(&(pEnv->Constants),pcTemp);
            }
            else
            {
                pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_LITERAL;
            }
            break;

        case DBLQ_LITERAL: /* string "xxx" or 'xxx' */
        case SQ_LITERAL: 
            /* remove last ' or " */
            iLen = strlen(pcVal);
			cBackUp = pcVal[iLen-1];
			pcVal[iLen-1]=0;
            
			pNode->iIndex = ConstTableAddString(&(pEnv->Constants),pcVal+1,FALSE);
       		
			/* restore char */
			pcVal[iLen-1]=cBackUp;
		    break;

        case NOQUOTE_STRING_LITERAL: 
            pNode->iIndex = ConstTableAddString(&(pEnv->Constants),pcVal,TRUE);
            break;
        default:
            assert(0);
        
        }

        if(pNode->iIndex == MEMORY_ERROR)
        {
            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
        }
        else
        {
            pNode->iSize = 1;

            if(pNode->iIndex > 255)
                pNode->iSize += 2;
            else if(pNode->iIndex > 15)
                pNode->iSize += 1;
        }
    }

    if(pEnv->status == WMLS_STATUS_ERROR_ILLEGAL_LITERAL)
    {
        pEnv->iErrorLineNumber = pNode->iLine;
        pNode = NULL;
    }

    return pNode;
}

/*********************************************************************

 FUNCTION     : GetLiteralMeta

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        char* pcVal
        
 DESCRIPTION:
        Copies given string to new buffer which is returned. These 
        strings are not added to bytecode.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetLiteralMeta(struct Environment* pEnv, char* pcVal)
{
    char* pcString = (char*) MemoryAlloc(pEnv->pMemTemporary,(INT16)strlen(pcVal));
	char  cBackUp;
	int	  iLen = strlen(pcVal);

    if(pcString)
    {
        /* remove last ' or " */
		cBackUp = pcVal[iLen-1];
		pcVal[iLen-1]=0;

        /* copy string to buffer */
        strcpy(pcString,pcVal+1);

		/* restore char */
		pcVal[iLen-1]=cBackUp;
    }

    return (struct CodeNode*)pcString;
}



/*********************************************************************

 FUNCTION     : GetAssignmentOperator

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        INT16 iID

 DESCRIPTION:   Creates an empty node and sets the OPCODE according
                the assignment operator.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetAssignmentOperator(struct Environment* pEnv, INT16 iID)
{
    struct CodeNode* pNode = GetEmptyNode(pEnv);
    
    if(pNode)
    {
        pNode->iSize = 1;

        pNode->bNodeType = NODE_TYPE_ASSIGNMENT;

        switch(iID)
        {

        case ASSIGN:
            pNode->bOpCode = OPCODE_STORE_VAR;
            break;

        case PLUSASSIGN:
            pNode->bOpCode = OPCODE_ADD_ASG;
            break;

        case MINUSASSIGN:
            pNode->bOpCode = OPCODE_SUB_ASG;
            break;

        case STARASSIGN:
            pNode->bOpCode = OPCODE_MUL;
            break;

        case SLASHASSIGN:
            pNode->bOpCode = OPCODE_DIV;
            break;

        case REMASSIGN:
            pNode->bOpCode = OPCODE_REM;
            break;

        case ANDASSIGN:
            pNode->bOpCode = OPCODE_B_AND;
            break;

        case ORASSIGN:
            pNode->bOpCode = OPCODE_B_OR;
            break;

        case XORASSIGN:
            pNode->bOpCode = OPCODE_B_XOR;
            break;

        case B_LEFTSHIFTASSIGN:
            pNode->bOpCode = OPCODE_B_LSHIFT;
            break;

        case B_RSIGNEDSHIFTASSIGN:
            pNode->bOpCode = OPCODE_B_RSSHIFT;
            break;

        case B_RZSHIFTASSIGN:
            pNode->bOpCode = OPCODE_B_RSZSHIFT;
            break;

        case DIVASSIGN:
            pNode->bOpCode = OPCODE_IDIV;
            break;
    
        default:
            assert(0);

        }
  
    }
    
    return pNode;
}



/*********************************************************************

 FUNCTION     : GetAssignment

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pOperator
        struct CodeNode* pVariable
        struct CodeNode* pRight

 DESCRIPTION:   Links the assignment, variable and expression tree nodes.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetAssignment(struct Environment* pEnv, struct CodeNode* pOperator, struct CodeNode* pVariable, struct CodeNode* pRight)
{
    if(pOperator)
    {
        /* add variable Index to operator index field */
        pOperator->iIndex = pVariable->iIndex;

        /* size of the node */
        switch(pOperator->bOpCode)
        {
        case OPCODE_STORE_VAR:
            if(pVariable->iIndex < 16)
                pOperator->iSize = 1;   /* command_index */
            else
                pOperator->iSize = 2;   /* command index */

            break;

        case OPCODE_ADD_ASG:
        case OPCODE_SUB_ASG:
            /* command index */
            pOperator->iSize = 2;
            break;

        default:
            /* LoadVar Binary StoreVar */
            if(pOperator->iIndex < 16) 
                pOperator->iSize = 3;
            else
                pOperator->iSize = 5;

        }
            
        /* add exression to data field */
        LinkRight(pOperator,pRight);
        
#if 0
        /* accept, bytecode interpreter handles these TIR 280599 */

        /* do not accept assignments like var a += false */
        switch(pRight->iIndex)
        {
        case CONST_TYPE_TRUE:
        case CONST_TYPE_FALSE:
        case CONST_TYPE_INVALID:
            if(pOperator->bOpCode != OPCODE_STORE_VAR)
            {
                pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_ASSIGMENT;
                pEnv->iErrorLineNumber = pOperator->iLine;
                
                pOperator = NULL;
            }
            break;
        }
#endif
    }


    return pOperator;
}




/*********************************************************************

 FUNCTION     : GetUnaryOperation

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pExpression
        INT16 iID

 DESCRIPTION:   Creates and links unary operation node, sets the
                appropriate OPCODE according the type of the unary 
                operation.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetUnaryOperation(struct Environment* pEnv, struct CodeNode* pExpression, INT16 iID)
{
    struct CodeNode* pNode = CreateAndLink(pEnv,pExpression,NULL,NODE_TYPE_UNARY);
    
    if(pNode)
    {
        pNode->iSize += 1;

        switch(iID)
        {

        case ISVALID:
            pNode->bOpCode = OPCODE_ISVALID;
            break;

        case MINUS:
            pNode->bOpCode = OPCODE_UMINUS;
            break;

        case TILDE:
            pNode->bOpCode = OPCODE_B_NOT;
            break;

        case BANG:
            pNode->bOpCode = OPCODE_NOT;
            break;

        case TYPEOF:
            pNode->bOpCode = OPCODE_TYPEOF;
            break;
        
        case PLUS:
            /* two operation needed: multiplication by one */
            pNode->bOpCode = 0;
			pNode->iSize += 1;
            break;
        default:
            assert(0);

        }
    }
    
    return pNode;
}




/*********************************************************************

 FUNCTION     : GetPrefix

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pVar
        INT16 iID

 DESCRIPTION:   Creates an empty node for prefix and sets the node type 
                and OPCODE according the type of the prefix (++/--).
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetPrefix(struct Environment* pEnv, struct CodeNode* pVar, INT16 iID)
{
    struct CodeNode* pNode = GetEmptyNode(pEnv);
    
    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_INCR_DECR;

        /* add variable Index to operator index field */
        pNode->iIndex= pVar->iIndex;

        if(iID==INCR) 
            pNode->bOpCode = OPCODE_INCR_VAR;
        else
            pNode->bOpCode = OPCODE_DECR_VAR;


        /* iSize already 1 for default */
        if(pNode->iIndex < 8)
            pNode->iSize = 2;   /* 1 for incr and 1 for possible load */
        else if(pNode->iIndex < 32)
            pNode->iSize =3;        /*  2 for incr and 1 for possible load */
        else
            pNode->iSize =4;        /*  2 for incr and 2 for possible load */

    }
    
    return pNode;

}


/*********************************************************************

 FUNCTION     : GetPostfix

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pVar
        INT16 iID

 DESCRIPTION:   Creates an empty node for postfix and sets the node type 
                and OPCODE according the type of the postfix (++/--).
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetPostfix(struct Environment* pEnv, struct CodeNode* pVar, INT16 iID)
{
    struct CodeNode* pNode = GetEmptyNode(pEnv);
    
    if(pNode)
    {
        pNode->bNodeType = NODE_TYPE_INCR_DECR;
        pNode->bStatus |= STATUS_POST;

        /* add variable Index to operator index field */
        pNode->iIndex= pVar->iIndex;

        if(iID==INCR) 
            pNode->bOpCode = OPCODE_INCR_VAR;
        else
            pNode->bOpCode = OPCODE_DECR_VAR;
        
        if(pNode->iIndex < 32)
            pNode->iSize = 3;   /* 2 for incr and 1 for possible load  */
        else
            pNode->iSize =4;    /* 2 for incr and 2 for possible load  */
    }
    
    return pNode;
}




/*********************************************************************

 FUNCTION     : GetBinaryOperation

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pLeft
        struct CodeNode* pRight
        INT16 iID

 DESCRIPTION:
    Binary operations are:  B_OR, B_XOR, B_AND, EQ, NE, LT, GT, LE, GE, LSHIFT,
                            RSSHIFT, RSZSHIFT, ADD, SUB, MUL, DIV, REM. 
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetBinaryOperation(struct Environment* pEnv, struct CodeNode* pLeft, struct CodeNode* pRight, INT16 iID)
{
    NodeType            type = NODE_TYPE_BINARY;
    struct CodeNode     *pNode,
                        *p1,
                        *p2=NULL;
    INT16               iSize;
    
    
    if(iID == SC_AND)
        type = NODE_TYPE_SCAND;

    if(iID == SC_OR)
        type = NODE_TYPE_SCOR;

    pNode = CreateAndLink(pEnv,pLeft,pRight,type);

    if(pNode)
    {
    
        /* handle SCAND and SCOR specially */
        if(type != NODE_TYPE_BINARY)
        {
            /*
                                SCAND

                    LEFT                EMPTY

                                TJUMP           RIGHT   
            */

    
            p1 = GetEmptyNode(pEnv);

            if(p1)
            {
                p1->bNodeType = NODE_TYPE_TJUMP;
                
                if(pNode->pRight)
                    iSize = pNode->pRight->iSize;
                else
                    iSize = 0;
                
                p1->iSize = GetJumpCodeSize(iSize);
                
                p2= CreateAndLink(pEnv,p1,pNode->pRight,NODE_TYPE_EMPTY);

                if(p2)
                {
                    /* add size of TJUMP */
                    pNode->iSize += p1->iSize;

                    /* add size of SCAND and TOBOOL operations */
                    pNode->iSize += 2;

                    /* link new node to right */
                    pNode->pRight = p2;
                    p2->pParent = pNode;

                    pNode->bOpCode = 0;
                }
                else
                    pNode = NULL;
            }
            else
                pNode = NULL;

        }
        else
        {
            /* add size of the operation */
            pNode->iSize += 1;

            /* put correct OpCode */
            switch(iID)
            {

            case PLUS:
                pNode->bOpCode = OPCODE_ADD;
                break;

            case MINUS:
                pNode->bOpCode = OPCODE_SUB;
                break;

            case STAR:
                pNode->bOpCode = OPCODE_MUL;
                break;

            case SLASH:
                pNode->bOpCode = OPCODE_DIV;
                break;

            case REM:
                pNode->bOpCode = OPCODE_REM;
                break;

            case BIT_AND:
                pNode->bOpCode = OPCODE_B_AND;
                break;

            case BIT_OR:
                pNode->bOpCode = OPCODE_B_OR;
                break;

            case XOR:
                pNode->bOpCode = OPCODE_B_XOR;
                break;

            case B_LSHIFT:
                pNode->bOpCode = OPCODE_B_LSHIFT;
                break;

            case B_RSIGNEDSHIFT:
                pNode->bOpCode = OPCODE_B_RSSHIFT;
                break;

            case B_RSZSHIFT:
                pNode->bOpCode = OPCODE_B_RSZSHIFT;
                break;

            case DIV:
                pNode->bOpCode = OPCODE_IDIV;
                break;

            /* logical operations */
            case EQ:
                pNode->bOpCode = OPCODE_EQ;
                break;

            case NE:
                pNode->bOpCode = OPCODE_NE;
                break;

            case LT:
                pNode->bOpCode = OPCODE_LT;
                break;

            case GT:
                pNode->bOpCode = OPCODE_GT;
                break;

            case LE:
                pNode->bOpCode = OPCODE_LE;
                break;

            case GE:
                pNode->bOpCode = OPCODE_GE;
                break;
            
            default:
                assert(0);
            }
 
#if 0       
            /* do not accept operations like "false * true" */
            switch(pRight->iIndex)
            {
            case CONST_TYPE_TRUE:
            case CONST_TYPE_FALSE:
            case CONST_TYPE_INVALID:
                    pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_OPERATION;
                    pEnv->iErrorLineNumber = pNode->iLine;
                    
                    pNode = NULL;
                break;
            }

            switch(pLeft->iIndex)
            {
            case CONST_TYPE_TRUE:
            case CONST_TYPE_FALSE:
            case CONST_TYPE_INVALID:
                    pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_OPERATION;
                    pEnv->iErrorLineNumber = pNode->iLine;
                    
                    pNode = NULL;
                break;

            }
#endif
        }
    }

    return pNode;
}


/*********************************************************************

 FUNCTION     : GetCallLocal

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pFunc
        struct CodeNode* pArguments

 DESCRIPTION:   Creates, links and sets the node size of the local
                function call node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetCallLocal(struct Environment* pEnv, struct CodeNode* pFunc, struct CodeNode* pArguments)
{
    struct CodeNode* pNode = CreateAndLink(pEnv,pArguments,pFunc,NODE_TYPE_CALL_LOCAL);

    if(pNode)
    {
        if(pFunc->iIndex < 8)
            pNode->iSize += 1;
        else
            pNode->iSize += 2;
    }

    return pNode;
}


/*********************************************************************

 FUNCTION     : GetCallExternal

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pUrl
        struct CodeNode* pFunc
        struct CodeNode* pArguments

 DESCRIPTION:   Creates, links and sets the node size of the external
                function call node.
     
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetCallExternal(struct Environment* pEnv, struct CodeNode* pUrl, struct CodeNode* pFunc, struct CodeNode* pArguments)
{
    struct CodeNode* pNode;

    /* URL is next one in const table following pragma name TIR 280699 */
    if(pUrl)
          (pUrl->iIndex)++;

    LinkRight(pUrl,pFunc);

    pNode = CreateAndLink(pEnv,pArguments,pUrl,NODE_TYPE_CALL_EXTERNAL);

    if(pNode)
    {
        if( (pUrl->iIndex < 256) && (pFunc->iIndex < 256) )
            pNode->iSize += 3;
        else
            pNode->iSize += 5;
    }
    
    return pNode;   
}



/*********************************************************************

 FUNCTION     : GetCallLibrary

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pLib
        struct CodeNode* pFunc
        struct CodeNode* pArguments

 DESCRIPTION:   Creates, links and sets the node size of the library
                call node.
     
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetCallLibrary(struct Environment* pEnv, struct CodeNode* pLib, struct CodeNode* pFunc, struct CodeNode* pArguments)
{
    struct  CodeNode* pNode; 

    LinkRight(pLib,pFunc);

    pNode = CreateAndLink(pEnv,pArguments,pLib,NODE_TYPE_CALL_LIBRARY);


    if(pNode)
    {
        if( (pFunc->iIndex < 8) && (pLib->iIndex < 256) )
            pNode->iSize += 2;
        else if(pFunc->iIndex < 256)
            pNode->iSize += 3;
        else
            pNode->iSize += 4;
    }
    


    
    return pNode;   
}



/*********************************************************************

 FUNCTION     : AddFunction

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct Environment* pEnv
        char* pcName
        INT16 iID

 DESCRIPTION:   Adds the function node into the pool and initialises
                it according to whether the function has already been
                defined earlier or not.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int AddFunction(struct Environment* pEnv, char* pcName, INT16 iID)
{
    struct HashNode*    pFunction;
    INT16               iType;

    if(iID)
        iType = FUNCTION_EXTERNAL;
    else
        iType = FUNCTION_INTERNAL;


	if ( strlen (pcName) > FUNCTION_NAME_MAXLENGTH && iType == FUNCTION_EXTERNAL )
	{	/* if the length is more than 255, the remainder should be ignored */
        pcName[FUNCTION_NAME_MAXLENGTH] = '\0';
		pEnv->warning = WMLS_STATUS_ERROR_FUNCTION_NAME_TOO_LONG;
	}

    pFunction = HashTableFind(&(pEnv->Functions),pcName,0);
        
    if(pFunction)
    {
        if(pFunction->iExtra == FUNCTION_NOT_DECLARED)
        {
            pFunction->iExtra = iType;
        }
        else
        {
            /* this is second declaration for this function */
            pEnv->status = WMLS_STATUS_ERROR_FUNCTION_ALREADY_DECLARED;
        }
        
    }
    else
    {
        pFunction = HashTableStore(&(pEnv->Functions),pcName,0,STORE_MODE_MAY_EXIST);

		if(pFunction)
			pFunction->iExtra = iType;
    }

    if(pFunction)
	   pEnv->pCurrentFunction = pFunction;
    else
        pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;


    return (int)( pEnv->status == WMLS_STATUS_OK );

}



/*********************************************************************

 FUNCTION     : AddFormalParameter

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct Environment* pEnv
        char* pcName

 DESCRIPTION:   Stores parameter into table and sets the environment
                struct status according to whether the operation was
                succesfull or not.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int AddFormalParameter(struct Environment* pEnv, char* pcName)
{
    /* variable must not be already declared */
    struct HashNode* pVar = HashTableStore(&(pEnv->Variables),pcName,0,STORE_MODE_MAY_NOT_EXIST);
    
    if(pVar)
    {
        if( pEnv->pCurrentFunction->bNumArguments < 255 )
          { (pEnv->pCurrentFunction->bNumArguments)++;
            pEnv->status = WMLS_STATUS_OK;
          }
        else
          pEnv->status = WMLS_STATUS_ERROR_TOO_MANY_FUNCTION_ARGUMENTS;

    }
    else
    {
        if(pEnv->Variables.iLastError == HASH_TABLE_STORE_MODE_ERROR)
            pEnv->status = WMLS_STATUS_ERROR_VARIABLE_ALREADY_DECLARED;
        else
            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
        
        pEnv->iErrorLineNumber = pEnv->lex.iRow;
    }

    return (int)( pEnv->status == WMLS_STATUS_OK );

}



/*********************************************************************

 FUNCTION     : SetFunctionBody

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pBody

 DESCRIPTION:   Calls ByteCodeInitialize function and if operation was 
                succesfull calls FunctionCompile. Updates the
                environment struct data according to ByteCodeInitialize
                return value.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* SetFunctionBody(struct Environment* pEnv, struct CodeNode* pBody)
{
    struct StorageStruct s;


    if( ByteCodeInitialize(&s, pBody->iSize + 10, 100) )
    {
        /* compile function */
        FunctionCompile(pEnv,&s,pBody);
        
        /* save code to current function */
        pEnv->pCurrentFunction->pbCode = s.pbData;
        pEnv->pCurrentFunction->iCodeSize = s.iIndex;
    }
    else
    {
        pEnv->pCurrentFunction->pbCode = NULL;
        pEnv->pCurrentFunction->iCodeSize = 0;
    }

    /* clear tables */
    MemoryDealloc(pEnv->pMemTemporary);
    HashTableRemoveNodes(&(pEnv->Variables));


    if(pEnv->status == WMLS_STATUS_OK)
        return (struct CodeNode*)1; /* != NULL to indicate no problemos */

    return NULL;
}





/*********************************************************************

 FUNCTION     : GetStatementReturn

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pReturn

 DESCRIPTION:   Creates, links and updates node for return statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementReturn(struct Environment* pEnv, struct CodeNode* pReturn)
{
    struct CodeNode* pNode = CreateAndLink(pEnv,pReturn,NULL,NODE_TYPE_RETURN);
    
    if(pNode)
    {
        pNode->iSize += 1;
    }

    return  pNode;}


    

/*********************************************************************

 FUNCTION     : GetStatementContinue

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv

 DESCRIPTION:   Creates, links and updates node for continue statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementContinue(struct Environment* pEnv)
{
    struct CodeNode* pNode = NULL;
    
    pNode = CreateAndLink(pEnv,NULL,NULL,NODE_TYPE_CONTINUE);

    if(pNode)
    {
        pNode->iSize = 3; /* longest possible jump, just in case  */
    }

    return  pNode;
}


/*********************************************************************

 FUNCTION     : GetStatementBreak

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv

 DESCRIPTION:   Creates, links and updates node for break statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementBreak(struct Environment* pEnv)
{
    struct CodeNode* pNode = NULL;
    
    pNode = CreateAndLink(pEnv,NULL,NULL,NODE_TYPE_BREAK);

    if(pNode)
    {
        
        pNode->iSize = 3; /* longest possible jump, just in case  */
    }


    return  pNode;
}



/*********************************************************************

 FUNCTION     : GetStatementCondExpress

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pExpression
        struct CodeNode* pTrue
        struct CodeNode* pFalse

 DESCRIPTION:   Creates and updates node for conditional expression
                statement (logical OR).
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementCondExpress(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pTrue, struct CodeNode* pFalse)
{
    struct CodeNode* pNode = GetIfNode(pEnv,pExpression,pTrue,pFalse);
    
    if(pNode)
    {
        /* load result to operand stack */
        pNode->bStatus = STATUS_LOAD;
    }

    return  pNode;
}




/*********************************************************************

 FUNCTION     : GetStatementIf

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pExpression
        struct CodeNode* pTrue
        struct CodeNode* pElse

 DESCRIPTION:   Creates node for if statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementIf(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pTrue,  struct CodeNode* pElse)
{
    return GetIfNode(pEnv,pExpression,pTrue,pElse);
}



/*********************************************************************

 FUNCTION     : GetStatementWhile

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pExpression
        struct CodeNode* pBody

 DESCRIPTION:   Creates node for while statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementWhile(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pBody)
{
    return GetLoopNode(pEnv,NULL,pExpression,pBody,NULL);
}



/*********************************************************************

 FUNCTION     : GetStatementFor

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pInit
        struct CodeNode* pCond
        struct CodeNode* pOperation
        struct CodeNode* pBody

 DESCRIPTION:   Creates node for for statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetStatementFor(struct Environment* pEnv, struct CodeNode* pInit, struct CodeNode* pCond, struct CodeNode* pOperation, struct CodeNode* pBody)
{   
    struct CodeNode* pCurrent = pOperation;

    while(pCurrent)
    {
        if(pCurrent->bNodeType == NODE_TYPE_EXPRESSION_LIST)
            pCurrent->bStatus = STATUS_NO_LOAD;
        else
            break;

        pCurrent = pCurrent->pLeft; 
    }

    return GetLoopNode(pEnv,pInit,pCond,pBody,pOperation);
}






/*********************************************************************

 FUNCTION     : PragmaAccessControl

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pFirst
        struct CodeNode* pSecond

 DESCRIPTION:   Sets the environment struct status according the pragma
                access identifier.
    
 NOTE: 

 CHANGES: 

		public access control specifier removed. JOS/CCC 040799


*********************************************************************/
int PragmaAccessControl(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pSecond)
{
    struct Pragma *tmp;    

    pEnv->status = WMLS_STATUS_OK;

    for( tmp = pEnv->pPragmas; tmp; tmp = tmp->pNext )
      if( tmp->type == PRAGMA_ACCESS_DOMAIN || 
          tmp->type == PRAGMA_ACCESS_PATH )
        { pEnv->status = WMLS_STATUS_ERROR_SECOND_ACCESS_PRAGMA_OF_TYPE;
          return 0;
        }

    if(pFirst!=NULL)
      {
        /* access domain  */
        if(PragmaTableAdd(&(pEnv->pPragmas),PRAGMA_ACCESS_DOMAIN,(INT16)pFirst->iIndex,0,0) == MEMORY_ERROR)
            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
      }
    
    if(pSecond!=NULL)
      {
        /* access path */
        if(PragmaTableAdd(&(pEnv->pPragmas),PRAGMA_ACCESS_PATH,(INT16)pSecond->iIndex,0,0) == MEMORY_ERROR)
            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
      }

    return (int)( pEnv->status == WMLS_STATUS_OK );

}





/*********************************************************************

 FUNCTION     : PragmaMeta

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pName
        struct CodeNode* pCont
        struct CodeNode* pScheme
        INT16 iID

 DESCRIPTION:   Handles meta pragmas.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int PragmaMeta(struct Environment* pEnv, struct CodeNode* pName, struct CodeNode* pCont, struct CodeNode* pScheme, INT16 iID)
{
    char *pcTarget,*p1,*p2,*p3;
    UINT32  uiLen=3,ui; /* one byte for each zero */
    
     pEnv->status = WMLS_STATUS_OK;

    switch(iID)
    {

    case NAME:
    case HTTP:
        
        p1=(char*)pName;
        p2=(char*)pCont;
        p3=(char*)pScheme;

        if(p1)
            uiLen += strlen(p1);

        if(p2)
            uiLen += strlen(p2);

        if(p3)
        {
            uiLen += strlen(p3);
        }

        
        /* allocate enough memory for pragma strings */
        pcTarget = (char*) WMLS_malloc(uiLen);

#ifdef MEMORY_OUT_TEST
        if(rand()%MEMORY_MAX < MEMORY_LIMIT)
        {
            if(pcTarget)
            {
                WMLS_free(pcTarget);
                pcTarget = NULL;
            }
        }
#endif

        if(pcTarget)
        {
            ui = 0;
            
            /* copy information to buffer */
            if(p1)
            {
                strcpy(pcTarget,p1);
                ui = strlen(p1) + 1;
            }

            if(p2)
            {
                strcpy(pcTarget + ui,p2);
                ui += (strlen(p2) + 1);
            }

            if(p3)
            {
                strcpy(pcTarget + ui,p3);
            }
            else
            {
                strcpy(pcTarget + ui,"");
            }


            if(iID==NAME)
            {
                /* do we need to add this to earlier buffer */
                if(pEnv->metaNameBuffer)
                {
                    pEnv->metaNameBuffer = (BYTE*)WMLS_realloc(pEnv->metaNameBuffer,pEnv->metaNameBufferSize + uiLen, pEnv->metaNameBufferSize);

                    if(pEnv->metaNameBuffer)
                    {
                        memcpy((void*)(pEnv->metaNameBuffer + pEnv->metaNameBufferSize),(void*) pcTarget, uiLen);
                        pEnv->metaNameBufferSize += (INT16)uiLen;
                        WMLS_free(pcTarget);
                    }
                    else
                    {
                        pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
                    }
                    
                }
                else
                {
                    pEnv->metaNameBuffer = pcTarget;
                    pEnv->metaNameBufferSize = (INT16)uiLen;
                }
                
                *(pEnv->metaNameStrings) += 1;
            }
            else
            {   
                /* do we need to add this to earlier buffer */
                if(pEnv->httpEquivBuffer)
                {
                    pEnv->httpEquivBuffer = (BYTE*)WMLS_realloc(pEnv->httpEquivBuffer,pEnv->httpEquivBufferSize + uiLen, pEnv->httpEquivBufferSize);

                    if(pEnv->httpEquivBuffer)
                    {
                        memcpy((void*)(pEnv->httpEquivBuffer + pEnv->httpEquivBufferSize),(void*) pcTarget, uiLen);
                        pEnv->httpEquivBufferSize += (INT16)uiLen;
                        WMLS_free(pcTarget);
                    }
                    else
                    {
                        pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
                    }
                    
                }
                else
                {
                    pEnv->httpEquivBuffer = pcTarget;
                    pEnv->httpEquivBufferSize = (INT16)uiLen;
                }
                
                *(pEnv->httpEquivStrings) += 1;

            }
        }
        else
        {
            pEnv->status =  WMLS_STATUS_ERROR_OUT_OF_MEMORY;
        }

        break;

    
    default: /* meta user goes to bytecode */

        if(pScheme==NULL)
        {
            if(PragmaTableAdd(&(pEnv->pPragmas),PRAGMA_USER_AGENT_PROPERTY,(INT16)pName->iIndex,(INT16)pCont->iIndex,0) == MEMORY_ERROR)
                pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
        }
        else
        {
            if(PragmaTableAdd(&(pEnv->pPragmas),PRAGMA_USER_AGENT_PROPERTY_AND_SCHEME,(INT16)pName->iIndex,(INT16)pCont->iIndex,(INT16)pScheme->iIndex) == MEMORY_ERROR)
                pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
        }

    }

    return (int)( pEnv->status == WMLS_STATUS_OK );
}






/********************************************************************************************
                                LOCAL FUNCTIONS
********************************************************************************************/



/*********************************************************************

 FUNCTION     : GetIfNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pExpression
        struct CodeNode* pTrue
        struct CodeNode* pElse

 DESCRIPTION:   Creates and initialises the IF node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetIfNode(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pTrue,  struct CodeNode* pElse)
{
    
    /*
                                        IF
    
                        BLOCK                       ELSE
    
            NODE                NODE
    
    EXPRESSION  TJUMP       TRUE    BREAK
    
    */
    
    struct CodeNode *p1, *p2, *pReturn=NULL;

    p2 = pTrue;

    /* if there is no else body then jump after true body is not needed */
    if(pElse)
    {
        if(pElse->iSize > 0)
        {
            p2 = NULL;
            p1 = GetEmptyNode(pEnv);
        
            if(p1)
            {
                p1->bNodeType = NODE_TYPE_BREAK;
                p1->bStatus = STATUS_IF_BREAK;
                p1->iSize = GetJumpCodeSize(pElse->iSize);
            
                p2= CreateAndLink(pEnv,pTrue,p1,NODE_TYPE_EMPTY);
            }
        }
    }

    if(p2)
    {
        p1 = GetBlockNode(pEnv,pExpression,p2);
        
        if(p1)
        {
            pReturn = CreateAndLink(pEnv,p1,pElse,NODE_TYPE_IF);
        }
    }

    return pReturn;
}



/*********************************************************************

 FUNCTION     : GetLoopNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pInit
        struct CodeNode* pExpression
        struct CodeNode* pBody
        struct CodeNode* pPostStatement

 DESCRIPTION:   Creates, initialises and links the loop node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* GetLoopNode(struct Environment* pEnv, struct CodeNode* pInit, struct CodeNode* pExpression, struct CodeNode* pBody,  struct CodeNode* pPostStatement)
{
    
    /*
                                LOOP

            NODE                                    BLOCK
        
    INIT        STORAGE                 NODE                    NODE
    
                                EXPRESSION  TJUMP           BODY        LOOP_END
                                    
                                                                    POST    
    
    
    STORAGE:    right links will be breaks connected to this loop
                left link will be link to previous loop

    LOOP_END:   right links will be breaks (continues) for FOR-loop
    */
    
    INT16 iSize;
    struct CodeNode *p1, *p2=NULL, *pReturn=NULL;

    
    if(pBody)
        iSize = pBody->iSize;
    else
        iSize = 0;

    if(pPostStatement)
        iSize += pPostStatement->iSize;

    p1 = CreateAndLink(pEnv,pPostStatement,NULL,NODE_TYPE_LOOP_END);
    
    if(p1)
    {
        /* JUMP_BW size */
        p1->iSize += GetJumpCodeSize(iSize);

        p2= CreateAndLink(pEnv,pBody,p1,NODE_TYPE_EMPTY);

        if(p2)
        {
            p1 = GetBlockNode(pEnv,pExpression,p2);
            
            if(p1)
            {
                /* create nodes where breaks are linked */
                p2 = GetEmptyNode(pEnv);

                if(p2)
                {
                    p2 = CreateAndLink(pEnv,pInit,p2,NODE_TYPE_EMPTY);

                    if(p2)
                        pReturn = CreateAndLink(pEnv,p2,p1,NODE_TYPE_LOOP);
                }
            }
        }
    }

    return pReturn;
}




/*********************************************************************

 FUNCTION     : GetBlockNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pExpression
        struct CodeNode* pRight

 DESCRIPTION:   Creates, initialises and links the block node.
    
 NOTE: 

 CHANGES:
    if pExpression is NULL then no need to build TJUMP node TIR 160699

*********************************************************************/
struct CodeNode* GetBlockNode(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pRight)
{
    
    /*
                                
                        BLOCK                       
    
            EXPRESSION_NODE         RIGHT
    
    EXPRESSION  TJUMP   
    
    */
    
    INT16 iSize;
    struct CodeNode *p1=NULL, *p2=NULL, *pReturn=NULL;

    
    /* if expression is NULL then TJUMP is not needed / allowed */
    if(pExpression != NULL)
    {
        p1 = GetEmptyNode(pEnv);

        if(p1)
        {
            p1->bNodeType = NODE_TYPE_TJUMP;
        
            if(pRight)
                iSize = pRight->iSize;
            else
                iSize = 0;
        
            p1->iSize = GetJumpCodeSize(iSize);
        
        }
    }
    

    p2= CreateAndLink(pEnv,pExpression,p1,NODE_TYPE_EXPRESSION);


    if(p2)
    {
        pReturn = CreateAndLink(pEnv,p2,pRight,NODE_TYPE_BLOCK);
    }

    return pReturn;
}



