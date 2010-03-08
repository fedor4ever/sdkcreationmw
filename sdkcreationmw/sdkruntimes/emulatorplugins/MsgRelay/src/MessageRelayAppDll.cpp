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



#include <e32base.h>
#include <s32mem.h>
#include <Gsmumsg.h>	// CSmsMessage
#include <gsmubuf.h>	// CSmsBuffer
#include "MessageRelayAppDll.h"
#include "FolderScanner.h"
#include "MsgRelay.h"

_LIT(KSmsInboxLocation, "c:\\smsin\\");
_LIT(KSmsFileFilter, "*.sms");


_LIT(KSmsOutboxPath, "c:\\smsout\\");
_LIT(KMessageNamePart, "message");
_LIT(KMessageNameExt, ".sms");

/****************************************************************************
*   CSmsFileWriter
*****************************************************************************
*
*
*
****************************************************************************/

static void CreateUniqueFileNameL(RFs &aFs, 
							 const TDesC &aDir,
							 const TDesC &aStaticPart,
							 TFileName &aFileName,
							 const TDesC &aFileExt,
							 TBool aLeaveFile = ETrue)
	{
	TInt err = KErrAlreadyExists;
	TInt count = 0;
	RFile file;

	do
		{
		aFileName.Zero();
		aFileName.Append(aDir);
		aFileName.Append(aStaticPart);
		aFileName.AppendNum(count++);
		aFileName.Append(aFileExt);
		
		err = file.Create(aFs, aFileName, EFileWrite | EFileRead);
		}
	while (err == KErrAlreadyExists);

	User::LeaveIfError(err);

	file.Close();

	if (aLeaveFile == EFalse)
		User::LeaveIfError(aFs.Delete(aFileName));

	}

class CSmsFileWriter : public CMessageWriter
{

protected:

	CSmsFileWriter();
	
	void ConstructL();

public:
	static CSmsFileWriter *NewL();
	static CSmsFileWriter *NewLC();

	virtual ~CSmsFileWriter();

	void WriteMessageL(TPtr8 &aMessage, TMessageType aType);
};

CSmsFileWriter::CSmsFileWriter() :
CMessageWriter()
	{

	}

CSmsFileWriter *CSmsFileWriter::NewL()
	{
	CSmsFileWriter *self = CSmsFileWriter::NewLC();
	CleanupStack::Pop();
	return self;
	}

