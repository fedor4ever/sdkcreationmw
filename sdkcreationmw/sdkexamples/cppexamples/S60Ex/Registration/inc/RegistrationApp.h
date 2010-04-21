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


#ifndef REGISTRATIONAPP_H
#define REGISTRATIONAPP_H

// INCLUDE FILES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidRegistration = { 0xE00001F1 };

// CLASS DECLARATION

/**
* CRegistrationApp application class.
* Provides factory to create concrete document object.
*/
class CRegistrationApp : public CAknApplication
    {
    
    public: // Functions from base classes
    
    private: // From CApaApplication
        /**
        * Creates CRegistrationDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * Returns application's UID (KUidRegistration).
        * @return The value of KUidRegistration.
        */
        TUid AppDllUid() const;
    };

#endif // REGISTRATIONAPP_H

// End of File

