/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants that define the binary message format
*
*/



#ifndef CECMTMESSAGEFORMAT_H
#define CECMTMESSAGEFORMAT_H

/**
*  From Series 60 Platform - Configuration and Monitoring Tool Message Protocol
*  
*  3.3	Message Format
*  The format of the messages that the protocol exchanges with the underlying 
*  transport layer is described by the following Extended BNF [2] syntax:
*  
*  message= header, body;
*  
*  header = message length, client uid;
*  
*  message length = hexadecimal integer;
*  
*  client uid = hexadecimal integer;
*  
*  body = byte, {byte};
*  
*  hexadecimal integer = '0', 'x', 8 * hexadecimal character;
*  
*  hexadecimal character = '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'a'|'b'|'c'|'d'|'e'|'f';
*  The message length is the length in bytes of the whole message including the body.
*  
*  The following is an example of a message to UID 0x0105ADAF:
*  0x000000220x0105adafHello there!!!
*  
*  The presented message format is easy to handle since it consist of a fixed size header 
*  that contains the data necessary to receive the actual payload of the message. 
*  Arguably, the header could be presented in binary and thus 12 bytes could be saved. 
*  However, this would result in a loss of human readability which is of great value 
*  in debugging since typical payload is plain text data.
*
*  @ingroup EcmtCore
*  
**/


const TInt KEcmtMessageHeaderLength = 20;
const TInt KEcmtMessageHexNumberLength = 10;
const TInt KEcmtMessageHexDigits = 8;

#endif

// End of File
