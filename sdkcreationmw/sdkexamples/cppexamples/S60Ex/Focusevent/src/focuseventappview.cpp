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
#include <hal.h>
#include <eikenv.h>
#include <AknUtils.h>

#include "FocusEventAppView.h"
#include "EikonEnvironment.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFocusEventAppView::CFocusEventAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFocusEventAppView::CFocusEventAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFocusEventAppView* CFocusEventAppView::NewL( const TRect& aRect )
    {
    CFocusEventAppView* self = NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFocusEventAppView* CFocusEventAppView::NewLC( const TRect& aRect )
    {
    CFocusEventAppView* self = new ( ELeave ) CFocusEventAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFocusEventAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();

    // Add the first blank line to the array of text lines
    User::LeaveIfError( iTextLines.Append( KNullDesC() ) );
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::~CFocusEventAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CFocusEventAppView::~CFocusEventAppView()
    {
    iTextLines.Close();
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CFocusEventAppView::Draw( const TRect& /*aRect*/ ) const
    {

    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect=Rect();
    gc.UseFont( NEikonEnvironment::EikEnv().AnnotationFont() );

    // Clears the screen
    gc.Clear( drawRect );

    // Draw all lines of text
    TInt count;
    for ( count = 0; count < iTextLines.Count(); count++ )
        {
        gc.DrawText( iTextLines[ count ],
        TPoint( KTextXMargin, KTextMinY+
                                     ( count*KTextDistanceBetweenLines ) ) );
        }
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::Print()
// Show text on screen.
// -----------------------------------------------------------------------------
//
void CFocusEventAppView::Print( const TDesC& aText )
    {
    ShowTextOnScreen( aText );
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::ShowTextOnScreen()
// Show text on screen.
// -----------------------------------------------------------------------------
//
void CFocusEventAppView::ShowTextOnScreen( const TDesC& aText )
    {
    // Add the text line onto the last line
    TInt lastTextLine = iTextLines.Count() - 1;

    // Make sure the text line doesn't get too long for the buffer.
    // This should really clip rather than not do anything
    // if the line is too long.
    if ( aText.Length() + iTextLines[ lastTextLine ].Length()
                                  <= KTestFrameworkMaxScreenMessageLength )
        {
        iTextLines[ lastTextLine ].Append( aText );
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CFocusEventAppView::PrintNewLineL()
// Print new line.
// -----------------------------------------------------------------------------
//
void CFocusEventAppView::PrintNewLineL()
    {
    // Add a new line onto the array
    // If the array is already at the maximum size,
    // delete the first line of text
    if ( iTextLines.Count() >= KMaxTextLines )
        {
        iTextLines.Remove( 0 );
        }
    User::LeaveIfError( iTextLines.Append( KNullDesC() ) );
    DrawNow();
    }

// ---------------------------------------------------------
// CFocusEventAppView::HandleResourceChange()
// Called by framework when layout is changed.
// ---------------------------------------------------------
//
void CFocusEventAppView::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
    }
// End of File
