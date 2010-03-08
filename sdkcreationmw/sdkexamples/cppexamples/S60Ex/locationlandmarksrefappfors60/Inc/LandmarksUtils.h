/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  See class description below
*
*/


#ifndef __LANDMARKS_UTILS_H__
#define __LANDMARKS_UTILS_H__

/** 
* Coordinate data conversion utility methods
*/
class LandmarksUtils
    {
      public:

        /** Converts a float to a descriptor.
        * Handles the case when the float is NaN
        * @param aFloat the source float
        * @param aDes the target descriptor
        * @param aRealFormat Format used for real value conversion to text
        */
        static void FloatToDesL(TReal aFloat, TDes& aDes, TRealFormat aRealFormat);

        /** Converts a float lat/long to a descriptor.
        * Handles the case when the float is NaN
        * @param aFloat the source float
        * @param aDes the target descriptor
        */
        static void LatLongToDes(TReal aFloat, TDes& aDes);

        /** Converts a latitude or longitude in float format 
        * to a descriptor containing degrees, minutes and seconds.
        *
        * @param aLatLong the source float
        * @param aDes the target descriptor (degrees, minutes and seconds)
        */
        static void LatLongFloatToDegMinSecDesL(TReal aLatLong, TDes& aDes);
        
        /** Panics the application due to some programming error
        * @param aReason The panic number or error code
        */
        static void Panic(TInt aReason);
      
      private:
      
        //! Internal struct for LatLongFloatToDegMinSecDesL
        struct SDegMinSec
            {
            //! degrees
            TInt32 iDegrees;
            //! minutes
            TInt32 iMinutes;
            //! seconds
            TReal iSeconds;
            };
    };

#endif // __LANDMARKS_UTILS_H__

// End of File

