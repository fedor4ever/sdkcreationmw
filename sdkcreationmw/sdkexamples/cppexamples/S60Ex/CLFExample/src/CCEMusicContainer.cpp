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
* Description: 
*
*/



// INCLUDE FILES
#include "CCEMusicContainer.h"

#include "CCEMusicView.h"
#include "CLFExample.hrh"
#include "CLFExampleConsts.h"

#include <aknlists.h>
#include <barsread.h>
#include <stringloader.h>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEMusicContainer::CCEMusicContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEMusicContainer::CCEMusicContainer( CAknView* aParent )
    : iParent( aParent )
    {
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::ConstructL()
    {
    // Create a window for the music view
    CreateWindowL();

    // Create a list box for the music files listing
    iListBox = new (ELeave) CAknDoubleStyleListBox;
    iListBox->MakeVisible( EFalse );
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, NULL );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Create text array for items of the list box
    iItemList = new (ELeave) CDesCArraySeg( KCEArrayGranularity );
    iListBox->Model()->SetItemTextArray( iItemList );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set size of the window and activate it
    SetRect( iAvkonAppUi->ClientRect() );
    ActivateL();
    }

// Destructor
CCEMusicContainer::~CCEMusicContainer()
    {
    delete iItemList;
    delete iListBox;
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::AppendListBoxItemL
// Append a new item with song name and artist to list box of group listing.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::AppendListBoxItemL( CDesCArray* aTextItems )
    {
    HBufC* listItem;

    // Load the string from the resource. The string will be used to
    // generate a new item to the list box. There are different strings
    // for items that have an album tag and items that does not
    // have album, title or artist tag.

    // If song name and artist tags are present this string will be used.
    // It is also used to view file name if name or artist tag is missing.
    if( aTextItems->Count() == 2 )
        {
        listItem = StringLoader::LoadLC(
            R_CE_MUSIC_LIST_2_ITEM, *aTextItems, iEikonEnv );
        }

    // If song name, artist and album tags are all found
    else if ( aTextItems->Count() == 3 )
        {
        listItem = StringLoader::LoadLC(
            R_CE_MUSIC_LIST_3_ITEM, *aTextItems, iEikonEnv );
        }

    // If this else is reached, the function has not been used properly
    else
        {
        return;
        }

    // Append the new item to the list box and refresh it
    iItemList->AppendL( *listItem );
    iListBox->HandleItemAdditionL();

    CleanupStack::PopAndDestroy( listItem );

    // Make the list box visible, if needed
    if( !iListBox->IsVisible() )
        {
        iListBox->MakeVisible( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::ResetListBoxL
// Reset the list box of group listing.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::ResetListBoxL()
    {
    // Clear the list box
    iItemList->Reset();
    iListBox->HandleItemRemovalL();
    iListBox->MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::CurrentItemIndex
// Get index of the item that is currently selected in the list box.
// -----------------------------------------------------------------------------
//
TInt CCEMusicContainer::CurrentItemIndex()
	{
	return iListBox->CurrentItemIndex();
	}

// -----------------------------------------------------------------------------
// CCEMusicContainer::SetCurrentItemIndex
// Selects specified item of the list box.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::SetCurrentItemIndex( TInt aIndex )
	{
    iListBox->SetCurrentItemIndexAndDraw( aIndex );
	}

// -----------------------------------------------------------------------------
// CCEMusicContainer::Draw
// Draw the screen.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::Draw( const TRect& aRect ) const
    {
    // Get the graphics content and clear the screen
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::OfferKeyEventL
// Handle user's key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CCEMusicContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
	{
	if( aType == EEventKey )
		{
		switch( aKeyEvent.iScanCode )
			{
			case EStdKeyUpArrow:
			case EStdKeyDownArrow:
				{
                // Send arrow key events to the list box for moving the focus
				iListBox->OfferKeyEventL( aKeyEvent, aType );
				return EKeyWasConsumed;
				}

			case EStdKeyDevice3:
				{
                // Send selection event to parent view
				iParent->HandleCommandL( ECECmdSelect );
				return EKeyWasConsumed;
				}

			default:
				{
				break;
				}
			}
		}
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CCEMusicContainer::CountComponentControls
// Get count of component controls in the container.
// -----------------------------------------------------------------------------
//
TInt CCEMusicContainer::CountComponentControls() const
    {
    return KCENumberOfComponentsInMusicContainer;
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::ComponentControl
// Get component control by its index.
// -----------------------------------------------------------------------------
//
CCoeControl* CCEMusicContainer::ComponentControl(TInt aIndex) const
    {
    switch( aIndex )
        {
    	case 0:
    	    {
    		return iListBox;
		    }
    	default:
    	    {
    		return NULL;
	        }
        }
    }

// -----------------------------------------------------------------------------
// CCEMusicContainer::HandleResourceChange
// Handle change of the screen resolution.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ( static_cast <CCEMusicView *> ( iParent ) )->HandleClientRectChange();
        }
    }


// -----------------------------------------------------------------------------
// CCEMusicContainer::SizeChanged
// Responds to size changes to sets the size and position of the contents of
// this control.
// -----------------------------------------------------------------------------
//
void CCEMusicContainer::SizeChanged()
	{
	iListBox->SetRect( Rect() );
	}

// End of File
