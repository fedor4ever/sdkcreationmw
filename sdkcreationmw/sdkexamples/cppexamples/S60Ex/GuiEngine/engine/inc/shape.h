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



#ifndef __SHAPE_H__
#define __SHAPE_H__


// INCLUDES
#include <e32std.h>
#include <w32std.h>


// CLASS DECLARATION

// Namespace containing shape definition
namespace NShapes
    {

    /**
    * TShapeType
    * Shape constants.
    * KRectangle represents a rectangle.
    * value KCircle represents a circle.
    */
    enum TShapeType
        {
        KRectangle = 1,
        KCircle
        };

    /**
    * TShape
    * Abstract class for shapes.
    */
    class TShape
        {
        public: // Constructor

            /**
            * TShape
            * Constructor that creates the shape at a specific point.
            * @param aShapeType the type of the actual shape.
            * @param aOrigin where the origin of the shape is located.
            */
            IMPORT_C TShape ( TShapeType aShapeType,
                              const TPoint& aOrigin );


        public: // New functions

            /**
            * Draw
            * Pure virtual method that draws the shape to the specified
            * graphics context.
            * @param aActiveGraphicsContext the active context to
            * draw the shape into.
            */
            virtual void Draw( CWindowGc& aActiveGraphicsContext ) const = 0;


            /**
            * GetRect
            * Pure virtual method that returns the smallest bounding
            * rectangle that entirely contains the shape.
            * @return The rectangle
            */
            virtual TRect GetRect() const = 0;

            /**
            * ReadShapeTypeL
            * Static method that reads the type of the next shape from
            * a stream.
            * @param aStream The stream to read from
            * @return Returns the type of the next shape
            */
            IMPORT_C static TShapeType ReadShapeTypeL( RReadStream& aStream );

            /**
            * ExternalizeL
            * Virtual method that writes the shape to the specified
            * write stream.
            * @param aStream the stream to write the shape to.
            */
            IMPORT_C virtual void ExternalizeL( RWriteStream& aStream ) const;

            /**
            * InternalizeL
            * Virtual method that reads the shape from the specified
            * read stream.
            * @param aStream the stream to read the shape from.
            */
            IMPORT_C virtual void InternalizeL( RReadStream& aStream );

        protected:  // Data

            /** iShapeType The type of the actual shape **/
            const TShapeType iShapeType;

            /** iOrigin The origin of the shape **/
            TPoint iOrigin;
        };

    }

#endif  //  __SHAPE_H__


// End of File
