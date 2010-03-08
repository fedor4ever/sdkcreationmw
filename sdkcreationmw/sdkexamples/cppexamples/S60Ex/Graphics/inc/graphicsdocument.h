/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __GRAPHICSDOCUMENT_H__
#define __GRAPHICSDOCUMENT_H__

// INCLUDES
#include <eikdoc.h>

// FORWARD DECLARATIONS
class CGraphicsAppUi;
class CEikApplication;



// CLASS DECLARATION

/**
* CGraphicsDocument application class.
* An instance of class CGraphicsDocument is the Document part of the AVKON
* application framework for the GraphicsDocument example application.
*/
class CGraphicsDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CGraphicsDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CGraphicsDocument.
        */
        static CGraphicsDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CGraphicsDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CGraphicsDocument.
        */
        static CGraphicsDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CGraphicsDocument
        * Virtual Destructor.
        */
        virtual ~CGraphicsDocument();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CGraphicsDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CGraphicsDocument( CEikApplication& aApp );

    private: // Functions from base classes

        /**
        * From CEikDocument, CreateAppUiL.
        * Create a CGraphicsAppUi object and return a pointer to it.
        * @return A pointer to the created instance of the AppUi created.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // __GRAPHICSDOCUMENT_H__
