/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "bitmapmethods.h"

#include <eikenv.h>
#include <fbs.h>

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateGraphicsContextLC()
// Create a graphics context and leave it on the cleanup stack.
// ----------------------------------------------------------------------------
//
CFbsBitGc* NBitmapMethods
::CreateGraphicsContextLC( CFbsBitmapDevice& aBitmapDevice )
    {
    CFbsBitGc* graphicsContext = NULL;
    User::LeaveIfError( aBitmapDevice.CreateContext( graphicsContext ) );
    CleanupStack::PushL( graphicsContext );
    return graphicsContext;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateGraphicsContextL()
// Create a graphics context.
// ----------------------------------------------------------------------------
//
CFbsBitGc* NBitmapMethods
::CreateGraphicsContextL( CFbsBitmapDevice& aBitmapDevice )
    {
    CFbsBitGc* gc = CreateGraphicsContextLC( aBitmapDevice );
    CleanupStack::Pop( gc );
    return gc;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateBitmapDeviceLC()
// Create a bitmap device and leave it on the cleanup stack.
// ----------------------------------------------------------------------------
//
CFbsBitmapDevice* NBitmapMethods::CreateBitmapDeviceLC( CFbsBitmap& aBitmap )
    {
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( &aBitmap );
    CleanupStack::PushL( bitmapDevice );
    return bitmapDevice;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateBitmapDeviceL()
// Create a bitmap device.
// ----------------------------------------------------------------------------
//
CFbsBitmapDevice* NBitmapMethods::CreateBitmapDeviceL( CFbsBitmap& aBitmap )
    {
    CFbsBitmapDevice* device = CreateBitmapDeviceLC( aBitmap );
    CleanupStack::Pop( device );
    return device;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateBitmapLC()
// Create a bitmap and leave it on the cleanup stack.
// ----------------------------------------------------------------------------
//
CFbsBitmap* NBitmapMethods
::CreateBitmapLC( TSize aSizeInPixels, TDisplayMode aDispMode )
    {
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( aSizeInPixels,aDispMode ) );
    ASSERT( ( bitmap->DisplayMode() == KColourDepth )
                                    || ( bitmap->DisplayMode()  == EGray2 ) );
    return bitmap;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateBitmapL()
// Create a bitmap.
// ----------------------------------------------------------------------------
//
CFbsBitmap* NBitmapMethods
::CreateBitmapL( TSize aSizeInPixels, TDisplayMode aDispMode )
    {
    CFbsBitmap* bitmap = CreateBitmapLC( aSizeInPixels, aDispMode );
    CleanupStack::Pop( bitmap );
    return bitmap;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateBitmapLC()
// Create a bitmap and leave it on the cleanup stack.
// ----------------------------------------------------------------------------
//
CFbsBitmap* NBitmapMethods::CreateBitmapLC( const TDesC& aFileName, TInt aId )
    {
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap();

    CleanupStack::PushL( bitmap );
    TInt loadException = bitmap->Load( aFileName, aId );
    User::LeaveIfError( loadException );

    if ( ( bitmap->DisplayMode() == KColourDepth )
                                 || ( bitmap->DisplayMode()  == EGray2 ) )
        {
        return bitmap;
        }
    else
        {
        CFbsBitmap* newBitmap = CreateBitmapLC(    bitmap->SizeInPixels(),
                                                KColourDepth );

        CFbsBitmapDevice* bitmapDevice = CreateBitmapDeviceLC( *newBitmap );
        CFbsBitGc* bitmapGc = CreateGraphicsContextLC( *bitmapDevice );
        bitmapGc->BitBlt( TPoint( 0,0 ),
                          bitmap, TRect( bitmap->SizeInPixels() ) );
        CleanupStack::PopAndDestroy( 2 ); // gc and device
        // The next three lines are here to get rid
        // of the old bitmap but keep the new one
        CleanupStack::Pop( newBitmap );
        CleanupStack::PopAndDestroy( bitmap );
        CleanupStack::PushL( newBitmap );

        return newBitmap;
        }
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::CreateBitmapL()
// Create a bitmap.
// ----------------------------------------------------------------------------
//
CFbsBitmap* NBitmapMethods::CreateBitmapL( const TDesC& aFileName, TInt aId )
    {
    CFbsBitmap* bitmap = CreateBitmapLC( aFileName, aId );
    CleanupStack::Pop( bitmap );
    return bitmap;
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::BitBltMaskedEntireBitmap()
// Blit the entire of a bitmap with a mask onto a gc.
// ----------------------------------------------------------------------------
//
void NBitmapMethods::BitBltMaskedEntireBitmap( CFbsBitGc& aTargetGc,
                                               TPoint aTopLeft,
                                               const CFbsBitmap& aBitmap,
                                               const CFbsBitmap& aBitMask )
    {
    PartialReset( aTargetGc );
    aTargetGc.BitBltMasked( aTopLeft,
                            &aBitmap,
                            aBitmap.SizeInPixels(),
                            &aBitMask,
                            ETrue );
    PartialReset( aTargetGc );
    }

// ----------------------------------------------------------------------------
// NBitmapMethods::PartialReset()
// Reset a gc.
// ----------------------------------------------------------------------------
//
void NBitmapMethods::PartialReset( CFbsBitGc& aGc )
    {
    aGc.SetPenSize( TSize( 1,1 ) );
    aGc.SetPenColor( KRgbBlack );
    aGc.SetPenStyle( CFbsBitGc::ESolidPen );
    aGc.SetDrawMode( CFbsBitGc::EDrawModePEN );
    aGc.DiscardFont();
    aGc.DiscardBrushPattern();
    aGc.SetBrushColor( KRgbWhite );
    aGc.SetBrushStyle( CFbsBitGc::ENullBrush );
    aGc.SetCharJustification( 0,0 );
    aGc.SetWordJustification( 0,0 );
    aGc.SetDitherOrigin( TPoint( 0,0 ) );
    aGc.SetPenStyle( CFbsBitGc::ENullPen );
    aGc.SetShadowMode( EFalse );
    aGc.SetStrikethroughStyle( EStrikethroughOff );
    aGc.SetUnderlineStyle( EUnderlineOff );
    aGc.SetUserDisplayMode( ENone );
    }

// End of File
