/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __LOCALIZATIONDOCUMENT_H__
#define __LOCALIZATIONDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CLocalizationAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CLocalizationDocument application class.
* An instance of class CLocalizationDocument is the Document part of the
* AVKON application framework for the Localization example application.
*/
class CLocalizationDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CLocalizationDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CLocalizationDocument.
        */
        static CLocalizationDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CLocalizationDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CLocalizationDocument.
        */
        static CLocalizationDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CLocalizationDocument
        * Virtual Destructor.
        */
        virtual ~CLocalizationDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CLocalizationAppUi object and return a pointer to it.
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
        * CLocalizationDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CLocalizationDocument( CEikApplication& aApp );

    };

#endif // __LOCALIZATIONDOCUMENT_H__

// End of File
