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
* Description:  ECMT protocol for MIDP debugging
 *
*/

#ifndef _ECMT_MIDP_DEBUG_PROTOCOL_H_
#define _ECMT_MIDP_DEBUG_PROTOCOL_H_

/*
 * Protocol is symmetric, except for OPEN and CONNECT
 * 
 *   UIDs: 0x1020D6CC (gateway)
 *         0x1020D6D2 (Ecmt plugin)
 * 
 *   Message structure: 5 bytes header following by opcode specific data.
 *   All header fields are sent in network (MSB first) byte order.
 *
 *   +--------+--------+--------+--------+
 *   |                SID                |
 *   +--------+--------+--------+--------+
 *   | OPCODE | Payload (optional)
 *   +--------+--------+-------->
 *
 *   SID (session id) - 4 bytes
 * 
 *     Identifies the client session. Each session may have a number of 
 *     connections associated with it, including (at least) the default
 *     control connection (its connection id is zero).
 *
 *   OPCODE - 1 byte
 * 
 *     Operation code. The following values are defined:
 * 
 *     1. OPEN (GW -> phone only)
 *     2. CLOSE
 *     3. SEND
 *     4. CONNECT (GW -> phone only)
 *     5. RESET
 *
 * Payload format for different kinds of packets: 
 *
 *   1. OPEN
 *
 *      No payload. This packet creates a new session with the specified
 *      SID. If a session with such SID already exists, it is destroyed
 *      and a new session is created. Creates the implicit control
 *      connection with connection id zero.
 *
 *   2. CLOSE
 *
 *      No payload. Terminates the session with the specified SID. It also
 *      terminates all the connections open within this session, including
 *      the default control connection.
 *
 *   3. SEND
 *
 *      Data packet. Payload format:
 *
 *      +--------+--------+--------+--------+
 *      |                CID                |
 *      +--------+--------+--------+--------+
 *      |                SEQ                |
 *      +--------+--------+--------+--------+
 *      | Data ...
 *      +--------+-------->
 *
 *      CID (connection id) - 4 bytes
 *
 *        Identifies the connection within this session. Zero means default
 *        control connection. Non-zero CIDs must first be be registered with
 *        the CONNECT message.
 *
 *      SEQ  (sequence number) - 4 bytes
 *    
 *        Incremented for each packet. Each connection has its own independent
 *        numbering sequence. SEQ field for the first data packet is zero.
 *
 *      The above two fields are following by packet data that are normally
 *      forwarded to a TCP socket.
 *
 *   4. CONNECT
 *
 *      Creates a new connection within the specified session. Payload format:
 *
 *      +--------+--------+--------+--------+
 *      |                CID                |
 *      +--------+--------+--------+--------+
 *      |      PORT       |
 *      +--------+--------+
 *
 *      CID (connection id) - 4 bytes
 *
 *        Identifies the connection within this session. Must be non-zero.
 *
 *
 *      PORT (port number) - 2 bytes
 *
 *        TCP port (on the handset) to connect to.
 *
 *   5. RESET
 *
 *      Resets the connection. Payload format:
 *
 *      +--------+--------+--------+--------+
 *      |                CID                |
 *      +--------+--------+--------+--------+
 *
 *      CID (connection id) - 4 bytes
 *
 *        Id of the connection to reset. Must be non-zero.
 */

/* common header offsets */
#define ECMT_MIDP_DEBUG_HEADER_SID_OFFSET 0
#define ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET 4
#define ECMT_MIDP_DEBUG_HEADER_SIZE 5

/* OPEN offsets */
#define ECMT_MIDP_DEBUG_OPEN_SIZE ECMT_MIDP_DEBUG_HEADER_SIZE

/* CLOSE offsets */
#define ECMT_MIDP_DEBUG_CLOSE_SIZE ECMT_MIDP_DEBUG_HEADER_SIZE

/* SEND offsets */
#define ECMT_MIDP_DEBUG_SEND_CID_OFFSET ECMT_MIDP_DEBUG_HEADER_SIZE
#define ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET (ECMT_MIDP_DEBUG_HEADER_SIZE+4)
#define ECMT_MIDP_DEBUG_SEND_DATA_OFFSET (ECMT_MIDP_DEBUG_HEADER_SIZE+8)
#define ECMT_MIDP_DEBUG_SEND_MIN_SIZE ECMT_MIDP_DEBUG_SEND_DATA_OFFSET
#define ECMT_MIDP_DEBUG_SEND_MAX_SIZE 2147483647

/* CONNECT offsets */
#define ECMT_MIDP_DEBUG_CONNECT_CID_OFFSET ECMT_MIDP_DEBUG_HEADER_SIZE
#define ECMT_MIDP_DEBUG_CONNECT_PORT_OFFSET (ECMT_MIDP_DEBUG_HEADER_SIZE+4)
#define ECMT_MIDP_DEBUG_CONNECT_SIZE (ECMT_MIDP_DEBUG_HEADER_SIZE+6)

/* RESET offsets */
#define ECMT_MIDP_DEBUG_RESET_CID_OFFSET ECMT_MIDP_DEBUG_HEADER_SIZE
#define ECMT_MIDP_DEBUG_RESET_SIZE (ECMT_MIDP_DEBUG_HEADER_SIZE+4)

/* Opcodes */
#define ECMT_MIDP_DEBUG_OPCODE_MIN 1  // minimum opcode
#define ECMT_MIDP_DEBUG_OPCODE_OPEN 1
#define ECMT_MIDP_DEBUG_OPCODE_CLOSE 2
#define ECMT_MIDP_DEBUG_OPCODE_SEND 3
#define ECMT_MIDP_DEBUG_OPCODE_CONNECT 4
#define ECMT_MIDP_DEBUG_OPCODE_RESET 5
#define ECMT_MIDP_DEBUG_OPCODE_MAX 5 // maximum opcode

#endif // _ECMT_MIDP_DEBUG_PROTOCOL_H_
