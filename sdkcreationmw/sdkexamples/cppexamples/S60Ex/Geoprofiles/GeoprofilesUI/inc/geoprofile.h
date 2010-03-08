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
* Description:  Geo Profile Structure
*
*/


#ifndef C_GEOPROFILE_H
#define C_GEOPROFILE_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <LbsPosition.h>

// FORWARD DECLARATIONS

/**
 * Geo Profile Exchange Structure
 */
class CGeoProfile : public CBase
    {
public:
    enum TGeoPlaceType
        {
        EUndefined,
        ECoordinate,
        ENetworkCell    
        };
        
    enum TGeoTriggerType
        {
        EEntryTrigger,
        EExitTrigger    
        };    
public:
    /**
     * Constructs a new instance of CGeoProfile Entry
     */
    static CGeoProfile* NewL();
    
    /**
     * Constructs a new instance of CGeoProfile Entry.        
     */
    static CGeoProfile* NewL( const TDesC&       aGeoPlaceName,
                                    TLocality&   aPositionInfo,
                                    TInt         aGeoProfileId = 0,
                                    TInt         aProfileRadius = 100,
                                    TInt         aTriggerId = -1,
                                    TInt         aTriggerType = EEntryTrigger );
    
    /**
     * C++ Destructor.
     * Frees all the resources
     */
    virtual ~CGeoProfile();
    
    /**
     * Accessor method for obtaining the Geo-Profile Name
     */    
    TPtrC GeoPlaceName();

    /**
     * Accessor method for obtaining the Profile Name
     */    
    TPtrC ProfileName();
        
    /**
     * Accessor method for obtaining the Item Id
     */     
    TInt GeoProfileId();
    
    /**
     * Accessor method for setting the Profile Name
     */
    void SetGeoProfileIdL( TInt               aGeoProfileId );
    
    /**
     * Location Information Type
     */
    TInt GeoPositionType(); 
    
    /**
     * Location Information
     */
    TLocality& GeoPositionInfo();
    
    /**
     * Accessor method for setting Location Information
     */    
    void SetGeoPositionInfoL( const TDesC&       aGeoPlaceName,
                                    TLocality&   aPositionInfo );

    /**
     * Accessor method for obtaining the radius
     */
    TInt GeoProfileRadius();

    /**
     * Accessor method for setting the radius
     */    
    void SetGeoProfileRadius( TInt  aRadius );
    
    /**
     * Accessor method for obtaining the Trigger Id
     */
    TInt TriggerId();

    /**
     * Accessor method for setting the Trigger Id
     */    
    void SetTriggerId( TInt  aTriggerId );
    
    /**
     * Accessor method for obtaining the Trigger Type
     */
    TInt TriggerType();

    /**
     * Accessor method for setting the Trigger Type
     */    
    void SetTriggerType( TInt  aTriggerType );    
     
    /**
     * Internalized the information available from the buffer
     * The buffer must be in following format
     * 1. Profile ID ( Integer )
     * 2. Location Type ( Integer )
     * 3. Horizontal Accuracy ( Integer )
     * 4. Radius ( Integer )
     */ 
    void PartialInternalizeL( const TDesC&   aInputBuffer ); 
    
    /**
     * Partially externalizes the contents of the Geo Profile to
     * a buffer
     * The following fields are externalized and in the following
     * format
     * 1. Profile ID ( Integer )
     * 2. Location Type ( Integer )
     * 3. Horizontal Accuracy ( Integer )
     * 4. Radius ( Integer )     
     */    
    HBufC* PartialExternalizeLC();      
    
private:
    /**
     * C++ Constructor
     */
    CGeoProfile();
                      
    /**
     * Second phase of two phase construction
     */                      
    void ConstructL( const TDesC&       aGeoPlaceName,
                           TInt         aGeoProfileId,
                           TLocality&   aPositionInfo,
                           TInt         aProfileRadius,
                           TInt         aTriggerId,
                           TInt         aTriggerType );
                           
    /**
     * Accessor method for setting the Profile Name
     */
    void SetProfileNameL( const TDesC&       aProfileName );
    
    
    /**
     * Accessor method for setting Location Information
     */    
    void SetGeoPositionType( TInt         aPositionType );
    
    /**
     * Accessor method for setting Location name
     */
    void SetGeoPlaceNameL( const TDesC&   aGeoPlaceName );        
                                                           
                   
private: // Member Variables                      
      
    /**
     * Geo Profile Name
     * Owns
     */  
    HBufC*       iGeoPlaceName;
    
    /**
     * Profile Name
     * Owns
     */     
    HBufC*       iProfileName;
    
    /**
     * Geo Profile Identifier
     */
    TInt         iGeoProfileId;
    
    /**
     * Geo Profile Location
     */
    TLocality    iPositionInfo;
    
    /** 
     * Position Type
     */
    TInt        iPositionType;
    
    /**
     * Radius of the profile
     */
    TInt        iProfileRadius;
    
    /** 
     * Trigger Id
     */
    TInt        iTriggerId;
    
    /**
     * Trigger Type
     */
    TInt        iTriggerType;
    }; 

#endif // C_GEOPROFILE_H
