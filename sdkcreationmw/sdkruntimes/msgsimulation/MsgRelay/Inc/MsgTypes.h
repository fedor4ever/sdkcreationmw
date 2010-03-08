/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#include <e32std.h>

typedef enum TMessageType { EUnknown, ESmsMessage, EMmsMessage } EMessageType ;

_LIT(KRelayServiceName, "MessageRelayService");

extern TVersion MessageRelayVersion;

/* Maximum size of externalized SMS message. */
#define KMaxSmsBufferSize	2048

/* Functions IDs. */

/* Send a message. Synchronous function. Leaves with error code, if fails.
   Parameters : 
   Index		Type			Desc
	0			TInt			Length of data
	1			TPtr8*			Message data
	2			TInt			TMessageType, type of the message (SMS) */
#define KNewMessage			1

/* Add a listener. 
   Parameters :
   Index		Type			Desc
	0			TPtr8*			Message data buffer for receiving
	1			TInt			TMessageType, type of the message (SMS) 
	2			TUint			The port to listen
	3			TRequestStatus* Pointer to request status to receive indication of message */
#define KStartListening		2

/* Stops listening. 
   Parameters : none */
#define KStopListening		3


#define KReceive          4

#define KCancelReceive    5

/* Type of the response for listening client. */
typedef enum TResponseState 
{
	/* The provided message buffer is too small. Client should
	   grow the buffer and start listening again. */
	EBufferOverflow,

	/* Message can be read from the buffer. */
	EMessageReady
} EResponseState;

#endif /* MESSAGE_TYPES_H */