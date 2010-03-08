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
* Description:  Application class for Geo Profiles Application UI.
*
*/


#ifndef C_GEOPROFILESAPPUI_H
#define C_GEOPROFILESAPPUI_H

// SYSTEM INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>

// FORWARD DECLARATIONS
class CGeoProfilesView;

/**
 *  GeoProfiles Application's App UI class. This class extents the CAknAppUi
 *  class for GeoProfiles.
 */
class CGeoProfilesAppUi : public CAknViewAppUi
    {
public:   
	/**
	 * Second phase of the two phase constructor
	 *
	 * Called by the Application framework
	 */ 
    void ConstructL();
  
  	/**
  	 * Virtual destructor
  	 */
    virtual ~CGeoProfilesAppUi();   
  
public: // Inherited from Base classes
  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    void HandleCommandL(TInt aCommand);
                       
private: // Inherited from Base classes

  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType );
                           
private:
	/**
	 * GeoProfiles Application's view class
	 */       
    CGeoProfilesView*    		iGeoProfilesView;
    };

#endif  // C_GEOPROFILESAPPUI_H

// End of File
