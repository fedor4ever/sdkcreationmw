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


#include "smsfilehandler.h"

// Literal Constants
_LIT(KPanicReadMessageL, "CSmsFileWriter::ReadMessageL(CMessage &msg)");
_LIT(KPanicCreateMessageL, "CSmsFileWriter::ReadMessageL(CMessage &msg)");
_LIT(KTeNumTooLong, "Telephone number too long");

// ============================================================================
//  C++ constructor
// ============================================================================
CSmsFileHandler::CSmsFileHandler() :
    CMessageWriter()
    {
    }

// ============================================================================
// static NewL constructor
// ============================================================================
CSmsFileHandler *CSmsFileHandler::NewL()
    {
	CSmsFileHandler *self = CSmsFileHandler::NewLC();
	CleanupStack::Pop();
	return self;
    }

// ============================================================================
// static NewLC constructor
// ============================================================================

CSmsFileHandler *CSmsFileHandler::NewLC()
    {
	CSmsFileHandler *self = new ( ELeave ) CSmsFileHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
    }

// ============================================================================
// ConstructL
// ============================================================================
void CSmsFileHandler::ConstructL()
    {
	User::LeaveIfError( iFs.Connect() );
    }

// ============================================================================
// C++ Destructor
// ============================================================================
CSmsFileHandler::~CSmsFileHandler()
    {
	iFs.Close();
    }

// ============================================================================
// Message file writer
// ============================================================================
void CSmsFileHandler::WriteMessageL(CMessage &msg, TMessageType aType)
    {

	if ( &msg == NULL )
		User::Leave( KErrNotReady );
	if ( aType != ESmsMessage )
		User::Leave( KErrNotSupported );

	TFileName fileName;

	RFileWriteStream ws;
// PushL ws in case of leave
	ws.PushL();

	TInt err;

	User::LeaveIfError( iFs.Connect() );

// Create the SMS outbox directory, if one does not exist
	TEntry entry;
	err = iFs.Entry( KSmsOutboxPath, entry );

	switch ( err )
	    {
	    case KErrNone:
		    break;
	    case KErrNotFound:
		    User::LeaveIfError( iFs.MkDirAll(KSmsOutboxPath) );
		    break;
	    default:
		    User::Leave( err );
	    }

	CreateUniqueFileNameL(
        iFs,
        KSmsOutboxPath,
        KMessageNamePart,
        fileName, KMessageNameExt );

	User::LeaveIfError( ws.Open( iFs, fileName, EFileWrite ) );

	CSmsMessage *smsMessage = CreateMessageL( msg );
    CleanupStack::PushL( smsMessage );
	ws << *smsMessage;
    ws.CommitL();
    ws.Release();
    //pop and  close ws
	CleanupStack::PopAndDestroy(2); // delete smsMessage and close ws
    }

