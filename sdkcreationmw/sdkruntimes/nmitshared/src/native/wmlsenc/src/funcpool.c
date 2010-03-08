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
/* File    : U:\wpsvob\wmlscript\work\ported\funcpool.c
   Version : \main\12
   Date    : 22-Nov-00.17:24:55
*/

/*
 * ========================================================================
 *  Name        : funcpool.c
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

      Reserves, initialises and frees memory for functions, adds them into 
      function pool and generates bytecode out of the function pool.

-----------------------------------------------------------------------------
*/

#include "main.h"
#include "common.h"
#include "bytecode.h"
#include "funcpool.h"
#include "opcodes.h"
#include "symtable.h"
#include "codenode.h"
#include <assert.h>



typedef enum
{
    OPERAND_STACK_NO_ADD,
    OPERAND_STACK_ADD
} OperandStackMode;


/* local protos */
void    LoadVar(int index, struct StorageStruct* pStorage);
void    StoreVar(int index, struct StorageStruct* pStorage);
void    IncrVar(int index, struct StorageStruct* pStorage);
void    DecrVar(int index, struct StorageStruct* pStorage);
void    IncrDecrVar(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode);
void    LoadConst(int index, struct StorageStruct* pStorage);
void    Variable(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode);
void    Assign(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode);
void    Binary(struct CodeNode* pNode, struct StorageStruct* pStorage);
void    Literal(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode);
void    CallLocal(struct Environment* pEnv,struct CodeNode* pNode, struct StorageStruct* pStorage);
void    CallLibrary(struct Environment* pEnv,struct CodeNode* pNode, struct StorageStruct* pStorage);
void    CallExternal(struct Environment* pEnv,struct CodeNode* pNode, struct StorageStruct* pStorage);
void    Return(struct Environment* pEnv, struct CodeNode* pNode, struct StorageStruct* pStorage);
void    UnaryOp(struct CodeNode* pNode, struct StorageStruct* pStorage);
void    UpdateTJUMP(struct CodeNode* pNode, struct StorageStruct* pStorage);
void    UpdateJUMP(struct CodeNode* pNode, struct StorageStruct* pStorage);
BYTE    AddArgumentsToOperandStack(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode);

void    LinkListRight(struct CodeNode* pParent, struct CodeNode* pNode);

void    AddContinue(struct CodeNode* pNode,struct CodeNode* pLoop, struct StorageStruct* pStorage);
void    AddTJUMP(struct CodeNode* pNode, struct StorageStruct* pStorage);
void    AddBreak(struct CodeNode* pNode,struct CodeNode* pLoop, struct CodeNode* pList, struct StorageStruct* pStorage);
void    UpdateBreak(struct CodeNode* pNode, struct StorageStruct* pStorage);
/*
void    AddContinueFor(struct CodeNode* pNode,struct CodeNode* pLoop, struct StorageStruct* pStorage);
*/
struct CodeNode* ExecuteNode(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode, OperandStackMode opMode);
struct CodeNode* ExecuteChilds(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode, OperandStackMode opMode);







/*********************************************************************************
                                PUBLIC FUNCTIONS 
*********************************************************************************/



/*********************************************************************

 FUNCTION     : ValidateFunctionTable

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct Environment* pEnv

 DESCRIPTION:
        Check that all functions have been declared.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int ValidateFunctionTable(struct Environment* pEnv)
{
    int                 i,
                        iRet = 0;
	INT32				iNumExternals = 0;
    INT16               iNumItems;
    struct HashNode**   pFunctions;
    struct FuncCall     *pCalls,*p;

    pFunctions = GetItemsInOrder(&(pEnv->Functions),&iNumItems);

    if(pFunctions)
    {
        iRet = 1;

        /* check that all functions are declared */
        for(i=0;i<iNumItems;i++)
        {
            if(pFunctions[i]->iExtra == FUNCTION_NOT_DECLARED)
            {
                pEnv->iErrorLineNumber = pFunctions[i]->iCodeSize;
                pEnv->status = WMLS_STATUS_ERROR_UNDECLARED_FUNCTION;
                iRet = 0;

                if(pEnv->pcErrorInfo)
                {
                    memset((void*)(pEnv->pcErrorInfo),0,ERROR_INFO_LEN);
                    strncpy(pEnv->pcErrorInfo,pFunctions[i]->pcKey,(ERROR_INFO_LEN-1));
                }

                break;
            }

            /* 1.2: calculate the number of external functions */
            if(pFunctions[i]->iExtra == FUNCTION_EXTERNAL)
				iNumExternals++;


        }


        /* 1.2: check that atleast one external function exists */
        if(iNumExternals == 0 && pEnv->status == WMLS_STATUS_OK)
        {
			pEnv->status = WMLS_STATUS_ERROR_NO_EXTERNAL_FUNCTION;
            pEnv->iErrorLineNumber = 1;
        }



        if(pEnv->status == WMLS_STATUS_OK)
        {   
            /* check that all calls to non declared functions at the time have correct
                amount of arguments */
            pCalls = pEnv->pCalls;

            while(pCalls)
            {
                p = pCalls->pNext;

                if(pFunctions[pCalls->iFunctionID]->bNumArguments != (BYTE) pCalls->iNumArguments)
                {
                    pEnv->status = WMLS_STATUS_ERROR_FUNCTION_ARGUMENT_NUMBER_MISMATCH;
                    pEnv->iErrorLineNumber = pCalls->iRow;
                    break;
                }

                /* there is no need to free these nodes, they are under memorynode management */
                pCalls = p;
            }
        }

        pEnv->pCalls = NULL;

        WMLS_free(pFunctions);
    }




    return iRet;
}


