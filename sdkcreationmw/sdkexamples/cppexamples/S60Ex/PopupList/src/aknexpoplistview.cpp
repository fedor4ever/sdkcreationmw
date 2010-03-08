/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknviewappui.h>
#include <avkon.hrh>
#include <AknPopup.h>
#include <AknLists.h>
#include <AknNoteDialog.h>
#include <BarsRead.h>
#include <eikdef.h>
#include <GulIcon.h>
#include <EikClbd.h>
#include <eikenv.h>
#include <eikapp.h>
#include <eikappui.h>
#include <AknIconArray.h>
#include <AknExPopList.mbg>
#include <AknExPopList.rsg>

#include "IconFileProvider.h"
#include "AknExPopList.hrh"
#include "AknExPopListView.h"
#include "AknExPopListContainer.h" 

// ========================= MEMBER FUNCTIONS ==================================

// ------------------------------------------------------------------------------
// CAknExPopListView::ConstructL()
// Symbian 2nd phase constructor can leave.
// ------------------------------------------------------------------------------
//
void CAknExPopListView::ConstructL()
    {
    BaseConstructL( R_AKNEXPOPLIST_VIEW1 );
    
	// create and open file server session
    User::LeaveIfError(iFsSession.Connect());
    iFsSession.ShareProtected();

    TFileName filePath;

    // set path of the bitmap file
    User::LeaveIfError(iFsSession.PrivatePath(filePath));

    // append the MBM file name to the private path
    filePath.Append(KPathAppMbm);

    // insert the drive to the private path
    TParsePtrC parse((CEikonEnv::Static()->EikAppUi()->Application())->AppFullName());
    filePath.Insert(0, parse.Drive());

    // with the help of icon-provider AknIconUtils can get get the file
    // from this application's private folder
    iIconProvider = CIconFileProvider::NewL(iFsSession, filePath);
    }

// ------------------------------------------------------------------------------
// CAknExPopListView::~CAknExPopListView()
// Destructor.
// ------------------------------------------------------------------------------
//
CAknExPopListView::~CAknExPopListView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    delete iContainer;

    iFsSession.Close();
    delete iIconProvider;
    }

// ------------------------------------------------------------------------------
// CAknExPopListView::Id()
// Returns view id
// ------------------------------------------------------------------------------
//
TUid CAknExPopListView::Id() const
    {
    return KViewId;
    }

// -----------------------------------------------------------------------------
// CAknExPopListView::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CAknExPopListView::HandleCommandL( TInt aCommand )
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            AppUi()->HandleCommandL( EAknCmdExit );
            break;

        case EAknExPopListCmdAuto:
        case EAKnTaPopListSoftkeyNext:
            AutoTestL();
            break;

        case EAknExPopListCmdView1Menu2:
            ShowSingleItemPopupListL();
            break;

        case EAknExPopListCmdView1Menu5:
            ShowSingleItemPopupListWithGraphicsL();
            break;

        case EAknExPopListCmdView1Menu9:
            ShowDoubleItemPopupListWithGraphicsL();
            break;

        default:
            AppUi()->HandleCommandL( aCommand );
            break;

        }
    }

// -----------------------------------------------------------------------------
// CAknExPopListView::HandleViewRectChange()
// Handles Clients Rect
// -----------------------------------------------------------------------------
//
void CAknExPopListView::HandleViewRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
// CAknExPopListView::DoActivateL()
// Creates container class object.
// -----------------------------------------------------------------------------
//
void CAknExPopListView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                     TUid /*aCustomMessageId*/,
                                     const TDesC8& /*aCustomMessage*/ )
    {

    iContainer = new( ELeave ) CAknExPopListContainer;
    iContainer->SetMopParent( this );
    iContainer->ConstructL( ClientRect() );
    AppUi()->AddToStackL( *this, iContainer );
   }


// -----------------------------------------------------------------------------
// CAknExPopListView::DoDeactivate()
// Deletes container class object.
// -----------------------------------------------------------------------------
//
void CAknExPopListView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }


// -----------------------------------------------------------------------------
// CAknExPopListView::AutoTestL()
// Shows all kind of popup-windows except list_single_number_pane,
// list_single_heading_pane, and list_single_number_heading_pane
// automatically.
// -----------------------------------------------------------------------------
//
void CAknExPopListView::AutoTestL()
    {
    ShowSingleItemPopupListL();
    ShowSingleItemPopupListWithGraphicsL();
    ShowDoubleItemPopupListWithGraphicsL();
    }


