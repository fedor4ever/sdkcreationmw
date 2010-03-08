/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Geo Profile Place selector
*
*/


#ifndef C_GEOPLACESELECTOR_H
#define C_GEOPLACESELECTOR_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <Lbs.h>
#include <LbsPosition.h>

// FORWARD DECLARATIONS
class CAknWaitDialog;

/**
 * Geo Profile Place selector
 */
class CGeoPlaceSelector : public CActive
    {
public:
    /**
     * Enumeration Defintions
     */
    enum EGeoPlaceSelection
        {
        EPlaceCurrentLocation,
        EPlaceLandmark
        };
                
public:
    /**
     * Constructs a new instance of CGeoPlaceSelector
     */
    static CGeoPlaceSelector* NewLC();
    
    /**
     * C++ Destructor.
     * Frees all the resources
     */
    virtual ~CGeoPlaceSelector();
    
    /**
     * Select Place
     */    
    TInt SelectGeoPlaceL( TInt&            aPlaceType,
                          TLocality&       aLocality,
                          TPtr&            aPlaceName );
        
private:
    /**
     * C++ Constructor
     */
    CGeoPlaceSelector();
                      
    /**
     * Second phase of two phase construction
     */                      
    void ConstructL();
    
    /**
     * Inherited from CActive
     */
    void RunL();
    
    /**
     * Inherited from CActive
     */
    void DoCancel();
    
    /**
     * Display Place Dialog
     */
    TInt DisplayPlacePopUpL();
    
    /**
     * Select place from Landmark
     */    
    TInt SelectPlaceFromLandmarkL( TLocality&       aLocality,
                                   TPtr&            aPlaceName );
    
    /**
     * Select place from current Location
     */
    TInt SelectPlaceFromCurrentLocationL( TLocality&    aLocality );
    
    /**
     * Launches the Wait dialog
     */
    void LaunchWaitDialogL();
    
    /**
     * Dismisses the Wait dialog
     */
    void DismissWaitDialog();
                   
private: // Member Variables     
    /**
     * Title text for pop-up dialog
     * Owns
     */                 
    HBufC*              iPlacePopupTitle;
    
    /**
     * Wait dialog for obtaining current position
     */ 
    CAknWaitDialog*     iWaitDialog;
    
    /**
     * Location Acquistion Request status
     */
    TInt                iLocAcqState;
    
    /**
     * Dialog Dismissal State
     */
    TInt                iDialogState;
    
    /**
     * Handle to Location Server
     */
    RPositionServer     iServer;
    
    /**
     * Handle to Server session
     */
    RPositioner         iPositioner;    
    }; 

#endif // C_GEOPLACESELECTOR_H
