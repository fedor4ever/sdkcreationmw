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
* Description:  Geo Profile selector
*
*/


#ifndef C_GEOPROFILESELECTOR_H
#define C_GEOPROFILESELECTOR_H

// SYSTEM INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

/**
 * Geo Profile Selector
 */
class CGeoProfileSelector : public CBase
    {     
public:
    /**
     * Constructs a new instance of CGeoProfileSelector
     */
    static CGeoProfileSelector* NewLC();
    
    /**
     * C++ Destructor.
     * Frees all the resources
     */
    virtual ~CGeoProfileSelector();
    
    /**
     * Select Profile
     */    
    TInt SelectGeoProfileL( TInt&    aProfileId );
        
private:
    /**
     * C++ Constructor
     */
    CGeoProfileSelector();
                      
    /**
     * Second phase of two phase construction
     */                      
    void ConstructL();        
                   
private: // Member Variables     
    /**
     * Title text for pop-up dialog
     * Owns
     */                 
    HBufC*      iProfilePopupTitle;            
    }; 

#endif // C_GEOPROFILESELECTOR_H
