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
* Description:  Document class for Geo Profiles Application
*
*/


#ifndef C_GEOPROFILESDOCUMENT_H
#define C_GEOPROFILESDOCUMENT_H

// SYSTEM INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

/**
 *  GeoProfiles Application's document class.
 */
class CGeoProfilesDocument : public CAknDocument
    {
public:
	/**
	 * Static Two phase constructor
	 */
    static CGeoProfilesDocument* NewL( CEikApplication&    aApp );

  	/**
  	 * Virtual destructor
  	 */
    virtual ~CGeoProfilesDocument();

private:
	/**
	 * C++ Default constructor
	 */
    CGeoProfilesDocument( CEikApplication&     aApp );

	/**
	 * Second phase of the two phase constructor
	 */
    void ConstructL();

private: // Inherited from Base class

	/**
	 * Inherited from CAknDocument
	 */
    CEikAppUi* CreateAppUiL();
    };

#endif  // C_GEOPROFILESDOCUMENT_H

// End of File