CSmsFileWriter *CSmsFileWriter::NewLC()
	{
	CSmsFileWriter *self = new (ELeave) CSmsFileWriter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSmsFileWriter::ConstructL()
	{
	}

CSmsFileWriter::~CSmsFileWriter()
	{
	}

void CSmsFileWriter::WriteMessageL(TPtr8 &aMessage, TMessageType aType)
	{
	if (aType != ESmsMessage)
		User::Leave(KErrNotSupported);

	RFs fs;
	TInt err;

    CleanupClosePushL(fs);	
	User::LeaveIfError(fs.Connect());

	TFileName fileName;
	RFileWriteStream ws;

	/* Create the SMS outbox directory, if necessary. */
	TEntry entry;
	err = fs.Entry(KSmsOutboxPath, entry);
	
	if (err == KErrNotFound)
		{
		err = fs.MkDirAll(KSmsOutboxPath);
		err = KErrNone; /* ??? */
		}

	User::LeaveIfError(err);

	CreateUniqueFileNameL(fs, KSmsOutboxPath, KMessageNamePart, fileName, KMessageNameExt);

	/* Convert TPtr8 message to CSmsMessage and externalize. */

	CSmsBuffer *smsBuf = CSmsBuffer::NewL();
	CSmsMessage *smsMessage = CSmsMessage::NewL(fs, CSmsPDU::ESmsSubmit, smsBuf);
	CleanupStack::PushL(smsMessage);

	RDesReadStream rs(aMessage);
    CleanupClosePushL(rs);	
	rs >> *smsMessage;
    CleanupStack::PopAndDestroy(); //rs

    CleanupClosePushL(ws);	
	User::LeaveIfError(ws.Open(fs, fileName, EFileWrite));
	ws << *smsMessage;
    ws.CommitL();
	ws.Close();
	CleanupStack::PopAndDestroy( ); // ws
	
	CleanupStack::PopAndDestroy( smsMessage ); //smsMessage
	CleanupStack::PopAndDestroy( ); // fs
	}

/****************************************************************************
*   CFolderObserver
*****************************************************************************
*
*
*
****************************************************************************/

class CFolderObserver : public CBase, public CFolderScanner_if
{
public:

	static CFolderObserver *NewL(CMessageRelay *aRelay);	
	virtual ~CFolderObserver();

protected:

	void FileFound(const TDesC& aFileName);
	void FileFoundL(const TDesC& aFileName);
	void ConstructL(CMessageRelay *aRelay);
	
private:

	CFolderObserver();
	
private:	

	CMessageRelay *iRelay;
	CFolderScanner *iSmsFolderScanner;
	RFs iFs;
};

CFolderObserver *CFolderObserver::NewL(CMessageRelay *aRelay)
	{
	CFolderObserver *self = new (ELeave) CFolderObserver;
	CleanupStack::PushL(self);
	self->ConstructL( aRelay );
	CleanupStack::Pop();
	return self;
	}

void CFolderObserver::ConstructL(CMessageRelay *aRelay)
	{
	iRelay = aRelay;
	iSmsFolderScanner = CFolderScanner::NewL( this, 
											  KSmsInboxLocation, 
										      KSmsFileFilter );
	iFs.Connect();
	}

CFolderObserver::CFolderObserver()
	{
	}
	
CFolderObserver::~CFolderObserver()
	{
	iFs.Close();
	delete iSmsFolderScanner;
	}

void CFolderObserver::FileFoundL(const TDesC &aFilename)
	{
	/* This is SMS */
	RFileBuf file;
	CleanupClosePushL(file);
	HBufC8 *bufPtr = HBufC8::NewL(KMaxSmsBufferSize);
	CleanupStack::PushL(bufPtr);

    User::LeaveIfError(file.Open(iFs, aFilename, EFileRead));

	RReadStream	rs(&file);
	CleanupClosePushL(rs);
    
	TPtr8 ptr(bufPtr->Des());
    ptr.FillZ(ptr.MaxLength());
    ptr.SetLength(0);
    TRAPD(err, rs.ReadL(ptr));
	switch(err)
	{
	case KErrNone:
	case KErrEof:
		break;
	default:
		iSmsFolderScanner->ContinueScan();
        CleanupStack::PopAndDestroy(3); //close rs & delete bufPtr & and closefile
        //delete bufPtr;
		User::Leave(KErrNone);	
	}
    
    TRAP(err, iRelay->NewMessageL(ptr, ESmsMessage));
    switch ( err )
        {
        case KErrNone:
            break;
        default:
//            User::Leave(err);
            User::Leave(KErrNone); // If the message was invalid, leave it in the msgin directory but skip it otherwise.
        }
	    
	CleanupStack::PopAndDestroy(3); //close rs & delete bufPtr & and closefile
	
	//delete file
	//TRAP(err, iFs.Delete(aFilename));
    User::LeaveIfError(iFs.Delete(aFilename));
	
	iSmsFolderScanner->ContinueScan();
	}
	
void CFolderObserver::FileFound(const TDesC &aFilename)
{
	TRAPD(err, FileFoundL(aFilename));
	switch(err)
	{
	case KErrNone:
		break;	// this is ok
	default:
		User::Panic(_L("CMessageRelayApplication::FileFound(), Unhandled error."), err);
	}
}
/****************************************************************************
*   CMessageRelayAppDll
*****************************************************************************
*
*
*
****************************************************************************/


CMessageRelayAppDll* CMessageRelayAppDll::NewL()
	{
	CMessageRelayAppDll* self = new (ELeave) CMessageRelayAppDll( );
	CleanupStack::PushL( self );
	self->ConstructL();
    CleanupStack::Pop();
	return self;
	}

CMessageRelayAppDll::CMessageRelayAppDll()
	{
	}
	
void CMessageRelayAppDll::ConstructL()
	{
	iMessageWriter = CSmsFileWriter::NewL();
	iMessageRelay = CMessageRelay::NewL( iMessageWriter );
	iFolderObserver = CFolderObserver::NewL( iMessageRelay );
	
	iMessageRelay->StartRelayL();
	}

CMessageRelayAppDll::~CMessageRelayAppDll()
	{
	delete iFolderObserver;
	delete iMessageRelay;
	delete iMessageWriter;
	}
