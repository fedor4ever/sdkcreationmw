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


#ifndef OCREXAMPLEDOCUMENT_H
#define OCREXAMPLEDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  COcrExampleDocument application class.
*/
class COcrExampleDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static COcrExampleDocument* NewL( CEikApplication& aApp );

        /** Destructor. */
        virtual ~COcrExampleDocument();

    private:

        /** C++ constructor. */
        COcrExampleDocument( CEikApplication& aApp );

        /** Symbian OS default constructor. */
        void ConstructL();

    private:

        /**
        * From CEikDocument, create COcrExampleAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File