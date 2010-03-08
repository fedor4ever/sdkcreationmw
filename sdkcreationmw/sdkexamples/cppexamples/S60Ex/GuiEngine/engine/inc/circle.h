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



#ifndef __CIRCLE_H__
#define __CIRCLE_H__


// INCLUDES
#include <e32std.h>
#include <w32std.h>
#include "shape.h"


// CLASS DECLARATION

// Namespace containing shape definition
namespace NShapes
{

    /**
    * TCircle
    * An instance of class TCircle is a circular shape.
    */
    class TCircle : public TShape
        {

        public: // Constructors and destructor

            /**
            * TCircle.
            * Create an instance of TCircle at a specified position,
            * and of a specified radius.
            * @param aCenter where the center of the circle is located.
            * @param aRadius The radius of the circle
            */
            IMPORT_C TCircle( const TPoint& aCenter, TInt aRadius );

            /**
            * TCircle.
            * Create an instance of TCircle
            */
            IMPORT_C TCircle();


        public: //  From TShape

            /**
            * Draw
            * Virtual method that draws the shape to the specified
            * graphics context.
            * @param aActiveGraphicsContext the active context
            * to draw the shape into.
            */
            IMPORT_C void Draw( CWindowGc& aActiveGraphicsContext ) const ;


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


        private:    // Data

            /** iRadius, The Radius of the circle   **/
            TInt iRadius;
        };

    }

#endif  //  __CIRCLE_H__

// End of File
