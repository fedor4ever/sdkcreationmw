/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef ISVTELINFOAPPAPPLICATION_H
#define ISVTELINFOAPPAPPLICATION_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidIsvTelInfoApp = { 0xE00001F0  }; // UID of the application

// CLASS DECLARATION

/**
* CIsvTelInfoAppApplication application class.
* Provides factory to create concrete document object.
*/
class CIsvTelInfoAppApplication : public CAknApplication
    {
    private: // From CApaApplication

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CIsvTelInfoAppDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, AppDllUid.
        * Returns application's UID (KUidIsvTelInfoApp).
        * @return The value of KUidIsvTelInfoApp.
        */
        TUid AppDllUid() const;

    };

#endif

// End of File
