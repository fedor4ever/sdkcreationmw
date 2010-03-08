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


#ifndef __HELLOWORLDBASICAPPLICATION_H__
#define __HELLOWORLDBASICAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CHelloWorldBasicApplication application class.
* Provides factory to create concrete document object.
* An instance of CHelloWorldBasicApplication is the application part of the
* AVKON application framework for the HelloWorldBasic example application.
*/
class CHelloWorldBasicApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidHelloWorldBasicApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CHelloWorldBasicDocument document object. The returned
        * pointer in not owned by the CHelloWorldBasicApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __HELLOWORLDBASICAPPLICATION_H__

// End of File

