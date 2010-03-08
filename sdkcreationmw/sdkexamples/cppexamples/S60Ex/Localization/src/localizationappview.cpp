/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LocalizationAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLocalizationAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLocalizationAppView* CLocalizationAppView::NewL( const TRect& aRect )
    {
    CLocalizationAppView* self = CLocalizationAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CLocalizationAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLocalizationAppView* CLocalizationAppView::NewLC( const TRect& aRect )
    {
    CLocalizationAppView* self = new ( ELeave ) CLocalizationAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CLocalizationAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLocalizationAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CLocalizationAppView::CLocalizationAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLocalizationAppView::CLocalizationAppView()
: iImage(0)
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CLocalizationAppView::~CLocalizationAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CLocalizationAppView::~CLocalizationAppView()
    {
    // No implementation required
    delete iImage;
    }


// ---------------------------------------------------------------------------
// CLocalizationAppView::DrawImage()
// Draw the image on the display.
// ---------------------------------------------------------------------------
//
void CLocalizationAppView::DrawImage(CFbsBitmap* aImage)
    {
    delete iImage;
    iImage = aImage;
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CLocalizationAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CLocalizationAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect( Rect());
    
    // Clears the screen
    gc.Clear( drawRect );
    
    if(iImage)
        {
        // Show the bitmap double size and centered to screen
        TInt scrWidth = Rect().Width();
        TInt scrHeight = Rect().Height();
        TInt bmpWidth = iImage->SizeInPixels().iWidth;
        TInt bmpHeight = iImage->SizeInPixels().iHeight;
        TInt aX = ( scrWidth - bmpWidth * 2 ) / 2;
        TInt aY = ( scrHeight - bmpHeight * 2 ) / 2;
        TInt bX = aX + bmpWidth * 2;
        TInt bY = aY + bmpHeight * 2;
        
        drawRect.SetRect( aX, aY, bX, bY );
        gc.DrawBitmap ( drawRect, iImage );
        }
  	}

// -----------------------------------------------------------------------------
// CLocalizationAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CLocalizationAppView::SizeChanged()
    {  
    DrawNow();
    }
// End of File
