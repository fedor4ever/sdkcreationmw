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


#ifndef __FOCUSEVENT_DOCUMENT_H__
#define __FOCUSEVENT_DOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CFocusEventAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
* CFocusEventDocument application class.
* An instance of class CFocusEventDocument is the Document part of the AVKON
* application framework for the FocusEventDocument example application.
*/
class CFocusEventDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CFocusEventDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CFocusEventDocument.
        */
        static CFocusEventDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CFocusEventDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CFocusEventDocument.
        */
        static CFocusEventDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CFocusEventDocument
        * Virtual Destructor.
        */
        virtual ~CFocusEventDocument();

    public: // from CEikDocument

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CFocusEventAppUi object and return a pointer to it.
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
        * CFocusEventDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CFocusEventDocument( CEikApplication& aApp );
    };

#endif // __FOCUSEVENT_DOCUMENT_H__

// End of File
