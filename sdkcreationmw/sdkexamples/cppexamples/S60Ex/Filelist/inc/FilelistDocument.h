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


#ifndef FILELISTDOCUMENT_H
#define FILELISTDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CFilelistDocument application class.
*/
class CFilelistDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CFilelistDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CFilelistDocument();

    public: // New functions

    public: // Functions from base classes
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * Symbian default constructor.
        */
        CFilelistDocument( CEikApplication& aApp );
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CFilelistAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

