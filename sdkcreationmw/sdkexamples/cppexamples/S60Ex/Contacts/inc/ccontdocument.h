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


#ifndef __CCONTDOCUMENT_H__
#define __CCONTDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CContDocument application class.
*/
class CContDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * NewL
        * Construct a CContDocument for the AVKON application aApp
        * using two phase construction, and return a pointer to the created object
        * @param aApp application creating this document
        * @return a pointer to the created instance of CContDocument
        */
        static CContDocument* NewL(CEikApplication& aApp);

        /**
        * ~CContDocument
        * Destroy the object and release all memory objects
        * Destructor is always virtual
        */
        virtual ~CContDocument();

    private:

        /**
        * CContDocument
        * Perform the first phase of two phase construction
        * @param aApp application creating this document
        */
        CContDocument(CEikApplication& aApp);

        /**
        * ConstructL
        * Perform the second phase construction of a
        * CContDocument object
        */
        void ConstructL();

    private:

        /**
        *  CreateAppUiL
        *  Create a CContAppUi object and return a pointer to it
        *  @return a pointer to the created instance of the AppUi created
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // __CCONTDOCUMENT_H__

// End of File

