/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SHADOWSDOCUMENT_H
#define SHADOWSDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CShadowsDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CShadowsDocument object.
         */
        static CShadowsDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CShadowsDocument();

    private: // Functions from base classes

        /**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CShadowsDocument(CEikApplication& aApp);

        /**
         * Second phase constructor. Does nothing.
         */
        void ConstructL();

    private: // New functions

        /**
         * From CEikDocument, creates and returns CShadowsAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

