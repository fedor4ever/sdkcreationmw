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


#ifndef ISVTELCALLAPPAPP_H
#define ISVTELCALLAPPAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidIsvTelCallApp = { 0xE00001EF }; // UID of the application

// CLASS DECLARATION
/**
* CIsvTelCallAppApplication application class.
* Provides factory to create concrete document object.
*/
class CIsvTelCallAppApplication : public CAknApplication
    {
    private: // From CApaApplication

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CIsvTelCallAppDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, AppDllUid.
        * Returns application's UID (KUidIsvTelCallApp).
        * @return The value of KUidIsvTelCallApp.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File