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



#ifndef __SHAPELISTMANAGER_H
#define __SHAPELISTMANAGER_H


// INCLUDES
#include <e32std.h>
#include <w32std.h>
#include "shape.h"


// CLASS DECLARATION

/**
* CShapeListManager
* An instance of this class is an engine that manages a list of shapes.
*/
class CShapeListManager : public CBase
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Creates a new instance of the engine.
        * @return Returns the engine
        */
        IMPORT_C static CShapeListManager* NewL();

        /**
        * NewLC
        * Creates a new instance of the engine.
        * @return Returns the engine
        */
        IMPORT_C static CShapeListManager* NewLC();


        /**
        * ~CShapeListManager.
        * Virtual destructor.
        */
        IMPORT_C virtual ~CShapeListManager();

    public: // New functions

        /**
        * Clear
        * Clear the list of shapes.
        */
        IMPORT_C void Clear();


         /**
        * AddShapeL
        * Add a shape to the list
        * @param aShape the object to add to the list.
        */
        IMPORT_C void AddShapeL( NShapes::TShape* aShape );


        /**
        * GetNextShape
        * Returns the next shape in the list and increments
        * the next shape index.
        * @return If there is no such shape, then it returns NULL,
        * and resets the next shape index.
        */
        IMPORT_C NShapes::TShape* GetNextShape();


        /**
        * StoreL
        * Store the instance to the specified store
        * @param aStore the store to stream the object to
        * @return Returns the id of the store within the stream.
        */
        IMPORT_C TStreamId StoreL( CStreamStore& aStore ) const;

        /**
        * ExternalizeL
        * Virtual method that writes the shape to the specified
        * write stream.
        * @param aStream the stream to write the shape to.
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

         /**
        * RestoreL
        * Restore the instance from the specified stream within
        * the specified store
        * @param aStore the store containing the data
        * @param aStreamId the id of the stream within the store
        */
        IMPORT_C void RestoreL( const CStreamStore& aStore,
                                const TStreamId& aStreamId );

        /**
        * InternalizeL
        * Restore the instance from the specified stream
        * @param aStream the stream to stream the object from
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );


    protected: // Constructors and destructor

        /**
        * CShapeListManager
        * C++ default constructor.
        */
        IMPORT_C CShapeListManager();

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        IMPORT_C void ConstructL();


    private:    // Data

        /** iNextShapeIndex The index of the next shape **/
        TInt iNextShapeIndex;

        /** iShapeList The list of shapes   **/
        RPointerArray<NShapes::TShape> iShapeList;
    };

#endif  //  __SHAPELISTMANAGER_H


// End of File
