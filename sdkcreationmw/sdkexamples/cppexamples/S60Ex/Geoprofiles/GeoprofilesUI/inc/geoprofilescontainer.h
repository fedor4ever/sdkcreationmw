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
* Description:  Container class for Geo Profiles Application
*
*/


#ifndef C_GEOPROFILESCONTAINER_H
#define C_GEOPROFILESCONTAINER_H

// SYSTEM INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CGeoProfilesAppUi;
class CGeoProfilesLBModel;
class CAknSettingStyleListBox;
class CGeoProfilesView;

/**
 *  GeoProfiles Application's Container class. This class is the chief
 *  windowing component of GeoProfiles which displays the List of
 *  Tagged profiles
 */
class CGeoProfilesContainer : public CCoeControl,
                              public MEikListBoxObserver
    {
public:
	/**
	 * Static Two phase constructor
	 *
	 * @param[in] aRect, 		 Client Rectangle for this window owning control.
	 */
	static CGeoProfilesContainer* NewL( const TRect& 	            aRect,
	                                          CGeoProfilesLBModel&  aListboxModel,
	                                          CGeoProfilesView&     aView );
	
  	/**
  	 * Virtual destructor
  	 */
    ~CGeoProfilesContainer();
    
    /**
     * Updates the container
     */
    void UpdateL( CGeoProfilesLBModel&  aListboxModel );
    
    /**
     * Returns the currently focussed element.
     */
    TInt GetFocussedItem();    
 	
protected: // Inherited from Base classes

  	/**
  	 * Inherited from CCoeControl
  	 */  
    void SizeChanged();

  	/**
  	 * Inherited from CCoeControl
  	 */
    TInt CountComponentControls() const;

  	/**
  	 * Inherited from CCoeControl
  	 */
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
  	/**
  	 * Inherited from CCoeControl
  	 */    
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                         			   TEventCode aType );
                         			   
	/**
	 * Inherited from CCoeControl
	 */
	void HandleResourceChange( TInt aType );
	
    /**
     * Inherited from MEikListBoxObserver
     */
    void HandleListBoxEventL( CEikListBox*      aListBox, 
                              TListBoxEvent     aEventType );
    
    /**
     * Focus changes event
     */
    void FocusChanged( TDrawNow aDrawNow );    
                                                             			            
private:
	
	/**
	 * C++ Default constructor
	 */
    CGeoProfilesContainer( CGeoProfilesLBModel&  aListboxModel,
                           CGeoProfilesView&     aView );
    
	/**
	 * Second phase of the two phase constructor
	 */     
    void ConstructL( const TRect& 	aRect );
    
    /**
     * Sets the Title text
     * @param aResourceText Resource to create title
     */
    void MakeTitleL( TInt aResourceText );
    
    /** 
     * Creates the Geo profiles List box       
     */
    void CreateListboxL();    
            
private:
    /**
     * Reference to the list box model
     * Does not own
     */
    CGeoProfilesLBModel*            iListboxModel;
    
    /**
     * Settings list box for Geo Profiles list
     * Owns
     */
    CAknSettingStyleListBox*        iListBox;
    
    /**
     * Reference to the Container's view
     */
    CGeoProfilesView&               iView;
                 
	};

#endif  // C_GEOPROFILESCONTAINER_H

// End of File
