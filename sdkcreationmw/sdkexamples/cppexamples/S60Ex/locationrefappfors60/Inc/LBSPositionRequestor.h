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
* Description:  Position requestor handles the position requesting process
*
*/


#ifndef LBSPOSITIONREQUESTOR_H
#define LBSPOSITIONREQUESTOR_H

//  INCLUDES
#include <lbs.h>
#include <LbsSatellite.h>

// FORWARD DECLARATIONS
class MLbsPositionListener;

// CLASS DECLARATION

/**
*  Position requestor.
*  Request the position from the Location FW and
*  handles the position requesting process
*/
class CLbsPositionRequestor : public CActive
    {
    public: //Construction and destruction

        /**
        * Creates a new instance of a @ref CLbsPositionRequestor
        * @param aPositionListener Position listener to be registered
        * @return The newly created CLbsPositionRequestor object
        */
        static CLbsPositionRequestor* NewL( 
            MLbsPositionListener& aPositionListener ) ;

        /**
        * Destructor.
        */
        virtual ~CLbsPositionRequestor();

    protected:  // Functions from base classes

        // From CActive
        void DoCancel();
        void RunL();


    private:  // New Functions

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

        /**
        * C++ default constructor.
        * @param aPositionListener Position listener to be registered
        */
        CLbsPositionRequestor( MLbsPositionListener& aPositionListener );

        /**
        * Starts a position requesting sequence
        */
        void DoInitialiseL();
        
        /**
        * Gets the name of the positioning module. If the name is not
        * available the name will be an empty string
        * @param   aModuleId The id of the module
        */
        void GetModuleName(const TPositionModuleId& aModuleId);  
        
        /**
        * Preprocess the position information
        */
        void PositionUpdatedL();

    private:    // Data

        //The id of the currently used PSY
        TPositionModuleId iUsedPsy; 

        // Position server
        RPositionServer iPosServer;

        // Positioner
        RPositioner iPositioner;

        // Basic location info
        TPositionInfo iPositionInfo;  
        
        // Satellite info
        TPositionSatelliteInfo iSatelliteInfo; 

        // Position listener
        MLbsPositionListener& iPositionListener;
    
        // Module name
        TBuf<KPositionMaxModuleName> iModuleName;
        
        // The id of the used psy
        TPositionUpdateOptions iUpdateops;
        
        // Position info base
        TPositionInfoBase* iPosInfoBase;

        // State variable used to mark if we are 
        // getting last known position
        TBool iGettingLastknownPosition;
    };
 #endif //LBSPOSITIONREQUESTOR_H
// End of File
