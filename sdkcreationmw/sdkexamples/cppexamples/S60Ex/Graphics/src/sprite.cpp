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
#include "Sprite.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSprite::CSprite()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSprite::CSprite( TInt aXVelocity,
                  TInt aYVelocity,
                  const TPoint& aInitialPosition,
                  CFbsBitmap* aImage,
                  CFbsBitmap* aMask )
:   iPosition( aInitialPosition ),
    iXVelocity( aXVelocity ),
    iYVelocity( aYVelocity ),
    iImage(aImage),
    iMask(aMask)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSprite::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSprite* CSprite::NewL( TInt aXVelocity,
                        TInt aYVelocity,
                        const TPoint& aInitialPosition,
                        CFbsBitmap* aImage,
                        CFbsBitmap* aMask )
    {
    CSprite* self = NewLC( aXVelocity,aYVelocity,aInitialPosition,aImage,aMask );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSprite::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSprite* CSprite::NewLC( TInt aXVelocity,
                         TInt aYVelocity,
                         const TPoint& aInitialPosition,
                         CFbsBitmap* aImage,
                         CFbsBitmap* aMask )
    {
    CSprite* self = new ( ELeave ) CSprite( aXVelocity,
                                            aYVelocity,
                                            aInitialPosition,
                                            aImage,
                                            aMask );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSprite::~CFocusEventDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CSprite::~CSprite()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSprite::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSprite::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSprite::Move()
// Move Sprite.
// -----------------------------------------------------------------------------
//
void CSprite::Move( const TSize& aSize, const TRect& aBounds )
    {
    TBool okToMove = ETrue;

    // Calculate the new sprite position
    TPoint newPostion = iPosition + TPoint( iXVelocity,iYVelocity );

    // Check that the new sprite X position is not out of bounds
    TInt width = aSize.iWidth;
    TInt leftX = newPostion.iX;
    TInt rightX = newPostion.iX + width;
    if ( ( rightX > aBounds.iBr.iX ) || ( leftX < aBounds.iTl.iX ) )
        {
        // If it is, "bounce" the sprite off the edge of the boundary
        iXVelocity = -iXVelocity;
        // and don't let it move this tick
        okToMove = EFalse;
        }

    // Check that the new sprite Y position is not out of bounds
    TInt height = aSize.iHeight;
    TInt topY = newPostion.iY;
    TInt bottomY = newPostion.iY + height;
    if ( ( bottomY > aBounds.iBr.iY ) || ( topY < aBounds.iTl.iY ) )
        {
        // If it is, "bounce" the sprite off the edge of the boundary
        iYVelocity = -iYVelocity;
        // and don't let it move this tick
        okToMove = EFalse;
        }

    // If the sprite can move this tick
    if ( okToMove )
        {
        // Move it
        iPosition = newPostion;
        }
    }

// -----------------------------------------------------------------------------
// CSprite::Move()
// Move Sprite.
// -----------------------------------------------------------------------------
//
void CSprite::Adjust( const TSize& aSize, const TRect& aBounds )
    {
    // check if right and bottom extremities are within the rect
    TPoint pos = iPosition + TPoint( aSize.iWidth, aSize.iHeight );
    if (!aBounds.Contains(pos))
        {
        // we have to adjust the position
        if (pos.iX > aBounds.iBr.iX)
            iPosition.iX = aBounds.iBr.iX - aSize.iWidth;

        if (pos.iY > aBounds.iBr.iY)
            iPosition.iY = aBounds.iBr.iY - aSize.iHeight;
        }
    }

// End of File
