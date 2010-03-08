/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MSG_RELAY_H
#define MSG_RELAY_H

#include <e32std.h>
#include <e32base.h>
#include <msvapi.h>
#include "MsgTypes.h"

class CMessageWriter;
class CRelaySession;

class CMessageRelay : public CServer2, public MMsvSessionObserver
{
	friend class CRelaySession;

	CArrayFixFlat<CRelaySession *> *iSessions;
	CMessageWriter *iWriter;

	CSession2* NewSessionL(const TVersion& aVersion) const;
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2&) const;

	/* Goes through all sessions and returns ETrue, is someone
	   is listening the given port. */
	TBool IsAnyBodyListening(TUint aPort, TMessageType aType);

	/* Copies the message to inbox. */
	void CopyMessageToSmsInboxL(TPtr8 &aMessage);
	
	/* Traverses the inbox and sends messages to listeners. */
	void TraverseInboxL();

protected:

	IMPORT_C CMessageRelay();
	IMPORT_C void ConstructL(CMessageWriter *aWriter);

public:

	IMPORT_C static CMessageRelay *NewL(CMessageWriter *aWriter = NULL);
	IMPORT_C static CMessageRelay *NewLC(CMessageWriter *aWriter = NULL);
	
	IMPORT_C ~CMessageRelay();

	/* Starts the server. */
	IMPORT_C void StartRelayL(); 

	/* Call this method when a new message is received to relay it
	   to possible listeners. Returns ETrue, if the message was 
	   accepted. If the message as not accepted, it must not be
	   destroyed, but it must be offered again, as soon as there
	   is a listener for it. */
	IMPORT_C TBool NewMessageL(TPtr8 &aMessage, TMessageType aType);

	/* Sets the message writer. Writers WriteMessageL(...) method
	   is called when a client wants to send a message. */
	IMPORT_C void SetWriter(CMessageWriter *aWriter);

	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, 
							 TAny* aArg2, TAny* aArg3);

};

class CMessageWriter
{

	IMPORT_C void ConstructL();

public:

	IMPORT_C static CMessageWriter *NewL();
	IMPORT_C static CMessageWriter *NewLC();

	IMPORT_C virtual ~CMessageWriter();

	/* Called when a message is received from client for sending. */
	IMPORT_C virtual void WriteMessageL(TPtr8 &aMessage, TMessageType aType);
};

#endif /* MSG_RELAY_H */
