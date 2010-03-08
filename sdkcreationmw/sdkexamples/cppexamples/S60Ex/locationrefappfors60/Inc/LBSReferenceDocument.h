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
* Description:  This class creates the user interface of the application
*
*/


#ifndef LBSREFERENCEDOCUMENT_H
#define LBSREFERENCEDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  This class creates the user interface of the application
*/
class CLbsReferenceDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CLbsReferenceDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CLbsReferenceDocument();

    private:

        /**
        * Symbian OS default constructor.
        */
        CLbsReferenceDocument(CEikApplication& aApp);
        
        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CLBSReferenceAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif //LBSREFERENCEDOCUMENT_H

// End of File

