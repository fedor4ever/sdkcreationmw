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


#ifndef __AKNEXPOPFIELDDOCUMENT_H__
#define __AKNEXPOPFIELDDOCUMENT_H__

// INCLUDES
#include <eikdoc.h>


// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
* CAknExPopFieldDocument application class.
*/
class CAknExPopFieldDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        * @return Pointer to created Document class object.
        * @param aApp Reference to Application class object.
        */
        static CAknExPopFieldDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CAknExPopFieldDocument();

    private: // Constructors

        /**
        * Overload constructor.
        * @param aApp Reference to Application class object.
        */
        CAknExPopFieldDocument( CEikApplication& aApp );

    private: // From CEikDocument

        /**
        * From CEikDocument, CreateAppUiL.
        * Creates CAknExPopFieldAppUi "App UI" object.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif  //__AKNEXPOPFIELDDOCUMENT_H__

// End of File