/*********************************************************************

 FUNCTION     : FunctionTableGenerateBytecode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: int 

 PARAMETERS:
        struct HashTable* pFunctionTable
        struct StorageStruct* pStorage

 DESCRIPTION:
        Generates bytecode for function table
    
 NOTE: 

 CHANGES: 

*********************************************************************/
int FunctionTableGenerateBytecode(struct HashTable* pFunctionTable, struct StorageStruct* pStorage)
{
    int                 i;
    INT16               iNumItems,
                        iNumExternals=0;
    struct HashNode**   pFunctions;
    
    pFunctions = GetItemsInOrder(pFunctionTable,&iNumItems);

    if(pFunctions)
    {
        /* Initialize FunctionPool */
        if( ByteCodeInitialize(pStorage,1000,500) == RET_OK)
        {
        
            /* add number of Functions */
            ByteCodeAppendByte((BYTE)iNumItems, pStorage);

            
            /* add Function name table (only external functions) */
            for(i=0;i<iNumItems;i++)
            {
                if(pFunctions[i]->iExtra == FUNCTION_EXTERNAL)
                    iNumExternals++;
            }

            /* add number of external function names */
            ByteCodeAppendByte((BYTE)iNumExternals, pStorage);

            
            if(iNumExternals)
            {
                for(i=0;i<iNumItems;i++)
                {
                    if(pFunctions[i]->iExtra == FUNCTION_EXTERNAL)
                    {
                        /* add function index */
                        ByteCodeAppendByte((BYTE)pFunctions[i]->iIndex, pStorage);
                        
                        /* add function name size */
                        ByteCodeAppendByte((BYTE)strlen(pFunctions[i]->pcKey), pStorage);
            
                        /* add function name  */
                        ByteCodeAppendString(pFunctions[i]->pcKey, 
                            strlen(pFunctions[i]->pcKey), pStorage);

                    }
                }
            }


            for(i=0;i<iNumItems;i++)
            {
                /* add number of arguments */
                ByteCodeAppendByte((BYTE)pFunctions[i]->bNumArguments, pStorage);

                /* add number of variables */
                ByteCodeAppendByte((BYTE)pFunctions[i]->bNumVariables, pStorage);

                /* add function size */
                ByteCodeAppendMBInt((UINT32)pFunctions[i]->iCodeSize, pStorage);


                if(pFunctions[i]->pbCode)
                {
                    /* add code */
                    ByteCodeAppendBytes(pFunctions[i]->pbCode, pFunctions[i]->iCodeSize, pStorage);

                    /* free unused memory */
                    WMLS_free(pFunctions[i]->pbCode);
                    pFunctions[i]->pbCode = NULL;
                }

            }

            ByteCodeResize(pStorage);
        }
    
        WMLS_free(pFunctions);
    }
    else
    {
        return RET_ERROR;
    }


    return ByteCodeGetStatus(pStorage);
    
}




/*********************************************************************

 FUNCTION     : FunctionTableCleanUp

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct HashTable* pFunctionTable

 DESCRIPTION:
        Frees any memory allocated by function table
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void FunctionTableCleanUp(struct HashTable* pFunctionTable)
{
    int                 i=0;
    struct HashNode*    pNode;
    
    /* is there anything to do */
    if(pFunctionTable->ppNodes == NULL)
        return;
    
    if(pFunctionTable->iNumItems == 0)
        return;


    while(i<pFunctionTable->iTableSize)
    {
        pNode = pFunctionTable->ppNodes[i++];

        while(pNode)
        {
            if(pNode->pbCode)
            {
                /* free unused memory */
                WMLS_free(pNode->pbCode);
                pNode->pbCode = NULL;
            }

            pNode = pNode->pNext;
        }
    }   
    
}




/*********************************************************************

 FUNCTION     : FunctionCompile

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT32 

 PARAMETERS:
        struct Environment* pEnv
        struct StorageStruct* pStorage
        struct CodeNode* pNode

 DESCRIPTION:
        Compiles given function.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT32 FunctionCompile(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode)
{
    struct CodeNode* pCurrent = pNode;
    int iReturnNeeded = 1;

    if(pNode)
    {
        /* if last statement in list is RETURN then RETURN_ES at the end of function
            is not needed */
        if(pNode->pRight)
        {
            if(pNode->bNodeType == NODE_TYPE_STATEMENT_LIST)
            {
                if(pNode->pRight->bNodeType == NODE_TYPE_RETURN)
                    iReturnNeeded = 0;
            }
        }

        /* if return only statement */
        if(pNode->bNodeType == NODE_TYPE_RETURN)
            iReturnNeeded = 0;

        pEnv->status = WMLS_STATUS_OK;

        while(pCurrent && (pEnv->status == WMLS_STATUS_OK) )
        {
            pCurrent = ExecuteNode(pEnv, pStorage, pCurrent,OPERAND_STACK_NO_ADD);
        }
    }

    /* if there is no return add RETURN_ES */
    if(iReturnNeeded==1)
        ByteCodeAppendByte( (BYTE)OPCODE_RETURN_ES, pStorage );

    return 1;
}

/*********************************************************************

 FUNCTION     : GetJumpCodeSize

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        INT16 iSize

 DESCRIPTION:
        How many bytes a jump byte instruction will take.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 GetJumpCodeSize(INT16 iSize)
{
    int iJumpSize = 1;

    if(iSize>255)
        iJumpSize = 3;
    else if(iSize>31)
        iJumpSize = 2;

    return iJumpSize;
}











/*********************************************************************************
                                LOCAL FUNCTIONS   
*********************************************************************************/






