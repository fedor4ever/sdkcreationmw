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
* Description:  Declares document class for application.
*
*/


#ifndef DATAMOBILITYDOCUMENT_H
#define DATAMOBILITYDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CDataMobilityAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CDataMobilityDocument application class.
* An instance of class CDataMobilityDocument is the Document part of the
* AVKON application framework for the datamobility example application.
*/
class CDataMobilityDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CDataMobilityDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CDataMobilityDocument.
        */
        static CDataMobilityDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CDataMobilityDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CDataMobilityDocument.
        */
        static CDataMobilityDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CDataMobilityDocument
        * Virtual Destructor.
        */
        virtual ~CDataMobilityDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CDataMobilityAppUi object and return a pointer to it.
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
        * CDataMobilityDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CDataMobilityDocument( CEikApplication& aApp );
    };

#endif // DATAMOBILITYDOCUMENT_H

// End of File
