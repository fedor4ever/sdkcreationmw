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
#include    "circle.h"


using namespace NShapes;


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// TCircle::TCircle()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TCircle::TCircle( const TPoint& aCenter, TInt aRadius )
: TShape( KCircle, aCenter - TSize( aRadius, aRadius ) ),
  iRadius( aRadius )
    {
    // Create an instance of TCircle at a specified position,
    // and of a specified radius.
    // No implementation required
    }

// -----------------------------------------------------------------------------
// TCircle::TCircle()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TCircle::TCircle() : TShape( KCircle, TPoint( 0,0 ) ), iRadius( 0 )
    {
    // Create an instance of TCircle.
    // No implementation required
    }


// -----------------------------------------------------------------------------
// TCircle::Draw()
// Virtual method that draws the shape to the specified graphics context.
// -----------------------------------------------------------------------------
//
EXPORT_C void TCircle::Draw ( CWindowGc& aActiveGraphicsContext ) const
    {
    aActiveGraphicsContext.DrawEllipse( GetRect() );
    }


// -----------------------------------------------------------------------------
// TCircle::GetRect()
// Return the smallest bounding rectangle that entirely contains the shape.
// -----------------------------------------------------------------------------
//
EXPORT_C TRect TCircle::GetRect() const
    {
    return TRect( iOrigin, TSize( iRadius * 2, iRadius * 2 ) );
    }

// -----------------------------------------------------------------------------
// TCircle::ExternalizeL()
// Virtual method that writes the shape to the specified write stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TCircle::ExternalizeL( RWriteStream& aStream ) const
    {
    TShape::ExternalizeL( aStream );
    aStream.WriteInt32L( iRadius );
    }

// -----------------------------------------------------------------------------
// TCircle::InternalizeL()
// Virtual method that reads the shape from the specified read stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TCircle::InternalizeL( RReadStream& aStream )
    {
    TShape::InternalizeL( aStream );
    iRadius = aStream.ReadInt32L();
    }

// End of File
