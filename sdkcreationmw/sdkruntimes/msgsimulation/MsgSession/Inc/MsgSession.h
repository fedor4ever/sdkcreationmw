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


#ifndef MSG_SESSION_H
#define MSG_SESSION_H

#include <e32std.h>
#include "MsgTypes.h"

class CMessageListener;
class CMessageNotifier;

class CMessageSession : public RSessionBase
{
	friend class CMessageNotifier;

	/* Request status for notifying incoming messages. */
	CMessageNotifier *iIncomingNotify;

	CMessageListener *iListener;
	
	/* Message buffer for new messages. */
	HBufC8 *iMessageBuffer;
	TPtr8 *iMessagePtr;

	IMPORT_C void ConstructL();
	
	IMPORT_C CMessageSession();
public:

	IMPORT_C static CMessageSession *NewL();
	IMPORT_C static CMessageSession *NewLC();

	IMPORT_C ~CMessageSession();

	/* Attaches to server. */
	IMPORT_C void AttachL();
	
	/* Sends a message. */
	IMPORT_C void SendMessageL(const TPtr8 &aMessage, TMessageType aType);

	/* Listen messages of given type in a given port. One
	   session may listen to one port only. 
	   Message listener MUST be set before calling ListenMessageL(...). 
	   Only one type/port may be listened at a time. */
	IMPORT_C void ListenMessagesL(TMessageType aType, TUint aPort);

	/* Stops listening message. */
	IMPORT_C void StopListeningL();

	/* Sets the message listener. Listener's ReceiveMessageL(...)
	   is called, when a new message is received. */
	IMPORT_C void SetListener(CMessageListener *aListener);

	/* Returns ETrue, is session set to listening state. */
	IMPORT_C TBool IsListening() const;

	/* Returns ETrue, if file based SMS should be used.
	   Reads the settings from c:\system\data\wma.ini file.
	   File format:
		# Starts a comment line
		# Set UseSocketOutput=1 to use native sockets implementation,
		# UseSocketOutput=0 to use file based SMS messaging.
		UseSocketOutput=0								*/
	IMPORT_C static TBool UseFileSMS();
};

class CMessageListener
{

protected:

	IMPORT_C void ConstructL();

public:

	IMPORT_C static CMessageListener *NewL();
	IMPORT_C static CMessageListener *NewLC();

	IMPORT_C virtual ~CMessageListener();

	/* Called when a message is received. */
	IMPORT_C virtual void ReceiveMessageL(const TPtr8 &aMessage);
};

#endif /* MSG_SESSION_H */