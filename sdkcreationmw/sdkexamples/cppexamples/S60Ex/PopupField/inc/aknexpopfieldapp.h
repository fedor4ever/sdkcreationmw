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


#ifndef __AKNEXPOPFIELDAPP_H__
#define __AKNEXPOPFIELDAPP_H__

// INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidAknExPopField = { 0xA000022A }; // UID of the application

// CLASS DECLARATION

/**
* CAknExPopFieldApplication application class.
* Provides factory to create concrete document object.
*/
class CAknExPopFieldApplication : public CAknApplication
    {
    private: // From CApaApplication

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CAknExPopFieldDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, AppDllUid.
        * Returns application's UID (KUidAknExPopField).
        * @return The value of KUidAknExPopField.
        */
        TUid AppDllUid() const;

    };

#endif  //__AKNEXPOPFIELDAPP_H__

// End of File
