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


#ifndef __HELPEXAMPLE_APPLICATION_H__
#define __HELPEXAMPLE_APPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "HelpExampleUID.hrh"

// CONSTANTS

// UID for the application; this should correspond to the UID
// defined in the mmp file
const TUid KUidHelpExampleApp = { HELP_UID3 };
const TUid KUidHelpFile = { HELPFILE_UID };

// CLASS DECLARATIONS

/**
* CHelpExampleApplication
* An instance of CHelpExampleApplication is the application part of the AVKON
* application framework for the HelpExample example application
*/

class CHelpExampleApplication : public CAknApplication
    {
        public:  // from CAknApplication

            /**
            * AppDllUid()
            * Return the application DLL UID value
            * @return The UID of this Application/Dll
            */
            TUid AppDllUid() const;

        protected: // from CAknApplication

            /**
            * CreateDocumentL()
            * Create a CApaDocument object and return a pointer to it
            * @return A pointer to the created document
            */
            CApaDocument* CreateDocumentL();
    };

#endif // __HELPEXAMPLE_APPLICATION_H__

// End of File