/*********************************************************************

 FUNCTION     : LinkListRight

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pParent
        struct CodeNode* pNode

 DESCRIPTION:
        Links new node to node pRight list.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void LinkListRight(struct CodeNode* pParent, struct CodeNode* pNode)
{
    
    while(pParent->pRight)
        pParent = pParent->pRight;


    pParent->pRight = pNode;

}


/*********************************************************************

 FUNCTION     : ExecuteLeft

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct StorageStruct* pStorage
        struct CodeNode* pNode
        OperandStackMode opMode

 DESCRIPTION:
        Compiles left side of given node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* ExecuteLeft(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode, OperandStackMode opMode)
{
    struct CodeNode* pParent = pNode;
    struct CodeNode* pCurrent;

    if(pParent)
    {
        if(pParent->pLeft)
        {
            pCurrent = pParent->pLeft;

            while(pCurrent != pParent)
            {
                pCurrent = ExecuteNode(pEnv, pStorage, pCurrent,opMode);

                if(pCurrent == NULL)
                    break;

                if(pEnv->status != WMLS_STATUS_OK)
                    break;
            }
        }
    }

    return pParent;
}

/*********************************************************************

 FUNCTION     : ExecuteRight

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct StorageStruct* pStorage
        struct CodeNode* pNode
        OperandStackMode opMode

 DESCRIPTION:
        Compiles right side of given node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* ExecuteRight(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode, OperandStackMode opMode)
{
    struct CodeNode* pParent = pNode;
    struct CodeNode* pCurrent;

    if(pParent)
    {
        if(pParent->pRight)
        {
            pCurrent = pParent->pRight;

            while(pCurrent != pParent)
            {
                pCurrent = ExecuteNode(pEnv, pStorage, pCurrent,opMode);

                if(pCurrent == NULL)
                    break;

                if(pEnv->status != WMLS_STATUS_OK)
                    break;
            }
        }
    }

    return pParent;
}


/*********************************************************************

 FUNCTION     : ExecuteChilds

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct StorageStruct* pStorage
        struct CodeNode* pNode
        OperandStackMode opMode

 DESCRIPTION:
        First compiles left side then right side of given node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* ExecuteChilds(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode, OperandStackMode opMode)
{
    if(pNode)
    {
        ExecuteLeft(pEnv, pStorage, pNode,opMode);
        ExecuteRight(pEnv, pStorage, pNode,opMode); 
    }

    return pNode;
}



/*********************************************************************

 FUNCTION     : ExecuteNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct CodeNode* 

 PARAMETERS:
        struct Environment* pEnv
        struct StorageStruct* pStorage
        struct CodeNode* pNode
        OperandStackMode opMode

 DESCRIPTION:
        Compiles given node and any nodes below it (left or right).
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct CodeNode* ExecuteNode(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode, OperandStackMode opMode)
{
    struct CodeNode* pLeft=NULL;
    struct CodeNode* pRight=NULL;
    struct CodeNode* pParent=NULL;

    struct CodeNode* pReturn = NULL;
    OperandStackMode newOpMode;
    
    /* if error occured during compiling */
    if(pEnv->status != WMLS_STATUS_OK)
        return NULL;


    if(pNode)
    {
        if(pNode->pLeft)
        {
            if(pNode->pLeft->bAlive)
                pLeft = pNode->pLeft;

            pReturn = pLeft;
        }
        
        if(pNode->pRight)
        {
            if(pNode->pRight->bAlive)
                pRight = pNode->pRight;

            if(pLeft == NULL)
                pReturn = pRight;
        }

        pParent = pNode->pParent;
        
        if( (pLeft == NULL) && (pRight == NULL) )
        {
            pReturn = pParent;
            KillNode(pNode);
        }

        
        switch(pNode->bNodeType)
        {
        case NODE_TYPE_EMPTY: /* no action needed */
        case NODE_TYPE_LIST: 
        case NODE_TYPE_STATEMENT_LIST: 
        case NODE_TYPE_ID:
        case NODE_TYPE_VARIABLE_DECLARATION:
            break;

        case NODE_TYPE_EXPRESSION:
            
            if(pLeft)
            {
                ExecuteLeft(pEnv,pStorage,pNode,OPERAND_STACK_ADD);
                KillNode(pLeft);

                if(pNode->pRight)
                    pReturn = pNode->pRight;
                else
                    pReturn = pNode->pParent;
            }

            break;

        case NODE_TYPE_EXPRESSION_LIST: 
            
            if(pLeft)
            {
                if(pLeft->bNodeType == NODE_TYPE_EXPRESSION_LIST)
                    return pLeft;

                /* there is always right side -> not last item in expression list */
                ExecuteNode(pEnv,pStorage,pNode->pLeft,OPERAND_STACK_NO_ADD);

                KillNode(pLeft);
            }
            
            if(pRight)
            {
                newOpMode = opMode;

                if(pNode->bStatus == STATUS_NO_LOAD)
                {
                    newOpMode = OPERAND_STACK_NO_ADD;
                }
                else
                {
                    /* is this the last item in expression list */
                    if(pParent)
                    {
                        if(pParent->bNodeType == NODE_TYPE_EXPRESSION_LIST)
                            newOpMode = OPERAND_STACK_NO_ADD;   
                    }
                }

                ExecuteNode(pEnv,pStorage,pNode->pRight,newOpMode);

                KillNode(pRight);
                
            }

            KillNode(pNode);

            pReturn = pParent;
            break;

        
        
        case NODE_TYPE_VARIABLE:
            if( (pLeft == NULL) && (pRight == NULL) )
            {
                Variable(pNode,pStorage,opMode);
            }
            
            break;

        case NODE_TYPE_UNARY:
            if(opMode == OPERAND_STACK_NO_ADD)
               {
                    /* we must compile these anyway to get side effects */
                    if(pLeft != NULL)
                      { ExecuteChilds(pEnv,pStorage,pNode,OPERAND_STACK_ADD);

                        UnaryOp(pNode,pStorage);

                        /* nobody is going to use calculated value */
                        ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );

                        KillNode(pNode);
                        pReturn = pParent;
                      }
                }
             else
               { if( (pLeft == NULL) && (pRight == NULL) )
                     UnaryOp(pNode,pStorage);
               }

            break;
        
        case NODE_TYPE_BINARY:
            if(opMode == OPERAND_STACK_NO_ADD)
			{
				/* we must compile these anyway to get side effects */
				ExecuteChilds(pEnv,pStorage,pNode,OPERAND_STACK_ADD);
				
				Binary(pNode,pStorage);

				/* nobody is going to use calculated value */
				ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );

				KillNode(pNode);

				pReturn = pParent;
			}
			else
			{
				if( (pLeft == NULL) && (pRight == NULL) )
				{		
			
					Binary(pNode,pStorage);
				}
			}
            break;

        case NODE_TYPE_SCAND:
        case NODE_TYPE_SCOR:
            switch(pNode->bOpCode)
            {
            case 0: /* no action needed, compile left side */
                break;

            case 1:
                
                if(opMode == OPERAND_STACK_ADD)
                {
                    /* now is time for SCAND or SCOR */
                    if(pNode->bNodeType == NODE_TYPE_SCAND)
                        ByteCodeAppendByte( (BYTE)OPCODE_SCAND, pStorage );
                    else
                        ByteCodeAppendByte( (BYTE)OPCODE_SCOR, pStorage );
                }
                else
                {
                    /* kill the TJUMP node */
                    if(pNode->pRight)
                    {
                        if(pNode->pRight->pLeft)
                            KillNode(pNode->pRight->pLeft);
                    }
                }

                /* now right side is compiled, TJUMP first */
                break;

            case 2:                 
                if(opMode == OPERAND_STACK_ADD)
                {
                    /* operand stack contains the value from right side -> to boolean */
                    ByteCodeAppendByte( (BYTE)OPCODE_TOBOOL, pStorage );
                    
                    /* update TJUMP to current place */
                    UpdateTJUMP(pNode->pRight->pLeft,pStorage);
                }
                break;

            }

            (pNode->bOpCode)++;

            break;

        case NODE_TYPE_ASSIGNMENT:
            
            if(pRight != NULL)
            {
                
                if( (pNode->bOpCode != OPCODE_STORE_VAR) &&
                    (pNode->bOpCode != OPCODE_ADD_ASG) &&
                    (pNode->bOpCode != OPCODE_SUB_ASG) )
                {

                    LoadVar(pNode->iIndex,pStorage);
                }

                ExecuteChilds(pEnv,pStorage,pNode,OPERAND_STACK_ADD);   

                Assign(pNode,pStorage,opMode);

                KillNode(pNode);

				pReturn = pParent;
            }

            break;


        case NODE_TYPE_INCR_DECR:
            IncrDecrVar(pNode,pStorage,opMode);
            break;


        case NODE_TYPE_CALL_LOCAL:          
            CallLocal(pEnv,pNode,pStorage);
            pReturn = pParent;
            KillNode(pNode);
        
            /* if return value is not used POP it out from operand stack */
            if(opMode == OPERAND_STACK_NO_ADD)
            {
                ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );
            }
            break;

        case NODE_TYPE_CALL_LIBRARY:
            CallLibrary(pEnv,pNode,pStorage);
            pReturn = pParent;
            KillNode(pNode);

            /* if return value is not used POP it out from operand stack */
            if(opMode == OPERAND_STACK_NO_ADD)
            {
                ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );
            }

            break;

        case NODE_TYPE_CALL_EXTERNAL:
            CallExternal(pEnv,pNode,pStorage);
            pReturn = pParent;
            KillNode(pNode);

            /* if return value is not used POP it out from operand stack */
            if(opMode == OPERAND_STACK_NO_ADD)
            {
                ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );
            }
