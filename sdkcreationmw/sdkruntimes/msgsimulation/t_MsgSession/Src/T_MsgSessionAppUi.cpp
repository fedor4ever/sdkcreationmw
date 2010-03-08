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

#include "T_MsgSessionAppUi.h"        // CT_MsgSessionAppUi
#include "T_MsgSessionView.h"         // CT_MsgSessionView
#include <eikenv.h>             // CEikonEnv
#include <Gsmumsg.h>		// CSmsMessage
#include <gsmubuf.h>		// CSmsBuffer
#include <gsmuset.h> // For TSmsUserDataSettings
#include <smsuaddr.h> // For TSmsServiceCenterAddress

#include <eikon.hrh>            // Global command ids
#include "T_MsgSession.hrh"           // T_MsgSession command ids
#include <t_msgsession.rsg>           // Resource string ids
#include "T_MsgSession.pan"           // panic codes
#include "TestStates.h"
#include "MsgSession.h"


/****************************************************************************
*   CTestListener
*****************************************************************************
* 
*	Message listener for the test...
*
****************************************************************************/
class CTestListener : public CMessageListener
{
	TRequestStatus *iRequest;

protected:
	
	void ConstructL(TRequestStatus *aRequest);

public:

	static CTestListener *NewL(TRequestStatus *aRequest);
	static CTestListener *NewLC(TRequestStatus *aRequest);

	virtual ~CTestListener();

	/* Called when a message is received. */
	void ReceiveMessageL(const TPtr8 &aMessage);
};

void CTestListener::ConstructL(TRequestStatus *aRequest)
	{
	CMessageListener::ConstructL();
	
	iRequest = aRequest;
	}

CTestListener *CTestListener::NewL(TRequestStatus *aRequest)
	{
	CTestListener *self = CTestListener::NewLC(aRequest);
	CleanupStack::Pop();
	return self;
	}

CTestListener *CTestListener::NewLC(TRequestStatus *aRequest)
	{
	CTestListener *self = new (ELeave) CTestListener();
	CleanupStack::PushL(self);
	self->ConstructL(aRequest);
	return self;
	}	

CTestListener::~CTestListener()
	{

	}

/* Called when a message is received. */
void CTestListener::ReceiveMessageL(const TPtr8 &aMessage)
	{
	/* XXX TODO */
	TRequestStatus *stat = iRequest;
	User::RequestComplete(stat, KErrNone);
	}

/****************************************************************************
*   CTest
*****************************************************************************
* 
*	Includes the test cases, proceeds to new test in RunL(...).
*
****************************************************************************/
CTest::CTest() :
CActive(EPriorityNormal)
	{
	iState = EStarting;
	CActiveScheduler::Add(this);
	}

void CTest::RunL()
	{
	HBufC8 *msgBuf = NULL;
	RDesWriteStream ws;
	CSmsMessage *msg = NULL;
	
	switch (iState)
		{
		case EStarting:
			{
			/* Construct a SMS text message, send it. */
			
			iSession = CMessageSession::NewL();
			
			iSession->AttachL();

			msg = CreateMessageL(EFalse);

			msgBuf = HBufC8::NewL(KMaxSmsBufferSize);
			TPtr8 ptr(msgBuf->Des());
			ws.Open(ptr);
			ws << *msg;
			ws.Close();
			iSession->SendMessageL(msgBuf->Des(), ESmsMessage);
			delete msgBuf;

			iState = ETextMessageSent;
			
			TRequestStatus *req = Start();
			User::RequestComplete(req, KErrNone);

			break;
			}
		case ETextMessageSent:
			/* Text message has been succesfully sent, register listener
			   to receive it back. Test should copy the sent message to
			   smsin. */
			iListener = CTestListener::NewL(&iStatus);

			iSession->SetListener(iListener);

			iState = ETextMessageReceived;

			iStatus = KRequestPending;
			SetActive();
			iSession->ListenMessagesL(ESmsMessage, 1234);

			break;

		case ETextMessageReceived:
			{
			/* Text message has been received, check port and contents.
			   Construct a new binary message and send it. */
			msg = CreateMessageL(ETrue);

			msgBuf = HBufC8::NewL(KMaxSmsBufferSize);
			TPtr8 ptr(msgBuf->Des());
			ws.Open(ptr);
			ws << *msg;
			ws.Close();
			iSession->SendMessageL(msgBuf->Des(), ESmsMessage);
			delete msgBuf;

			iState = EBinaryMessageSent;

			TRequestStatus *req = Start();
			User::RequestComplete(req, KErrNone);

			break;
			}
		case EBinaryMessageSent:
			/* Binary message has been succesfully sent, register listener
			   to receive it back. Test should copy the sent message to
			   smsin. */
			/* XXX The same listener listens for binary messages, too. */
			iState = EBinaryMessageReceived;

			iStatus = KRequestPending;
			SetActive();
			break;

		case EBinaryMessageReceived:
			/* Binary message has been received, check port and contents. */
			iState = EDone;
			break;

		case EDone:
		default:

			break;
			
		}
	}

