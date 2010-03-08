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
* Description:  This is a container class for the EcmtGuiSettingsView. It has
*                one control, CAknEnumeratedTextPopupSettingItem.
*
*/


#include "EcmtGuiSettingList.h"
#include "EcmtGui.hrh"
#include "EcmtGuiSettingItem.h"
#include "EcmtClientServerCommon.h"

#include <apsettingshandlerui.h> 
#include <aknlistquerydialog.h> 
#include <EcmtGui.rsg>
#include <apaccesspointitem.h>
#include <apdatahandler.h> 


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingList::CEcmtGuiSettingList()
    {
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//        
CEcmtGuiSettingList::~CEcmtGuiSettingList()
    {
    if( iData )
        delete iData;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//    
void CEcmtGuiSettingList::ConstructL( const TRect& /*aRect*/ )
    {    
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//	
void CEcmtGuiSettingList::Draw(const TRect& /*aRect*/) const
    {        
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItemList.
// Creates setting item.
// ---------------------------------------------------------------------------
//
CAknSettingItem* CEcmtGuiSettingList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknSettingItem* settingItem = NULL;
    iValue = 0;
    
    switch ( aIdentifier )
        {              
        case EEcmtGuiSettingBearer:
            iSettingItem = new ( ELeave ) 
                        CEcmtGuiSettingItem( *this, aIdentifier, iValue );
            settingItem = iSettingItem;
            break;
               
        default:
            break;
        }
    
    return settingItem;    
    }
    
// ---------------------------------------------------------------------------
// From class CAknSettingItemList.
// Edits setting item data.
// ---------------------------------------------------------------------------
//     
void CEcmtGuiSettingList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
  ( *SettingItemArray() )[aIndex]->UpdateListBoxTextL();
  ( *SettingItemArray() )[aIndex]->StoreL();    

    }

// ---------------------------------------------------------------------------
// This function is called from CEcmtGuiSettingItem when user presses OK
// softkey from the setting page. If WLAN is selected it displays a list query
// of WLAN access points. 
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingList::SettingPageOkSelectedL( const TDesC8& aSelItemText )
    {    
    //Ok selected, check is WLAN selected, if so launch IAP query
    if( aSelItemText.Find( KEcmtWlan8() ) == KErrNotFound )
        {
        //WLAN is not selected
        return;
        }
           
    //So WLAN selected, ask access point
    TInt index = 0;
    CAknListQueryDialog* dialog = new (ELeave) CAknListQueryDialog( &index );    
    dialog->PrepareLC( R_ECMTGUI_LIST_QUERY );    
    CDesCArray* itemArray = new (ELeave) CDesCArrayFlat(5); 

    //Array for storing record ids from the commsDb
    RArray<TUint32> commsDbIDs;
 
    //Get WlAN IAP record IDs and titles.
    iData->WlanIAPIDsL( *itemArray, commsDbIDs );
   
    dialog->SetItemTextArray( itemArray );
    TBool isOkSelected = dialog->RunLD();
    
    if( isOkSelected )
        {
        //Store selected commsDbId to DataHandler.
        iData->SetSelectedWLANIAPCommsDbId( commsDbIDs[ index ] );
        }
    else
        {
        /*Cancel selected, do not let user from setting page. By this 
        way user is forced to select IAP after WLAN bearer selection.*/
        EditItemL( 0, EFalse );
        }
    
    commsDbIDs.Close();
    }

// ---------------------------------------------------------------------------
// Sets data items to the iSettingItem member. It uses 
// CEcmtGuiSettingDataHandler to retrieve the data.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingList::SetDataL()
    {
    if( !iSettingItem )
        return;
    
    iData = CEcmtGuiSettingDataHandler::NewL();
                
    CArrayPtr<CAknEnumeratedText>* enumeratedTextArray = NULL;
    CArrayPtr<HBufC>* poppedUpTextArray = NULL;
    
    TInt featCount = iData->NumberOfSupportedFeaturesL();
    if( featCount > 0 )
        {
        enumeratedTextArray = 
                    new (ELeave) CArrayPtrFlat<CAknEnumeratedText> (featCount);
        CleanupStack::PushL( enumeratedTextArray );
        poppedUpTextArray = 
                    new (ELeave) CArrayPtrFlat<HBufC> (featCount);
        CleanupStack::PushL( poppedUpTextArray );
        }
    else
        return;
    
    // Get data to arrays
    TInt err = iData->GetDataL( *enumeratedTextArray, *poppedUpTextArray ); 
    
    CleanupStack::Pop( poppedUpTextArray );
    CleanupStack::Pop( enumeratedTextArray );   
    
    if( err == KErrNotFound )
        {
        delete enumeratedTextArray;
        delete poppedUpTextArray;
        return;
        }
    
    // Set enumerated text arrays and give ownership to settin item object.        
    iSettingItem->SetEnumeratedTextArrays( enumeratedTextArray, poppedUpTextArray);
    iSettingItem->HandleTextArrayUpdateL();
    ((CAknEnumeratedTextSettingItem*)iSettingItem)->IndexFromValue(0);
    iSettingItem->UpdateListBoxTextL();    
    }
    
// ---------------------------------------------------------------------------
// Stores user selection to file for later use. It uses 
// CEcmtGuiSettingDataHandler to store the selection.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingList::StoreSelectionL()
    {
    iData->SaveSelectionL( iSettingItem->SettingTextL() ); 
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItemList.
// Handles the key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CEcmtGuiSettingList::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType )
    {
    // No action if not key event
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    // The code of key event is checked
    switch ( aKeyEvent.iCode ) 
        {
        // These events have no function in this application
        case EKeySpace:
        case EKeyRightArrow:         
        case EKeyLeftArrow:          
            break;

        default:
            CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
            break;
        }
    return EKeyWasConsumed;    
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// The function is called whenever SetExtent(), SetSize(), SetRect(), 
// SetCornerAndSize(), or SetExtentToWholeScreen() are called on the 
// control.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingList::SizeChanged()
    {
    if ( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }    
    }
