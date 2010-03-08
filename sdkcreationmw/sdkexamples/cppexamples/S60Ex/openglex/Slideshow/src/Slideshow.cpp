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

#include <aknnotewrappers.h>
#include <e32math.h>
#include "slideshow.h"

// CONSTANTS

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CSlideshow::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CSlideshow* CSlideshow::NewL( TUint aWidth, TUint aHeight )
	{
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CSlideshow* self = new (ELeave) CSlideshow( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

// ----------------------------------------------------------------------------
// CSlideshow::CSlideshow
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSlideshow::CSlideshow( TUint aWidth, TUint aHeight )
	: CFiniteStateMachine(),
      iCurrentPicture(0),
      iNextPicture(0),
      iRequestedPictureDelta(0),
      iTransitionRenderer(0),
      iTransitionTime(0)
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
	}

// ----------------------------------------------------------------------------
// CSlideshow::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CSlideshow::ConstructL( void )
	{
	}

// ----------------------------------------------------------------------------
// CSlideshow::~CSlideshow()
// Destructor.
// ----------------------------------------------------------------------------
//
CSlideshow::~CSlideshow()
	{
	}

// ----------------------------------------------------------------------------
// CSlideshow::AppInit
// Application initialization code.
// ----------------------------------------------------------------------------
//
void CSlideshow::AppInitL( void )
	{
    // Construct a texture manager that uses the application's private
    // directory as the source location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight, 0, 0, 0, 0, 0,
                                              this );
    // Initialize viewport and projection.
		SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Enable back face culling.
    glDisable( GL_CULL_FACE  );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    // Disable lighting
    glDisable( GL_LIGHTING );

    // Initialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );
    glMatrixMode( GL_PROJECTION );

    // Set best perspective correction
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Enable vertex arrays.
    glEnableClientState( GL_VERTEX_ARRAY );

    // Enable texture arrays.
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );


    // Push the textures into the loading queue
    iTextureManager->RequestToLoad( _L("los_cristianos.jpg"), &iPictures[0] );
    iTextureManager->RequestToLoad( _L("sky.jpg"), &iPictures[1] );
    iTextureManager->RequestToLoad( _L("sugar_loaf.jpg"), &iPictures[2] );
    iTextureManager->RequestToLoad( _L("parati.jpg"), &iPictures[3] );
    iTextureManager->RequestToLoad( _L("banana_trees.jpg"), &iPictures[4] );
    iPictureCount = sizeof(iPictures) / sizeof(iPictures[0]);

    // Generate some masks
    glGenTextures( 1, &iMasks[0].iID );
    glGenTextures( 1, &iMasks[1].iID );

    TUint8* maskData = new TUint8[64 * 64 * 4];
    TUint8* maskDataPtr = maskData;
    TInt x, y;

    // The first mask has a band of colors in the middle
    for (y = 0; y < 64; y++)
        {
        for (x = 0; x < 64; x++)
            {
            *maskDataPtr++ = Min(Max(Abs(24 - y) * 16, 0), 255);
            *maskDataPtr++ = Min(Max(Abs(28 - y) * 16, 0), 255);
            *maskDataPtr++ = Min(Max(Abs(32 - y) * 16, 0), 255);
            *maskDataPtr++ = Min(Max((32 - y) * 16, 0), 255);
            }
        }

    glBindTexture( GL_TEXTURE_2D, iMasks[0].iID );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, maskData);

    // The second mask has a wavy alpha threshold in the middle
    maskDataPtr = maskData;
    for (y = 0; y < 64; y++)
        {
        for (x = 0; x < 64; x++)
            {
            TReal s;
            Math::Sin(s, x / 4.0);
            TInt maskFactor = Min(Max((32 - y - s * 6) * 16, 0), 255);
            *maskDataPtr++ = 255;
            *maskDataPtr++ = 255;
            *maskDataPtr++ = 255;
            *maskDataPtr++ = maskFactor;
            }
        }
    glBindTexture( GL_TEXTURE_2D, iMasks[1].iID );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, maskData);

    delete[] maskData;

    // Start to load the queued textures
    iTextureManager->DoLoadL();
	}

