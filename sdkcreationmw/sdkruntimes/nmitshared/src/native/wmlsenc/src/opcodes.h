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
/* File    : U:\wpsvob\wmlscript\work\ported\opcodes.h
   Version : \main\3
   Date    : 01-Sep-99.10:35:40
*/

/*
 * ========================================================================
 *  Name        : opcodes.h
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

      Byte code values for WMLScript instructions.

-----------------------------------------------------------------------------
*/

 #if !defined(OPCODES_H)
#define OPCODES_H


  /* Control Flow Instructions */
  #define  OPCODE_JUMP_FW_S  0x80
  #define  OPCODE_JUMP_FW    0x01
  #define  OPCODE_JUMP_FW_W  0x02
  #define  OPCODE_JUMP_BW_S  0xA0
  #define  OPCODE_JUMP_BW    0x03
  #define  OPCODE_JUMP_BW_W  0x04
  #define  OPCODE_TJUMP_FW_S  0xC0
  #define  OPCODE_TJUMP_FW    0x05
  #define  OPCODE_TJUMP_FW_W  0x06
  #define  OPCODE_TJUMP_BW    0x07
  #define  OPCODE_TJUMP_BW_W  0x08

  /* Function Call Instructions */
  #define  OPCODE_CALL_S  0x60
  #define  OPCODE_CALL    0x09
  #define  OPCODE_CALL_LIB_S  0x68
  #define  OPCODE_CALL_LIB    0x0A
  #define  OPCODE_CALL_LIB_W  0x0B
  #define  OPCODE_CALL_URL    0x0C
  #define  OPCODE_CALL_URL_W  0x0D


  /*Variable Access and Manipulation */
  #define  OPCODE_LOAD_VAR_S  0xE0
  #define  OPCODE_LOAD_VAR    0x0E
  #define  OPCODE_STORE_VAR_S  0x40
  #define  OPCODE_STORE_VAR    0x0F
  #define  OPCODE_INCR_VAR_S  0x70
  #define  OPCODE_INCR_VAR    0x10
  #define  OPCODE_DECR_VAR    0x11


  /*Access to Constants */
  #define  OPCODE_LOAD_CONST_S  0x50
  #define  OPCODE_LOAD_CONST    0x12
  #define  OPCODE_LOAD_CONST_W  0x13
  #define  OPCODE_CONST_0   0x14
  #define  OPCODE_CONST_1   0x15
  #define  OPCODE_CONST_M1  0x16
  #define  OPCODE_CONST_ES  0x17
  #define  OPCODE_CONST_INVALID  0x18
  #define  OPCODE_CONST_TRUE   0x19
  #define  OPCODE_CONST_FALSE  0x1A


    
  /*Arthmetic Instructions */
  #define  OPCODE_INCR  0x1B
  #define  OPCODE_DECR  0x1C
  #define  OPCODE_ADD_ASG  0x1D
  #define  OPCODE_SUB_ASG  0x1E
  #define  OPCODE_UMINUS  0x1F
  #define  OPCODE_ADD     0x20
  #define  OPCODE_SUB     0x21
  #define  OPCODE_MUL     0x22
  #define  OPCODE_DIV     0x23
  #define  OPCODE_IDIV     0x24
  #define  OPCODE_REM     0x25


  /*Bitwise Instructions */
  #define  OPCODE_B_AND   0x26
  #define  OPCODE_B_OR    0x27
  #define  OPCODE_B_XOR   0x28
  #define  OPCODE_B_NOT   0x29
  #define  OPCODE_B_LSHIFT     0x2A
  #define  OPCODE_B_RSSHIFT    0x2B
  #define  OPCODE_B_RSZSHIFT   0x2C


  /*Comparison Instrunctions */
  #define  OPCODE_EQ  0x2D
  #define  OPCODE_LE  0x2E
  #define  OPCODE_LT  0x2F
  #define  OPCODE_GE  0x30
  #define  OPCODE_GT  0x31
  #define  OPCODE_NE  0x32

  /*Logical Instructions */
  #define  OPCODE_NOT    0x33
  #define  OPCODE_SCAND    0x34
  #define  OPCODE_SCOR    0x35
  #define  OPCODE_TOBOOL    0x36

  /*Stack Instructions */
  #define  OPCODE_POP    0x37

  /*Access to Operand Type */
  #define  OPCODE_TYPEOF     0x38
  #define  OPCODE_ISVALID     0x39

  /*Function Return Instructions */
  #define  OPCODE_RETURN  0x3A
  #define  OPCODE_RETURN_ES    0x3B

  /*Miscellaneous Instructions */
  #define  OPCODE_DEBUG  0x3C


#endif
