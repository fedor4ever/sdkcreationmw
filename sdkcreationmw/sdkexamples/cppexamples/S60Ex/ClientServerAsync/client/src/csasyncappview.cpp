/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikenv.h>
#include <avkon.rsg>

#include "CSAsyncAppView.h"
#include "CSAsyncDocument.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSAsyncAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSAsyncAppView* CCSAsyncAppView::NewL( const TRect& aRect,
                                        CCSAsyncDocument& aDocument )
    {
    CCSAsyncAppView* self = CCSAsyncAppView::NewLC( aRect, aDocument );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSAsyncAppView* CCSAsyncAppView::NewLC( const TRect& aRect,
                                         CCSAsyncDocument& aDocument )
    {
    CCSAsyncAppView* self = new ( ELeave ) CCSAsyncAppView( aDocument );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSAsyncAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppView::CCSAsyncAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCSAsyncAppView::CCSAsyncAppView( CCSAsyncDocument& aDocument )
: iDocument( aDocument )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppView::~CCSAsyncAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSAsyncAppView::~CCSAsyncAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppView::Draw()
// Draws this CCSAsyncAppView to the screen.
// -----------------------------------------------------------------------------
//
void CCSAsyncAppView::Draw( const TRect& /*aRect*/ ) const
    {

    // Clear the screen
    CWindowGc& gc = SystemGc();
    gc.Clear( Rect() );

    TBuf<30> des;
    const TTime& time = iDocument.Time();

    // Read time format string from AVKON resource
    HBufC* timeFormatString = iEikonEnv->AllocReadResourceLC(R_QTN_TIME_LONG);
    // It would also be possible to define your own format string instead, 
    // for example like this:
    // _LIT( KHoursMinsSecs, "%-B%:0%J%:1%T%:2%S%:3%+B" );

    TRAPD( err, time.FormatL( des, *timeFormatString ) );
    CleanupStack::PopAndDestroy(); // timeFormatString

    if ( err == KErrNone )
        {
        const CFont* font = iEikonEnv->NormalFont();
        gc.UseFont( font );
        gc.DrawText( des, TPoint( 10,20 ) );
        }
    }


// End of File

