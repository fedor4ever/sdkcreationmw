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


#ifndef TEXTUREDOCUMENT_H
#define TEXTUREDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CTextureDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCubeDocument object.
         */
        static CTextureDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CTextureDocument();

    private: // Functions from base classes

        /**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CTextureDocument(CEikApplication& aApp);

        /**
         * Second phase constructor. Does nothing.
         */
        void ConstructL();

    private:

        /**
         * From CEikDocument, creates and returns CTextureAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

