/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "RegistrationListbox.h"
#include "RegistrationView.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRegistrationListbox::CreateSettingItemL()
// Updates descriptor array for list box
// ----------------------------------------------------------------------------
//
CAknSettingItem* CRegistrationListbox::CreateSettingItemL( TInt aIdentifier  )
    {

    CAknSettingItem* settingItem = NULL;

    switch ( aIdentifier )
        {
        case ERegistrationSettingName:
            settingItem = new ( ELeave ) CAknTextSettingItem( aIdentifier, 
                                                          iItemData->iName );
            break;
        
        case ERegistrationSettingEmail:
            settingItem = new ( ELeave ) CAknTextSettingItem( aIdentifier, 
                                                          iItemData->iEmail );
            break;
        
        case ERegistrationSettingPhone:
            settingItem = new ( ELeave ) CAknTextSettingItem( aIdentifier, 
                                                          iItemData->iPhone );
            break;

        case ERegistrationSettingDate:
            settingItem = new ( ELeave ) CAknTimeOrDateSettingItem( 
                  aIdentifier, CAknTimeOrDateSettingItem::EDate, iItemData->iDate );
            break;

        case ERegistrationSettingTime:
            settingItem = new ( ELeave ) CAknTimeOrDateSettingItem( 
                  aIdentifier, CAknTimeOrDateSettingItem::ETime, iItemData->iTime );
            break;

        case ERegistrationSettingGender:
            settingItem = new ( ELeave ) CAknBinaryPopupSettingItem( 
                                                  aIdentifier, iItemData->iGender );
            break;
        
        case ERegistrationSettingLogin:
            settingItem = new ( ELeave ) CAknTextSettingItem( aIdentifier, 
                                                          iItemData->iLogin );
            break;

        case ERegistrationSettingPassWord:
            settingItem = new ( ELeave ) CAknPasswordSettingItem( 
                     aIdentifier, CAknPasswordSettingItem::EAlpha, iItemData->iPw );
            break;

        case ERegistrationSettingSlider:
            settingItem = new ( ELeave ) CAknSliderSettingItem( 
                                aIdentifier, iItemData->iSliderValue );
            break;
    
        case ERegistrationSettingAccept:
            settingItem = new ( ELeave ) CAknBinaryPopupSettingItem( 
                                                  aIdentifier, iItemData->iAccept );
            break;

        default:
            break;
        }
    return settingItem;
    }
    
// ----------------------------------------------------------------------------
// CRegistrationListbox::EditItemL(TInt aIndex, TBool aCalledFromMenu)
// Edit data
// ----------------------------------------------------------------------------
//    
void CRegistrationListbox::EditItemL( TInt aIndex, TBool aCalledFromMenu )
  {
  CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
  ( *SettingItemArray() )[aIndex]->UpdateListBoxTextL();
  ( *SettingItemArray() )[aIndex]->StoreL();
  }

// ----------------------------------------------------------------------------
// CRegistrationListbox::SetData()
// Set ListItem data
// ----------------------------------------------------------------------------
//
void CRegistrationListbox::SetData( TRegistrationItemdata* aItemData )
    {
    iItemData = aItemData;
    }

// ----------------------------------------------------------------------------
// TKeyResponse CRegistrationListbox::OfferKeyEventL()
// Handles the key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CRegistrationListbox::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                       TEventCode aType  )
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

// ----------------------------------------------------------------------------
// CRegistrationListbox::SizeChanged()
// Called by framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CRegistrationListbox::SizeChanged()
    {
    if ( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }
    }

// End of File
