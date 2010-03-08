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
#include <aknutils.h> // for AKN_LAF_COLOR
#include <eikappui.h>
#include <eikenv.h>
#include <eiklabel.h> // for label
#include <aknform.h>  // for form control
#include <aknexpopfield.rsg>

#include "AknExPopField.hrh"
#include "AknExPopFieldView.h"
#include "AknExPopFieldContainer.h"
#include "AknExPopFieldTextValueDialog.h"
#include "AknExPopFieldOptionalListDialog.h"
#include "AknExPopFieldManyLineDialog.h"
#include "AknExPopFieldMultiPageDialog.h"
#include "AknExPopFieldAttribSetDialog.h"



// MACROS
/**
* Define background color with AKN_LAF_COLOR macro:
* DO NOT CALL it in SizeChanged(), ConstructL() or ActivateL() method,
* because it messes up color scheme changes. Especially if you're using 
* colors 226-248. If you store color values, be prepared to update 
* TRgb's you store when color palette is changed! Best thing to do 
* is to make your Draw() methods call AKN_LAF_COLOR(). 
*/
#define AKNEXPOPFIELD_BACKGROUND_COLOR AKN_LAF_COLOR( 219 )


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::CAknExPopFieldContainer()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldContainer::CAknExPopFieldContainer( CAknExPopFieldView* aView )
                            : iView( aView ), iLabel( NULL )
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::ConstructL( const TRect& aRect )
    {
    // Creates window.
    CreateWindowL();

    // Creates label object.
    CreateLabelL();

    SetRect( aRect ); // Sets rectangle of frame.
    ActivateL(); // Activates the window. (Ready to draw)
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::~CAknExPopFieldContainer()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldContainer::~CAknExPopFieldContainer()
    {
    delete iLabel;
    }

// ----------------------------------------------------------------------------
// void CAknExPopFieldContainer::SetTextToLabelL()
//
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::SetTextToLabelL( TInt aResourceId )
    {
    TBuf<KAknExPopFieldMessageBufLength> messageString;
    iCoeEnv->ReadResource( messageString , aResourceId );
    iLabel->SetTextL( messageString );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::DisplayTextPopFieldL()
// Display Text PopField
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::DisplayTextPopFieldL( TInt aResourceId )
    {
    CAknExPopFieldTextValueDialog* dlg =
        CAknExPopFieldTextValueDialog::NewLC();
    CleanupStack::Pop();
    dlg->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::DisplayOptionalTextPopFieldL()
// Display Optional TextPopField
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::DisplayOptionalTextPopFieldL( TInt aResourceId )
    {
    CAknExPopFieldOptionalListDialog* dlg =
        CAknExPopFieldOptionalListDialog::NewLC();
    CleanupStack::Pop();
    dlg->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::DisplayManyLineTextPopFieldL()
// Display ManyLine Text PopField
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::DisplayManyLineTextPopFieldL( TInt aResourceId )
    {
    CAknExPopFieldManyLineDialog* dlg =
        CAknExPopFieldManyLineDialog::NewLC();
    CleanupStack::Pop();
    dlg->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::DisplayMultiPagePopFieldL( )
// Display MultiPage PopField
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::DisplayMultiPagePopFieldL( TInt aResourceId )
    {
    CAknExPopFieldMultiPageDialog* dlg =
        CAknExPopFieldMultiPageDialog::NewLC();
    CleanupStack::Pop();
    dlg->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::DisplaySetAttribsPopFieldL()
// Display Set Attribs PopField
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::DisplaySetAttribsPopFieldL( TInt aResourceId )
    {
    CAknExPopFieldAttribSetDialog* dlg =
        CAknExPopFieldAttribSetDialog::NewLC();
    CleanupStack::Pop();
    dlg->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::CreateLabelL()
// Creates label object.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::CreateLabelL()
    {
    if ( !iLabel )
        {
        iLabel = new ( ELeave ) CEikLabel;
        iLabel->SetContainerWindowL( *this );
        iLabel->SetAlignment( EHCenterVCenter );
        iLabel->SetFont( LatinBold19() );
        }
    SetTextToLabelL( R_AKNEXPOPFIELD_TEXT_LABEL_TITLE );
    }

// ----------------------------------------------------------------------------
// TKeyResponse CAknExPopFieldContainer::OfferKeyEventL()
// Handles the key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CAknExPopFieldContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType != EEventKey ) // Is not key event?
        {
        return EKeyWasNotConsumed;
        }
    switch ( aKeyEvent.iCode ) // The code of key event is...
        {
        case EKeySpace: // Space key.
            // Requires to display next outline screen.
            iView->DisplayNextOutlineL();
            DrawNow();
            break;
        default:
            return EKeyWasNotConsumed;
        }
    return EKeyWasConsumed;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::SizeChanged()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::SizeChanged()
    {
    iLabel->SetRect( Rect() );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::CountComponentControls()
// Returns number of component.
// ----------------------------------------------------------------------------
//
TInt CAknExPopFieldContainer::CountComponentControls() const
    {
    // returns nbr of controls inside this container
    return KAknExPopFieldCountComponent;
    }

// ----------------------------------------------------------------------------
// CCoeControl* CAknExPopFieldContainer::ComponentControl()
// Returns pointer to particular component.
// ----------------------------------------------------------------------------
//
CCoeControl* CAknExPopFieldContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex ) // Component is...
        {
        case EAknExPopFieldComponentLabel: // Label object.
            return iLabel;
        default: // Exception : returns NULL value.
            return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldContainer::Draw() 
// Clears the window's rectangle.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( AKNEXPOPFIELD_BACKGROUND_COLOR);
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// End of File
