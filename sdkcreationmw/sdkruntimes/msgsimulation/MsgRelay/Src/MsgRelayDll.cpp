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
#include <s32mem.h>     // RDesReadStream
#include <Gsmumsg.h>	// CSmsMessage
#include <gsmubuf.h>	// CSmsBuffer
#include <smut.h>		// KUidMsgTypeSMS
#include <msvuids.h>	// KUidMsvMessageEntry
#include <smsclnt.h>	// CSmsClientMtm
#include <mtclreg.h>	// CClientMtmRegistry
#include <mtmuibas.h>	// CBaseMtmUi
#include <txtrich.h>	// CRichText
#include <smutset.h>	// CSmsSettings
#include <smuthdr.h>	// CSmsHeader
#include <e32property.h> // RProperty 
#include "MsgRelay.h"

static const TInt KMaxSubjectLength = 30;

const TUid KPSUidIdleInformation = {0x102071C0};
const TUint32 KTelephonyIdleStatus = 0x00000001;
enum TPSTelephonyIdleStatus { EPSTelephonyNotIdle, EPSTelephonyIdle };
const TUid KPSUidAiInformation = {0x102750F0}; // ActiveIdle2 SID
const TUint KActiveIdleState = 0x00000002; // Contains one value from following emuneration 
enum EPSActiveIdleState { EPSAiBackground = 0,EPSAiForeground, EPSAiNumberEntry };

TVersion MessageRelayVersion = TVersion(1, 0, 0);

/****************************************************************************
*   CRelaySession
*****************************************************************************
*
*
*
****************************************************************************/
/* Implements actual functionality for clients. */
class CRelaySession : public CSession2
{
	CMessageRelay *iOwner;
	
	/* Is the session listening for messages. */
	TBool iListening;

	/* The port that is listened, if iListening is ETrue.
	   Port 0 means that port is not specified. */
	TUint iListenPort;

	/* The message type that is listened. */
	TMessageType iListenType;

	/* Notifies new messages to client...*/
	TRequestStatus *iListenStatus;

	/* Used for message passing. */
	RMessage2 iRcvMessage;

	void ConstructL();

	CRelaySession(CMessageRelay *aOwner);
public:
	
	static CRelaySession *NewL(CMessageRelay *aOwner);
	static CRelaySession *NewLC(CMessageRelay *aOwner);

	~CRelaySession();

	void ServiceL(const RMessage2& aMessage);

	/* If message is accepted ETrue is returned, otherwise EFalse. 
	   An accepted message is relayed to the client listening for the
	   messages or to the system Inbox(if no port specified). */
	TBool AcceptL(TPtr8 &aMessage, TUint aPort, TMessageType aType);

	/* Returns ETrue, is the port is already listened for. */
	TBool IsListening(TUint aPort, TMessageType aType);

};

CRelaySession::CRelaySession(CMessageRelay *aOwner) : 
CSession2(),
iOwner(aOwner),
iListening(EFalse)
	{

	}
	
void CRelaySession::ConstructL()
	{
	
	}
		
CRelaySession *CRelaySession::NewL(CMessageRelay *aOwner)
	{
	CRelaySession *self = CRelaySession::NewLC(aOwner);
	CleanupStack::Pop();
	return self;
	}

