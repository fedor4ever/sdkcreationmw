/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Protocol constants
 *
*/

#ifndef _ECMT_MIDP_DEBUG_PROTOCOL_H_
#define _ECMT_MIDP_DEBUG_PROTOCOL_H_

#define ECMT_MIDP_DEBUG_HEADER_SIZE 9
#define ECMT_MIDP_DEBUG_HEADER_CID_OFFSET 0
#define ECMT_MIDP_DEBUG_HEADER_SEQ_OFFSET 4
#define ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET 8

#define ECMT_MIDP_DEBUG_OPCODE_OPEN 1
#define ECMT_MIDP_DEBUG_OPCODE_CLOSE 2
#define ECMT_MIDP_DEBUG_OPCODE_SEND 3

#endif // _ECMT_MIDP_DEBUG_PROTOCOL_H_
