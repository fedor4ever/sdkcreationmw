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
#include    "rectangle.h"

using namespace NShapes;

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// TRectangle::TRectangle()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TRectangle::TRectangle( const TPoint& aOrigin, TInt aWidth, TInt aHeight ):
                                 TShape( KRectangle, aOrigin ),
                                 iWidth( aWidth ),
                                 iHeight( aHeight )
    {
    // Create an instance of TRectangle at a specified position,
    // and of a specified size.
    // No implementation required
    }


// -----------------------------------------------------------------------------
// TRectangle::TRectangle()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TRectangle::TRectangle() : TShape( KRectangle, TPoint( 0,0 ) ),
                                            iWidth( 0 ),
                                            iHeight( 0 )
    {
    // Create an instance of TRectangle
    //  No implementation required
    }

// -----------------------------------------------------------------------------
// TRectangle::Draw() const
// Virtual method that draws the shape to the specified graphics context.
// -----------------------------------------------------------------------------
//
EXPORT_C void TRectangle::Draw( CWindowGc& aActiveGraphicsContext ) const
    {
    aActiveGraphicsContext.DrawRect( GetRect() );
    }

// -----------------------------------------------------------------------------
// TRectangle::GetRect() const
// Return the smallest bounding rectangle that entirely contains the shape.
// -----------------------------------------------------------------------------
//
EXPORT_C TRect TRectangle::GetRect() const
    {
    return TRect( iOrigin, TSize( iWidth, iHeight ) );
    }

// -----------------------------------------------------------------------------
// TRectangle::ExternalizeL() const
// Virtual method that writes the shape to the specified write stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TRectangle::ExternalizeL( RWriteStream& aStream ) const
    {
    TShape::ExternalizeL( aStream );
    aStream.WriteInt32L( iWidth );
    aStream.WriteInt32L( iHeight );
    }

// -----------------------------------------------------------------------------
// TRectangle::InternalizeL() const
// Virtual method that reads the shape from the specified read stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TRectangle::InternalizeL( RReadStream& aStream )
    {
    TShape::InternalizeL( aStream );
    iWidth = aStream.ReadInt32L();
    iHeight = aStream.ReadInt32L();
    }


// End of File
