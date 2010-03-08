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


#ifndef __HELPEXAMPLE_DOCUMENT_H__
#define __HELPEXAMPLE_DOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS

class CHelpExampleAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
* CHelpExampleDocument
* An instance of class CHelpExampleDocument is the Document part of the AVKON
* application framework for the HelpExample example application
*/

class CHelpExampleDocument : public CAknDocument
    {
        public: // Constructors and destructor.

            /**
            * NewL()
            * Construct a CHelpExampleDocument for the AVKON application aApp 
            * using two phase construction, and return a pointer to 
            * the created object
            * @param aApp Application creating this document
            * @return A pointer to the created instance of CHelpExampleDocument
            */
            static CHelpExampleDocument* NewL( CEikApplication& aApp );

            /**
            * NewLC()
            * Construct a CHelpExampleDocument for the AVKON application aApp 
            * using two phase construction, and return a pointer to 
            * the created object
            * @param aApp Application creating this document
            * @return A pointer to the created instance of CHelpExampleDocument
            */
            static CHelpExampleDocument* NewLC( CEikApplication& aApp );

            /**
            * ~CHelpExampleDocument()
            * Destroy the object
            */
            virtual ~CHelpExampleDocument();

        public: // from CAknDocument

            /**
            * CreateAppUiL()
            * Create a CHelpExampleAppUi object and return a pointer to it
            * @return A pointer to the created instance of the AppUi created
            */
            CEikAppUi* CreateAppUiL();

        private:    // Constructors

            /**
            * ConstructL()
            * Perform the second phase construction of a CHelpExampleDocument object
            */

            void ConstructL();

            /**
            * CHelpExampleDocument()
            * Perform the first phase of two phase construction 
            * @param aApp Application creating this document
            */
            CHelpExampleDocument( CEikApplication& aApp );

    };


#endif // __HELPEXAMPLE_DOCUMENT_H__

// End of File
