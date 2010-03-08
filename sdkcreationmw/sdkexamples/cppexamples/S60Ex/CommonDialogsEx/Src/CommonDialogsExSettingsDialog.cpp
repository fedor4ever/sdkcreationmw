/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For settings dialog.
*
*/


#include <AknLists.h>
#include <CAknMemorySelectionSettingItem.h>
#include <CAknMemorySelectionSettingItemMultiDrive.h>
#include <CommonDialogsEx.rsg>

#include "CommonDialogsEx.hrh"
#include "CommonDialogsExSettingsDialog.h"


CCommonDialogsExSettingsDialog::~CCommonDialogsExSettingsDialog()
    {
    iSettingItemArray.ResetAndDestroy();
    }

CCommonDialogsExSettingsDialog* CCommonDialogsExSettingsDialog::NewL(TInt aResId, TBool aMultiDriveSupport)
    {
    CCommonDialogsExSettingsDialog* self = new( ELeave ) CCommonDialogsExSettingsDialog;
    CleanupStack::PushL( self );
    self->iResourceId = aResId;
    self->iMultiDriveSupport = aMultiDriveSupport;
    CleanupStack::Pop();
    return self;
    }

TBool CCommonDialogsExSettingsDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeySelect )
        {
        ProcessCommandL( aButtonId );
        return EFalse;
        }
    return ETrue;
    }

void CCommonDialogsExSettingsDialog::PreLayoutDynInitL()
    {
    iMemory = CAknMemorySelectionSettingPage::EPhoneMemory;
    iDriveNumber = EDriveC;

    CAknSettingStyleListBox* listBox = 
                            static_cast< CAknSettingStyleListBox* >
                            ( Control( ECommonDialogsExSettingsListboxId ) );
    User::LeaveIfNull( listBox );

    CAknSettingItem* settingItem;
    HBufC* itemTitle;

    // Create the memory selection item
    if ( iMultiDriveSupport )
        {    
        settingItem =
                new( ELeave ) CAknMemorySelectionSettingItemMultiDrive(
                                            iResourceId, iDriveNumber );
        }
    else
        {
        settingItem =
                new( ELeave ) CAknMemorySelectionSettingItem(
                                            iResourceId, iMemory );
        }
    CleanupStack::PushL( settingItem );

    // Set memory selection item's title
    itemTitle = iCoeEnv->AllocReadResourceLC( R_STRING_COMMONDIALOGSEX_SET_MEMORY );
    settingItem->ConstructL( EFalse, 0, *itemTitle, NULL, 0,
                                                    EAknCtPopupSettingList );
    CleanupStack::PopAndDestroy();  // itemTitle

    // Add memo store item to the settings array
    iSettingItemArray.AppendL( settingItem );
    CleanupStack::Pop();    // settingItem

    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( &iSettingItemArray );
    // Ownership retained by us
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iSettingItemArray.RecalculateVisibleIndicesL();
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listBox->HandleItemAdditionL();
    listBox->SetListBoxObserver( this );
    listBox->UpdateScrollBarsL();
    }

void CCommonDialogsExSettingsDialog::HandleListBoxEventL( CEikListBox* aListBox,
                                             TListBoxEvent aEventType )
    {
    TInt index( aListBox->CurrentItemIndex() );
    if ( index >=0 )    // index is -1 if there are no items in the list
        {
        iSettingItemArray.At( index )->EditItemL(
                         aEventType == EEventEditingStarted );
        aListBox->DrawItem( index );
        }
    }

void CCommonDialogsExSettingsDialog::ProcessCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case EAknSoftkeySelect:
            {
            CEikListBox* listBox = static_cast< CEikListBox* >
                            ( Control( ECommonDialogsExSettingsListboxId ) );
            HandleListBoxEventL( listBox, EEventEditingStarted );
            break;
            }
        default:
            {
            CEikonEnv::Static()->EikAppUi()->HandleCommandL( aCommandId );
            break;
            }
        }
    }
