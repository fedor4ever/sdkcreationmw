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
#include <barsread.h> 
#include <listbox.rsg>
#include <eikclb.h> 
#include <StringLoader.h>
#include <Listbox.rsg>

#include "ListboxNumberContainer.h"
#include "NaviScrollTimer.h"


// Listbox class name is shown on navigation pane
_LIT( KNaviPaneText, "CAknSingleNumberStyleListBox" );
_LIT( KNumberSeparator, "\t");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListboxNumberContainer::CListboxNumberContainer()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CListboxNumberContainer::CListboxNumberContainer()
    {        
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::~CListboxNumberContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CListboxNumberContainer::~CListboxNumberContainer()
    {
    if( iListBox ) delete iListBox;    
    if( iTimer ) delete iTimer;
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//    
void CListboxNumberContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL(); // Creates window.
        
    SetRect( aRect ); // Sets rectangle of frame.
        
    CreateListboxL();
    iListBox->SetRect(Rect());
    
    ActivateL(); // Activates window. ( Ready to draw )       
    
    // Show listbox class name in navigation pane
    iTimer = CNaviScrollTimer::NewL( KNaviPaneText );
    iTimer->StartScroll();         
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::CreateListboxL()
// Constructs listbox from resource, creates scroll bar and sets empty
// list background text. 
// -----------------------------------------------------------------------------
//
void CListboxNumberContainer::CreateListboxL()
    {
    iListBox = new ( ELeave ) CAknSingleNumberStyleListBox();  
    
    iListBox->SetContainerWindowL( *this );

    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, 
                    R_LISTBOX_SINGLE_NUMBER_STYLE );
    
    // Construct listbox from resource    
    iListBox->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); 
    
    // Creates scrollbar.
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto ); 
    
    // Set the listbox empty text
    HBufC* emptyText = StringLoader::LoadLC( R_LISTBOX_EMPTYTEXT );   
    iListBox->View()->SetListEmptyTextL( emptyText->Des() );  
    CleanupStack::PopAndDestroy( emptyText );        
    }
 

// -----------------------------------------------------------------------------
// CListboxNumberContainer::CountComponentControls()
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CListboxNumberContainer::CountComponentControls() const
    {
    return 1; 
    }
    
// -----------------------------------------------------------------------------
// CListboxNumberContainer::ComponentControl()
// Gets an indexed component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CListboxNumberContainer::ComponentControl(TInt aIndex) const
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
// CListboxNumberContainer::SizeChanged()
// Responds to changes to the size and position of the contents of this control.
// -----------------------------------------------------------------------------
//
void CListboxNumberContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::AddItemL()
// Adds item to the end of the list.
// -----------------------------------------------------------------------------
//   
void CListboxNumberContainer::AddItemL()
    {
    // Number of items
    TInt numOfItems = iListBox->Model()->NumberOfItems();
    // Item array. By default iListBox owns the item array, so no deletion 
    // is needed.
    CDesCArray* itemArray = 
        static_cast<CDesC16ArrayFlat*>(iListBox->Model()->ItemTextArray());
        
    // if listbox does not have items, load first item from resource.
    if( numOfItems < 1 )
    	{
    	HBufC* itemText = StringLoader::LoadLC( R_LISTBOX_ITEMTEXT ); 
	    itemArray->AppendL( itemText->Des() );
	    CleanupStack::PopAndDestroy( itemText );
    	}
   	else // Add new item to the end of the listbox's item array
   		{
   		// Item text of last item
    	TPtrC itemPtr = iListBox->Model()->ItemText( numOfItems - 1 );
        	    
	    TInt numSep = itemPtr.FindC( KNumberSeparator );
	    
        // Item number in TDes format
        TBuf<5> numBuf = itemPtr.Mid( 0, numSep );
        // Convert to TInt and add 1
        TInt itemIndex(0);
        TLex conv( numBuf );
        conv.Val( itemIndex );        
        ++itemIndex;
        
        // Create new item text 
        _LIT( KItemFormat, "%d\t%S" );
        TBuf<10> itemEnd = itemPtr.Mid( numSep + KNumberSeparator().Length() );
        TBuf<60> itemText;        
        itemText.Format( KItemFormat, itemIndex, &itemEnd );
                
        itemArray->AppendL( itemText );       
   		}   
    
    // Update the listbox
    iListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::RemoveItemL()
// Removes item from the end of the list.
// -----------------------------------------------------------------------------
//          
void CListboxNumberContainer::RemoveItemL()
    {
    // Number of items
    TInt numOfItems = iListBox->Model()->NumberOfItems();
    
    if( numOfItems > 0 )
        {
        TInt currentItemIndex = iListBox->CurrentItemIndex();
        // Item array. By default iListBox owns the item array, so no deletion 
        // is needed.
        CDesCArray* itemArray = 
            static_cast<CDesC16ArrayFlat*>(iListBox->Model()->ItemTextArray());
        // Delete last item
        itemArray->Delete( numOfItems - 1 );
        
        if( currentItemIndex >= ( numOfItems - 1 ) &&
            currentItemIndex > 0 )
            iListBox->SetCurrentItemIndex( numOfItems - 2 );
        }
    else
        return;           
    
    // Update the listbox
    iListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::ItemCount()
// Getter for listbox item count
// -----------------------------------------------------------------------------
//   
TInt CListboxNumberContainer::ItemCount()
    {
    return iListBox->Model()->NumberOfItems();
    }

// -----------------------------------------------------------------------------
// CListboxNumberContainer::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CListboxNumberContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent, 
                                                   TEventCode aType)
    {
    if ( aKeyEvent.iCode == EKeyDownArrow || aKeyEvent.iCode == EKeyUpArrow )
        // Forward key events to listbox                
        return iListBox->OfferKeyEventL( aKeyEvent, aType );
    
    return EKeyWasNotConsumed;    
    }

// End of File

