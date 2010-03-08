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
#include "ShapeListManager.h"
#include "rectangle.h"
#include "circle.h"


using namespace NShapes;


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CShapeListManager::CShapeListManager()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CShapeListManager::CShapeListManager()
    {
    //  No implementation required
    }

// -----------------------------------------------------------------------------
// CShapeListManager::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CShapeListManager* CShapeListManager::NewL()
    {
    CShapeListManager* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CShapeListManager::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CShapeListManager* CShapeListManager::NewLC()
    {
    CShapeListManager* self = new ( ELeave ) CShapeListManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CShapeListManager::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CShapeListManager::ConstructL()
    {
    //  No implementation required
    }

// -----------------------------------------------------------------------------
// CShapeListManager::~CShapeListManager()
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CShapeListManager::~CShapeListManager()
    {
    iShapeList.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CShapeListManager::Clear()
// Clear the list of shapes.
// -----------------------------------------------------------------------------
//
EXPORT_C void CShapeListManager::Clear()
    {
    iNextShapeIndex = 0;    //  The first shape in the list
    iShapeList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CShapeListManager::AddShapeL()
// Add a shape to the list
// -----------------------------------------------------------------------------
//
EXPORT_C void CShapeListManager::AddShapeL( TShape* aShape )
    {
    User::LeaveIfError( iShapeList.Append( aShape ) );
    }

// -----------------------------------------------------------------------------
// CShapeListManager::GetNextShape()
// Returns the next shape in the list and increments the next shape index.
// -----------------------------------------------------------------------------
//
EXPORT_C TShape* CShapeListManager::GetNextShape()
    {
    if ( iShapeList.Count() > 0)
        {
        if ( iNextShapeIndex < iShapeList.Count())
            {
            TShape* shape = iShapeList[ iNextShapeIndex ];
            ++iNextShapeIndex;
            return shape;
            }
        //  Gone past the end, so reset the index
        iNextShapeIndex = 0;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CShapeListManager::StoreL()
// Store the instance to the specified store
// -----------------------------------------------------------------------------
//
EXPORT_C TStreamId CShapeListManager::StoreL( CStreamStore& aStore ) const
    {

    RStoreWriteStream stream;

    // Create a stream within the store
    TStreamId id = stream.CreateLC( aStore );

    // Write the data to the stream
    ExternalizeL( stream );
    stream.CommitL();

    CleanupStack::PopAndDestroy(); // the stream

    return id; // return the stream ID
    }

// -----------------------------------------------------------------------------
// CShapeListManager::ExternalizeL()
// Virtual method that writes the shape to the specified write stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CShapeListManager::ExternalizeL( RWriteStream& aStream ) const
    {

    TInt count = iShapeList.Count();
    aStream.WriteInt16L( static_cast<TInt16>( count ) );

    for ( TInt index = 0; index < count; ++index )
        {
        const TShape* shape = static_cast<TShape*>( iShapeList[ index ] );
        shape->ExternalizeL( aStream );
        }
    }

// -----------------------------------------------------------------------------
// CShapeListManager::RestoreL()
// Restore the instance from the specified stream within the specified store
// -----------------------------------------------------------------------------
//
EXPORT_C void CShapeListManager::RestoreL( const CStreamStore& aStore,
                                           const TStreamId& aStreamId )
    {

    Clear();
    RStoreReadStream stream;

    // Open the stream
    stream.OpenLC( aStore, aStreamId );

    // Read the data from the stream
    InternalizeL( stream );

    CleanupStack::PopAndDestroy(); // the stream
    }

// -----------------------------------------------------------------------------
// CShapeListManager::InternalizeL()
// Restore the instance from the specified stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CShapeListManager::InternalizeL( RReadStream& aStream )
    {

    TInt count = aStream.ReadInt16L();
    for ( TInt index = 0; index < count; ++index )
        {

        TShape* shape = NULL;
        switch ( TShape::ReadShapeTypeL( aStream ) )
            {
            case KRectangle:
                shape = new ( ELeave ) TRectangle();
                break;

            case KCircle:
                shape = new ( ELeave ) TCircle();
                break;

            default:
                User::Leave( KErrCorrupt );
            }


        CleanupStack::PushL( shape );
        shape->InternalizeL( aStream );
        User::LeaveIfError( iShapeList.Append( shape ) );

        CleanupStack::Pop( shape ); //  Now owned by iShapeList
        }
    }


// End of File
