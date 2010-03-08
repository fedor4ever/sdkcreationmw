/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef LENSFLAREDOCUMENT_H
#define LENSFLAREDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CLensFlareDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CLensFlareDocument object.
		 * Calls the hidden constructors.
         */
        static CLensFlareDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CLensFlareDocument();

	protected: // Hidden constructors
        /**
         * Second phase constructor that can call methods that may leave. Does nothing.
         */
        void ConstructL();

    public: // Hidden constructors

		/**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CLensFlareDocument(CEikApplication& aApp);

    private: // Functions from base classes

        /**
         * From CEikDocument, creates and returns CLensFlareAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
};

#endif

// End of File