#if 0
            /* if return value is not used POP it out from operand stack */
            if(pParent==NULL)
            {
                ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );
            }
            else if(pParent->bNodeType == NODE_TYPE_STATEMENT_LIST)
                ByteCodeAppendByte( (BYTE)OPCODE_POP, pStorage );
#endif
            break;

        case NODE_TYPE_RETURN:
            Return(pEnv,pNode,pStorage);
            pReturn = pParent;
            KillNode(pNode);
            break;

        case NODE_TYPE_LITERAL:
            Literal(pNode,pStorage,opMode);
            KillNode(pNode);

            break;
        
        case NODE_TYPE_BLOCK:
            if( (pLeft == NULL) && (pRight == NULL) )
            {
                UpdateTJUMP(pNode->pLeft->pRight,pStorage);
            }

            break;
        
        case NODE_TYPE_IF:
            if(pLeft)
            {
                pReturn = pNode->pLeft->pLeft->pLeft;

                while(pReturn != pNode->pLeft->pLeft)
                {
                    pReturn = ExecuteNode(pEnv, pStorage, pReturn, OPERAND_STACK_ADD);
                    
                    if(pReturn==NULL)
                        break;
                }
            }
            else
            {   
                if( (pLeft == NULL) && (pRight == NULL) )
                {
                    if(pNode->pRight)
                    {
                        if(pNode->pRight->iSize > 0)
                            UpdateJUMP(pNode->pLeft->pRight->pRight, pStorage);
                    }
                }
            }
            break;

        case NODE_TYPE_LOOP:
            switch(pNode->bOpCode)
            {
            case 0:
                break;

            case 1:
                /* this is the place where while loop starts */
                pNode->iIndex = pStorage->iIndex;
                
                pNode->pLeft->pRight->pLeft = pEnv->pCurrentLoop;
                pEnv->pCurrentLoop = pNode;
                break;

            case 2:                 
                if(pReturn == pNode->pParent)
                {
                    UpdateBreak(pNode->pLeft->pRight,pStorage);
                    
                    /* return current loop */
                    pEnv->pCurrentLoop = pNode->pLeft->pRight->pLeft;
                }
            }

            (pNode->bOpCode)++;
            break;

    
        case NODE_TYPE_BREAK:
            /* this is the place where break jump goes */
            pNode->iIndex = pStorage->iIndex;

            if(pNode->bStatus == STATUS_IF_BREAK)
            {
                AddBreak(pNode,NULL,NULL, pStorage);
            }
            else
            {
                if(pEnv->pCurrentLoop == NULL)
                {
                    pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_BREAK;
                    pEnv->iErrorLineNumber = pNode->iLine;
                }
                else
                {   
                    AddBreak(pNode,pEnv->pCurrentLoop,pEnv->pCurrentLoop->pLeft->pRight, pStorage);
                }
            }
            break;

        case NODE_TYPE_TJUMP:
            /* this is the place where TJUMP jump goes */
            pNode->iIndex = pStorage->iIndex;
            AddTJUMP(pNode,pStorage);
            break;

        case NODE_TYPE_CONTINUE:
            if(pEnv->pCurrentLoop == NULL)
            {
                pEnv->status = WMLS_STATUS_ERROR_ILLEGAL_CONTINUE;
                pEnv->iErrorLineNumber = pNode->iLine;
            }
            else
            {
                /* this is the place where continue jump goes */
                pNode->iIndex = pStorage->iIndex;
                
                /* if for loop then continue must jump to post expression (i++) */
                if(pEnv->pCurrentLoop->pLeft->pLeft)
                {
                    /* link break to LOOP_END node */
                    AddBreak(pNode,pEnv->pCurrentLoop, pEnv->pCurrentLoop->pRight->pRight->pRight,pStorage);
                }
                else
                {
                    AddContinue(pNode,pEnv->pCurrentLoop,pStorage);
                }
            }
            break;


        case NODE_TYPE_LOOP_END:
            switch(pNode->bOpCode)
            {
            case 0:
                pNode->iIndex = pStorage->iIndex;

                /* update for loop continues to this point before post statement*/
                UpdateBreak(pNode,pStorage);

                if(pLeft == NULL)
                {
                    /* jump back to beginning of loop */
                    AddContinue(pNode,pEnv->pCurrentLoop,pStorage);

                    pReturn = pNode->pParent;
                }
                break;

            case 1:
                pNode->iIndex = pStorage->iIndex;

                /* jump back to beginning of loop */
                AddContinue(pNode,pEnv->pCurrentLoop,pStorage);

                pReturn = pNode->pParent;

                break;
            }

            (pNode->bOpCode)++;
            break;
        
        }

    }
    return pReturn;
}








