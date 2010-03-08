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
#include "shape.h"


using namespace NShapes;


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// TShape::TShape()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TShape::TShape( TShapeType aShapeType, const TPoint& aOrigin ):
                                                iShapeType( aShapeType ),
                                                iOrigin( aOrigin )
    {
    // Constructor that creates the shape at a specific point
    // No implementation required
    }

// -----------------------------------------------------------------------------
// TShape::ReadShapeTypeL()
// Static method that reads the type of the next shape from a stream
// -----------------------------------------------------------------------------
//
EXPORT_C TShapeType TShape::ReadShapeTypeL( RReadStream& aStream )
    {
    return static_cast<TShapeType>( aStream.ReadInt32L() );
    }
// -----------------------------------------------------------------------------
// TShape::ExternalizeL()
// Virtual method that writes the shape to the specified write stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TShape::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iShapeType );
    aStream.WriteInt32L( iOrigin.iX );
    aStream.WriteInt32L( iOrigin.iY );
    }

// -----------------------------------------------------------------------------
// TShape::InternalizeL()
// Virtual method that reads the shape from the specified  read stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void TShape::InternalizeL( RReadStream& aStream )
    {
    //  iShapeType has already been read
    iOrigin.iX = aStream.ReadInt32L();
    iOrigin.iY = aStream.ReadInt32L();
    }


// End of File
