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
* Description: 
*
*/


#include <aknlists.h>
#include <Listbox_bitmaps.mbg>
#include <gulicon.h>
#include <AknIconUtils.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <eikapp.h>
#include <aknquerydialog.h>
#include <StringLoader.h>
#include <listbox.rsg>

#include "ListboxGraphicContainer.h"
#include "IconFileProvider.h"
#include "NaviScrollTimer.h"


// Format string for listbox items
_LIT( KListItemFormat, "0\tItem %d" );
// Listbox class name is shown on navigation pane
_LIT( KNaviPaneText, "CAknSingleGraphicStyleListBox" );
// Listbox icon file name
_LIT(KMifFileName, "listbox_bitmaps.mif");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::CListboxGraphicContainer()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CListboxGraphicContainer::CListboxGraphicContainer()
    {
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::~CListboxGraphicContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CListboxGraphicContainer::~CListboxGraphicContainer()
    {
    if( iListBox ) delete iListBox;
    iFsSession.Close();
	if( iFileProvider ) delete iFileProvider;
	if( iTimer ) delete iTimer;
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::ConstructL( const TRect& aRect )
    {
    // Creates window.
    CreateWindowL();
    // Sets rectangle of frame.
    SetRect( aRect );

    CreateListboxL();

    iListBox->SetRect(Rect());

    // Set lisbox icons
    SetIconsL();
    // Set listbox item texts
    SetItemsL();
    // Activates window. ( Ready to draw )
    ActivateL();
    // Show listbox class name in navigation pane
    iTimer = CNaviScrollTimer::NewL( KNaviPaneText );
    iTimer->StartScroll();

    CEikMenuBar* menuBar = CEikonEnv::Static()->AppUiFactory()->MenuBar();
    menuBar->SetContextMenuTitleResourceId(
    		R_LISTBOX_GRAPHICVIEW_MSK_MENUBAR );
    }


// -----------------------------------------------------------------------------
// CListboxGraphicContainer::CreateListboxL()
// Constructs listbox, creates scroll bar, sets empty list background text, and
// enables marquee.
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::CreateListboxL()
    {
    iListBox = new ( ELeave ) CAknSingleGraphicStyleListBox();
    iListBox->ConstructL(this);
    iListBox->SetContainerWindowL( *this );

    // Creates scrollbar.
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Enable marquee
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    // Set empty text for the listbox.
    HBufC* emptyText = StringLoader::LoadLC( R_LISTBOX_EMPTYTEXT );
    iListBox->View()->SetListEmptyTextL( emptyText->Des() );
    CleanupStack::PopAndDestroy( emptyText );
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::SetIconsL()
// Sets icons for the listbox.
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::SetIconsL()
    {
    CArrayPtr<CGulIcon>* iconList = new (ELeave) CAknIconArray(1);
    // push iconarray to stack
    CleanupStack::PushL(iconList);

    User::LeaveIfError( iFsSession.Connect() );
    // Let other sessions to use this. ShareProtected() must be called for
    // enabling other other threads in this process to use this handle.
    iFsSession.ShareProtected();
    TBuf<256> path;
    iFsSession.PrivatePath( path );

    path.Append( KMifFileName() );
    // insert the drive to the private path
    TParsePtrC parse(
        (CEikonEnv::Static()->EikAppUi()->Application())->AppFullName() );
    path.Insert( 0, parse.Drive() );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    iFileProvider = CIconFileProvider::NewL( iFsSession, path );

    // Create icons. This function uses CIconFileProvider to pass handle to
    // icon file (which lies in application private directory) to AknIcon
    // server. This server does not have access to private directories.
	AknIconUtils::CreateIconL(bitmap,
	                          mask,
	                          *iFileProvider,
	                          EMbmListbox_bitmapsListbox_icon,
	                          EMbmListbox_bitmapsListbox_icon_mask);

    CGulIcon* newIcon = CGulIcon::NewL( bitmap, mask );
    iconList->AppendL( newIcon );

    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconList );
    CleanupStack::Pop(iconList);
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::SetItemsL()
// Sets items to the listbox.
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::SetItemsL()
    {
    TBuf<20> itemText;

    CDesCArray* itemArray = new ( ELeave ) CDesCArrayFlat(4);

    for( TInt i = 1; i < 5; ++i )
        {
        itemText.Format( KListItemFormat(), i );
        itemArray->AppendL( itemText );
        }

    iListBox->Model()->SetItemTextArray( itemArray );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::AddItemL()
// Adds item to the listbox.
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::AddItemL()
    {
    // Icon number for all items.
    _LIT( KItemStart, "0\t" );
    // New item text
    TBuf<50> itemText;
    CAknTextQueryDialog *dialog = CAknTextQueryDialog::NewL( itemText );
    // Maximum length for input text. Can also be defined in
    // R_LISTBOX_DATA_QUERY_DIALOG resourse.
    dialog->SetMaxLength( 45 );
    // Execute dialog
    if (dialog->ExecuteLD( R_LISTBOX_DATA_QUERY_DIALOG ) )
        {
        // Insert icon number at the beginning of itemText. Note that max
        // length for dialog input text is 45 and itemText size is 50. So
        // there is space for item number and column separator.
        itemText.Insert( 0, KItemStart() );
        // Item array. By default iListBox owns the item array, so no
        // deletion is needed.
        CDesCArray* itemArray =
            static_cast<CDesC16ArrayFlat*>(iListBox->Model()->ItemTextArray());

        // Insert item to the item array
        if( itemArray->Count() > 0 )
            itemArray->InsertL( iListBox->View()->CurrentItemIndex(),itemText );
        else
            itemArray->InsertL( 0, itemText );

        // Update the listbox
        iListBox->HandleItemAdditionL();
        }
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::RemoveItemL()
// Removes focused item from the listbox.
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::RemoveItemL()
    {
    // If we don't have items, do nothing and return
    if( iListBox->Model()->NumberOfItems() < 1 )
        return;

    TInt oldIndex = iListBox->View()->CurrentItemIndex();
    // Item array. By default iListBox owns the item array, so no deletion is
    // needed.
    CDesCArray* itemArray =
        static_cast<CDesC16ArrayFlat*>(iListBox->Model()->ItemTextArray());

    // Remove highlighted item from the listbox.
    itemArray->Delete( iListBox->View()->CurrentItemIndex() );

    // If focused item is out of range, bring it forth
    if( (oldIndex > ( itemArray->Count()-1 )) &&
        ( (itemArray->Count()-1) >= 0 ) )
        {
        iListBox->View()->SetCurrentItemIndex( itemArray->Count()-1 );
        }

    // Update the listbox
    iListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::CountComponentControls()
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CListboxGraphicContainer::CountComponentControls() const
    {
    return 1;
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::ComponentControl()
// Gets an indexed component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CListboxGraphicContainer::ComponentControl(TInt aIndex) const
    {
    switch( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::SizeChanged()
// Responds to changes to the size and position of the contents of this control.
// -----------------------------------------------------------------------------
//
void CListboxGraphicContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CListboxGraphicContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                                   TEventCode aType)
    {
    if ( aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow )
        // Forward key events to listbox
        return iListBox->OfferKeyEventL( aKeyEvent, aType );

    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CListboxGraphicContainer::ItemCount()
// Getter for listbox item count.
// -----------------------------------------------------------------------------
//
TInt CListboxGraphicContainer::ItemCount()
    {
    return iListBox->Model()->NumberOfItems();
    }

// End of File

