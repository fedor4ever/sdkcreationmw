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


#ifndef __SPRITE_H__
#define __SPRITE_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
* CSprite.
* An instance of class CSprite represents an object
* being displayed on the screen
*/
class CSprite : public CBase
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CSprite object.
        * @param aXVelocity The velocity of the sprite on the x axis.
        * @param aYVelocity The velocity of the sprite on the y axis.
        * @param aInitialPosition The starting position of the sprite
        * on the screen.
        * @param aImage pointer to the image represented by this sprite.
        *   Note that ownershiup of the image is not taken.
        * @param aMask pointer to the mask of the image represented by this
        *   sprite. Note that ownershiup of the mask is not taken.
        * @return A pointer to the created instance of CSprite.
        */
        static CSprite* NewL( TInt aXVelocity,
                              TInt aYVelocity,
                              const TPoint& aInitialPosition,
                              CFbsBitmap* aImage,
                              CFbsBitmap* aMask );
        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CSprite object.
        * @param aXVelocity The velocity of the sprite on the x axis.
        * @param aYVelocity The velocity of the sprite on the y axis.
        * @param aInitialPosition The starting position of the sprite
        * on the screen.
        * @param aImage pointer to the image represented by this sprite.
        *   Note that ownershiup of the image is not taken.
        * @param aMask pointer to the mask of the image represented by this
        *   sprite. Note that ownershiup of the mask is not taken.
        * @return A pointer to the created instance of CSprite.
        */
        static CSprite* NewLC( TInt aXVelocity,
                               TInt aYVelocity,
                               const TPoint& aInitialPosition,
                               CFbsBitmap* aImage,
                               CFbsBitmap* aMask );
        /**
        * ~CSprite
        * Virtual Destructor.
        */
        virtual ~CSprite();

    public: // New functions

        /**
        * Position
        * Return the position of the sprite on the screen.
        * @return The position
        */
        TPoint Position() const { return iPosition; }

        /**
        * Image
        * @return pointer to the image represented by this sprite.
        */
        CFbsBitmap* Image() const { return const_cast<CFbsBitmap*>(iImage); }

        /**
        * Mask
        * @return pointer to the image mask represented by this sprite.
        */
        CFbsBitmap* Mask() const { return const_cast<CFbsBitmap*>(iMask); }

        /**
        * Move
        * Move the sprite.
        * @param aSize The size of the sprite
        * @param aBounds The rectangle the sprite is allowed to move in
        */
        void Move( const TSize& aSize, const TRect& aBounds );

        /**
        * Adjust
        * If resolution or orientation of the screen has changed, the current
        * position may be out of bounds. In suc case a new position within the
        * rectangle is set.
        * @param aSize The size of the sprite
        * @param aBounds The rectangle the sprite is allowed to move in
        */
        void Adjust( const TSize& aSize, const TRect& aBounds );

    private: // Constructors

        /**
        * CSprite
        * Perform the first phase of two phase construction.
        * @param aXVelocity The velocity of the sprite on the x axis.
        * @param aYVelocity The velocity of the sprite on the y axis.
        * @param aInitialPosition The starting position of the sprite
        * on the screen.
        * @param aImage pointer to the image represented by this sprite.
        *   Note that ownershiup of the image is not taken.
        * @param aMask pointer to the mask of the image represented by this
        *   sprite. Note that ownershiup of the mask is not taken.
        */
        CSprite( TInt aXVelocity,
                 TInt aYVelocity,
                 const TPoint& aInitialPosition,
                 CFbsBitmap* aImage,
                 CFbsBitmap* aMask );

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a CSprite object.
        */
        void ConstructL();

    private: // Data

        /**
        * iPosition
        */
        TPoint iPosition;

        /**
        * iXVelocity
        */
        TInt iXVelocity;

        /**
        * iYVelocity
        */
        TInt iYVelocity;

        /**
        * iImage
        * The "sprite" bitmap image.
        * Not owned. This is just a reference so that the user of the sprite
        * can get the pointer to the image represented by this sprite.
        */
        const CFbsBitmap * const iImage;

        /**
        * iMask
        * The "sprite" bitblit mask
        * Not owned. This is just a reference so that the user of the sprite
        * can get the pointer to the image represented by this sprite.
        */
        const CFbsBitmap * const iMask;
    };

#endif // __SPRITE_H__

// End of File
