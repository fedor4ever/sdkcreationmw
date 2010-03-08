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
* Description:  List box model for Geo Profiles Application UI.
*
*/


#ifndef C_GEOPROFILESLBMODEL_H
#define C_GEOPROFILESLBMODEL_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <bamdesca.h>  // MDesCArray

// FORWARD DECLARATIONS
class CGeoProfile;
 
/**
 * List box model for Geo Profiles Application
 *  
 * This class provides the implementation of the list-box model for 
 * Geo Profiles lists.
 */
class CGeoProfilesLBModel : public CBase,
						    public MDesCArray
    {
public:    
    /**
     * Constructs a new instance of Geo Profiles list box model.        
     */
    static CGeoProfilesLBModel* NewL( RPointerArray<CGeoProfile>*	 aGeoProfiles );        

    /**
     * C++ Destructor.
     * Frees all the resources
     */
    virtual ~CGeoProfilesLBModel();
    
    /**
     * Updates the Geo Profiles model
     */
    void UpdateModel( RPointerArray<CGeoProfile>*	            aGeoProfiles );    
        
public: // Inherited from Base classes
    /**
     * Inherited from MDesCArray
     */
    virtual TInt MdcaCount() const;

    /**
     * Inherited from MDesCArray
     */
    virtual TPtrC16 MdcaPoint( TInt aIndex ) const;

private:
    /**
     * Overloaded constructor
     */
    CGeoProfilesLBModel( RPointerArray<CGeoProfile>*	        aGeoProfiles );

    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();

private: // data
    
    /**
     * Buffer for packing the MdcaPoint
     *
     * Owns
     */
    HBufC16*					        iBuffer;
    
    /**
     * Profile Array
     */
    RPointerArray<CGeoProfile>*	        iGeoProfiles;
    };

#endif // C_GEOPROFILESLBMODEL_H
