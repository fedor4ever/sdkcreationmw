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



//  Include Files  

#include <e32std.h>         // GLDEF_C
#include <e32base.h>
#include "MsgSession.h"
#include "..\..\..\..\sdkcommonutils\util\Inc\sysini.h"

#define KInitialMessageBufferSize		1024

TVersion MessageRelayVersion = TVersion(1, 0, 0);


/****************************************************************************
*   CMessageNotifier
*****************************************************************************
*
*
*
****************************************************************************/
class CMessageNotifier : public CActive
{
	CMessageSession *iOwner;

	void ConstructL(CMessageSession *aOwner);
	
	CMessageNotifier();


public:
	static CMessageNotifier *NewL(CMessageSession *aOwner);
	static CMessageNotifier *NewLC(CMessageSession *aOwner);

	void RunL();
	void DoCancel();	

	void StartReceiveL();

	TRequestStatus *GetStatusPtr() { return &iStatus;}
};

CMessageNotifier::CMessageNotifier() :
CActive(EPriorityNormal)
	{

	}

void CMessageNotifier::ConstructL(CMessageSession *aOwner)
	{
	iOwner = aOwner;
	CActiveScheduler::Add(this);
	}

CMessageNotifier *CMessageNotifier::NewL(CMessageSession *aOwner)
	{	
	CMessageNotifier *self = CMessageNotifier::NewLC(aOwner);
	CleanupStack::Pop();
	return self;
	}

CMessageNotifier *CMessageNotifier::NewLC(CMessageSession *aOwner)
	{
	CMessageNotifier *self = new (ELeave) CMessageNotifier();
	CleanupStack::PushL(self);
	self->ConstructL(aOwner);
	return self;
	}



void CMessageNotifier::RunL()
{
	/* RunL(...) is called only when a message is received. */

	ASSERT(iOwner != NULL);
	ASSERT(iOwner->iListener != NULL);
	
  if (iStatus.Int() == KErrNone) {
    if (iOwner->iMessageBuffer->Length() > 0) {
	    iOwner->iListener->ReceiveMessageL(iOwner->iMessageBuffer->Des());
    }
    StartReceiveL();
  }
}


void CMessageNotifier::DoCancel()
{
  iOwner->SendReceive(KCancelReceive, TIpcArgs()); 
}

void CMessageNotifier::StartReceiveL()
{
    iStatus = KRequestPending;	
    SetActive();
    iOwner->iMessagePtr->Zero();
    TIpcArgs args(iOwner->iMessagePtr);
    iOwner->SendReceive(KReceive, args, iStatus);
}

/****************************************************************************
*   CMessageSession
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CMessageSession::CMessageSession() : 
RSessionBase()
	{

	}

EXPORT_C void CMessageSession::ConstructL()	
	{
	iMessageBuffer = HBufC8::NewL(KMaxSmsBufferSize);
	iMessagePtr = new (ELeave) TPtr8(iMessageBuffer->Des());

	iListener = NULL;
	iIncomingNotify = CMessageNotifier::NewL(this);
	}

EXPORT_C CMessageSession *CMessageSession::NewL()
	{
	CMessageSession *self = CMessageSession::NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMessageSession *CMessageSession::NewLC()
	{
	CMessageSession *self = new (ELeave) CMessageSession;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CMessageSession::~CMessageSession()
	{
	StopListeningL();
	delete iIncomingNotify;
	delete iMessageBuffer;
	delete iMessagePtr;
	}

/* Attaches to server. */
EXPORT_C void CMessageSession::AttachL()
	{
	User::LeaveIfError(CreateSession(KRelayServiceName, MessageRelayVersion));
	}
	
/* Sends a message. */
EXPORT_C void CMessageSession::SendMessageL(const TPtr8 &aMessage, TMessageType aType)
	{
	TIpcArgs args( aMessage.Length(), &aMessage, aType );
	User::LeaveIfError( SendReceive( KNewMessage, args ) );
	}

/* Listen messages of given type in a given port. One
   session may listen to one port only. */
EXPORT_C void CMessageSession::ListenMessagesL(TMessageType aType, TUint aPort)
	{
	ASSERT(iListener != NULL);
	TIpcArgs args( iMessagePtr, aType, aPort);
    User::LeaveIfError(SendReceive(KStartListening, args));
	iIncomingNotify->StartReceiveL();
	}

/* Stops listening message. */
EXPORT_C void CMessageSession::StopListeningL()
	{
	User::LeaveIfError(SendReceive(KStopListening, TIpcArgs()));
	iIncomingNotify->Cancel();
	}

/* Sets the message listener. Listener's ReceiveMessageL(...)
   is called, when a new message is received. */
EXPORT_C void CMessageSession::SetListener(CMessageListener *aListener)
	{
	iListener = aListener;
	}

/* Returns ETrue, is session set to listening state. */
EXPORT_C TBool CMessageSession::IsListening() const
	{
	if( iIncomingNotify->IsActive() )
		return ETrue;
	else 
		return EFalse;
	}

/* Returns ETrue, if file based SMS should be used.
   Reads the settings from c:\system\data\wma.ini file.
   File format:
	# Starts a comment line
	# Set UseSocketOutput=1 to use native sockets implementation,
	# UseSocketOutput=0 to use file based SMS messaging.
	UseSocketOutput=0								*/
EXPORT_C TBool CMessageSession::UseFileSMS()
	{
	/* The config file. */
	_LIT(KWMAIniFile, "c:\\system\\data\\wma.ini");
	

	/* The write mode; 0 == file (default), 1 == socket. */
	int mode = 0;
	CSysIni *ini = NULL;

	/* Open the config file. NewLC leaves, if the file is not found, in which
	   case we default to writing to file. */ 
	TRAPD(err, ini = CSysIni::NewL(KWMAIniFile));
	CleanupStack::PushL(ini);
	switch ( err )
	{
	case KErrNone:
	   if (ini->SeekBool("UseSocketOutput"))
	      mode = 1; // Use socket output
	   break;
	case KErrNotFound:
		mode = 0;
       break;
	default:
		User::Panic(_L("CMessageSession::UseFileSms() panic:"), err);
	} 
		
	//delete ini;
	CleanupStack::PopAndDestroy();

	if (mode == 1)
		return EFalse;
	else
		return ETrue;
	
	}

/****************************************************************************
*   CMessageListener
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CMessageListener::ConstructL()
	{

	}

EXPORT_C CMessageListener *CMessageListener::NewL()
	{
	CMessageListener *self = CMessageListener::NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMessageListener *CMessageListener::NewLC()
	{
	CMessageListener *self = new (ELeave) CMessageListener;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}	

EXPORT_C CMessageListener::~CMessageListener()
	{

	}

/* Called when a message is received. */
EXPORT_C void CMessageListener::ReceiveMessageL(const TPtr8 &/*aMessage*/)
	{

	}

// End of file  
