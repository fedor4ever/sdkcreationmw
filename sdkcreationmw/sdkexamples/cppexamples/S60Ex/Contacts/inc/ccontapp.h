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


#ifndef __CCONTAPP_H__
#define __CCONTAPP_H__

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidContacts = { 0xA000022E };

// CLASS DECLARATION

/**
* CContApp application class.
* Provides factory to create concrete document object.
*
*/
class CContApp : public CAknApplication
    {

    private:

        /**
        * From CApaApplication, creates CContDocument document object
        * @return A pointer to the created document object
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, returns application's UID (KUidContacts)
        * @return The value of KUidContacts
        */
        TUid AppDllUid() const;
    };

#endif // __CCONTAPP_H__

// End of File

