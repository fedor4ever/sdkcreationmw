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
* Description:  Application class for Geo Profiles  Application.
*
*/


#ifndef C_GEOPROFILESAPP_H
#define C_GEOPROFILESAPP_H

// SYSTEM INCLUDES
#include <aknapp.h>

/**
 *  GeoProfiles Application's App class. This class extents the CAknApplication
 *  class for GeoProfiles.
 */	
class CGeoProfilesApp : public CAknApplication
	{
private: // Inherited from Base classes
	/**
	 * Inherited from CAknApplication
	 */ 
    CApaDocument* CreateDocumentL();
    
	/**
	 * Inherited from CAknApplication
	 */         
    TUid AppDllUid() const;
    };

#endif  // C_GEOPROFILESAPP_H

// End of File