/*********************************************************************

 FUNCTION     : AddArgumentsToOperandStack

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE 

 PARAMETERS:
        struct Environment* pEnv
        struct StorageStruct* pStorage
        struct CodeNode* pNode

 DESCRIPTION:
        Adds function argument to operand stack.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE AddArgumentsToOperandStack(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode)
{
    BYTE                bNumArguments = 0;
    struct CodeNode*    pCurrent = pNode;
    struct CodeNode*    pEnd; 
    /*
                                ...

                        LIST 

                LIST            3rd argument

    1st argument     2nd argument
     
    */

    if(pCurrent)
    {
        bNumArguments = 1;

        if(pCurrent->bNodeType == NODE_TYPE_ARGUMENT_LIST)
        {
            bNumArguments++;

            /* find first argument */
            while(pCurrent->pLeft)
            {
                if(pCurrent->pLeft->bNodeType != NODE_TYPE_ARGUMENT_LIST)
                    break;
                
                pCurrent = pCurrent->pLeft; 
                bNumArguments++;
            }
        }

        pEnd = pNode->pParent;

        /* start adding arguments to operand stack */
        while( (pCurrent != pEnd) && (pEnv->status == WMLS_STATUS_OK) )
        {
            pCurrent = ExecuteNode(pEnv, pStorage, pCurrent,OPERAND_STACK_ADD);
        }

    }

    return bNumArguments;
}



/*********************************************************************

 FUNCTION     : Variable

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage
        OperandStackMode opMode

 DESCRIPTION:
        Handle variable node
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void Variable(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode)
{
    if(pNode->pParent)
    {
        if(pNode->pParent->bNodeType == NODE_TYPE_EXPRESSION_LIST)
        {
            if(opMode == OPERAND_STACK_NO_ADD)
                return;
        }
    }

    /*pNode->iIndex;    variable index */
    LoadVar(pNode->iIndex, pStorage);
}




/*********************************************************************

 FUNCTION     : Literal

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage
        OperandStackMode opMode

 DESCRIPTION:
        Handle literal node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void Literal(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode)
{
    /*pNode->iIndex;        const index, */

    BYTE pbConsts[]={   OPCODE_CONST_FALSE, OPCODE_CONST_TRUE, OPCODE_CONST_INVALID,
                        OPCODE_CONST_ES, OPCODE_CONST_1, OPCODE_CONST_0, OPCODE_CONST_M1};

    if(pNode->pParent)
    {
        if(pNode->pParent->bNodeType == NODE_TYPE_EXPRESSION_LIST)
        {
            if(opMode == OPERAND_STACK_NO_ADD)
                return;
        }
    }

    /* handle internal constants */
    if(pNode->iIndex < 0)
        ByteCodeAppendByte( (BYTE)pbConsts[pNode->iIndex + 7], pStorage );
    else
        LoadConst(pNode->iIndex,pStorage);  
}





/*********************************************************************

 FUNCTION     : Assign

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage
        OperandStackMode opMode

 DESCRIPTION:
        Handle Assign node.
        assignment operations are:  =, +=, -= *=, /=, %=, &=, ^=, |=, <<=, >>=
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void Assign(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode)
{
    /*pNode->iIndex;        variable index */
    /*pNode->bOpCode;       correct opCode */
    /*pNode->bStatus        STATUS_LOAD if wanted on operand stack */

    switch(pNode->bOpCode)
    {       
    case OPCODE_STORE_VAR:

        StoreVar((BYTE)pNode->iIndex,pStorage);
        break;

    case OPCODE_ADD_ASG:
    case OPCODE_SUB_ASG:

        /* add operation */
        ByteCodeAppendByte((BYTE)pNode->bOpCode, pStorage);

        /* add variable index */
        ByteCodeAppendByte((BYTE)pNode->iIndex, pStorage);

        break;

    case OPCODE_MUL:
    case OPCODE_DIV:
    case OPCODE_REM:
    case OPCODE_B_AND:
    case OPCODE_B_XOR:
    case OPCODE_B_OR:
    case OPCODE_B_LSHIFT:
    case OPCODE_B_RSSHIFT:
    case OPCODE_B_RSZSHIFT:
    case OPCODE_IDIV:
      
        /* add operation */
        ByteCodeAppendByte((BYTE)pNode->bOpCode, pStorage);

        /* store variable */
        StoreVar(pNode->iIndex,pStorage);

        break;

    }
    
    /* if following operation needs this load it available */
    if(opMode == OPERAND_STACK_ADD)
        LoadVar(pNode->iIndex,pStorage);

}


/*********************************************************************

 FUNCTION     : IncrDecrVar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage
        OperandStackMode opMode

 DESCRIPTION:
        Handle IncrDecrVar node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void IncrDecrVar(struct CodeNode* pNode, struct StorageStruct* pStorage, OperandStackMode opMode)
{
    /*pNode->bOpCode        correct opCode INCR_VAR, DECR_VAR */
    /*pNode->iIndex         variable index */
    /*pNode->bStatus        STATUS_POST if post operation */
    
    if(pNode->bOpCode == OPCODE_INCR_VAR)
    {
        if(opMode == OPERAND_STACK_ADD)
        {
            if(pNode->bStatus & STATUS_POST)
            {
                LoadVar(pNode->iIndex,pStorage);
                IncrVar(pNode->iIndex,pStorage);
            }
            else
            {
                IncrVar(pNode->iIndex,pStorage);
                LoadVar(pNode->iIndex,pStorage);
            }
        }
        else
            IncrVar(pNode->iIndex,pStorage);        
    }
    else
    {
        if(opMode == OPERAND_STACK_ADD)
        {
            if(pNode->bStatus & STATUS_POST)
            {
                LoadVar(pNode->iIndex,pStorage);
                DecrVar(pNode->iIndex,pStorage);
            }
            else
            {
                DecrVar(pNode->iIndex,pStorage);
                LoadVar(pNode->iIndex,pStorage);
            }
        }
        else
            DecrVar(pNode->iIndex,pStorage);        
    }
    

 }



