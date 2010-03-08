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
/* File    : U:\wpsvob\wmlscript\work\ported\codenode.h
   Version : \main\7
   Date    : 10-Jan-00.04:23:18
*/

/*
 * ========================================================================
 *  Name        : codenode.h
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

      Definitions and interface function declarations for adding nodes to
      tree according the Bison parser.

-----------------------------------------------------------------------------
*/
#ifndef CODENODE_H
#define CODENODE_H


#include "common.h"


#define STATUS_LOAD     0x01    /* load to operand stack */
#define STATUS_LOADED   0x02    /* already loaded in operand stack */
#define STATUS_POST     0x04    /* incr or decr is post prefixed */
#define STATUS_IF_BREAK 0x08    /* break for if block */
#define STATUS_NO_LOAD  0x10    /* expression list node is for-loop end expression */

#define NODE_DEAD   0
#define NODE_ALIVE  1

typedef enum
{
    NODE_TYPE_EMPTY,
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_LIST,
    NODE_TYPE_EXPRESSION_LIST,
    NODE_TYPE_ARGUMENT_LIST,
    NODE_TYPE_STATEMENT_LIST,
    NODE_TYPE_ID,
    NODE_TYPE_LITERAL,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_VARIABLE_DECLARATION,
    NODE_TYPE_BINARY,
    NODE_TYPE_ASSIGNMENT,
    NODE_TYPE_CALL_LOCAL,
    NODE_TYPE_CALL_LIBRARY,
    NODE_TYPE_CALL_EXTERNAL,
    NODE_TYPE_INCR_DECR,
    NODE_TYPE_RETURN,
    NODE_TYPE_UNARY,
    NODE_TYPE_CONTINUE,
    NODE_TYPE_BREAK,
    NODE_TYPE_IF,
    NODE_TYPE_LOOP,
    NODE_TYPE_LOOP_END,
    NODE_TYPE_BLOCK,
    NODE_TYPE_TJUMP,
    NODE_TYPE_SCOR,
    NODE_TYPE_SCAND,

    NODE_TYPE_LAST
} NodeType;



struct CodeNode
{
    struct CodeNode*    pParent;    /* points to parent node */
    struct CodeNode*    pLeft;      /* points to left side  */
    struct CodeNode*    pRight;     /* points to right side */

    BYTE                bAlive;     /* 0 = dead, 1 = alive */
    BYTE                bNodeType;  /* enumered NodeType */
     
    BYTE                bStatus;    /* status flag containing defined status value(s) */
    BYTE                bOpCode;    /* opCode for the instruction */
    INT32               iIndex;     /* variable, const or function index */
    INT16               iSize;      /* size of the code below node */
    INT16               iLine;      /* line number, where code is */
};


/* common */
void    KillNode(struct CodeNode* pNode);
struct CodeNode* GetEmptyNode(struct Environment* pEnv); 

struct CodeNode* GetIdFunction(struct Environment* pEnv, char* pcName); 
struct CodeNode* GetIdLibrary(struct Environment* pEnv, char* pcName); 
struct CodeNode* GetIdLibraryFunction(struct Environment* pEnv, char* pcName); 

struct CodeNode* AddNodeToList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast);
struct CodeNode* AddNodeToExpressionList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast);
struct CodeNode* AddNodeToArgumentList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast);
struct CodeNode* AddNodeToStatementList(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pLast);

/* variables */
struct CodeNode* GetVariableDeclaration(struct Environment* pEnv, char* pcVal);
struct CodeNode* GetVariable(struct Environment* pEnv, char* pcVal);


/* literals  */
struct CodeNode* GetLiteral(struct Environment* pEnv, char* pcVal, INT16 iID);
struct CodeNode* GetLiteralMeta(struct Environment* pEnv, char* pcVal);


/* assignments */
struct CodeNode* GetAssignmentOperator(struct Environment* pEnv, INT16 iID);
struct CodeNode* GetAssignment(struct Environment* pEnv, struct CodeNode* pOperator, struct CodeNode* pVariable, struct CodeNode* pRight);


/* unary */
struct CodeNode* GetUnaryOperation(struct Environment* pEnv, struct CodeNode* pExpression, INT16 iID);
struct CodeNode* GetPrefix(struct Environment* pEnv, struct CodeNode* pVar, INT16 iID);
struct CodeNode* GetPostfix(struct Environment* pEnv, struct CodeNode* pVar, INT16 iID);


/* binary */
struct CodeNode* GetBinaryOperation(struct Environment* pEnv, struct CodeNode* pLeft, struct CodeNode* pRight, INT16 iID);


/* calls */
struct CodeNode* GetCallLocal(struct Environment* pEnv, struct CodeNode* pFunc, struct CodeNode* pArguments);
struct CodeNode* GetCallExternal(struct Environment* pEnv, struct CodeNode* pUrl, struct CodeNode* pFunc, struct CodeNode* pArguments);
struct CodeNode* GetCallLibrary(struct Environment* pEnv, struct CodeNode* pLib, struct CodeNode* pFunc, struct CodeNode* pArguments);

int AddFunction(struct Environment* pEnv, char* pcName, INT16 iID);
int AddFormalParameter(struct Environment* pEnv, char* pcName);
struct CodeNode* SetFunctionBody(struct Environment* pEnv, struct CodeNode* pBody);


/* statements */
struct CodeNode* GetStatementReturn(struct Environment* pEnv, struct CodeNode* pReturn);
struct CodeNode* GetStatementContinue(struct Environment* pEnv);
struct CodeNode* GetStatementBreak(struct Environment* pEnv);
struct CodeNode* GetStatementCondExpress(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pTrue, struct CodeNode* pFalse);
struct CodeNode* GetStatementIf(struct Environment* pEnv, struct CodeNode* pTrue, struct CodeNode* pFalse, struct CodeNode* pElse);
struct CodeNode* GetStatementWhile(struct Environment* pEnv, struct CodeNode* pExpression, struct CodeNode* pBody);
struct CodeNode* GetStatementFor(struct Environment* pEnv, struct CodeNode* pInit, struct CodeNode* pCond, struct CodeNode* pOperation, struct CodeNode* pBody);


/* pragmas */
int PragmaAccessControl(struct Environment* pEnv, struct CodeNode* pFirst, struct CodeNode* pSecond);
int PragmaMeta(struct Environment* pEnv, struct CodeNode* pName, struct CodeNode* pCont, struct CodeNode* pScheme, INT16 iID);


#endif