// -----------------------------------------------------------------------------
// CAknExPopListView::ShowSingleItemPopupListL()
// shows "List pane for single item"
// -----------------------------------------------------------------------------
//
void CAknExPopListView::ShowSingleItemPopupListL()
    {

    // Create CEikTextListBox instance, list
    CEikTextListBox* list = new( ELeave ) CAknSinglePopupMenuStyleListBox;

    // Push list'pointer to CleanupStack.
    CleanupStack::PushL( list );

    // Create CAknPopupList instance, popupList
    CAknPopupList* popupList = CAknPopupList::NewL( list, 
                                                    R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                                    AknPopupLayouts::EMenuWindow );
    // Push popupList'pointer to CleanupStack.
    CleanupStack::PushL( popupList );

    
    // Initialize listbox.
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                     CEikScrollBarFrame::EAuto );

    // Create CDesCArrayFlat instance, items
    CDesCArrayFlat* items = iCoeEnv->ReadDesCArrayResourceL(
                                       R_AKNEXPOPLIST_MENU_ITEMS_TEST1 );
        // Push items'pointer to CleanupStack.  
    CleanupStack::PushL( items );

    // Set listitems.
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    
    CleanupStack::Pop();

    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();

    // Pop the popupList's pointer from CleanupStack
    CleanupStack::Pop();

    if ( popupOk )
        {
        TInt index = list->CurrentItemIndex();
        TBuf<KMessageLength> msg;
        TBuf<KMessageLength> format;
        iCoeEnv->ReadResource( format, R_AKNEXPOPLIST_SELECT_MESSAGE );
        msg.Format( format, index + 1 );
        CAknNoteDialog* dlg = new( ELeave ) CAknNoteDialog( 
                                                CAknNoteDialog::ENoTone, 
                                                CAknNoteDialog::ENoTimeout );

        dlg->PrepareLC(  R_AKNEXPOPLIST_SELECT_MESSAGE_NOTE  );
        dlg->SetTextPluralityL(  EFalse  );
        dlg->SetTextL(  msg  );
        dlg->RunLD();
        }
    // Pop and Destroy the list's pointer from CleanupStack
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CAknExPopListView::ShowSingleItemPopupListWithGraphicsL( )
// shows "List pane for single item with graphic"
// -----------------------------------------------------------------------------
void CAknExPopListView::ShowSingleItemPopupListWithGraphicsL()
    {

    // Create CAknSingleGraphicPopupMenuStyleListBox instance, list
    CAknSingleGraphicPopupMenuStyleListBox* list = 
                            new( ELeave ) CAknSingleGraphicPopupMenuStyleListBox;
    
    // Push list'pointer to CleanupStack.
    CleanupStack::PushL( list );

    // Create CAknPopupList instance, popupList
    CAknPopupList* popupList = CAknPopupList::NewL( 
                                            list, R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                            AknPopupLayouts::EMenuGraphicWindow );    
    // Push popupList'pointer to CleanupStack.
    CleanupStack::PushL( popupList );

    // Initialize list.
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                     CEikScrollBarFrame::EAuto );
    // Reads a resource into a descriptor.
    CDesCArrayFlat* items =  iCoeEnv->ReadDesCArrayResourceL(
                                               R_AKNEXPOPLIST_MENU_ITEMS_TEST2 );
    // Push items'pointer to CleanupStack.
    CleanupStack::PushL( items );

    // Set listitems.
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop();

    // Create Icon object
    CArrayPtr<CGulIcon>* icons =  new( ELeave ) CAknIconArray( KMaxIcons );

    // Push icons'pointer to CleanupStack.
    CleanupStack::PushL( icons );

    CFbsBitmap* bitmap = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistCard );
    CleanupStack::PushL( bitmap );

    CFbsBitmap* mask = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistCard_mask );
    CleanupStack::PushL( mask );
    icons->AppendL( CGulIcon::NewL( bitmap, mask ) );

    CleanupStack::Pop(); // mask
    CleanupStack::Pop(); // bitmap

    CFbsBitmap* bitmap2 = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistTel );
    CleanupStack::PushL( bitmap2 );

    CFbsBitmap* mask2 = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistTel_mask );
    CleanupStack::PushL( mask2 );
    icons->AppendL( CGulIcon::NewL( bitmap2, mask2 ) );

    CleanupStack::Pop(); // mask2
    CleanupStack::Pop(); // bitmap2

    list->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    
    // Pop the Icon's pointer from CleanupStack
    CleanupStack::Pop();

    list->ClearSelection();
    CArrayFixFlat<TInt>* array1 = new( ELeave ) 
                                       CArrayFixFlat<TInt>( KIndexArraySize );

    CleanupStack::PushL( array1 );
    array1->AppendL( 1 );
    array1->AppendL( 2 );
    
    list->SetSelectionIndexesL( array1 );

    // Pop and Destroy the array1's pointer from CleanupStack
    CleanupStack::PopAndDestroy();

    list->ItemDrawer()->SetItemMarkPosition( KInitPos );
    TPtrC blank( KInitPosString );
    list->ItemDrawer()->SetItemMarkReplacement( blank );

    // Set title
    // Allocate TBuf with constant length.
    TBuf<KMaxTitleLength> title;

    // Reads a resource into a descriptor.
    iCoeEnv->ReadResource( title, R_AKNEXPOPLIST_MENU_TITLE_TEST2 );

    popupList->SetTitleL( title );

    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();
    
     // Pop the popupList's pointer from CleanupStack
    CleanupStack::Pop();
    if ( popupOk )
        {
        TInt index = list->CurrentItemIndex();
        //Allocate TBuf with constant length.
        TBuf<KMessageLength> msg;
        
        //Allocate TBuf with constant length.
        TBuf<KMessageLength> format;

        // Reads a resource into a descriptor.
        iCoeEnv->ReadResource( format, R_AKNEXPOPLIST_SELECT_MESSAGE );

        msg.Format( format, index + 1 );
        CAknNoteDialog* dlg = new( ELeave ) CAknNoteDialog( 
                                            CAknNoteDialog::ENoTone, 
                                            CAknNoteDialog::ENoTimeout );

        dlg->PrepareLC(  R_AKNEXPOPLIST_SELECT_MESSAGE_NOTE  );
        dlg->SetTextPluralityL(  EFalse  );
        dlg->SetTextL(  msg  );
        dlg->RunLD();
        }

    // Pop and Destroy the list's pointer from CleanupStack
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CAknExPopListView::ShowDoubleItemPopupListWithGraphicsL()
// shows "List pane for double item"
// -----------------------------------------------------------------------------
//
void CAknExPopListView::ShowDoubleItemPopupListWithGraphicsL()
    {

    CAknDoubleLargeGraphicPopupMenuStyleListBox* list = 
                     new( ELeave ) CAknDoubleLargeGraphicPopupMenuStyleListBox;
    
    CleanupStack::PushL( list );
    CAknPopupList* popupList = CAknPopupList::NewL( 
                                        list, R_AVKON_SOFTKEYS_SELECT_CANCEL,
                                        AknPopupLayouts::EMenuDoubleLargeGraphicWindow );    
    CleanupStack::PushL( popupList );

    TInt flags = 0; // Initialize flag
    list->ConstructL( popupList, flags );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( 
                                CEikScrollBarFrame::EOff,
                                CEikScrollBarFrame::EAuto );

    // Set Item text
    CDesCArrayFlat* items = 
            iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPLIST_MENU_ITEMS_TEST5 );
    CleanupStack::PushL( items );

    // Set listitems.
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop();

    // Create Icon object
    CArrayPtr<CGulIcon>* icons =  new( ELeave ) CAknIconArray( KMaxIcons );
   
    CleanupStack::PushL( icons );

    // Load bitmap and append them to "icons" array.
    CFbsBitmap* bitmap = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistFinflag );
    CleanupStack::PushL( bitmap );

    CFbsBitmap* mask = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistFinflag_mask );
    CleanupStack::PushL( mask );
    icons->AppendL( CGulIcon::NewL( bitmap, mask ) );

    CleanupStack::Pop(); // mask
    CleanupStack::Pop(); // bitmap

    CFbsBitmap* bitmap2 = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistJpnflag );
    CleanupStack::PushL( bitmap2 );

    CFbsBitmap* mask2 = AknIconUtils::CreateIconL( *iIconProvider,
            EMbmAknexpoplistJpnflag_mask );
    CleanupStack::PushL( mask2 );
    icons->AppendL( CGulIcon::NewL( bitmap2, mask2 ) );

    CleanupStack::Pop(); // mask2
    CleanupStack::Pop(); // bitmap2

    list->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    
    // Pop the icons's pointer from CleanupStack 
    CleanupStack::Pop();

    list->ClearSelection();
    CArrayFixFlat<TInt>* array1 =
                            new( ELeave ) CArrayFixFlat<TInt>( KIndexArraySize );

    CleanupStack::PushL( array1 );
    array1->AppendL( 1 );
    array1->AppendL( 2 );
    
    list->SetSelectionIndexesL( array1 );

    // Pop and Destroy the array1's pointer from CleanupStack
    CleanupStack::PopAndDestroy();

    list->ItemDrawer()->SetItemMarkPosition( KInitPos );
    TPtrC blank( KInitPosString );
    list->ItemDrawer()->SetItemMarkReplacement( blank );

    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();

    // Pop the popupList's pointer from CleanupStack 
    CleanupStack::Pop();

    if ( popupOk )
        {
        TInt index = list->CurrentItemIndex();
        TBuf<KMessageLength> msg;
        TBuf<KMessageLength> format;
        iCoeEnv->ReadResource( format, R_AKNEXPOPLIST_SELECT_MESSAGE );
        msg.Format( format, index + 1 );

        CAknNoteDialog* dlg = new( ELeave ) CAknNoteDialog(
            CAknNoteDialog::ENoTone, CAknNoteDialog::ENoTimeout );

        dlg->PrepareLC( R_AKNEXPOPLIST_SELECT_MESSAGE_NOTE );
        dlg->SetTextPluralityL( EFalse );
        dlg->SetTextL( msg );
        dlg->RunLD();
        }

    // Pop and Destroy the list's pointer from CleanupStack
    CleanupStack::PopAndDestroy(); 

    }

// End of File
