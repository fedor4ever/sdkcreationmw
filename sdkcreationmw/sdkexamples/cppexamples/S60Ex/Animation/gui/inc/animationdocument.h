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


#ifndef __ANIMATIONDOCUMENT_H__
#define __ANIMATIONDOCUMENT_H__

// INCLUDES
#include <eikdoc.h>

// FORWARD DECLARATIONS
class CAnimationAppUi;
class CEikApplication;

// CLASS DECLARATION
/**
* CAnimationDocument.
* An instance of class CAnimationDocument is the Document part of the AVKON
* application framework for the Animation example application.
*/
class CAnimationDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CAnimationDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp application creating this document.
        * @return A pointer to the created instance of CAnimationDocument.
        */
        static CAnimationDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CAnimationDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CAnimationDocument.
        */
        static CAnimationDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CAnimationDocument
        * Virtual Destructor.
        */
        virtual ~CAnimationDocument();

    public: // from CAknDocument

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CAnimationAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * CAnimationDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CAnimationDocument( CEikApplication& aApp );
           
        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();
    };

#endif // __ANIMATIONDOCUMENT_H__

// End of File
