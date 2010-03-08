/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container class for Geo Profiles Application
*
*/


// SYSTEM INCLUDES
#include <avkon.hrh>
#include <barsread.h> 
#include <barsread.h>   		// TResourceReader
#include <geoprofilesui.rsg>
#include <aknutils.h>
#include <eikspane.h> 		    // Status pane
#include <akntitle.h>       	// CAknTitlePane
#include <stringloader.h>
#include <aknlists.h>       // Settings Style list box

// USER INCLUDES
#include "geoprofilescontainer.h"
#include "geoprofilesappui.h"
#include "geoprofilesuid.hrh"
#include "geoprofiles.hrh"
#include "geoprofileslbmodel.h"
#include "geoprofilesview.h"

// CONSTANT DEFINITIONS
 
// ----------------- Member funtions for CGeoProfilesContainer class ------------------

// ---------------------------------------------------------------------------
// CGeoProfilesContainer::CGeoProfilesContainer
// ---------------------------------------------------------------------------
//
CGeoProfilesContainer::CGeoProfilesContainer( CGeoProfilesLBModel&  aListboxModel,
                                              CGeoProfilesView&     aView )
    :iListboxModel( &aListboxModel ),
    iView( aView )
    {
    }

// ---------------------------------------------------------------------------
// CGeoProfilesContainer::~CGeoProfilesContainer
// ---------------------------------------------------------------------------
//
CGeoProfilesContainer::~CGeoProfilesContainer()
    {
    // Delete the list box
    delete iListBox;
    
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesContainer::NewL
// ---------------------------------------------------------------------------
//
CGeoProfilesContainer* CGeoProfilesContainer::NewL( 
                                    const TRect&	            aRect,
                                          CGeoProfilesLBModel&  aListboxModel,
                                          CGeoProfilesView&     aView )     
    {
    CGeoProfilesContainer* self = new (ELeave) CGeoProfilesContainer( aListboxModel,
                                                                      aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::ConstructL( const TRect& 	aRect )
    {
    // This is the cheif control for this application. This has to be made a 
    // Window owning control
    CreateWindowL();
    
	// Create a New Title for the View
	MakeTitleL( R_GEOPROFILES_TITLE );
    
    // Create the Geo Profiles list box
    CreateListboxL();
            
    SetRect(aRect);
    ActivateL();
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::UpdateL
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::UpdateL( CGeoProfilesLBModel&  aListboxModel )
    {
    // Store the new list box model
    iListboxModel = &aListboxModel;
    
    // Assign it to the listbox
    iListBox->Model()->SetItemTextArray( iListboxModel );
    
    // Update the list box
    iListBox->HandleItemAdditionL();
    iListBox->HandleItemRemovalL();
    
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfilesContainer::GetFocussedItem
// ---------------------------------------------------------------------------
//
TInt CGeoProfilesContainer::GetFocussedItem()
    {
    if ( !iListBox )
        {
        return KErrNotFound;        
        }
    else 
        {
        return iListBox->CurrentItemIndex();
        }
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect());
        }
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfilesContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CGeoProfilesContainer::CountComponentControls() const
    {
    if ( iListBox )
        {
        return 1;    
        }
    else
        {
        return 0;    
        }
    }

// ---------------------------------------------------------------------------
// CCoeControl* CGeoProfilesContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CGeoProfilesContainer::ComponentControl(TInt /* aIndex*/ ) const
    {
    return iListBox;
    }
    
// ---------------------------------------------------------------------------
// TKeyResponse CGeoProfilesContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//    
TKeyResponse CGeoProfilesContainer::OfferKeyEventL( 
                                        const TKeyEvent& aKeyEvent,
								  			  TEventCode aType )
    {
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
            {
            // No action in this application for the Right arrow and left
            // arrow.
            return EKeyWasNotConsumed;  
            }
        default:
            {
            break;  
            }
        }
    return iListBox->OfferKeyEventL( aKeyEvent, aType ); 
    }   

// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::HandleResourceChange( TInt aType )
	{
    // Pass the event to the base class
    CCoeControl::HandleResourceChange( aType );
    
    switch( aType )
        {
        // Dynamic Layout switch and Skin Change
        case KEikDynamicLayoutVariantSwitch:
            {
            iView.ContainerCommandL( aType );
            break;
            }        
        case KAknsMessageSkinChange:
            {
            break;
            }
        default:
            {
            break;
            }
        }	
	}

// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::HandleListBoxEventL( 
                                            CEikListBox*      /* aListBox */, 
                                            TListBoxEvent     aEventType )
    {
    switch (aEventType)
        {
        // List box Item Selection
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
            iView.ContainerCommandL( EGeoProfilesOpen );
            break;  
            }
        default:
           break;
        }        
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::FocusChanged
//
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    // The focus event has to be explicitly handed over to all the compound
    // controls since CCoeControl does not do that implicitly
    iListBox->SetFocus( IsFocused(), aDrawNow );
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::CreateListboxL
// ---------------------------------------------------------------------------  
//
void CGeoProfilesContainer::CreateListboxL()
    {
    
    // Create the List box
    iListBox = new ( ELeave ) CAknSettingStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListBox->SetContainerWindowL( *this ); 
    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                         CEikScrollBarFrame::EAuto ); 
                                                         
    // The ownership of the list box model is retained with the Container.
    // This is because the model contains the additional functionality of
    // changing the settings values in addition to retreiving it.                                                        
    iListBox->Model()->SetItemTextArray( iListboxModel );  
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    // Set empty text for the list box
    HBufC* emptyText = StringLoader::LoadLC( R_LBL_GEOPROFILES_NOPROFILES );
    // Set text for empty list
    CEikListBox* listbox = iListBox;
    listbox->View()->SetListEmptyTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );
                                         
    iListBox->ActivateL();
    }
                                      
// ---------------------------------------------------------------------------
// void CGeoProfilesContainer::MakeTitleL
// Sets the Title text
//
// @param aResourceText Resource to create title
// ---------------------------------------------------------------------------
//
void CGeoProfilesContainer::MakeTitleL( TInt aResourceText )
	{
	CEikStatusPane* statusPane = 
	    static_cast<CEikStatusPane*>( iEikonEnv->AppUiFactory()->StatusPane());
	// Obtain the title from the Status Pane
	CAknTitlePane* title = static_cast<CAknTitlePane*>( statusPane->
        ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
       
    // Set the Title's buffer    
    HBufC* buf = StringLoader::LoadL( aResourceText );
    title->SetText( buf ); // Takes ownership of buf
	}
