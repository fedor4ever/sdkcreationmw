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


#include <coemain.h>
#include <badesca.h> 
#include <eiklabel.h> 
#include <aknbutton.h> 
#include <aknchoicelist.h>
#include <barsread.h>
#include <choicelistex.rsg>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>

#include "ChoiceListExContainer.h"


// ============================== CONSTANTS ==================================

const TInt KArrayGranularity ( 1 );
const TRect KListRect( TPoint( 120, 200 ), TPoint( 400, 250 ) );
const TRect KLabelRect( TPoint( 20, 50 ), TPoint( 450, 100 ) );
                                      
// Texts for choice list items
_LIT( KListItem, "Item index %d" );
_LIT( KListItemNew, "New Item" );

// Message texts
_LIT( KAppName," Choicelist example" );
_LIT( KNoList," No choice list created" );
_LIT( KNoItems, "No items on list");
_LIT( KCreate, " Choice list created" );
_LIT( KSelect, "Selected item index %d" );
_LIT( KAdd, "Added item index %d" );
_LIT( KInsert, "Inserted item to index %d" );
_LIT( KShow, "Choice list opened" );
_LIT( KRemove, "Removed item index %d" );
_LIT( KButtonTxt, "Choice list" );
_LIT( KButtonHelpTxt, "Button help text" );
_LIT( KFlag," List position changed" );

// =========================== MEMBER FUNCTIONS ==============================


// ---------------------------------------------------------------------------
// C++ default Constructor
// ---------------------------------------------------------------------------
//
CChoiceListExContainer::CChoiceListExContainer()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CChoiceListExContainer::~CChoiceListExContainer()
    {

    delete iLabel;
    delete iBgContext;

    if ( iChoiceList )
        {
        delete iChoiceList;
        }
    }

// ---------------------------------------------------------------------------
// Symbian 2nd Constructor
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );

    // Label for showing messages    
    iLabel = new ( ELeave ) CEikLabel;
    iLabel->SetLabelAlignment( ELayoutAlignCenter );
    iLabel->SetContainerWindowL( *this );
    iLabel->SetTextL( KAppName );
    iLabel->SetRect( KLabelRect );
    
    // Background context for skinned background
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
        KAknsIIDSkinBmpMainPaneUsual, TRect( 0, 0, 0, 0 ), ETrue );
    iBgContext->SetRect( Rect() );
    iBgContext->SetParentPos( PositionRelativeToScreen() );

    SizeChanged();
    ActivateL();
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::Draw
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::Draw( const TRect& aRect ) const
    {
    // Draw background
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( !AknsDrawUtils::Background( skin, iBgContext, gc, aRect ) )
        {
        SystemGc().Clear( aRect );
        }
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CChoiceListExContainer::CountComponentControls() const
    {
    TInt count ( 1 ); // iLabel

    if ( iChoiceList )    
        {
        ++count;
        }
        
    return count;
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CChoiceListExContainer::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iLabel;
        }
    else if ( aIndex == 1 )
        {
        return iChoiceList;
        }
        
    return NULL;
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::OfferKeyEventL
// Handles key events by passing them to choice list
// ---------------------------------------------------------------------------
//
TKeyResponse CChoiceListExContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,TEventCode aType )
    {
    if ( iChoiceList )
        {
        return iChoiceList->OfferKeyEventL( aKeyEvent, aType );    
        }
    return EKeyWasNotConsumed;
    }
    
