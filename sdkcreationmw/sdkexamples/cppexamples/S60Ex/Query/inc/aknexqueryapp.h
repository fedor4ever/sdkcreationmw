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


#ifndef __AKNEXQUERYAPP_H__
#define __AKNEXQUERYAPP_H__

// INCLUDES
#include <Aknapp.h>

// CONSTANTS
const TUid KUidAknExQuery = { 0xA0000232 }; // UID of the application

// CLASS DECLARATION

/**
* CAknExQueryApp application class.
* Provides factory to create concrete document object.
*/
class CAknExQueryApp : public CAknApplication
    {
    private: // Form CApalication

        /**
        * CreateDocumentL
        * From CApaApplication, creates CAknExQueryDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * AppDllUid
        * From CApaApplication, returns application's UID ( KUidAknExQuery ).
        * @return The value of KUidAknExQuery.
        */
        TUid AppDllUid() const;

    };

#endif // __AKNEXQUERYAPP_H__

// End of File
