/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __FOCUSEVENT_APPLICATION_H__
#define __FOCUSEVENT_APPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID for the application; this should match the uid defined in the mmp file.
const TUid KUidFocusEventApp = { 0xE0000234 };

// CLASS DECLARATION

/**
* CFocusEventApplication application class.
* An instance of CFocusEventApplication is the application part of the AVKON
* application framework for the CFocusEventApplication example application.
*/
class CFocusEventApplication : public CAknApplication
    {
    public:  // from CAknApplication

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID.
        */
        TUid AppDllUid() const;

    protected:   // New functions

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CFocusEventApplication document object.
        * The returned pointer in not owned by the CFocusEventApplication
        * object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __FOCUSEVENT_APPLICATION_H__

// End of File
