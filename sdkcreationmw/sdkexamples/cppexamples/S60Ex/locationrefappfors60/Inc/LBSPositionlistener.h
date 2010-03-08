/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Position listener interface
*
*/


#ifndef LBSPOSITIONLISTENER_H
#define LBSPOSITIONLISTENER_H

#include <lbsSatellite.h>

class TPositionInfo;
class TPositionSatelliteInfo;

/**
*   Position data listener interface listens position information.
*   It can also be used to transfer error messages and codes.
*/
class MLbsPositionListener
    {
    public:

        /**
        * PositionInfoUpdatedL informs the listener about new location information
        * @param aModulename The name of the module which provided this information
        * @param aPosInfo Position information class
        */
        virtual void PositionInfoUpdatedL(TPositionInfoBase& aPosInfo,
            const TDesC& aModulename) = 0;

        /**
        * ShowErrorL informs the listener about occurred error during position 
        * request process
        * @param aErrorString The error in string format
        */
        virtual void ShowErrorL(const TDesC& aErrorString) = 0;
        
        /**
        * ShowMessageL informs the listener about occurred event during position 
        * request process
        * @param aMessage The message in string format
        */
        virtual void ShowMessageL(const TDesC& aMessage) = 0;
        
    };

#endif //LBSPOSITIONLISTENER_H


// End of File
