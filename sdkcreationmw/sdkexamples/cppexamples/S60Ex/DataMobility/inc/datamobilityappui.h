/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for application.
*
*/


#ifndef DATAMOBILITYAPPUI_H
#define DATAMOBILITYAPPUI_H

// INCLUDES
#include <aknviewappui.h>


// CLASS DECLARATION

/**
* CDataMobilityAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CDataMobilityAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CDataMobilityAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CDataMobilityAppUi();

        /**
        * ~CDataMobilityAppUi.
        * Virtual Destructor.
        */
        virtual ~CDataMobilityAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

    };

#endif // DATAMOBILITYAPPUI_H

// End of File
