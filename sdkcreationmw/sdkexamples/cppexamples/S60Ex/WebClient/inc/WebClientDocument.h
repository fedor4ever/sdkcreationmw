/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WEBCLIENTDOCUMENT_H
#define WEBCLIENTDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// CLASS DECLARATION

/**
*  CWebClientDocument application class.
*/
class CWebClientDocument : public CAknDocument
    {
    public:
        /**
        * NewL()
        * Create a CWebClientDocument object.
        * @param aApp: Reference to application this document relates.
        */
        static CWebClientDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC()
        * Create a CWebClientDocument object.
        * @param aApp: Reference to application this document relates.
        */
        static CWebClientDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CWebClientDocument()
        * Destructor for CWebClientDocument.
        */
        ~CWebClientDocument();

    /*
    * From CAknDocument
    */
    public:
        /**
        * CreateAppUiL()
        * Creates the application user interface, and returns a pointer to it;
        * the framework takes ownership of this object
        * @return Pointer to created application ui.
        */
        CEikAppUi* CreateAppUiL();

    private:
        /**
        * ConstructL()
        * Second phase construction.
        */
        void ConstructL();

        /**
        * CWebClientDocument()
        * First phase construction.
        * @param aApp: Reference to application this document relates.
        */
        CWebClientDocument( CEikApplication& aApp );

        };

#endif // WEBCLIENTDOCUMENT_H
