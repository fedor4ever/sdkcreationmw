/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <coemain.h>
#include <aknutils.h>
#include "HelloWorldBasicAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppView* CHelloWorldBasicAppView::NewL( const TRect& aRect )
    {
    CHelloWorldBasicAppView* self = CHelloWorldBasicAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppView* CHelloWorldBasicAppView::NewLC( const TRect& aRect )
    {
    CHelloWorldBasicAppView* self = new ( ELeave ) CHelloWorldBasicAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // set the font
    iFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);

    iText.Zero();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::CHelloWorldBasicAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppView::CHelloWorldBasicAppView()
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::~CHelloWorldBasicAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppView::~CHelloWorldBasicAppView()
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // note that the whole screen is drawn everytime, so aRect-parameter
    // is ignored

    // Get the standard graphics context
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite);
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    // Gets the control's extent
    TRect rect( Rect());

    // draw also text, if user has given it via dialog
    if (iText.Length() > 0)
        {
        gc.UseFont(iFont);
        gc.DrawText(iText, rect, Rect().Height()/3, CGraphicsContext::ECenter );
        gc.DiscardFont();
        }
    else
        {
        gc.Clear( rect );
        }
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppView::SizeChanged()
    {
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::GetText()
// Called by AppUi to either set the text (via dialog) or zero the contents.
// -----------------------------------------------------------------------------
//
TDes & CHelloWorldBasicAppView::GetText()
    {
    return iText;
    }

// End of File

