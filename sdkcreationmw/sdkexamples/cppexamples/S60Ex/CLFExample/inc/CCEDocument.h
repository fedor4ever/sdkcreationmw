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
* Description: 
*
*/



#ifndef __CCEDOCUMENT_H__
#define __CCEDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CEikAppUi;

// CLASS DECLARATION

/**
*  Document for CLF Example Application.
*/
class CCEDocument : public CAknDocument
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCEDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CCEDocument();

    private: // Constructors

        /**
        * C++ default constructor.
        */
        CCEDocument( CEikApplication& aApp );

    protected:  // Functions from base classes

        /**
        * From CEikDocument. Creates the Application UI
        */
        CEikAppUi* CreateAppUiL();

    private: // Data

    };

#endif // __CCEDOCUMENT_H__

// End of File
