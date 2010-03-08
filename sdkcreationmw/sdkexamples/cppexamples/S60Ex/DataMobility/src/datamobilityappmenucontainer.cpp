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
* Description:  CDataMobilityAppMenuContainer implementation.
*
*/


// INCLUDE FILES
#include <aknlists.h>  // ListBox
#include <barsread.h> // for TResourceReader
#include <AknUtils.h>

#include "datamobilityappmenucontainer.h"
#include "datamobility.hrh"
#include <datamobility.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::ConstructL
//
// Symbian OS two phased constructor
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    iListBox = new ( ELeave ) CAknSingleStyleListBox();
    iListBox->SetContainerWindowL( *this );
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_MENU_LISTBOX );
    iListBox->SetListBoxObserver( this );
    iListBox->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); // resource stuffs.
    iListBox->ActivateL(); // Sets control as ready to be drawn

    SetRect( aRect );
    ActivateL();
    }


// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::~CDataMobilityAppMenuContainer
//
// Destructor
// -----------------------------------------------------------------------------
//
CDataMobilityAppMenuContainer::~CDataMobilityAppMenuContainer()
    {
    if( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::SizeChanged
//
// Called by framework when the view size is changed
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() ); // Sets rectangle of listbox.
        }
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::CountComponentControls
//
// Returns number of controls indside this container.
// -----------------------------------------------------------------------------
//
TInt CDataMobilityAppMenuContainer::CountComponentControls() const
    {
    return 1;
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::ComponentControl
//
// Gets the specified component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CDataMobilityAppMenuContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::HandleListBoxEventL
//
// Handles list box events.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuContainer::HandleListBoxEventL( CEikListBox* aListBox,
                                                         TListBoxEvent aEventType )
    {
    if ( aEventType == EEventEnterKeyPressed && aListBox )
        {
        HandleSelectedListItemL( aListBox->CurrentItemIndex() );
        }
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::HandleSelectedListItemL
//
// Method HandleSelectedListItemL handles valix index.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppMenuContainer::HandleSelectedListItemL( TInt aIndex ) const
    {
    CEikAppUi* appUi = static_cast<CEikAppUi*>( iCoeEnv->AppUi() );

    // Change active view.
    switch ( aIndex )
        {
        case 0:
            appUi->HandleCommandL( EAppUIGoToStandardApplication );
            break;
        case 1:
            appUi->HandleCommandL( EAppUIGoToApplicationUsingSnap );
            break;
        case 2:
            appUi->HandleCommandL( EAppUIGoToTriggeringConnectionDialog );
            break;
        case 3:
            appUi->HandleCommandL( EAppUIGoToALRApplication );
            break;
        case 4:
            appUi->HandleCommandL( EAppUIGoToALRApplicationUsingDefaultConnection );
            break;
        case 5:
            appUi->HandleCommandL( EAppUIGoToUsingConnectionSetting );
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer::OfferKeyEventL
//
// Handles key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CDataMobilityAppMenuContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    // Offers key events to list box
    if ( iListBox )
        {
        return iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppMenuContainer:::GetActiveLine
//
// Get currently selected items index.
// ----------------------------------------------------------------------------
//
TInt CDataMobilityAppMenuContainer::GetActiveLine() const
    {
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------
// CDataMobilityAppMenuContainer::HandleResourceChange()
//
// Called by framework when layout is changed.
// ---------------------------------------------------------
//
void CDataMobilityAppMenuContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        }
    }

// End of File
