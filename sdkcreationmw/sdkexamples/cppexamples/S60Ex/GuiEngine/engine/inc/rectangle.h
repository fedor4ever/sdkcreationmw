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



#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__


// INCLUDES
#include <e32std.h>
#include <w32std.h>
#include "shape.h"


// CLASS DECLARATION

// Namespace containing shape definition
namespace NShapes
{

    /**
    * TRectangle
    * An instance of class TRectangle is a rectangular shape.
    */
    class TRectangle : public TShape
        {
        public: // Constructors

            /**
            * TRectangle
            * Create an instance of TRectangle at a specified position,
            * and of a specified size.
            * @param aOrigin where the origin of the shape is located.
            * @param aWidth The width of the rectangle
            * @param aHeight The height of the rectangle
            */
            IMPORT_C TRectangle( const TPoint& aOrigin,
                                 TInt aWidth,
                                 TInt aHeight );

            /**
            * TRectangle
            * Create an instance of TRectangle
            */
            IMPORT_C TRectangle();


        public: //  From TShape

            /**
            * Draw
            * Virtual method that draws the shape to the specified
            * graphics context.
            * @param aActiveGraphicsContext the active context to
            * draw the shape into.
            */
            IMPORT_C void Draw( CWindowGc& aActiveGraphicsContext ) const;

            /**
            * GetRect
            * Return the smallest bounding rectangle that entirely
            * contains the shape.
            * @return The rectangle
            */
            IMPORT_C TRect GetRect() const;

            /**
            * ExternalizeL
            * Virtual method that writes the shape to the specified
            * write stream.
            * @param aStream the stream to write the shape to.
            */
            IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

            /**
            * InternalizeL
            * Virtual method that reads the shape from the specified
            * read stream.
            * @param aStream the stream to read the shape from.
            */
            IMPORT_C void InternalizeL( RReadStream& aStream );

        private: // Data

            /** iWidth  The width of the rectangle  **/
            TInt iWidth;

            /** iHeight The height of the rectangle **/
            TInt iHeight;
        };


    }

#endif  //  __RECTANGLE_H__


// End of File