// ----------------------------------------------------------------------------
// CSlideshow::AppExit
// Application cleanup code.
// ----------------------------------------------------------------------------
//
void CSlideshow::AppExit( void )
	{
    delete iTextureManager;
    delete iTransitionRenderer;
	}


// ----------------------------------------------------------------------------
// CSlideshow::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// ----------------------------------------------------------------------------
//
void CSlideshow::AppCycle( TInt /*aFrame*/,
                       TReal /*aTimeSecs*/,
                       TReal aDeltaTimeSecs )
	{
	  // Make sure the timestep is non-zero, since otherwise the animation wouldn't progress at all.
	  if (aDeltaTimeSecs < 0.04)
	    aDeltaTimeSecs = 0.04;

    // Stop the transition if it's complete
    if (iTransitionRenderer && iTransitionTime <= 0)
        {
        delete iTransitionRenderer;
        iTransitionRenderer = 0;
        iCurrentPicture = iNextPicture;
        }

    // Advance the slideshow if we have been requested to
    if (iRequestedPictureDelta && !iTransitionRenderer)
        {
        iTransitionRenderer = CreateTransitionRenderer((iRequestedPictureDelta > 0) ? ENext : EPrevious);
        iTransitionTime = 1.0;

        TInt direction = (iRequestedPictureDelta > 0) ? 1 : -1;
        iNextPicture = (iCurrentPicture + direction + iPictureCount) % iPictureCount;
        iRequestedPictureDelta -= direction;
        }

    // Render the current picture. If there is no transition going on, enable
    // bilinear filtering to improve image quality.
    iStaticRenderer.Render(iPictures[iCurrentPicture], iTransitionRenderer == NULL);

    // Render the transition and the next picture
    if (iTransitionTime > 0 && iTransitionRenderer)
        {
        iTransitionRenderer->Render(iPictures[iNextPicture], iTransitionTime);
        // Accelerate the transition speed if we are many pictures behind
        iTransitionTime -= aDeltaTimeSecs * (Abs(iRequestedPictureDelta) + 1);
        }
	}

void CSlideshow::ShowNextPicture( void )
    {
    iRequestedPictureDelta++;
    }

void CSlideshow::ShowPreviousPicture( void )
    {
    iRequestedPictureDelta--;
    }

// ----------------------------------------------------------------------------
// CSlideshow::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ----------------------------------------------------------------------------
//
void CSlideshow::OnEnterStateL( TInt /*aState*/ )
	{
	}

// ----------------------------------------------------------------------------
// CSlideshow::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation starts
// ----------------------------------------------------------------------------
//
void CSlideshow::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ----------------------------------------------------------------------------
// CSlideshow::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation is completed
// ----------------------------------------------------------------------------
void CSlideshow::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
        SetStateL( ERunning );
		}
	}

// ----------------------------------------------------------------------------
// CSlideshow::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// ----------------------------------------------------------------------------
//
void CSlideshow::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Notify the texture manager of screen size change
    iTextureManager->SetScreenSize( aWidth, aHeight );

    // Reinitialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );
    }

MTransitionRenderer* CSlideshow::CreateTransitionRenderer(TDirection dir)
    {
    // Choose a transition renderer class randomly
    switch (Math::Random() % 3)
        {
        case 0:
            {
            // Choose a sweep style randomly
            const CMaskTransition::TSweepStyle s[] =
                {
                CMaskTransition::ESweepCurved,
                CMaskTransition::ESweepCurved2,
                CMaskTransition::ESweepHorizontal,
                CMaskTransition::ESweepVertical
                };
            return new CMaskTransition(iMasks[Math::Random() % (sizeof(iMasks) / sizeof(iMasks[0]))],
                                       s[Math::Random() % (sizeof(s) / sizeof(s[0]))]);
            }
        case 1:
            if (dir == ENext)
                return new CWhiskTransition(CWhiskTransition::ERight);
            else
                return new CWhiskTransition(CWhiskTransition::ELeft);
        case 2:
            if (dir == ENext)
                return new CCurtainTransition(CCurtainTransition::ERight);
            else
                return new CCurtainTransition(CCurtainTransition::ELeft);
        }
    return 0;
    }

// End of File
