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
* Description:  Geo Profile Editor
*
*/


#ifndef C_GEOPROFILEEDITOR_H
#define C_GEOPROFILEEDITOR_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <AknForm.h>

// USER INCLUDE
#include "geoprofile.h"

// FORWARD DECLARATIONS
class CAknInformationNote;

/**
 * Geo Profile Editor
 */
class CGeoProfileEditor : public CAknForm
    {
public:    
    /**
     * Constructs a new instance of CGeoProfile Entry.        
     */
    static CGeoProfileEditor* NewL( CGeoProfile&      aGeoProfile );
    
    /**
     * C++ Destructor.
     * Frees all the resources
     */
    virtual ~CGeoProfileEditor();

public:    
    /*
     * Inherited from CEikDialog
     */
    TInt ExecuteLD();
            
private:    
    /**
     * Inherited from CEikDialog
     */
    TBool OkToExitL( TInt aButtonId );
    
    /**
     * Inherited from CEikDialog
     */    
    void PreLayoutDynInitL();
    
    /**
     * Inherited from CAknDialog
     */
    void LineChangedL( TInt  aControlId );
    
    /**
     * Inherited from CAknForm
     */
    void HandleControlStateChangeL( TInt aControlId );    
    
    /**
     * Inherited from CCoeControl
     */ 
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
       
private: // Initialization functions

    /**
     * Default Constructor
     */
    CGeoProfileEditor( CGeoProfile&      aGeoProfile );
    
    /** 
     * Second phase of two phase constructor
     */
    void ConstructL();
    
    /**
     * Loads all the Values
     */
    void LoadFormValuesL();
    
    /** 
     * Set the Title text
     */ 
    void MakeTitleL( TDesC& aText );                
            
    /**
     * Loads place
     */    
    void LoadPlaceL();
            
    /**
     * Loads the profile name
     */    
    void LoadProfileL();
    
    /**
     * Loads the Trigger Type
     */
    void LoadTriggerTypeL();
        
    /**
     * Process Profile Selector Command
     */
    void HandleProfileSelectionL();
    
    /**
     * Process Place selection command
     */
    void HandlePlaceSelectionL();
    
    /**
     *
     */
    TInt CheckIfSavableL();    
             
private:
    /**
     * Reference to a Geo Profile
     */
    CGeoProfile&                iGeoProfile; 
    
    /**
     * Temporary Geo Profile object
     */
    CGeoProfile*                iNewGeoProfile;
    
    /**
     *
     */
    TInt                        iSelectedPlace;
    
    CAknInformationNote*        iInfoNote;
        
    };

#endif // C_GEOPROFILEEDITOR_H