void CTest::DoCancel()
	{
	/* XXX TODO */
	}

TRequestStatus *CTest::Start()
	{
	SetActive();
	return &iStatus;
	}

CSmsMessage *CTest::CreateMessageL(TBool aBinary)
	{
	_LIT(KHostAddress, "+123456789");
	_LIT(KSCAddress, "+987654321");
	_LIT(KBinaryData, "1234567890");
	_LIT(KTextData, "Hello world!");

	RFs fs;

	User::LeaveIfError(fs.Connect());

	CSmsBuffer* smsBuffer = CSmsBuffer::NewL();
	CSmsMessage *smsMessage = CSmsMessage::NewL( fs, CSmsPDU::ESmsSubmit, smsBuffer );
	
	smsMessage->SmsPDU().SetServiceCenterAddressL(KSCAddress);
	smsMessage->SetToFromAddressL(KHostAddress);	
	smsMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, 1234, 1234, ETrue);

	smsBuffer->Reset();

	TSmsUserDataSettings smsSettings;

	if (aBinary)
		{
		smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
		smsBuffer->InsertL(0, KBinaryData);
		}
	else
		{
		smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
		smsBuffer->InsertL(0, KTextData);
		}

	smsSettings.SetTextCompressed(EFalse);
	smsSettings.SetTextConcatenated(ETrue, ETrue);
	smsMessage->SetUserDataSettingsL(smsSettings);

	smsMessage->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeConcatenation16BitReference);

	fs.Close();

	return smsMessage;
	}

/****************************************************************************
*   CT_MsgSessionAppUi
*****************************************************************************
*
*
*
****************************************************************************/
CT_MsgSessionAppUi::CT_MsgSessionAppUi() : CEikAppUi()
    {
    }

CT_MsgSessionAppUi::~CT_MsgSessionAppUi()
    {
    delete iView;
    }

void CT_MsgSessionAppUi::ConstructL()
    {
    BaseConstructL();

	iTest = new (ELeave) CTest();

    // create application view
    iView = new (ELeave) CT_MsgSessionView();
    iView->ConstructL(ClientRect(), iTest);

	TRequestStatus *req = iTest->Start();

	/* This effectively starts the tests run. */
	User::RequestComplete(req, KErrNone);

    // enable key event handling
    AddToStackL(iView);
    }

void CT_MsgSessionAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
    case EEikCmdFileNew:
        HandleFileNewL();
        break;

    case EEikCmdFileSave:
       HandleFileSaveL();
       break;

    //
    // add commands here
    //

    case EEikCmdExit:
        Exit();
        break;

    default:
        __ASSERT_DEBUG(EFalse, Panic(ET_MsgSessionUnknownCommand));
        iEikonEnv->InfoMsg(R_NOT_IMPLEMENTED);
        break;
        }
    }

void CT_MsgSessionAppUi::HandleFileNewL()
    {
    //
    // add code here
    //
    }

void CT_MsgSessionAppUi::HandleFileSaveL()
    {
    //
    // add code here
    //
    }
// End of file
