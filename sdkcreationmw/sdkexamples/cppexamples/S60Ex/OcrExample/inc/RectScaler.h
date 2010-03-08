/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef RECT_SCALER_H
#define RECT_SCALER_H

#include <e32cmn.h>
#include <e32std.h>

/**
 * namespace contains various utility classes
 */
namespace NOCRUtils
{
   /**
    * TRectScaler is a simple utility class to calculate and adjust rectangles
    * based on the original and scaled rectangle size.
    *
    * @note TRectScaler assumes that the scaled rectangle has the same aspect
    *   ratio as the original image
    */
    class TRectScaler
        {
        public:
            /** constructor */
            TRectScaler();

           /**
            * Calculates the factors for height and width from the
            * original and scaled rectangle size
            * @param aOriginal size of the original rectangle
            * @param aScaled size of the resized rectangle
            */
            void CalculateFactor( TSize aOriginal, TSize aScaled );

           /**
            * calculates a scaled up rectangle based on the current
            * relational factors. Both the size and position of the returned
            * rectangle are scaled.
            * @param aScaledRect a rectangle, which is used as a basis for
            *   the scaled up rectangle
            * @return a scaled up rectangle
            */
            TRect ScaleUp( const TRect& aScaledRect ) const;

           /**
            * calculates a scaled down rectangle based on the current
            * relational factors. Both the size and position of the returned
            * rectangle are scaled.
            * @param aOriginalRect a rectangle, which is used as a basis for
            *   the scaled down rectangle
            * @return a scaled down rectangle
            */
            TRect ScaleDown( const TRect& aOriginalRect ) const;

        private:
            /** scale factor */
            TReal   iFactor;
        };

} // namespace NOCRUtils

#endif
