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
#include "CCEGroupContainer.h"

#include "CCEGroupView.h"
#include "CLFExample.hrh"
#include "CLFExampleConsts.h"

#include <aknlists.h>
#include <barsread.h>
#include <stringloader.h>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ constructor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEGroupContainer::CCEGroupContainer( CAknView* aParent )
    : iParent( aParent )
    {
    }

// -----------------------------------------------------------------------------
// CCEGroupContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::ConstructL()
    {
    // Create a window for the group view
    CreateWindowL();

    // Create a label for info text
    HBufC* labelText = StringLoader::LoadLC( R_CE_INFO_LABEL_TEXT );
    iInfoLabel = new (ELeave) CEikLabel;
    iInfoLabel->SetContainerWindowL( *this );
    iInfoLabel->SetAlignment( EHCenterVCenter );
    iInfoLabel->SetTextL( *labelText );
    iInfoLabel->SetExtentToWholeScreen();
    CleanupStack::PopAndDestroy( labelText );

    // Create a list box for listing the groups of music files
    iListBox = new (ELeave) CAknSingleStyleListBox;
    iListBox->MakeVisible( EFalse );
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, NULL );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    HBufC* emptyText = StringLoader::LoadLC( R_CE_NO_GROUPS );
    iListBox->View()->SetListEmptyTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );

    // Create a text array for items of the list box
    iItemList = new (ELeave) CDesCArraySeg( KCEArrayGranularity );
    iListBox->Model()->SetItemTextArray( iItemList );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set size of the window and activate it
    SetRect( iAvkonAppUi->ClientRect() );
    ActivateL();
    }

// Destructor
CCEGroupContainer::~CCEGroupContainer()
    {
    delete iItemList;
    delete iListBox;
    delete iInfoLabel;
    }

// -----------------------------------------------------------------------------
// CCEGroupContainer::AppendListBoxItemL
// Append new item to list box of group listing.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::AppendListBoxItemL( const TDesC& aItem )
    {
    // Load a string from the resource. The string is used to generate
    // the item that will be added to the list box.
    HBufC* listItem = StringLoader::LoadLC( R_CE_GROUP_LIST_ITEM, aItem );

    // Append the item to the list box and refresh it
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
// CCEGroupContainer::CurrentItemIndex
// Get index of the item that is currently selected in the list box.
// -----------------------------------------------------------------------------
//
TInt CCEGroupContainer::CurrentItemIndex()
	{
	return iListBox->CurrentItemIndex();
	}

// -----------------------------------------------------------------------------
// CCEGroupContainer::ResetListBoxL
// Reset list box of group listing.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::ResetListBoxL()
    {
    // Clear the list box model, refresh the list box and make it invisible
    iItemList->Reset();
    iListBox->HandleItemRemovalL();
    }

// -----------------------------------------------------------------------------
// CCEGroupContainer::SetCurrentItemIndex
// Selects specified item of the list box.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::SetCurrentItemIndex( TInt aIndex )
	{
    iListBox->SetCurrentItemIndexAndDraw( aIndex );
	}

// -----------------------------------------------------------------------------
// CCEGroupContainer::Draw
// Draw the screen.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::Draw( const TRect& aRect ) const
    {
    // Get the graphics content, set background color and clear the screen
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( AKN_LAF_COLOR( KCEBackgroundColor ) );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.Clear( aRect );
    }

// -----------------------------------------------------------------------------
// CCEGroupContainer::OfferKeyEventL
// Handle user's key event
// -----------------------------------------------------------------------------
//
TKeyResponse CCEGroupContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                      TEventCode aType )
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
// CCEGroupContainer::CountComponentControls
// Get count of component controls in the container.
// -----------------------------------------------------------------------------
//
TInt CCEGroupContainer::CountComponentControls() const
    {
    return KCENumberOfComponentsInGroupContainer;
    }

// -----------------------------------------------------------------------------
// CCEGroupContainer::ComponentControl
// Get component control by its index.
// -----------------------------------------------------------------------------
//
CCoeControl* CCEGroupContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
    	case 0:
    	    {
    		return iInfoLabel;
		    }
    	case 1:
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
// CCEGroupContainer::HandleResourceChange
// Handle change of the screen resolution.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ( static_cast <CCEGroupView *> ( iParent ) )->HandleClientRectChange();
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupContainer::SizeChanged
// Responds to size changes to sets the size and position of the contents of
// this control.
// -----------------------------------------------------------------------------
//
void CCEGroupContainer::SizeChanged()
	{
	if( iListBox )
	    {
    	iListBox->SetRect( Rect() );
	    }
	if( iInfoLabel )
	    {
    	iInfoLabel->SetRect( Rect() );
	    }
	}

// End of File
