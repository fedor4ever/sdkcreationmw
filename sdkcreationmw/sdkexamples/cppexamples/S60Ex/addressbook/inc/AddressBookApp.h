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
* Description:  A main application class, CAddressBookApp 
*				 - starting point of the program
*
*/


#ifndef ADDRESSBOOKAPP_H
#define ADDRESSBOOKAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidAddressBookApp = { 0xA0000180 };

// CLASS DECLARATION

/**
* CAddressBookApp application class.
* Provides factory to create concrete document object.
*/
class CAddressBookApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
        * From CApaApplication, creates CAddressBookDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidAddressBookApp).
        * @return The value of KUidAddressBookApp.
        */
        TUid AppDllUid() const;
    };

#endif //ADDRESSBOOKAPP_H

// End of File