/*********************************************************************

 FUNCTION     : Binary

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Handle binary node
    
        Binary operations are:  B_OR, B_XOR, B_AND, EQ, NE, LT, GT, LE,
                                GE, LSHIFT, RSSHIFT, RSZSHIFT, ADD, SUB,
                                MUL, DIV, REM. 
 NOTE: 

 CHANGES: 

*********************************************************************/
void Binary(struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /*pNode->bOpCode;       correct opCode */

   ByteCodeAppendByte( (BYTE)pNode->bOpCode, pStorage); 
}





/*********************************************************************

 FUNCTION     : CallLocal

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Handle local call node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void CallLocal(struct Environment* pEnv,struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    int iIndex = pNode->pRight->iIndex;
    struct HashNode* pFunction;
    BYTE    bNumArguments;
    struct FuncCall* pCall,*p;
    
    
    /* arguments <-- node --> id */
    
    /* does this function exist */
    pFunction = HashTableFindByIndex(&(pEnv->Functions),(INT16)pNode->pRight->iIndex);

    if(pFunction)
    {   
        
        bNumArguments = AddArgumentsToOperandStack(pEnv, pStorage, pNode->pLeft);

        /* check that argument number match */
        if(pFunction->iExtra != FUNCTION_NOT_DECLARED)
        {
            if(pFunction->bNumArguments != bNumArguments)
            {
                pEnv->status = WMLS_STATUS_ERROR_FUNCTION_ARGUMENT_NUMBER_MISMATCH;
                pEnv->iErrorLineNumber = pNode->iLine;
                return;
            }
        }
        else
        {
            /* function not declared yet -> argument check must be done later -> 
                store call info */
            pCall = (struct FuncCall*) MemoryAlloc(pEnv->pMemPermanent,sizeof(struct FuncCall));
            
            if(pCall)
            {
                pCall->iRow = pNode->iLine;
                pCall->iFunctionID = (INT16)pNode->pRight->iIndex;
                pCall->iNumArguments = bNumArguments;
                pCall->pNext = NULL;

                /* add to env struct */
                if(pEnv->pCalls)
                {
                    p=pEnv->pCalls;

                    while(p->pNext)
                    {
                        p=p->pNext;
                    }

                    p->pNext = pCall;
                }
                else
                {
                    pEnv->pCalls = pCall;
                }

            }
        }
    
        

        if(iIndex < 8)
        {
            ByteCodeAppendByte( (BYTE)(OPCODE_CALL_S + iIndex), pStorage );
        }
        else
        {
            ByteCodeAppendByte( (BYTE)OPCODE_CALL , pStorage );
            ByteCodeAppendByte( (BYTE)iIndex , pStorage );
        }
    }
    else
    {
        pEnv->status = WMLS_STATUS_ERROR_PARSE;
        pEnv->iErrorLineNumber = pNode->iLine;
    }
}




/*********************************************************************

 FUNCTION     : CallLibrary

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Handle library call node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void CallLibrary(struct Environment* pEnv, struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    INT32 iLibId = pNode->pRight->iIndex;
    INT16 iFuncId = (INT16)pNode->pRight->pRight->iIndex;
    BYTE    bNumArguments;

    /* arguments <-- node --> lib id -> func id */

    
    bNumArguments = AddArgumentsToOperandStack(pEnv, pStorage, pNode->pLeft);

    /* check that argument number match: (func id)->bStatus contains number of arguments*/
    if(pNode->pRight->pRight->bStatus != bNumArguments)
    {
        pEnv->status = WMLS_STATUS_ERROR_FUNCTION_ARGUMENT_NUMBER_MISMATCH;
        pEnv->iErrorLineNumber = pNode->iLine;
        return;
    }

    if( (iFuncId<8) && (iLibId<256) )
    {
        ByteCodeAppendByte( (BYTE)(OPCODE_CALL_LIB_S + iFuncId), pStorage );
        ByteCodeAppendByte( (BYTE)iLibId, pStorage );
    }
    else if(iLibId<256)
    {
        ByteCodeAppendByte( (BYTE)OPCODE_CALL_LIB , pStorage );
        ByteCodeAppendByte( (BYTE)iFuncId , pStorage );
        ByteCodeAppendByte( (BYTE)iLibId , pStorage );
    }
    else
    {               
        ByteCodeAppendByte( (BYTE)OPCODE_CALL_LIB_W , pStorage );
        ByteCodeAppendByte( (BYTE)iFuncId , pStorage );
        ByteCodeAppendWord( (WORD)iLibId , pStorage );
    }       
}



/*********************************************************************

 FUNCTION     : CallExternal

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Handle external call node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void CallExternal(struct Environment* pEnv, struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /* arguments <-- node --> url id -> func id */
    
    INT16   iUrlIndex= (INT16)pNode->pRight->iIndex;
    INT16   iFuncIndex = (INT16)pNode->pRight->pRight->iIndex;
    BYTE    bNumArguments = AddArgumentsToOperandStack(pEnv, pStorage, pNode->pLeft);


    if( (iUrlIndex<256) && (iFuncIndex<256) )
    {
        ByteCodeAppendByte( (BYTE)OPCODE_CALL_URL, pStorage );
        ByteCodeAppendByte( (BYTE)iUrlIndex, pStorage );
        ByteCodeAppendByte( (BYTE)iFuncIndex, pStorage );
    }
    else
    {               
        ByteCodeAppendByte( (BYTE)OPCODE_CALL_URL_W, pStorage );
        ByteCodeAppendWord( (WORD)iUrlIndex, pStorage );
        ByteCodeAppendWord( (WORD)iFuncIndex, pStorage );
    }       
    
    ByteCodeAppendByte( (BYTE)bNumArguments, pStorage );
}




