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
* Description:  Engine for Geoprofiles application
*
*/


#ifndef C_GEOPROFILESENGINE_H
#define C_GEOPROFILESENGINE_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <lbtcommon.h>
#include <lbtserver.h> 
#include <lbt.h>
#include <lbttriggerchangeeventobserver.h>
#include <lbttriggerchangeeventnotifier.h>

// FORWARD DECLARATIONS
class CGeoProfile;
class CLbtStartupTrigger;
class CAknWaitDialog;

/**
 * Observer class for CGeoProilesEngine
 */
class MGeoProfilesEngineObserver
    {
public:    
    virtual void ProfileStatusChangedL() = 0;    
    };
 
/**
 * Geo Profile Engine
 */
class CGeoProfilesEngine : public CActive,
                           public MLbtTriggerChangeEventObserver
    {
public:
    enum TGeoProfileEngState
        {
        ENoAction,
        ECreateProfile,
        EUpdateProfile,
        EDeleteProfile    
        };       
public:
    /**
     * Constructs a new instance of CGeoProfilesEngine
     */
    static CGeoProfilesEngine* NewL( MGeoProfilesEngineObserver&        aObserver );
    
    /**
     * C++ Destructor.
     * Frees all the resources
     */
    virtual ~CGeoProfilesEngine();
    
    /**
     * Add a new profile
     */
    void AddProfileL( CGeoProfile&      aProfile );

    /**
     * Updates a profile
     */
    void UpdateProfileL( CGeoProfile&      aProfile );
        
    /**
     * Delete an existing profile
     */
    void RemoveProfileL( CGeoProfile&      aProfile );
    
    /**
     * List profiles
     */
    void ListProfilesL( RPointerArray< CGeoProfile>&   aGeoProfiles );
    
    /**
     * Inherited from MLbtTriggerChangeEventObserver
     */
    void TriggerChangedL( const TLbtTriggerChangeEvent& aEvent );
    
private:
    /**
     * C++ Constructor
     */
    CGeoProfilesEngine( MGeoProfilesEngineObserver&        aObserver );
                      
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
    
    void LaunchWaitDialogL( TInt aResource );
    
    void DismissWaitDialogL();
    
    void DoUpdateProfileL( CGeoProfile&      aProfile );
                   
private: // Member Variables
    MGeoProfilesEngineObserver&         iObserver;
    
    RLbt                                iLbt;
    
    RLbt                                iLbtNotifier;
    
    RLbtServer                          iLbtServer;
    
    CLbtTriggerChangeEventNotifier*     iNotifier;
    
    CLbtStartupTrigger*                 iStartupTrigger;
    
    TUint32                             iTriggerId;
    
    CGeoProfile*                        iProfile;
    
    TGeoProfileEngState                 iEngineState;                        
    
    CAknWaitDialog*                     iWaitDialog;
    }; 

#endif // C_GEOPROFILESENGINE_H
