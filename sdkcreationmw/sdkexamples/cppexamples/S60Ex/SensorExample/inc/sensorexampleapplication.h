/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SENSOREXAMPLEAPPLICATION_H__
#define __SENSOREXAMPLEAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CSensorExampleApplication application class.
* Provides factory to create concrete document object.
* An instance of CSensorExampleApplication is the application part of the
* AVKON application framework for the sensorexample application.
*/
class CSensorExampleApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID.
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CSensorExampleApplication document object. The returned
        * pointer in not owned by the CSensorExampleApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __SENSOREXAMPLEAPPLICATION_H__

// End of File