/*********************************************************************

 FUNCTION     : Return

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct Environment* pEnv
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Handle return node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void Return(struct Environment* pEnv, struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /*pNode->pLeft          contains what to return, null if to return empty string */

    if(pNode->pLeft == NULL)
        ByteCodeAppendByte( (BYTE)OPCODE_RETURN_ES, pStorage );
    else
    {

        ExecuteChilds(pEnv, pStorage, pNode, OPERAND_STACK_ADD);
        
        /*
        pReturn = pNode->pLeft;

        while( (pReturn != pNode) && (pEnv->status == WMLS_STATUS_OK) )
            pReturn = ExecuteNode(pEnv, pStorage, pReturn, OPERAND_STACK_ADD);

        */

        ByteCodeAppendByte( (BYTE)OPCODE_RETURN, pStorage );
    }
}


/*********************************************************************

 FUNCTION     : UnaryOp

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Handle unary operation node. Values already in operand stack at
        the top.

        IsValid, UMINUS, B_NOT,NOT,TypeOf 
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void UnaryOp(struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /*pNode->bOpCode;       correct OpCode */
    
    /* 0 for unary plus -> no operation needed */
    if(pNode->bOpCode != 0)
	{
        ByteCodeAppendByte( (BYTE)pNode->bOpCode, pStorage );
	}
	else
	{	/* unary plus is equal with multiplication by 1 */
		ByteCodeAppendByte( (BYTE)OPCODE_CONST_1, pStorage );
		ByteCodeAppendByte( (BYTE)OPCODE_MUL, pStorage );
	}
}






/*********************************************************************

 FUNCTION     : AddTJUMP

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Add logical jump operation.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void AddTJUMP(struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /* pNode->iSize is 1,2 or 3 depending how big jump will be */
    
    switch(pNode->iSize)
    {
    case 1:
        ByteCodeAppendByte((BYTE)(OPCODE_TJUMP_FW_S), pStorage );
        break;

    case 2:
        ByteCodeAppendByte((BYTE)(OPCODE_TJUMP_FW), pStorage );
        ByteCodeAppendByte((BYTE)(0xff), pStorage );
        break;

    case 3:
        ByteCodeAppendByte((BYTE)(OPCODE_TJUMP_FW_W), pStorage );
        ByteCodeAppendWord((WORD)(0xffff), pStorage );
        break;

    default:
        assert(0);
    }

}




/*********************************************************************

 FUNCTION     : UpdateTJUMP

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Update the jump offset of TJUMP to current location
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void UpdateTJUMP(struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /* pNode->iIndex contains the place where TJUMP statement is.
       pStorage->iIndex contains current place. Calculate the difference */
    
    int iSize;

    if(pNode == NULL)
		return;

	if(pNode->iIndex)
    {       
        iSize = pStorage->iIndex - pNode->iIndex;

        switch(pStorage->pbData[pNode->iIndex])
        {
        case OPCODE_TJUMP_FW_S:
            ByteCodeSetByte((INT16)(pNode->iIndex), (BYTE)(OPCODE_TJUMP_FW_S + iSize - 1), pStorage );
            break;

        case OPCODE_TJUMP_FW:
            ByteCodeSetByte((INT16)(pNode->iIndex + 1), (BYTE)(iSize - 2), pStorage );
            break;

        case OPCODE_TJUMP_FW_W:
            ByteCodeSetWord((INT16)(pNode->iIndex + 1), (WORD)(iSize - 3), pStorage );
            break;

        default:
            assert(0);
        }
    }
}




/*********************************************************************

 FUNCTION     : UpdateJUMP

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Update the jump offset of JUMP to current location
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void UpdateJUMP(struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /* pNode->iIndex contains the place where JUMP statement is.
       pStorage->iIndex contains current place. Calculate the difference */
    
    int iSize;

    if(pNode->iIndex)
    {
        iSize = pStorage->iIndex - pNode->iIndex;

        switch(pStorage->pbData[pNode->iIndex])
        {
        case OPCODE_JUMP_FW_S:
            ByteCodeSetByte((INT16)(pNode->iIndex), (BYTE)(OPCODE_JUMP_FW_S + iSize - 1), pStorage );
            break;

        case OPCODE_JUMP_FW:
            ByteCodeSetByte((INT16)(pNode->iIndex + 1), (BYTE)(iSize - 2), pStorage );
            break;

        case OPCODE_JUMP_FW_W:
            ByteCodeSetWord((INT16)(pNode->iIndex + 1), (WORD)(iSize - 3), pStorage );
            break;

        default:
            assert(0);
        }
    }
}



 


/*********************************************************************

 FUNCTION     : AddBreak

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct CodeNode* pLoop
        struct CodeNode* pList
        struct StorageStruct* pStorage

 DESCRIPTION:
        Add code for break statement.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void AddBreak(struct CodeNode* pNode,struct CodeNode* pLoop, struct CodeNode* pList, struct StorageStruct* pStorage)
{
    INT16 iSize,
        iJumpSize;

    if(pNode->bStatus != STATUS_IF_BREAK)
    {
        /* how many bytes to the end of loop - worst case */
        iSize = pLoop->iSize - (INT16)(pNode->iIndex - pLoop->iIndex);
        
        /* calculate the jump size */
        iJumpSize = GetJumpCodeSize(iSize);
        

        /* add link to this node to the loop node, so correct jump address is added when loop resdy */
        if(pList)
            LinkListRight(pList,pNode);
        else
            assert(0);
    }
    else
    {
        /* break in if construct, iSize contains the jump size over else */
        iJumpSize = pNode->iSize;
    }


    switch(iJumpSize)
    {
    case 1:
        ByteCodeAppendByte( (BYTE)OPCODE_JUMP_FW_S, pStorage );
        break;

    case 2:
        ByteCodeAppendByte( (BYTE) OPCODE_JUMP_FW, pStorage );
        ByteCodeAppendByte( (BYTE) 0xff, pStorage );
        break;

    case 3:
        ByteCodeAppendByte( (BYTE)OPCODE_JUMP_FW_W, pStorage );
        ByteCodeAppendWord( (WORD) 0xffff, pStorage );
        break;

    default:
        assert(0);
    }
}


