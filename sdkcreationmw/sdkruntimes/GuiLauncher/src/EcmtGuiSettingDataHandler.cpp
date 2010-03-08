/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides all neccessary data for CEcmtGuiSettingList.
*
*/


#include <features.hrh>
#include <commdb.h>
#include <aknsettingitemlist.h>
#include <f32file.h> 

#include "EcmtGuiSettingDataHandler.h"
#include "EcmtClientServerCommon.h"

_LIT8( KLinebreak8, "\n" );	


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingDataHandler::~CEcmtGuiSettingDataHandler()
    {
    }

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//    
CEcmtGuiSettingDataHandler::CEcmtGuiSettingDataHandler()
    {
    iSelectedCommsDbId = 0;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingDataHandler* CEcmtGuiSettingDataHandler::NewL()
    {
    CEcmtGuiSettingDataHandler* self = new (ELeave) CEcmtGuiSettingDataHandler;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingDataHandler::ConstructL()
    {    
    }

// ---------------------------------------------------------------------------
// Returns the number of supported connectivities. Possible ones are WLAN, 
// USB, and Bluetooth.
// ---------------------------------------------------------------------------
//   
TInt CEcmtGuiSettingDataHandler::NumberOfSupportedFeaturesL(  
                                        CFeatureDiscovery* aFeatTest )
    {
    CFeatureDiscovery* featTest = NULL;
    
    if( aFeatTest == NULL )
        featTest = CFeatureDiscovery::NewL();
    else
        featTest = aFeatTest;
        
    TInt count(0);
    if( featTest->IsSupported( KFeatureIdBt ) ) ++count;
    if( featTest->IsSupported( KFeatureIdUsb ) ) ++count;
    if( featTest->IsSupported( KFeatureIdProtocolWlan ) ) ++count;
    
    if( aFeatTest == NULL )
        delete featTest;
    
    return count;
    }

// ---------------------------------------------------------------------------
// Gets data for CAknSettingItemList.
// ---------------------------------------------------------------------------
//   
TInt CEcmtGuiSettingDataHandler::GetDataL(
                CArrayPtr<CAknEnumeratedText>& aEnumeratedTextArray, 
                CArrayPtr<HBufC>& aPoppedUpTextArray )
    {
    CFeatureDiscovery* featTest = CFeatureDiscovery::NewLC();  
        
    TInt count = NumberOfSupportedFeaturesL( featTest );
    
    // If there is no support for BT,USB, or WLAN, return
    if( count == 0 )
        {
        CleanupStack::PopAndDestroy( featTest );
        return KErrNotFound;    
        }
        
    //Add feature titles to given arrays based on feature supports
    if( featTest->IsSupported( KFeatureIdBt ) )    
    	AddTextL( aEnumeratedTextArray, aPoppedUpTextArray, KEcmtBluetooth8(), 0 );
    if( featTest->IsSupported( KFeatureIdProtocolWlan ) )    	
    	AddTextL( aEnumeratedTextArray, aPoppedUpTextArray, KEcmtWlan8(), 1 );    	
    if( featTest->IsSupported( KFeatureIdUsb ) )
    	AddTextL( aEnumeratedTextArray, aPoppedUpTextArray, KEcmtUsb8(), 2 );
    
    // Now we have to check do we have default bearer stored, if so modify 
    // it to be the selected one
    HBufC* defaultBearer = DefaultBearerL();
    
    if( defaultBearer )
        {        
        for( TInt i=0; i<aEnumeratedTextArray.Count(); ++i )
            {            
            CAknEnumeratedText* text = aEnumeratedTextArray.At(i);
            TPtrC value = text->Value()->Des();
            if( defaultBearer->Compare( value ) == KErrNone )
                {
                text->SetEnumerationValue(0);                  
                }
            else
                text->SetEnumerationValue(1);
            }
        delete defaultBearer;
        }
    
    CleanupStack::PopAndDestroy( featTest );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Adds item to the setting item arrays.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingDataHandler::AddTextL( 
                        CArrayPtr <CAknEnumeratedText>& aEnumeratedTextArray, 
                        CArrayPtr <HBufC>& aPoppedUpTextArray,
                        const TDesC8& aText,
                        TInt aIndex )
    {   	
    CAknEnumeratedText* enumeratedText = NULL;    
    //Convert from 8bit to 16bit
    HBufC* enumerated16 = HBufC::NewLC( aText.Length() );
    enumerated16->Des().Copy( aText );    
    //Ownership of numerated16 is given to enumeratedText
    enumeratedText = new (ELeave) CAknEnumeratedText( aIndex, enumerated16); 
    aEnumeratedTextArray.AppendL(enumeratedText);     
    CleanupStack::Pop( enumerated16 );
    
    HBufC* popped16 = enumerated16->AllocLC();
    //Ownership of popped16 is given to aPoppedUpTextArray
    aPoppedUpTextArray.AppendL(popped16);
    CleanupStack::Pop( popped16 );
    }

// ---------------------------------------------------------------------------
// Retrieves the default bearer from file c:\Data\EcmtBearer.dat. 
// Allocates title from heap.
// ---------------------------------------------------------------------------
//
HBufC* CEcmtGuiSettingDataHandler::DefaultBearerL() const
	{	        	
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );
    
    RFile file;
    
    TInt err=file.Open( fsSession, 
                        KEcmtDefBearerFileName, 
                        EFileStreamText|EFileRead|EFileShareAny );
                        
    if( err != KErrNone )
    	{
    	CleanupStack::PopAndDestroy();
    	return NULL;
    	}
    	
    CleanupClosePushL( file );
    TBuf8<20> bearerData;      
    file.Read(0, bearerData);
    
    CleanupStack::PopAndDestroy( 2 ); // fsSession and file
    
    TInt length = bearerData.Length();    
    if( length < 1 )
        return NULL;
        
    //Remove iapId for WLAN bearer, if existing
    TInt offset = bearerData.Find( KLinebreak8() );
    
    HBufC* buffer = NULL;
    if( offset != KErrNotFound )
        {
        //WLAN is the default
        TInt newLength = length-( length-offset );
        buffer = HBufC::NewL( newLength );
        buffer->Des().Copy( bearerData.Left( newLength ) );
        }
    else
        {
        //USB or bluetooth is the default
        buffer = HBufC::NewL( length );
        buffer->Des().Copy( bearerData );
        }        
      
    return buffer;
	}

// ---------------------------------------------------------------------------
// Getter for WLAN internet accesspoints. Gets WLAN IAP titles and database
// record IDs from Symbian comms database.
// ---------------------------------------------------------------------------
//
TInt CEcmtGuiSettingDataHandler::WlanIAPIDsL( CDesCArray& aIAP, 
                                                RArray<TUint32>& aId )
    {
    CCommsDatabase* commDB = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commDB );

    // Initialize the view
    CCommsDbTableView* commDBView = commDB->OpenTableLC( TPtrC( IAP ) );  
    
    User::LeaveIfError(commDBView->GotoFirstRecord());
      
    // Iterate thru all the records and add only WLAN bearers to the list 
    do {
        HBufC* bearerType = 
                commDBView->ReadLongTextLC( TPtrC(IAP_BEARER_TYPE) );
     
        if( bearerType->Des().Compare( TPtrC(LAN_BEARER) ) == KErrNone )
            {
            // Title of the IAP
            HBufC* iapName = 
                    commDBView->ReadLongTextLC( TPtrC(COMMDB_NAME) );
            
            // Record ID of the IAP
            TUint32 id(0);
            commDBView->ReadUintL( TPtrC(COMMDB_ID), id );  
            aId.Append( id );   
            aIAP.AppendL( *iapName );  
           
            CleanupStack::PopAndDestroy( iapName );   
            }             
        CleanupStack::PopAndDestroy( bearerType );
        }
    while(KErrNone == commDBView->GotoNextRecord());
    
    // Pop and destroy the IAP View
    CleanupStack::PopAndDestroy(commDBView);

    // Pop and destroy the database
    CleanupStack::PopAndDestroy(commDB);
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter for iSelectedCommsDbId. This commsDb recordID is stored to file 
// in SaveSelectionL, if user has selected WLAN.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingDataHandler::
                SetSelectedWLANIAPCommsDbId( TUint32 aSelectedCommsDbId )
    {
    iSelectedCommsDbId = aSelectedCommsDbId;
    }

// ---------------------------------------------------------------------------
// Saves user selection to the file c:\Data\EcmtBearer.dat. It saves the 
// title of connection and WLAN accespoint database record id, if WLAN is 
// selected.
// ---------------------------------------------------------------------------
//    
TInt CEcmtGuiSettingDataHandler::SaveSelectionL( const TDesC& aSelection )
    {
    //Get selected item, modify it to 8-bit
    HBufC8* selItem = HBufC8::NewLC( aSelection.Length() );
    selItem->Des().Copy( aSelection );
    
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );

    RFile file;
    TInt err=file.Replace( fsSession, KEcmtDefBearerFileName, 
                                        EFileStreamText|
                                        EFileWrite|
                                        EFileShareAny );
    if( err != KErrNone )
    	{    	
    	CleanupStack::PopAndDestroy( 2 ); // selItem, fsSession
       	return err;
    	}
    CleanupClosePushL( file );
    
    if( selItem->Des().Compare( KEcmtBluetooth8() ) == KErrNone )
        {
        //Bluetooth bearer
    	file.Write( KEcmtBluetooth8() );
    	}
    else if( selItem->Des().Compare( KEcmtWlan8() ) == KErrNone )
    	{
    	//WLAN bearer
    	file.Write( KEcmtWlan8() );
    	//Separate bearer name with line break
        file.Write( KLinebreak8() );
      
        _LIT8( KFormat, "%d" );
        TBuf8<5> intString;
        intString.Format( KFormat, iSelectedCommsDbId );
        file.Write( intString );             
    	}
   	else if( selItem->Des().Compare( KEcmtUsb8() ) == KErrNone )
       	{
       	//USB bearer
       	file.Write( KEcmtUsb8() );
       	}
    else
        {
        // Not supported bearer
        CleanupStack::PopAndDestroy( 3 );
        return KErrNotSupported;
        }
         
    CleanupStack::PopAndDestroy( 3 );
    
    return KErrNone;
    }