// ---------------------------------------------------------------------------
// CChoiceListExContainer::HandleControlEventL
// Handles events from the Choice List
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::HandleControlEventL( CCoeControl* aControl,
                                                  TCoeEvent aEventType )
    {
    if ( aControl == iChoiceList )
        {
        TInt selection;
        switch ( aEventType )
            {
            case EEventStateChanged:
                selection = iChoiceList->SelectedIndex();
                if ( iSelection != selection )
                    {
                    // selected item changed
                    iSelection = selection;
                    TBuf<32> buf;
                    buf.Format( KSelect, selection );
                    iLabel->SetTextL( buf );
                    iLabel->DrawDeferred();
                    }
                break;
                    
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::CreateDefaultChoiceListL
// Deletes previous Choice list if exists and constructs a new one with an
// array of items.
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::CreateDefaultChoiceListL()
    {
    if ( iChoiceList )
        {
        delete iChoiceList;
        iChoiceList = NULL;
        }
   
    // Create array of choice list items        
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat ( KArrayGranularity );
    CleanupStack::PushL( array );
    
    for ( TInt k = 0; k < 10; k++ )
        {
        TBuf<32> buf;
        buf.Format( KListItem, k );
        array->AppendL( buf );
        }

    iArraySize = array->Count();
    iSelection = 0;
    
    // Create choice list with array
    iChoiceList = CAknChoiceList::NewL( this, array );
    
    CleanupStack::Pop( array );
    
    iChoiceList->SetObserver( this );
    iChoiceList->SetRect( KListRect );

    iLabel->SetTextL( KCreate );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::CreateButtonChoiceListL
// Deletes previous Choice list if exists and constructs a new one with an
// button and item array.
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::CreateButtonChoiceListL()
    {
    if ( iChoiceList )
        {
        delete iChoiceList;
        iChoiceList = NULL;
        }
           
    // Create array of choice list items        
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat ( KArrayGranularity );
    
    CleanupStack::PushL( array );
    
    for ( TInt k = 0; k < 10; k++ )
        {
        TBuf<32> buf;
        buf.Format( KListItem, k );
        array->AppendL( buf );
        }

    iArraySize = array->Count();
    iSelection = 0;

    // Create a button for the choice list
    CAknButton* button = CAknButton::NewLC( NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            KButtonTxt,
                                            KButtonHelpTxt,
                                            KAknButtonSizeFitText,
                                            0 );
    
    // Create a choice list
    iChoiceList = CAknChoiceList::NewL( this, array, 
            CAknChoiceList::EAknChoiceListWithoutCurrentSelection, 
            button );
    CleanupStack::Pop( button );
    CleanupStack::Pop( array );

    iChoiceList->SetObserver( this );
    iChoiceList->SetRect( KListRect );
       
    iLabel->SetTextL( KCreate );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::CreateResourceChoiceListL
// Deletes previous Choice list if exists and constructs a new one from
// resources defined in resource file ChoiceListEx.rss
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::CreateResourceChoiceListL()
    {
    if ( iChoiceList )
        {
        delete iChoiceList;
        iChoiceList = NULL;
        }

    // Create empty Choice list
    iChoiceList = CAknChoiceList::NewL( this, NULL );

    // Add items from resource
    iChoiceList->SetItemsL( R_CHOICELISTEX_ITEM_ARRAY );
    iArraySize = ArraySizeL( R_CHOICELISTEX_ITEM_ARRAY );
    iSelection = 0;
    
    iChoiceList->SetObserver( this );
    iChoiceList->SetRect( KListRect );
  
    iLabel->SetTextL( KCreate );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::ShowListL
// Opens the choice list 
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::ShowListL()
    {
    if ( !iChoiceList )
        {
        iLabel->SetTextL( KNoList );
        return;
        }
    iChoiceList->ShowChoiceListL();
    
    iLabel->SetTextL( KShow );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::AddItemL
// Adds a new item to the choice list
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::AddItemL()
    {
    if ( !iChoiceList )
        {
        iLabel->SetTextL( KNoList );
        return;
        }
            
    TInt index = iChoiceList->AddItemL( &KListItemNew );
    ++iArraySize;
    
    TBuf<32> buf;
    buf.Format( KAdd, index );
    iLabel->SetTextL( buf );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::InsertItemL
// Inserts a new item in the middle of the list.
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::InsertItemL()
    {
    if ( !iChoiceList )
        {
        iLabel->SetTextL( KNoList );
        return;
        }
    TInt point = TInt ( iArraySize / 2 );
    
    TInt index = iChoiceList->InsertItemL( point, KListItemNew );
    ++iArraySize;
    
    TBuf<32> buf;
    buf.Format( KInsert, index );
    iLabel->SetTextL( buf );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::RemoveItemL
// Removes the last item from the choice list
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::RemoveItemL()
    {
    if ( !iChoiceList )
        {
        iLabel->SetTextL( KNoList );
        return;
        }
        
    if ( iArraySize < 1 )
        {
        iLabel->SetTextL( KNoItems );
        return;
        }
        
    iArraySize--;
    iChoiceList->RemoveItem( iArraySize );

    TBuf<32> buf;
    buf.Format( KRemove, iArraySize );
    iLabel->SetTextL( buf );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::SelectItemL
// Sets the last item as selected
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::SelectItemL()
    {
    if ( !iChoiceList )
        {
        iLabel->SetTextL( KNoList );
        return;
        }
    
    if ( iArraySize < 1 )
        {
        iLabel->SetTextL( KNoItems );
        return;
        }
        
    iChoiceList->SetSelectedIndex( iArraySize - 1 );
    
    TBuf<32> buf;
    buf.Format( KSelect, iArraySize-1 );
    iLabel->SetTextL( buf );
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::ChangeListFlagsL
// Changes the choice list flags ( Position )
// ---------------------------------------------------------------------------
//    
void CChoiceListExContainer::ChangeListFlagsL( 
                            CAknChoiceList::TAknChoiceListFlags aFlag )
    {
    if ( !iChoiceList )
        {
        iLabel->SetTextL( KNoList );
        return;
        }
    iChoiceList->SetFlags( aFlag );
    iLabel->SetTextL( KFlag );
    }
    
// ---------------------------------------------------------------------------
// CChoiceListExContainer::ArraySize
// Returns the size of the given array
// ---------------------------------------------------------------------------
//
TInt CChoiceListExContainer::ArraySizeL( TInt aResourceId )
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );    
    CDesCArray* array = NULL;    
    array = reader.ReadDesC16ArrayL();
    CleanupStack::PopAndDestroy(); // reader
    
    TInt size = 0;
    size = array->Count();
    delete array;

    return size;
    }
    
// ---------------------------------------------------------------------------
// CChoiceListExContainer::HandleResourceChange
// 
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }
    }

// ---------------------------------------------------------------------------
// CChoiceListExContainer::SizeChanged
// 
// ---------------------------------------------------------------------------
//
void CChoiceListExContainer::SizeChanged()
    {
    if( iLabel) 
        {
        iLabel->SetRect( TRect( TPoint(Rect().iTl.iX, Rect().iTl.iY + 35), TPoint(Rect().iBr.iX, Rect().iTl.iY + 75  ) ) );
        }
    if( iChoiceList )
        {
        iChoiceList->SetRect( TRect( TPoint(Rect().iTl.iX, Rect().iTl.iY + 75), TPoint(Rect().iBr.iX, Rect().iTl.iY + 105  ) ) );
        }
    
    }
