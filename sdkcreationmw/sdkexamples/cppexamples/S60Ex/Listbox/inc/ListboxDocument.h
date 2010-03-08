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


#ifndef __LISTBOXDOCUMENT_H__
#define __LISTBOXDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

class CListboxAppUi;
class CEikApplication;


/**
* CListboxDocument application class.
* An instance of class CListboxDocument is the Document part of the
* AVKON application framework for the Listbox example application.
*/
class CListboxDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * Construct a CListboxDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CListboxDocument.
        */
        static CListboxDocument* NewL( CEikApplication& aApp );

        /**
        * Two-phased constructor.
        * Construct a CListboxDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CListboxDocument.
        */
        static CListboxDocument* NewLC( CEikApplication& aApp );

        /**
        * Virtual Destructor.
        */
        virtual ~CListboxDocument();

    public: // Functions from base classes

        /**
        * From CEikDocument, CreateAppUiL.
        * Create a CListboxAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CListboxDocument( CEikApplication& aApp );

    };

#endif // __LISTBOXDOCUMENT_H__

// End of File

