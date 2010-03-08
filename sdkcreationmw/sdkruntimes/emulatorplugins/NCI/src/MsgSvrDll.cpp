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


#include "msgsvr.h"
#include "smsfilehandler.h"
#include "message.h"
#include "msgsvreng.h"
#include "ncilogger.h"

// Console macros
#define _ENDL() NCI_DEBUG(KLineBreak);

//Literal constants
_LIT(KStartServer, "Starting NCIMsgServer\n");
_LIT(KAddress,"\n\tAddress: %S");
_LIT(KType,"\n\tType: %S");
_LIT(KTimeStamp,"\n\tTimeStamp: Year: %04d, month: %02d, day: %02d, Time: %02d:%02d:%02d.%06d");
_LIT(KPayload,"\n\tPayload: %S");
_LIT(KSendingMsg, "Processing send command:\n");
_LIT(KReceivingMsg, "Processing receive command:\n");
// _LIT(KMessageSucceed, "\tMessage Created successfully\n");
_LIT(KReceivedMessageSucceed, "\tMessage received successfully\n");
_LIT(KSentMessageSucceed, "\tMessage sent successfully\n");
_LIT(KErrorOnMsgProcess, "\tError occured while creating message - error: %d\n");
_LIT(KErrorNoMessages, "\tError occured while creating message - Outbox Empty\n");
_LIT(KLineBreak, "\n");



CMsgSvr::CMsgSvr()
    {
    }

CMsgSvr::~CMsgSvr()
    {
	delete iSvrEngine;
    delete iSmsFileHandler;
    }

void CMsgSvr::ConstructL()
    {
    iSmsFileHandler = CSmsFileHandler::NewL();
    iSvrEngine = CMsgSvrEng::NewL(this);
    User::LeaveIfError(iSvrEngine->StartL());
    }

CMsgSvr* CMsgSvr::NewLC()
    {
	CMsgSvr* self = new ( ELeave ) CMsgSvr();
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
    }

CMsgSvr* CMsgSvr::NewL()
    {
	CMsgSvr* self = CMsgSvr::NewLC();
	CleanupStack::Pop();
	return self;
    }

void CMsgSvr::ProcessSendL(CMessage &aMsg) const
    {
	//do sending;
	NCI_DEBUG(KSendingMsg);

        PrintMessage(aMsg);

	TRAPD(error, iSmsFileHandler->WriteMessageL(aMsg, ESmsMessage));

	switch(error)
	    {
	    case KErrNone:
		    NCI_DEBUG(KSentMessageSucceed);
		    // all OK
		    break;
	    default:
		    NCI_DEBUG_INT(KErrorOnMsgProcess, (TInt) error);
            User::Leave(error);
	    }
    }

void CMsgSvr::ProcessReceiveL(CMessage &aMsg)
    {
	//do receive
	NCI_DEBUG(KReceivingMsg);

	//read message from the file
	TRAPD(error, iSmsFileHandler->ReadMessageL(aMsg));

	switch(error)
	    {
	    case KErrNone:
		    PrintMessage(aMsg);
		    NCI_DEBUG(KReceivedMessageSucceed);
		    // all OK
		    break;
        case KErrNotFound:
            NCI_DEBUG(KErrorNoMessages);
		    User::Leave(error);
	    default:
		    NCI_DEBUG_INT(KErrorOnMsgProcess, (TInt) error);
		    User::Leave(error);
	    }
    }


void CMsgSvr::PrintMessage(CMessage &aMsg) const
    {
	HorizontalLine();
	NCI_DEBUG_STR(KAddress,aMsg.getAddress());
	NCI_DEBUG_STR(KType, aMsg.getType());
	TDateTime dtime = aMsg.getTimestamp().DateTime();
	
	TBuf<256> tStamp;
	tStamp.Format(KTimeStamp,
        dtime.Year(),
        dtime.Month(),    // Day and month ranges begin at zero (0-30 and 0-11),
        dtime.Day(),
        dtime.Hour(),
        dtime.Minute(),
        dtime.Second(),
        dtime.MicroSecond());
    NCI_DEBUG(tStamp);
	NCI_DEBUG_STR(KPayload, aMsg.getPayload());
	HorizontalLine();
    _ENDL();
    }

void CMsgSvr::HorizontalLine() const
    {
    _ENDL();
	NCI_DEBUG( _L("-----------------------------------------------------") );
    }

void CMsgSvr::WriteToConsole(const TDesC& aText)
    {
    NCI_DEBUG(aText);
    }
