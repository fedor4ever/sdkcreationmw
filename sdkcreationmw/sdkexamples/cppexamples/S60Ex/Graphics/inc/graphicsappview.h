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


#ifndef __GRAPHICSAPPVIEW_H__
#define __GRAPHICSAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>
#include <eikenv.h>
#include <AknIconUtils.h>

// FORWARD DECLARATIONS
class CIconFileProvider;
class CSprite;

namespace
{
    const TInt KInitialXSpeed = 4;
    const TInt KInitialYSpeed = 4;
    _LIT ( KMultiBitmapFilename,"GraphicsAppImages.mbm" );
}

/**
* CGraphicsAppView view class.
* An instance of this class is the Application View object
* for the Graphics example application
*/
class CGraphicsAppView : public CCoeControl
    {
    public: // Constructors and destructor
        /**
        * NewL.
        * Two-phased constructor.
        * Create a CGraphicsAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CGraphicsAppView.
        */
        static CGraphicsAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CGraphicsAppView object, which will draw itself to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CGraphicsAppView.
        */
        static CGraphicsAppView* NewLC( const TRect& aRect );

        /**
        * ~CGraphicsAppView
        * Virtual Destructor.
        */
        virtual ~CGraphicsAppView();

    public: // New functions

        /**
        * StartOffScreenDemo
        * Start demo using off-screen bitmap.
        */
        void StartOffScreenDemo();

        /**
        * StartNoOffScreenDemo
        * Start demo without using off-screen bitmap.
        */
        void StartNoOffScreenDemo();

        /**
        * StopDemo
        * Stop animation.
        */
        void StopDemo();

    private: // Functions from base classes

        /**
        * From CCoeControl, Draw
        * Draw this CGraphicsAppView to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;

		void HandleResourceChange(TInt aType, const TRect& aRect);

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a CGraphicsAppView object
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL( const TRect& aRect );

        /**
        * CGraphicsAppView.
        * C++ default constructor.
        */
        CGraphicsAppView();

    private: // New functions

        /**
        * DoPeriodTask.
        * Called by period task static function.
        */
        void DoPeriodTask();

        /**
        * Period.
        * Call back function for a periodic timer
        * @param aPtr a parameter passed to the timer when the timer is started.
        * @return Value TRUE indicates the callback should be done again.
        */
        static TInt Period( TAny* aPtr );

        /**
        * UpdateDisplay.
        * Perfom the drawing, does not activate the GC.
        */
        void UpdateDisplay() const;

        /**
        * SetUpSpritesL.
        * Initialise the objects being drawn.
        */
        void SetUpSpritesL();

        /**
        * StartTimer.
        * Start the timer.
        */
        void StartTimer();

        /**
        * ScaleImages
        * Scales background and sprite according to current layout.
        */
        void ScaleImages();

    private: // Data

        /**
        * iBackgroundImage
        * The background bitmap
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmap* iBackgroundImage;

        /**
        * iSpriteImage
        * The "sprite" bitmap image
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmap* iSpriteImage;

        /**
        * iSpriteMask
        * The "sprite" bitblit mask
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmap* iSpriteMask;

        /**
        * iSpriteImage
        * The "sprite" bitmap image
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmap* iSpriteImage2;

        /**
        * iSpriteMask
        * The "sprite" bitblit mask
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmap* iSpriteMask2;

        /**
        * iOffScreenBitmap
        * The off screen bitmap
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmap* iOffScreenBitmap;

        /**
        * iOffScreenBitmapDevice
        * The off screen bitmap device
        * Owned by CGraphicsAppView object.
        */
        CFbsBitmapDevice* iOffScreenBitmapDevice;

        /**
        * iOffScreenBitmapGc
        * The off screen bitmap gc
        * Owned by CGraphicsAppView object.
        */
        CFbsBitGc* iOffScreenBitmapGc;

        /**
        * iPeriodicTimer
        * The periodic timer used to control the animation
        * Owned by CGraphicsAppView object.
        */
        CPeriodic* iPeriodicTimer;

        /**
        * iUsingOffScreenBitmap
        * The flag to keep track of whether or not the demo is
        * using an off screen bitmap
        */
        TBool iUsingOffScreenBitmap;

        /**
        * iSprites
        * The array of sprites to be drawn
        * Owned by CGraphicsAppView object.
        */
        CArrayPtrFlat<CSprite>* iSprites;

    	RFs iFsSession;
	    CIconFileProvider* iIconProvider;
    };

#endif // __GRAPHICSAPPVIEW_H__

// End of File