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
#include <GraphicsAppImages.mbg>
#include <aknview.h>
#include <AknUtils.h>
#include <akndef.h>
#include <eikapp.h>
#include <eikappui.h>
#include "IconFileProvider.h"
#include "GraphicsAppView.h"
#include "bitmapmethods.h"
#include "sprite.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGraphicsAppView::CGraphicsAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGraphicsAppView::CGraphicsAppView()
    : iOffScreenBitmap(NULL)
    , iOffScreenBitmapDevice(NULL)
    , iOffScreenBitmapGc(NULL)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGraphicsAppView* CGraphicsAppView::NewL( const TRect& aRect )
    {
    CGraphicsAppView* self = NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGraphicsAppView* CGraphicsAppView::NewLC( const TRect& aRect )
    {
    CGraphicsAppView* self = new ( ELeave ) CGraphicsAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the window's size and position
    SetRect( aRect );

    // Load in the bitmap images from the multi bitmap file

    // create and open file server session
    User::LeaveIfError(iFsSession.Connect());
    iFsSession.ShareProtected();
    TFileName filePath;

    // set path of the bitmap file
    User::LeaveIfError(iFsSession.PrivatePath(filePath));

    // append the MBM file name to the private path
    filePath.Append(KMultiBitmapFilename);

    // insert the drive to the private path
    TParsePtrC parse((CEikonEnv::Static()->EikAppUi()->Application())->AppFullName());
    filePath.Insert(0, parse.Drive());

    // with the help of icon-provider AknIconUtils can get get the file
    // from this application's private folder
	iIconProvider = CIconFileProvider::NewL( iFsSession, filePath );

    iBackgroundImage = AknIconUtils::CreateIconL(*iIconProvider,
                                                 EMbmGraphicsappimagesImage1);
	// Load sprite bitmap and mask with a single call
	AknIconUtils::CreateIconL(iSpriteImage,
	                          iSpriteMask,
	                          *iIconProvider,
	                          EMbmGraphicsappimagesImage2,
	                          EMbmGraphicsappimagesImage2_mask);

	// Load svg.-image and mask with a single call
	AknIconUtils::CreateIconL(iSpriteImage2,
	                          iSpriteMask2,
	                          *iIconProvider,
	                          EMbmGraphicsappimagesStar,
	                          EMbmGraphicsappimagesStar_mask);
    // This call preserves SVG-T vector data in memory so that the data is not needed to
    // load from the resource when the icon is subsequently scaled.
    AknIconUtils::PreserveIconData(iSpriteImage2);

    ScaleImages();

    // Create a periodic timer but don't start it yet
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    // Create the array of sprite pointers
    iSprites = new ( ELeave ) CArrayPtrFlat<CSprite> ( 1 );

    SetUpSpritesL();

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::~CGraphicsAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CGraphicsAppView::~CGraphicsAppView()
    {
	if ( iPeriodicTimer )
		{
		// Stop the periodic timer
		iPeriodicTimer->Cancel();
		}
	delete iPeriodicTimer;
	iPeriodicTimer = NULL;

	delete iOffScreenBitmapGc;
	iOffScreenBitmapGc = NULL;

	delete iOffScreenBitmapDevice;
	iOffScreenBitmapDevice = NULL;

	delete iOffScreenBitmap;
	iOffScreenBitmap = NULL;

	delete iBackgroundImage;
	iBackgroundImage = NULL;

	if ( iSprites )
		{
		iSprites->ResetAndDestroy();
		delete iSprites;
		iSprites = NULL;
		}

	delete iSpriteImage;
	iSpriteImage = NULL;

	delete iSpriteMask;
	iSpriteMask = NULL;

	delete iSpriteImage2;
	iSpriteImage2 = NULL;

	delete iSpriteMask2;
	iSpriteMask2 = NULL;

	iFsSession.Close();
	delete iIconProvider;
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::SetUpSpritesL()
// Sets up sprites.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::SetUpSpritesL()
	{
    ASSERT( iSpriteImage );
    ASSERT( iSpriteImage2 );
	ASSERT( iSprites );

	// Create the sprites
	CSprite* sprite = NULL;

	sprite = CSprite::NewLC( KInitialXSpeed,
	                         KInitialYSpeed+1,
	                         Rect().iTl,
	                         iSpriteImage,
	                         iSpriteMask );
	iSprites->AppendL( sprite );
	CleanupStack::Pop( sprite );

	sprite = CSprite::NewLC( -KInitialXSpeed,
	                         -KInitialYSpeed,
	                         Rect().iBr - iSpriteImage->SizeInPixels(),
	                         iSpriteImage,
	                         iSpriteMask );
	iSprites->AppendL( sprite );
	CleanupStack::Pop( sprite );

	sprite = CSprite::NewLC( KInitialXSpeed,
	                         -KInitialYSpeed+1,
	                         Rect().iTl + iSpriteImage2->SizeInPixels(),
	                         iSpriteImage2,
	                         iSpriteMask2 );
	iSprites->AppendL( sprite );
	CleanupStack::Pop( sprite );
	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::Draw( const TRect& /*aRect*/ ) const
    {
	// The system GC will already be activated when this function is called
	// by the framework
	UpdateDisplay();
    }

// -----------------------------------------------------------------------------
// Handle resource change event, here it is the event that occurs
// when screen mode changes.
// -----------------------------------------------------------------------------
void CGraphicsAppView::HandleResourceChange(TInt aType, const TRect& aRect)
    {
	CCoeControl::HandleResourceChange(aType);

	if( aType == KEikDynamicLayoutVariantSwitch)
	    {
        SetRect(aRect);
        ScaleImages();

        // adjust sprite positions if needed (new resolution may have caused
        // current position to be out of bounds
    	for ( TInt count = 0; count < iSprites->Count(); ++count )
    		{
            CSprite *sprite = iSprites->At(count);
    		sprite->Adjust( sprite->Image()->SizeInPixels(), Rect() );
    		}
	    }
    }


// -----------------------------------------------------------------------------
// Scales background and sprite images according to current layout.
// -----------------------------------------------------------------------------
void CGraphicsAppView::ScaleImages()
    {
    TInt x = Size().iWidth;
    TInt y = Size().iHeight;

    AknIconUtils::SetSize(iBackgroundImage,
                          Size(),
                          EAspectRatioNotPreserved);

    // target size for the sprites is ~40x40 in 352x416 resolution.
    // orientation does not affect on scaling.
    AknIconUtils::SetSize(iSpriteImage, TSize((x+y)/19, (x+y)/19));
    AknIconUtils::SetSize(iSpriteImage2, TSize((x+y)/19, (x+y)/19));

	delete iOffScreenBitmapGc;
	iOffScreenBitmapGc = NULL;
	delete iOffScreenBitmapDevice;
	iOffScreenBitmapDevice = NULL;
	delete iOffScreenBitmap;
	iOffScreenBitmap = NULL;

    // Create the off screen bitmap and device / gc
    iOffScreenBitmap = NBitmapMethods::CreateBitmapL( Size(), KColourDepth );
    iOffScreenBitmapDevice = NBitmapMethods::CreateBitmapDeviceL( *iOffScreenBitmap );
    iOffScreenBitmapGc = NBitmapMethods::CreateGraphicsContextL( *iOffScreenBitmapDevice );
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::UpdateDisplay()
// Updates the display.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::UpdateDisplay() const
    {
    CWindowGc& gc = SystemGc();

    if ( !iUsingOffScreenBitmap )
        {
        // Blit the background image onto the screen at the top left position
        gc.BitBlt( Rect().iTl,iBackgroundImage );

        // Blit the sprites on top of it using their mask to retain
		// the background where necessary
        for ( TInt count = 0; count<iSprites->Count(); ++count )
            {
            CSprite *sprite = iSprites->At(count);
            TRect sourceRect( TPoint( 0,0 ), sprite->Image()->SizeInPixels() );
            gc.BitBltMasked( sprite->Position(),
                             sprite->Image(),
                             sourceRect,
                             sprite->Mask(),
                             ETrue );
            }
        }
    else
        {
        // Blit the background image onto the off screen bitmap
		// at the top left position
        iOffScreenBitmapGc->BitBlt( TPoint( 0,0 ), iBackgroundImage );

        // Blit the sprites on top of it using its mask to retain
		// the background where necessary
        for ( TInt count = 0; count<iSprites->Count(); ++count )
            {
            CSprite *sprite = iSprites->At(count);
            NBitmapMethods::BitBltMaskedEntireBitmap( *iOffScreenBitmapGc,
                                            sprite->Position(),
                                            *(sprite->Image()),
                                            *(sprite->Mask()));
            }

        // Blit the offscreen image onto the screen at the top left position
        gc.BitBlt( Rect().iTl,iOffScreenBitmap );
        }
    }

// -----------------------------------------------------------------------------
// CGraphicsAppView::StopDemo()
// Stops the balls.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::StopDemo()
	{
	// Stop the timer if it is active
	if ( iPeriodicTimer->IsActive() )
		{
		iPeriodicTimer->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::StartOffScreenDemo()
// Start Off Screen Demo.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::StartOffScreenDemo()
	{
	iUsingOffScreenBitmap = ETrue;
	StartTimer();
	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::StartNoOffScreenDemo()
// Start No Off Screen Demo.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::StartNoOffScreenDemo()
	{
	iUsingOffScreenBitmap = EFalse;
	StartTimer();
	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::StartTimer()
// Start Timer.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::StartTimer()
	{
	// If the timer is not already running, start it
	if ( !iPeriodicTimer->IsActive() )
		{
		iPeriodicTimer->Start(1, 1, TCallBack( CGraphicsAppView::Period, this));
		}

	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::DoPeriodTask()
// Move the sprites.
// -----------------------------------------------------------------------------
//
void CGraphicsAppView::DoPeriodTask()
	{
	for ( TInt count = 0; count < iSprites->Count(); ++count )
		{
        CSprite *sprite = iSprites->At(count);
		sprite->Move( sprite->Image()->SizeInPixels(), Rect() );
		}

	// Update the screen
	CWindowGc& gc = SystemGc();
	gc.Activate( *DrawableWindow() );
	UpdateDisplay();
	gc.Deactivate();
	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::Period()
// This function is called by the periodic timer.
// -----------------------------------------------------------------------------
//
TInt CGraphicsAppView::Period( TAny* aPtr )
	{
    ( static_cast<CGraphicsAppView*>( aPtr ) )->DoPeriodTask();
    // Returning a value of TRUE indicates the callback should be done again
	return ETrue;
	}

// End of File
