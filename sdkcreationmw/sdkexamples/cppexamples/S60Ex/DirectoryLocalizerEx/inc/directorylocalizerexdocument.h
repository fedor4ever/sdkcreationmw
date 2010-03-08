/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application Document class, CDirectoryLocalizerExDocument - 
*				 stores and restores the state of the application
*
*/


#ifndef C_DirectoryLocalizerExDOCUMENT_H
#define C_DirectoryLocalizerExDOCUMENT_H

// INCLUDES
#include <eikdoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
* CDirectoryLocalizerExDocument application class.
*/
class CDirectoryLocalizerExDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor.
        * @return Pointer to created Document class object.
        * @param aApp Reference to Application class object.
        */
        static CDirectoryLocalizerExDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CDirectoryLocalizerExDocument();

    private: // Constructors

        /**
        * Overload constructor.
        * @param aApp Reference to Application class object.
        */
        CDirectoryLocalizerExDocument( CEikApplication& aApp );

    private: // From CEikDocument

        /**
        * From CEikDocument, CreateAppUiL.
        * Creates CDirectoryLocalizerExAppUi "App UI" object.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif  // C_DirectoryLocalizerExDOCUMENT_H

// End of File
