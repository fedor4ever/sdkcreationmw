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
* Description:  View class for Geo Profiles Application
*
*/


#ifndef C_GEOPROFILESAPPVIEW_H_
#define C_GEOPROFILESAPPVIEW_H_

//  SYSTEM INCLUDES
#include <aknview.h>
#include <aknviewappui.h>

// USER INCLUDES
#include "geoprofilesengine.h"

// FORWARD DECLARATIONS
class CGeoProfilesContainer;
class CGeoProfilesLBModel;
class CGeoProfile;

/**
 *  GeoProfiles Application's View class.
 *
 *  @since S60 v5.0
 */
class CGeoProfilesView : public CAknView,
                         public MGeoProfilesEngineObserver
    {
public:
	/**
	 * Static Two phase constructor
	 *
	 * @return CGeoProfilesView*, Pointer to the newly created object.
	 */                                              
    static CGeoProfilesView* NewL();
    
	/**
	 * Static Two phase constructor.
	 * Leaves the created instance on the Cleanup Stack.
	 *
	 * @return CGeoProfilesView*, Pointer to the newly created object.
	 */                                                
    static CGeoProfilesView* NewLC();
    
    /**
     * C++ Destructor.
     */
    virtual ~CGeoProfilesView();

	/**
	 * Handles the event generated when the Screen size is changed when
	 * this View is being displayed.
	 */
	void ScreenSizeChanged();
	
	/**
	 * Handles the Container specific commands
	 */
	void ContainerCommandL( TInt aCommand );
	    
public: // Inherited from Base classes
    
  	/**
  	 * Inherited from CAknView
  	 */     
    void DoActivateL( const TVwsViewId& aPrevViewId,
                            TUid        aCustomMessageId,
                      const TDesC8&     aCustomMessage );

  	/**
  	 * Inherited from CAknView
  	 */ 
    void DoDeactivate();

  	/**
  	 * Inherited from CAknView
  	 */ 
    TUid Id() const;

  	/**
  	 * Inherited from CAknView
  	 */ 
    void HandleCommandL(TInt aCommand);
   
  	/**
  	 * Inherited from CAknView
  	 */    
    void DynInitMenuPaneL( TInt 			aResourceId, 
    					   CEikMenuPane* 	aMenuPane );
    
  	/**
  	 * Inherited from MGeoProfilesEngineObserver
  	 */      					       					   
    void ProfileStatusChangedL();
        					                   
private:
	/**
	 * Overloaded C++ Contructor.
	 */
    CGeoProfilesView();

	/**
	 * Second phase of the two phase constructor.
	 */
    void ConstructL();
    
	/**
	 * Sets the Command button array based on the number of elements
	 */    
    void SetCbaL();
        
private:
	/**
	 * GeoProfiles Container.
	 * Owns.
	 */
    CGeoProfilesContainer*   	iContainer;
    
    /**
     * List box model
     */
    CGeoProfilesLBModel*        iLBModel;
    
    /**
     *
     */
    RPointerArray<CGeoProfile>  iGeoProfiles;
    
    /**
     *
     */
    CGeoProfilesEngine*         iEngine;  
    };          

#endif // C_GEOPROFILESAPPVIEW_H_