#if 0
/*********************************************************************

 FUNCTION     : AddContinueFor

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct CodeNode* pLoop
        struct StorageStruct* pStorage

 DESCRIPTION:
        Add continue statement for for-loop
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void AddContinueFor(struct CodeNode* pNode,struct CodeNode* pLoop, struct StorageStruct* pStorage)
{
    INT16 iSize,
        iJumpSize;

    /* how many bytes to the end of loop - worst case */
    iSize = pLoop->iSize - (pNode->iIndex - pLoop->iIndex);
    
    /* calculate the jump size */
    iJumpSize = GetJumpCodeSize(iSize);
    

    /* add link to this node to the loop node, so correct jump address is added when loop resdy */
    if(pLoop)
        LinkListRight(pLoop->pRight->pRight->pRight,pNode);
    else
        assert(0);

    switch(iJumpSize)
    {
    case 1:
        ByteCodeAppendByte( (BYTE)OPCODE_JUMP_FW_S, pStorage );
        break;

    case 2:
        ByteCodeAppendByte( (BYTE) OPCODE_JUMP_FW, pStorage );
        ByteCodeAppendByte( (BYTE) 0xff, pStorage );
        break;

    case 3:
        ByteCodeAppendByte( (BYTE)OPCODE_JUMP_FW_W, pStorage );
        ByteCodeAppendWord( (WORD) 0xffff, pStorage );
        break;

    default:
        assert(0);
    }
}
#endif

/*********************************************************************

 FUNCTION     : AddContinue

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct CodeNode* pLoop
        struct StorageStruct* pStorage

 DESCRIPTION:
        Add code for continue.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void AddContinue(struct CodeNode* pNode,struct CodeNode* pLoop, struct StorageStruct* pStorage)
{
    int iSize;

    if(pLoop)
    {
        /* how many bytes to the beginning of loop */
        iSize = pNode->iIndex - pLoop->iIndex;

        if(iSize < 32)
        {
            ByteCodeAppendByte( (BYTE)(OPCODE_JUMP_BW_S + iSize), pStorage );
        }
        else if(iSize < 256)
        {
            ByteCodeAppendByte( (BYTE) OPCODE_JUMP_BW, pStorage );
            ByteCodeAppendByte( (BYTE) iSize, pStorage );
        }
        else
        {
            ByteCodeAppendByte( (BYTE)OPCODE_JUMP_BW_W, pStorage );
            ByteCodeAppendWord( (WORD) iSize, pStorage );
        }
    }
}




/*********************************************************************

 FUNCTION     : UpdateBreak

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct CodeNode* pNode
        struct StorageStruct* pStorage

 DESCRIPTION:
        Update break jump offset.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void UpdateBreak(struct CodeNode* pNode, struct StorageStruct* pStorage)
{
    /* break nodes linked to pRight */
    while(pNode->pRight)
    {
        pNode = pNode->pRight;

        /* jump to current location */
        UpdateJUMP(pNode,pStorage);
    }

}





/*********************************************************************

 FUNCTION     : LoadConst

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        int index
        struct StorageStruct* pStorage

 DESCRIPTION:   Loads constants to operand stack.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void LoadConst(int index, struct StorageStruct* pStorage)
{

    /* load const to operand stack */
    if(index < 16)
    {
        ByteCodeAppendByte( (BYTE)(OPCODE_LOAD_CONST_S + index), pStorage); 
    }   
    else if(index < 256)
    {
        ByteCodeAppendByte((BYTE)OPCODE_LOAD_CONST, pStorage);
        ByteCodeAppendByte((BYTE)index, pStorage);
    }
    else
    {
        ByteCodeAppendByte((BYTE)OPCODE_LOAD_CONST_W, pStorage);
        ByteCodeAppendWord((WORD) index, pStorage);
    }
}




/*********************************************************************

 FUNCTION     : LoadVar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        int index
        struct StorageStruct* pStorage

 DESCRIPTION:   Loads variables to operand stack.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void LoadVar(int index, struct StorageStruct* pStorage)
{
    /* load variable to operand stack */
    if(index < 32)
    {
        ByteCodeAppendByte( (BYTE)(OPCODE_LOAD_VAR_S + index), pStorage);   
    }   
    else
    {
        ByteCodeAppendByte((BYTE)OPCODE_LOAD_VAR, pStorage);
        ByteCodeAppendByte((BYTE)index, pStorage);
    }
}




/*********************************************************************

 FUNCTION     : StoreVar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        int index
        struct StorageStruct* pStorage

 DESCRIPTION:   Stores variable to operand stack.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void StoreVar(int index, struct StorageStruct* pStorage)
{
    /* store variable to operand stack */
    if(index < 16)
    {
        ByteCodeAppendByte( (BYTE)(OPCODE_STORE_VAR_S + index), pStorage);  
    }   
    else
    {
        ByteCodeAppendByte((BYTE)OPCODE_STORE_VAR, pStorage);
        ByteCodeAppendByte((BYTE)index, pStorage);
    }
}




/*********************************************************************

 FUNCTION     : IncrVar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        int index
        struct StorageStruct* pStorage

 DESCRIPTION:   Stores increment variable into operand stack.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void IncrVar(int index, struct StorageStruct* pStorage)
{
    if(index < 8)
        ByteCodeAppendByte((BYTE)(OPCODE_INCR_VAR_S + index), pStorage);
    else
    {
        ByteCodeAppendByte((BYTE)OPCODE_INCR_VAR, pStorage);
        ByteCodeAppendByte((BYTE)index, pStorage);
    }
}



/*********************************************************************

 FUNCTION     : DecrVar

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        int index
        struct StorageStruct* pStorage

 DESCRIPTION:   Stores decrement variable into operand stack.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void DecrVar(int index, struct StorageStruct* pStorage)
{
    ByteCodeAppendByte((BYTE)OPCODE_DECR_VAR, pStorage);
    ByteCodeAppendByte((BYTE)index, pStorage);
}