// ============================================================================
// Message File Reader
// ============================================================================
void CSmsFileHandler::ReadMessageL( CMessage &msg )
    {
	RFileBuf file;
// push file in case of leave
	CleanupClosePushL(file);

	HBufC8 *bufPtr = HBufC8::NewL( KMaxSmsBufferSize );
//push bufPtr
	CleanupStack::PushL( bufPtr );
	TPtr8 ptr( bufPtr->Des() );
	User::LeaveIfError( iFs.Connect() );

	RReadStream	rs( &file );
//push rs
	rs.PushL();

	TFileName fileName;
    fileName.FillZ(fileName.MaxLength());
    fileName.SetLength(0);
	TRAPD(err, GetFileL( fileName) );
	TInt error = err;
	switch ( error )
	    {
	    case KErrNone:
		    if( file.Open( iFs, fileName, EFileRead ) != KErrNone )
			{
			    User::Leave( err );
			}
		    TRAP( err, rs.ReadL( ptr ) );
		    error = err;
		    switch(error)
		        {
		        case KErrNone:
			        // this is ok
			        break;
		        case KErrEof:
			        //this is ok file is read and end of file reached
			        break;
		        default:
			        User::Panic( KPanicReadMessageL, error );
		        }
		    break;
	case KErrNotFound:
		// this is ok there is just not files to read
		User::Leave( error ); // do cleanup
	default:
		User::Panic( KPanicReadMessageL, error );
	}

// pop and close rs
	rs.Release();
	CleanupStack::PopAndDestroy();

	CSmsBuffer *smsBuf = CSmsBuffer::NewL();
	CSmsMessage *smsMsg	= CSmsMessage::NewL( iFs, CSmsPDU::ESmsSubmit, smsBuf );
// push smsMsg
	CleanupStack::PushL( smsMsg );
	smsBuf->Reset();
// push drs
	RDesReadStream drs( ptr );
	drs.PushL();
	// Internalize "file"-message to CSmsMessage
	TRAP( err, smsMsg->InternalizeL( drs ) );
	switch ( err )
	    {
	    case KErrNone:
	    case KErrEof:
		    break;
	    default:
		    User::Leave( err );
	    }

// pop and close drs
	drs.Release();
	CleanupStack::PopAndDestroy();


	// read CSmsMessage values to CMessage

	// get Address for CMessage from CSmsMessage
	TGsmSmsTelNumber iParsedAddress;
	smsMsg->ParsedToFromAddress( iParsedAddress );
	TBuf<TGsmSmsTelNumberMaxLen> iTelNumber; 
	iTelNumber.FillZ(iTelNumber.MaxLength());
	iTelNumber= iParsedAddress.iTelNumber;
	__ASSERT_ALWAYS(iTelNumber.MaxLength() > ( iTelNumber.Length()),
		User::Panic(KTeNumTooLong, KErrOverflow));

	msg.setAddress( iTelNumber );


	// get Type of the message for CMessage from CSmsMessage
	TSmsUserDataSettings smsSettings;
	smsMsg->UserDataSettings( smsSettings );


	if ( smsSettings.Alphabet() ==  TSmsDataCodingScheme::ESmsAlphabet8Bit )
		//binary
		msg.setType( BINARY_MESSAGE );
	else
		msg.setType( TEXT_MESSAGE );

	// get TimeStamp of the CSmsMessage for CMessage
	msg.setTimeStamp( smsMsg->Time() );

	// get Payload of the CSmsMessage for CMessage
	HBufC* payloadBuf = HBufC::NewL( smsBuf->Length() );
// push payloadBuf
	CleanupStack::PushL( payloadBuf );
 	TPtr payloadPtr( payloadBuf->Des() );
	payloadPtr.FillZ( payloadPtr.MaxLength() );
	smsBuf->Extract( payloadPtr, 0, smsBuf->Length() );
    msg.setPayload(payloadPtr );


	CleanupStack::PopAndDestroy( payloadBuf ); // delete payloadBuf
	payloadBuf = NULL;
	CleanupStack::PopAndDestroy( smsMsg ); // delete smsMsg
	smsMsg = NULL;
	smsBuf=NULL;
	CleanupStack::PopAndDestroy( bufPtr ); // delete buf
	bufPtr = NULL;
	CleanupStack::PopAndDestroy();       // close file

    //delete file;
    iFs.Delete(fileName);
    }

