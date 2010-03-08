/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CommonDialogsEx document.
*
*/


#ifndef CCOMMONDIALOGSEXDOCUMENT_H
#define CCOMMONDIALOGSEXDOCUMENT_H

// INCLUDES
#include <eikdoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CCommonDialogsExDocument application class.
*/
class CCommonDialogsExDocument : public CEikDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CCommonDialogsExDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CCommonDialogsExDocument();

    private:

        /**
        * EPOC default constructor.
        */
        CCommonDialogsExDocument(CEikApplication& aApp): CEikDocument(aApp) { }
        void ConstructL();

    private:

        /**
        * From CEikDocument, CreateAppUiL.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

