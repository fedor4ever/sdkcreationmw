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
* Description:  Class to handle MMC Event simulation events
*
*/


// INCLUDE FILES
#include "ecmtmmcevent.h"				// CEcmtMMCEvent
#include "EcmtFile.h"					// for REctmFile
#include "EcmtSeekFile.h"				// for Scalable UI Ini files
#include "EcmtOsUtils.h"	// for W32 utils 
#include "ecmteventsimplugin.h"			// of the instance of this plugin
#include "eventwrapper.h"
#include <utf.h> 						// for CnvUtfConverter
#include "winsdrives.h" 				// this contains windows.h

// ============================= LOCAL FUNCTIONS ===============================
// none

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CEcmtMMCEvent::CEcmtMMCEvent
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CEcmtMMCEvent::CEcmtMMCEvent()
    {
    }


// -----------------------------------------------------------------------------
// CEcmtMMCEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtMMCEvent::ConstructL()
    {
    iFs.Connect();
    
    //instantiate the removable drive object 
    iWinsRemDrives = NULL;
    iWinsRemDrives = new CRemovableDriveList();
    User::LeaveIfNull( iWinsRemDrives );
    }

// -----------------------------------------------------------------------------
// CEcmtMMCEvent::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtMMCEvent* CEcmtMMCEvent::NewL()
    {
    CEcmtMMCEvent* self = new( ELeave ) CEcmtMMCEvent;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

    
// -----------------------------------------------------------------------------
// CEcmtMMCEvent::~CEcmtMMCEvent
// Destructor
// -----------------------------------------------------------------------------
//
CEcmtMMCEvent::~CEcmtMMCEvent()
    {
    delete iWinsRemDrives;
    iFs.Close();
    }


// -----------------------------------------------------------------------------
// CEcmtMMCEvent::OpenMMCDoor
// Public Method to set MMC card seem to be as removed
// -----------------------------------------------------------------------------
//
void CEcmtMMCEvent::OpenMMCDoor()
	{
	_LIT(KECust,"ecust.dll");
    const TInt KECustUidValue=0x1000008d;
    const TUid KECustUid={KECustUidValue};

	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	RLibrary library;
		// Dynamically load DLL
	User::LeaveIfError(library.Load(KECust));
	if (library.Type()[1] != KECustUid)
		{
		User::Leave(KErrGeneral);
		}
	
	TLibraryFunction MediaChangeCallBack = library.Lookup(6);
	MediaChangeCallBack();
	
	TLibraryFunction MediaDoorOpenPtr = library.Lookup(7);
	TBool* value = (TBool*) MediaDoorOpenPtr();
	*value = ETrue;
	library.Close();	
	fs.Close();
    }


// -----------------------------------------------------------------------------
// CEcmtMMCEvent::CloseMMCDoor
// Public method to set MMC card seem to be as inserted
// -----------------------------------------------------------------------------
//
void CEcmtMMCEvent::CloseMMCDoor()
	{
	//Release Mediadoor
	_LIT(KECust,"ecust.dll");
    const TInt KECustUidValue=0x1000008d;
    const TUid KECustUid={KECustUidValue};


	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	RLibrary library;
		// Dynamically load DLL
	User::LeaveIfError(library.Load(KECust));
	if (library.Type()[1] != KECustUid)
		{
		User::Leave(KErrGeneral);
		}

	TLibraryFunction MediaChangeCallBack = library.Lookup(6);
	MediaChangeCallBack();
	
	TLibraryFunction MediaDoorOpenPtr = library.Lookup(7);
	TBool* value = (TBool*) MediaDoorOpenPtr();
	*value = EFalse;

	library.Close();	
	fs.Close();
	}


// -----------------------------------------------------------------------------
// CEcmtMMCEvent::NumOfWinsRemovableDrives
// Public method to get number of removable drives 
// -----------------------------------------------------------------------------
//
TInt CEcmtMMCEvent::NumOfWinsRemovableDrives()
	{
	TInt retVal = 0;
	if (iWinsRemDrives)
		{
		iWinsRemDrives->UpdateRemDrives();
		retVal = iWinsRemDrives->NumOfDrives();
		}
	else
		{
		//this should panic because someone has set iWinsRemDrives to null
		//after successfull contruction
		User::Panic( KEcmtMMCEvent, KErrNoMemory );		
		}
	return retVal;
	}
	
	
// -----------------------------------------------------------------------------
// CEcmtMMCEvent::WinsRemovableDrivesL
// Public method to get descrpitor containing removable drives
// -----------------------------------------------------------------------------
//
void CEcmtMMCEvent::WinsRemovableDrivesL( TDes8& aBuff )
	{
	//update list to get number of drives up to date
	iWinsRemDrives->UpdateRemDrives();
	//allocate space for drive letters string
	char* drives = new char[sizeof(char)* 3* iWinsRemDrives->NumOfDrives()+1];
	User::LeaveIfNull( drives );
	//get letters
	iWinsRemDrives->GetRemovableDrives( drives );
	const TPtrC8 iWinsDrives( (const TUint8*) drives, (TInt) strlen(drives) );
	aBuff.Copy( iWinsDrives );
	//remove / from string
	TChar ch('\\');
	TInt pos=aBuff.Locate( ch );
	while ( pos != KErrNotFound )
	{
		aBuff.Replace( pos, 1, KBlank );
		pos=aBuff.Locate( ch );
	}
		
	delete [] drives;
	}



// -----------------------------------------------------------------------------
// CEcmtMMCEvent::GetEmulatedMMCPathL
// Public method to get MMC disk status from Fileserver
// -----------------------------------------------------------------------------
//
void CEcmtMMCEvent::GetMMCStatus( TDes8& aStatus ) const
	{
	TInt status = 0;
	status = iFs.CheckDisk( KMMCDrive16 );
	switch(status)
		{
			case KErrNone:
				{
				aStatus.Copy( KMMCInsert );
				break;
				}
			case KErrNotReady:
				{
				aStatus.Copy( KMMCRemove );
				break;
				}
			default:
				{
				aStatus.Copy( KMMCCorrupted );
				}
		}
	}



// -----------------------------------------------------------------------------
// CEcmtMMCEvent::SetEventL
// Set MMC event
// -----------------------------------------------------------------------------
//
void CEcmtMMCEvent::SetEventL( TDesC& aParams )
	{
	//aParams should be changed to 8 bit 
	HBufC8 *aParams8 = HBufC8::NewL( aParams.Length() );
	CleanupStack::PushL( aParams8 );
	TPtr8 ptrParams8 = aParams8->Des();
	//convert from unicode to utf8
	CnvUtfConverter::ConvertFromUnicodeToUtf8( ptrParams8, aParams );
	
	TLex8 lexer( ptrParams8 );
	
	TPtrC8 command = lexer.NextToken();
  	
	// if REMOVE
	//call OpenMMCDoor();
	//return
	if ( command == KMMCRemove )
		{
		OpenMMCDoor();
		}
	
	//if INSERT
	//call CloseMMCDoor();
	//return
	else if ( command == KMMCInsert )
		{
		CloseMMCDoor();
		}
			

	else
		{
		User::Leave( KErrNotSupported );
		}
						
	CleanupStack::PopAndDestroy(); //aParams8
	}
	
	
// -----------------------------------------------------------------------------
// CEcmtMMCEvent::SendMMCStatusLC
// Send MMC status
// -----------------------------------------------------------------------------
//
HBufC8* CEcmtMMCEvent::SendMMCStatusLC() 
	{
    // MMC Status
    TBuf8<2> mmcId;
    mmcId.AppendNum(EWrapperMMC);
	//length comes from longes status value
	//which can be REMOVE (6), INSERT (6) or CORRUPTED (9)
	TBuf8<9> bufStatus; 
	GetMMCStatus( bufStatus );
		
	//drive srting looks like "A: B: "
	HBufC8* drives = HBufC8::NewL(NumOfWinsRemovableDrives() * 3);
	CleanupStack::PushL(drives);
	TPtr8 ptrDrives = drives->Des();
		
	WinsRemovableDrivesL(ptrDrives);
	ptrDrives.TrimRight();
		
	//send status <STATUS><blank><WINS_DRIVES><blank><drives...><blank><emul_drive><blank><drive>
	HBufC8 *msg = HBufC8::NewL(mmcId.Length()			//<ID>
		+ KBlank().Length()      //<BLANK>
		+ bufStatus.Length()      //<STATUS>
		+ KBlank().Length()      //<BLANK>
		+ KWINSDrives().Length() //<WINS_DRIVES>
		+ KBlank().Length()      //<BLANK>
		+ drives->Length()       //<DRIVES> ...
		);
		
		CleanupStack::PushL(msg);
		TPtr8 msgPtr = msg->Des();
		msgPtr += mmcId;		 //<ID>
		msgPtr += KBlank;        //<BLANK>
		msgPtr += bufStatus;     //<STATUS>
		msgPtr += KBlank;        //<BLANK>
		msgPtr += KWINSDrives;   //<WINS_DRIVES>
		msgPtr += KBlank;        //<BLANK>
		msgPtr += drives->Des(); //<DRIVES> ...
		const TPtrC8 msgPtrC(msgPtr);
		CleanupStack::Pop(); //msg
		CleanupStack::PopAndDestroy();//drives
		CleanupStack::PushL(msg);//set back to cleanupstack
			
		//pass msg pointer as return value
		return msg;		
		
	}
// End of File