// ============================================================================
// Create message CMessage -> CSmsMessage*
// ============================================================================
CSmsMessage *CSmsFileHandler::CreateMessageL( CMessage &msg )
    {
	_LIT( KSCAddress, "+987654321" ); // could get message from database
									  // but HC is used bacause this value
									  // is not cared on Java side

    _LIT( KPortSeparator, ":" );
	CSmsBuffer* smsBuffer = CSmsBuffer::NewL();
	CSmsMessage *smsMessage = CSmsMessage::NewL(
        iFs,
        CSmsPDU::ESmsSubmit,
        smsBuffer);

	smsMessage->SmsPDU().SetServiceCenterAddressL( KSCAddress );

    TPtrC16 toAddressAndPort = msg.getAddress();
    TInt index = toAddressAndPort.Find(KPortSeparator);
    TPtrC16 toPort;
    TPtrC16 toAddress;
    if (index != KErrNotFound)
        {
         __ASSERT_ALWAYS(index <  toAddressAndPort.Length(),
                User::Panic(KPanicCreateMessageL, KErrOverflow));
         toPort.Set(toAddressAndPort.Mid(index + 1,
             toAddressAndPort.Length() - index - 1));
         toAddress.Set(toAddressAndPort.Left(index));
        }
    else
        {
        toPort.Set(_L("1234"));
        toAddress.Set(toAddressAndPort);
        }

    HBufC *buf = toPort.AllocL();
    CleanupStack::PushL(buf);
    TRAPD( err, buf = buf->ReAllocL(buf->Length() + 1));
    TPtr portPtr = buf->Des();                 // set null termination
    switch ( err )
        {
        case KErrNone:
            portPtr.ZeroTerminate(); 
            break;
        default:
            User::Panic(KPanicCreateMessageL, err);
        }
    
    TLex portLex(portPtr);
    TUint port(0);
    portLex.Val(port, EDecimal);               // extract port number 

    CleanupStack::PopAndDestroy();             // delete buf

	smsMessage->SetToFromAddressL(toAddress);
	smsMessage->SmsPDU().SetApplicationPortAddressingL(
        ETrue,
        port,
        port,
        ETrue );
	smsBuffer->Reset();

	TSmsUserDataSettings smsSettings;

	if (isBinary(msg))
		{
		smsSettings.SetAlphabet( TSmsDataCodingScheme::ESmsAlphabet8Bit );
		smsBuffer->InsertL(0, msg.getPayload());
		}
	else
		{
		smsSettings.SetAlphabet( TSmsDataCodingScheme::ESmsAlphabetUCS2 );
		smsBuffer->InsertL(0, msg.getPayload());
		}

	smsSettings.SetTextCompressed( EFalse );
	smsSettings.SetTextConcatenated( ETrue, ETrue );
	smsMessage->SetUserDataSettingsL( smsSettings );

	smsMessage->OptimizeSettingsL(
        CSmsMessage::ESmsFlagOptimizeConcatenation16BitReference );

	return smsMessage;
    }

// ============================================================================
// Filename scanner
// ============================================================================
TInt CSmsFileHandler::GetFileL(TFileName &fileName)
    {
	CDir* iFileList = 0;

	// Perform scan
	TFindFile finder(iFs);
	TPtrC location( KSmsInboxLocation );
	TPtrC wildName( KSmsFileFilter );
	TRAPD(err, finder.FindWildByDir( wildName, location, iFileList ));
	switch (err)
        {
        case KErrNone:
            if(iFileList == NULL)
                User::Leave(KErrNotFound);
		    break;
        default:
		    User::Leave(err); // Empty inbox or some other error
        }
	
	iFileList->Sort(ESortByName);
	
	fileName.Append( KSmsInboxLocation );
 	fileName.Append( (*iFileList)[0].iName );
	delete iFileList;
	return KErrNone;
    }


// ============================================================================
// Unique file namer
// ============================================================================
void CSmsFileHandler::CreateUniqueFileNameL(
        RFs &aFs,
        const TDesC &aDir,
	    const TDesC &aStaticPart,
	    TFileName &aFileName,
	    const TDesC &aFileExt,
	    TBool aLeaveFile)
    {
	TInt err = KErrAlreadyExists;
	TInt count = 0;
	RFile file;
//push file
	CleanupClosePushL( file );

	do
		{
		aFileName.Zero();
		aFileName.Append( aDir );
		aFileName.Append( aStaticPart );
		aFileName.AppendNum( count++ );
		aFileName.Append( aFileExt );

		err = file.Create( aFs, aFileName, EFileWrite | EFileRead );
		}
	while ( err == KErrAlreadyExists );

	User::LeaveIfError(err);

// pop and close file
	CleanupStack::PopAndDestroy();

	if ( aLeaveFile == EFalse )
		User::LeaveIfError( aFs.Delete(aFileName) );
    }

// ============================================================================
/// Check if CMessage::iType is BINARY_MESSAGE
// ============================================================================
TBool CSmsFileHandler::isBinary( CMessage& msg )
    {
	TBool match = EFalse;

    TPtrC ptr( msg.getType() );
    if (ptr.Match( BINARY_MESSAGE ) != KErrNotFound)
        match = ETrue;
	
    return match;
        
    }

