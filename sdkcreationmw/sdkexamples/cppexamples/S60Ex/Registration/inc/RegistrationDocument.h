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


#ifndef REGISTRATIONDOCUMENT_H
#define REGISTRATIONDOCUMENT_H

// INCLUDE FILES
#include <akndoc.h>
   
// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
* CRegistrationDocument application class.
*/
class CRegistrationDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CRegistrationDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CRegistrationDocument();

    public: // New functions

    public: // Functions from base classes
    
    protected: // New functions

    protected: // Functions from base classes

    private:
        /**
        * Symbian default constructor.
        */
        CRegistrationDocument( CEikApplication& aApp );
        
        /**
        * Symbian default constructor can leave.
        */
        void ConstructL();

    private: // From CEikDocument
        /**
        * Create CRegistrationAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // REGISTRATIONDOCUMENT_H

// End of File

