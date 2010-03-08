/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SNOWDOCUMENT_H
#define SNOWDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CSnowDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CSnowDocument object.
		 * Calls the hidden constructors.
         */
        static CSnowDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CSnowDocument();

	protected: // Hidden constructors
        /**
         * Second phase constructor that can call methods that may leave. Does nothing.
         */
        void ConstructL();

    public: // Hidden constructors

		/**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CSnowDocument(CEikApplication& aApp);

    private: // Functions from base classes

        /**
         * From CEikDocument, creates and returns CSnowAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
};

#endif

// End of File
