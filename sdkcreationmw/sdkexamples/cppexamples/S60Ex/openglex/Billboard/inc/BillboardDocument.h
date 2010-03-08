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


#ifndef BILLBOARDDOCUMENT_H
#define BILLBOARDDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CBillboardDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CBillboardDocument object.
		 * Calls the hidden constructors.
         */
        static CBillboardDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CBillboardDocument();

	protected: // Hidden constructors
        /**
         * Second phase constructor that can call methods that may leave. Does nothing.
         */
        void ConstructL();

    public: // Hidden constructors

		/**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CBillboardDocument(CEikApplication& aApp);

    private: // Functions from base classes

        /**
         * From CEikDocument, creates and returns CBillboardAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
};

#endif

// End of File