CRelaySession *CRelaySession::NewLC(CMessageRelay *aOwner)
	{
	CRelaySession *self = new (ELeave) CRelaySession(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRelaySession::~CRelaySession()
	{
	for( int i = 0; i < iOwner->iSessions->Count(); i++)
		{
		if( iOwner->iSessions->At( i ) == this)
			{
			iOwner->iSessions->Delete( i );
			break;
			}
		}
	}

/* Returns ETrue, is the port is already listened for. */
TBool CRelaySession::IsListening(TUint aPort, TMessageType aType)
	{
	if (iListening == EFalse)
		return EFalse;

	if (aType != iListenType)
		return EFalse;

	if (aPort != iListenPort)
		return EFalse;

	return ETrue;
	}

void CRelaySession::ServiceL(const RMessage2& aMessage)
	{
	HBufC8 *newMessage = NULL;
	TMessageType type;
	TUint port;
	
	switch (aMessage.Function())
		{
		case KNewMessage:
			{
			if (iOwner == NULL)
				User::Leave(KErrNotReady);

			if (iOwner->iWriter == NULL)
				User::Leave(KErrNotReady);

			newMessage = HBufC8::NewL(aMessage.Int0());
			TPtr8 ptr(newMessage->Des());
			aMessage.ReadL( 1, ptr ); 

			type = (TMessageType)aMessage.Int2();
			iOwner->iWriter->WriteMessageL(ptr, type);
			
			aMessage.Complete(KErrNone);	

			delete newMessage;

			break;
			}
    case KReceive:
      
      if (iListening) {
        if (!iRcvMessage.IsNull()) {
          iRcvMessage.Complete(KErrCancel);
        }
        iRcvMessage = aMessage;
			  iOwner->TraverseInboxL();
      }
      else {
        aMessage.Complete(KErrNotReady);
      }
      break;
    case KCancelReceive:
      if (!iRcvMessage.IsNull()) {
        iRcvMessage.Complete(KErrCancel);
      }
      aMessage.Complete(KErrNone);
      break;
		case KStartListening:

			if (iOwner == NULL)
				User::Leave(KErrNotReady);

			type = (TMessageType)aMessage.Int1();
			port = (TUint)aMessage.Int2();

			if (iOwner->IsAnyBodyListening(port, type) != EFalse)
				{
				aMessage.Complete(KErrInUse);
				return;
				}

			iListenType = type;
			iListenPort = port;
			iListening = ETrue;
      aMessage.Complete(KErrNone);	    	
			break;

		case KStopListening:
			iListening = EFalse;
      if (!iRcvMessage.IsNull()) {
        iRcvMessage.Complete(KErrCancel);
      }
			aMessage.Complete(KErrNone);	
			break;

		default:
			User::Leave(KErrNotSupported);
		}

	}

/* If message is accepted ETrue is returned, otherwise EFalse. 
   An accepted message is relayed to the client listening for the
   messages. */
TBool CRelaySession::AcceptL(TPtr8 &aMessage, TUint aPort, TMessageType aType)
	{
	if (IsListening(aPort, aType) == EFalse)
		return EFalse;

	/* Write data to client */
  if (!iRcvMessage.IsNull()) {
    iRcvMessage.WriteL(0, aMessage);
    iRcvMessage.Complete(KErrNone);
  }
  
  return ETrue;
	}

/****************************************************************************
*   CMessageRelay
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CMessageRelay::CMessageRelay() : CServer2(EPriorityNormal)
	{

	}

EXPORT_C void CMessageRelay::ConstructL(CMessageWriter *aWriter)
	{
	iSessions = new (ELeave) CArrayFixFlat<CRelaySession *>(1);

	SetWriter(aWriter);

	}

EXPORT_C CMessageRelay *CMessageRelay::NewL(CMessageWriter *aWriter /*= NULL*/)
	{
	CMessageRelay *self = CMessageRelay::NewLC(aWriter);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMessageRelay *CMessageRelay::NewLC(CMessageWriter *aWriter /*= NULL*/)
	{
	CMessageRelay *self = new (ELeave) CMessageRelay();
	CleanupStack::PushL(self);
	self->ConstructL(aWriter);
	return self;
	}

EXPORT_C CMessageRelay::~CMessageRelay()
	{
	delete iSessions;
	}

/* Goes through all sessions and returns ETrue, is someone
   is listening the given port. */
TBool CMessageRelay::IsAnyBodyListening(TUint aPort, TMessageType aType)
	{
	TInt i = 0;

	for (i = 0; i < iSessions->Count(); i++)
		{
		if (iSessions->At(i)->IsListening(aPort, aType) != EFalse)
			return ETrue;
		}

	return EFalse;
	}

CSession2* CMessageRelay::NewSessionL(const TVersion& aVersion) const
	{
	if (!User::QueryVersionSupported(MessageRelayVersion,
                                     aVersion))
        {
		/* Wrong version. */
		User::Leave(KErrNotSupported);
        }

	CRelaySession *session = CRelaySession::NewL( (CMessageRelay *)this );

	iSessions->AppendL(session);

	return session;
	}

CSession2* CMessageRelay::NewSessionL(const TVersion& aVersion, const RMessage2&) const
	{
	return NewSessionL( aVersion );
	}

/* Starts the server. */
void CMessageRelay::StartRelayL()
	{
	CServer2::StartL(KRelayServiceName);
	} 

/* Call this method when a new message is received to relay it
   to possible listeners. Returns ETrue, if the message was 
   accepted. If the message was not accepted, it must not be
   destroyed, but it must be offered again, as soon as there
   is a listener for it. */
EXPORT_C TBool CMessageRelay::NewMessageL(TPtr8 &aMessage, TMessageType aType)
	{
	TInt i = 0;

	if (aType != ESmsMessage)
		User::Leave(KErrNotSupported);
    

		/* Extract the port and check if it's listened to. */
		RFs fs;
		fs.Connect();

		CSmsBuffer *smsBuf = CSmsBuffer::NewL();
		CSmsMessage *smsMsg	= CSmsMessage::NewL(fs, CSmsPDU::ESmsSubmit, smsBuf);
		
		RDesReadStream rs(aMessage);
		smsMsg->InternalizeL(rs);

		rs.Close();
		fs.Close();

		CSmsPDU &pdu = smsMsg->SmsPDU(); 

		TInt src, dst;

		TBool isPorts = pdu.ApplicationPortAddressing(dst, src);

		if (isPorts == EFalse)
			{
			/* Copy to inbox. */
			CopyMessageToSmsInboxL(aMessage);
			return ETrue;
           	}

		for (i = 0; i < iSessions->Count(); i++)
			{
			if (iSessions->At(i)->AcceptL(aMessage, dst, aType) != EFalse)
				return ETrue;
			}

		/* No listeners. Cache the message to Inbox. Inbox is traversed,
		   when new listener are created. */
		TPtr8 &aMessage2 = aMessage;   
		CopyMessageToSmsInboxL(aMessage2);
		return ETrue;
		}

/* Sets the message writer. Writers WriteMessageL(...) method
   is called when a client wants to send a message. */
EXPORT_C void CMessageRelay::SetWriter(CMessageWriter *aWriter)
	{
	iWriter = aWriter;
	}

/* Copies the message to inbox. */
void CMessageRelay::CopyMessageToSmsInboxL(TPtr8 &aMessage)
	{
	// Get the idle statuses from P&S
	TInt aiStatus(0);    
    TInt aiGetErr = RProperty::Get( KPSUidAiInformation,
									KActiveIdleState,
									aiStatus );
	TInt tiStatus(0);
	TInt tiGetErr = RProperty::Get( KPSUidIdleInformation,
									KTelephonyIdleStatus,
									tiStatus );
	
	_LIT(KIdleStatus,"MSGRelay -- aiState:%d aiGetErr:%d tiState:%d tiGetErr:%d");
	RDebug::Print(KIdleStatus, aiStatus, aiGetErr, tiStatus, tiGetErr);

	// Update telephony idle status to idle and active idle status 
	// to foreground if needed. Without these "New message" notifications
	// might not be visible in some cases.
	if( aiStatus != EPSAiForeground )
		{
		TInt setAiErr = RProperty::Set( KPSUidAiInformation, 
											KActiveIdleState, 
											EPSAiForeground );
		_LIT( KAISetErr,"MSGRelay -- setAiErr:%d" );
		RDebug::Print( KAISetErr, setAiErr );
		}

	if( tiStatus != EPSTelephonyIdle )
		{
		TInt setTiErr = RProperty::Set( KPSUidIdleInformation, 
											KTelephonyIdleStatus, 
											EPSTelephonyIdle );
		_LIT( KTISetErr,"MSGRelay -- setTiErr:%d" );
		RDebug::Print( KTISetErr, setTiErr );
		}

	RFs fs;

	User::LeaveIfError(fs.Connect());

	CSmsBuffer *smsBuf = CSmsBuffer::NewL();
	CSmsMessage *smsMessage = CSmsMessage::NewL(fs, CSmsPDU::ESmsSubmit, smsBuf);
	CMsvSession *iMsvSession = CMsvSession::OpenSyncL(*this);
	CClientMtmRegistry *registry = CClientMtmRegistry::NewL(*iMsvSession);

	RDesReadStream rs(aMessage);
	rs >> *smsMessage;
	rs.Close();

	TMsvEntry index;

	index.iMtm = KUidMsgTypeSMS;
	index.iType = KUidMsvMessageEntry;
	index.iServiceId = KMsvLocalServiceIndexEntryId;
	index.iDate.HomeTime();
	index.SetInPreparation(ETrue);

	TMsvSelectionOrdering ordering;
	CMsvEntry *entry = CMsvEntry::NewL(*iMsvSession, KMsvGlobalInBoxIndexEntryIdValue,
									   ordering);

	entry->CreateL(index);

	entry->SetEntryL(index.Id());

	CSmsClientMtm* mtm = (CSmsClientMtm*)registry->NewMtmL(KUidMsgTypeSMS);

	mtm->SetCurrentEntryL(entry);

	index = mtm->Entry().Entry();
	
	index.iDetails.Set(smsMessage->ToFromAddress());

	index.SetInPreparation(EFalse);

	index.SetSendingState(KMsvSendStateNotApplicable);
	index.iDate = smsMessage->Time();

	// Set the new and unread flags. Enables "New message" notifications.
	index.SetNew( ETrue );
	index.SetUnread( ETrue );

	CRichText &mtmBody = mtm->Body();
	mtmBody.Reset();

	HBufC *data = HBufC::NewLC(smsBuf->Length());
	TPtr ptr(data->Des());
	
	smsBuf->Extract(ptr, 0, smsBuf->Length());
	
	mtmBody.InsertL(0, ptr);

	if (ptr.Length() > KMaxSubjectLength)
		index.iDescription.Set(ptr.Left(KMaxSubjectLength));
	else
		index.iDescription.Set(ptr);

	mtm->RestoreServiceAndSettingsL();

	CSmsHeader &hdr = mtm->SmsHeader();

	CSmsSettings *sendOpt = CSmsSettings::NewL();
	CleanupStack::PushL(sendOpt);
	
	sendOpt->CopyL(mtm->ServiceSettings());
	sendOpt->SetDelivery(ESmsDeliveryImmediately);
	
	hdr.SetSmsSettingsL(*sendOpt);
	hdr.SetFromAddressL(smsMessage->ToFromAddress());

	CleanupStack::PopAndDestroy(sendOpt);

	CSmsMessage &newMsg = hdr.Message();

	newMsg.SetServiceCenterAddressL(smsMessage->ServiceCenterAddress());

	CSmsPDU &pdu = newMsg.SmsPDU();
	CSmsPDU &oldPdu = smsMessage->SmsPDU();

	pdu.SetAlphabet(oldPdu.Alphabet());

	TSmsDataCodingScheme::TSmsClass coding;
	TBool b = oldPdu.Class(coding);
	pdu.SetClass(b, coding);

	TInt dst = -1;
	TInt src = -1; 
	TBool is16;
	
	b = oldPdu.ApplicationPortAddressing(dst, src, &is16);

	if(b != EFalse)
		{
		pdu.SetApplicationPortAddressingL(b, dst, src, is16);
		}

	mtm->AddAddresseeL(smsMessage->ToFromAddress(), index.iDetails);

    CMsvEntry &newEntry = mtm->Entry();    
    newEntry.ChangeL(index);                

	mtm->SaveMessageL();                 

	CleanupStack::PopAndDestroy(1);

	}

static CSmsMessage *ConvertEntryToMessageL(TMsvEntry &aIndex, CMsvSession *aSession)
	{
	CClientMtmRegistry *registry = CClientMtmRegistry::NewL(*aSession);
	CleanupStack::PushL(registry);

	CSmsClientMtm* mtm = (CSmsClientMtm*)registry->NewMtmL(aIndex.iMtm);
	CleanupStack::PushL(mtm);

	mtm->SwitchCurrentEntryL(aIndex.Id());
	mtm->LoadMessageL();

	CSmsHeader &hdr = mtm->SmsHeader();

	CSmsMessage &msg = hdr.Message();

	HBufC8 *buf = HBufC8::NewLC(KMaxSmsBufferSize);
	TPtr8 ptr(buf->Des());
	RDesWriteStream ws(ptr);

	ws << msg;

	ws.Close();

	RFs fs;

	User::LeaveIfError(fs.Connect());

	CSmsBuffer *smsBuf = CSmsBuffer::NewL();
	CSmsMessage *newMsg = CSmsMessage::NewL(fs, CSmsPDU::ESmsSubmit, smsBuf);

	RDesReadStream rs(ptr);
	rs >> *newMsg;

	rs.Close();
	fs.Close();

	CleanupStack::PopAndDestroy(3);

	return newMsg;
	}

static void DeleteEntryFromInboxL(CMsvEntry *aEntry, CMsvSession *aSession)
    {
	TMsvEntry index = aEntry->Entry();
	TMsvSelectionOrdering sort = aEntry->SortType();
	sort.SetShowInvisibleEntries(ETrue);
	CMsvEntry* parentEntry = CMsvEntry::NewL(*aSession, index.Parent(), sort);
	CleanupStack::PushL(parentEntry);  
    
    TRAPD(err, parentEntry->DeleteL(index.Id()));

    if(err != KErrNone)
        {
        aSession->RemoveEntry(index.Id());
        }

	CleanupStack::PopAndDestroy(parentEntry);
    }

/* Traverses the inbox and sends messages to listeners. */
void CMessageRelay::TraverseInboxL()
	{
    TInt count = 0;
    CMsvEntrySelection* childrenEntrySelection;
    CMsvEntry* rootEntry;
    CMsvEntry* child;

	CMsvSession *iMsvSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(iMsvSession);

    rootEntry = iMsvSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    count = rootEntry->Count();
        
    childrenEntrySelection = rootEntry->ChildrenL();

    for(TInt i = 0; i < count; i++)
        {
        child = iMsvSession->GetEntryL((*childrenEntrySelection)[i]);
        TMsvEntry msvEntry = child->Entry();
        
		if (msvEntry.iMtm != KUidMsgTypeSMS)
			/* Not to us. */
			continue;

		CSmsMessage *smsMsg = ConvertEntryToMessageL(msvEntry, iMsvSession);
		
		HBufC8 *buf = HBufC8::NewLC(KMaxSmsBufferSize);
		TPtr8 ptr(buf->Des());
		RDesWriteStream ws(ptr);
		
		ws << *smsMsg;

		ws.Close();
        
		CSmsPDU &pdu = smsMsg->SmsPDU(); 

		TInt src, dst;

		TBool isPorts = pdu.ApplicationPortAddressing(dst, src);

		if (isPorts == EFalse)
			{
			/* No ports, no listeners, leave to inbox. */
            CleanupStack::PopAndDestroy(1); //buf
			continue;

			}

		for (i = 0; i < iSessions->Count(); i++)
			{
			if (iSessions->At(i)->AcceptL(ptr, dst, ESmsMessage) != EFalse)
				{
				/* Remove from inbox */
				DeleteEntryFromInboxL(child, iMsvSession);
				
				/* Inbox has changed, start traversing again. */
				rootEntry = iMsvSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
				count = rootEntry->Count();
				childrenEntrySelection = rootEntry->ChildrenL();
				
				i = -1; /* Will be incremented before testing... */
				break;
				}
			}

		CleanupStack::PopAndDestroy(1); //buf

        }
	CleanupStack::PopAndDestroy(1); // iMsvSesssion
	}

void CMessageRelay::HandleSessionEventL(TMsvSessionEvent, TAny*, 
										TAny*, TAny*)
	{
	}
/****************************************************************************
*   CMessageWriter
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CMessageWriter::ConstructL()
	{

	}

EXPORT_C CMessageWriter *CMessageWriter::NewL()
	{
	CMessageWriter *self = CMessageWriter::NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMessageWriter *CMessageWriter::NewLC()
	{
	CMessageWriter *self = new (ELeave) CMessageWriter;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CMessageWriter::~CMessageWriter()
	{

	}

/* Called when a message is received from client for sending. */
EXPORT_C void CMessageWriter::WriteMessageL(TPtr8 & /*aMessage*/, TMessageType /*aType*/)
	{
	/* Nothing to do, inherited class will override. */
	}

// End of file  
