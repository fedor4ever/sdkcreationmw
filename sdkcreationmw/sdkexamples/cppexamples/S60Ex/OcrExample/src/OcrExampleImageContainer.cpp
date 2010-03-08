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
#include "OcrExampleImageContainer.h"
#include "OcrExampleSettingsData.h"

// ================= MEMBER FUNCTIONS ==========================================

COcrExampleImageContainer::COcrExampleImageContainer(
                                            TOcrExampleSettingsData& aSettings,
                                            TScreenUpdateData &aData )
    : iSettings(aSettings)
    , iScrData(aData)
    {
    }

// -----------------------------------------------------------------------------
// COcrExampleImageContainer::NewL()
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
COcrExampleImageContainer* COcrExampleImageContainer::NewL(
                                            const TRect& aRect,
                                            TOcrExampleSettingsData& aSettings,
                                            TScreenUpdateData &aData )
    {
    COcrExampleImageContainer* self =
        COcrExampleImageContainer::NewLC( aRect, aSettings, aData );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// COcrExampleImageContainer::NewLC()
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
COcrExampleImageContainer* COcrExampleImageContainer::NewLC(
                                            const TRect& aRect,
                                            TOcrExampleSettingsData& aSettings,
                                            TScreenUpdateData &aData )
    {
    COcrExampleImageContainer* self =
        new ( ELeave ) COcrExampleImageContainer(aSettings, aData);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// COcrExampleImageContainer::ConstructL(const TRect& aRect)
// Symbian two phased constructor
// -----------------------------------------------------------------------------
//
void COcrExampleImageContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    // Set the windows size
    SetRect( aRect );

    // note, parent view will activate this container
    }

// -----------------------------------------------------------------------------
// COcrExampleImageContainer::~COcrExampleImageContainer()
// Destructor
// -----------------------------------------------------------------------------
//
COcrExampleImageContainer::~COcrExampleImageContainer()
    {
    }


// -----------------------------------------------------------------------------
// COcrExampleImageContainer::OfferKeyEventL
// From CCoeControl.
// -----------------------------------------------------------------------------
//
TKeyResponse COcrExampleImageContainer::OfferKeyEventL( const TKeyEvent& aEvent,
                                                        TEventCode aType )
    {
    const TInt stepInPixels = 10;
    TKeyResponse resp = EKeyWasNotConsumed;

    // key-events meaningful only with crosshair
    if ((iScrData.iCrosshair) && (aType == EEventKey))
        {
        switch( aEvent.iScanCode )
            {
            case EStdKeyRightArrow:
                iScrData.iCrosshair->Move( stepInPixels, 0 );
                DrawDeferred();
                return EKeyWasConsumed;

            case EStdKeyLeftArrow:
                iScrData.iCrosshair->Move( -stepInPixels, 0 );
                DrawDeferred();
                return EKeyWasConsumed;

            case EStdKeyDownArrow:
                iScrData.iCrosshair->Move( 0, stepInPixels );
                DrawDeferred();
                return EKeyWasConsumed;

            case EStdKeyUpArrow:
                iScrData.iCrosshair->Move( 0, -stepInPixels );
                DrawDeferred();
                return EKeyWasConsumed;
            }
        }
    return resp;
    }

// -----------------------------------------------------------------------------
// COcrExampleImageContainer::Draw(const TRect& aRect) const
// Draw this application's view to the screen
// -----------------------------------------------------------------------------
//
void COcrExampleImageContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    // draw empty background
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );

    // if image has been loaded, draw it also
    if (iScrData.iScaledBitMap->Handle())
        {
        gc.BitBlt(TPoint(0,0), iScrData.iScaledBitMap);

        gc.SetPenStyle( CGraphicsContext::EDashedPen );
        gc.SetPenColor( iSettings.GetHighlightColor() );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );

        // if crosshair activated, draw it
        if (iScrData.iCrosshair)
            {
            TSize size = iSettings.GetCrosshairSize(iScrData.iScaledBitMap->SizeInPixels());
            iScrData.iCrosshair->SetSize(size);
            gc.DrawRect( *(iScrData.iCrosshair) );
            }
        else if (iScrData.iBlockInfo)
            {
            // if image layout has been analyzed, hightlight text-areas
            for ( TInt i=0; i<iScrData.iBlockCount; ++i)
                {
                // Note! blockInfo contains rects from the original image.
                // adjust sizes and positions to fit the resized image on the
                // screen.
                gc.DrawRect( iScrData.iRectScaler.ScaleDown(iScrData.iBlockInfo[i].iRect) );
                }
            }
        }
    }


// End of File
