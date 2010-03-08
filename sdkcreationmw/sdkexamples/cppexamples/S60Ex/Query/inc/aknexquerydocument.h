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


#ifndef __AKNEXQUERYDOCUMENT_H__
#define __AKNEXQUERYDOCUMENT_H__

// INCLUDES
#include <eikdoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CAknExQueryDocument application class.
*/
class CAknExQueryDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Symbian two-phased constructor.
        * @return Pointer to created Document class object.
        * @param aApp Reference to Application class object.
        */
        static CAknExQueryDocument* NewL( CEikApplication& aApp );

        /**
        * ~CAknExQueryDocument
        * Destructor.
        */
        virtual ~CAknExQueryDocument();

    private: // Constructors

        /**
        * CAknExQueryDocument
        * Overload constructor.
        * @param aApp Reference to Application class object.
        */
        CAknExQueryDocument( CEikApplication& aApp );

    private: // From CEikDocument

        /**
        * From CEikDocument, CreateAppUiL.
        * Creates CAknExQueryAppUi "App UI" object.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif // __AKNEXQUERYDOCUMENT_H__

// End of File
