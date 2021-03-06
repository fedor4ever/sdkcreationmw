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


#ifndef SNOWAPP_H
#define SNOWAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
/** UID of the application. */
const TUid KUidSnow = { 0xA000021E };

// CLASS DECLARATION

/**
 * Application class. Provides factory method to create a concrete document object.
 */
class CSnowApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
         * From CApaApplication, creates and returns CSnowDocument document object.
         * @return Pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /**
         * From CApaApplication, returns application's UID (KUidSnow).
         * @return Value of KUidSnow.
         */
        TUid AppDllUid() const;
    };

// OTHER EXPORTED FUNCTIONS

/**
 * Factory method used by the E32Main method to create a new application instance.
 */
LOCAL_C CApaApplication* NewApplication();

/**
 * Entry point to the EXE application. Creates new application instance and
 * runs it by giving it as parameter to EikStart::RunApplication() method.
 */
GLDEF_C TInt E32Main();

#endif

// End of File
