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
* Description: 
*
*/


#ifndef __SENSOREXAMPLEDOCUMENT_H__
#define __SENSOREXAMPLEDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CSensorExampleAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CSensorExampleDocument application class.
* An instance of class CSensorExampleDocument is the Document part of the
* AVKON application framework for the sensor example application.
*/
class CSensorExampleDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CSensorExampleDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CSensorExampleDocument.
        */
        static CSensorExampleDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CSensorExampleDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CSensorExampleDocument.
        */
        static CSensorExampleDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CSensorExampleDocument
        * Virtual Destructor.
        */
        virtual ~CSensorExampleDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CSensorExampleAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CSensorExampleDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CSensorExampleDocument( CEikApplication& aApp );

    };

#endif // __SENSOREXAMPLEDOCUMENT_H__

// End of File
