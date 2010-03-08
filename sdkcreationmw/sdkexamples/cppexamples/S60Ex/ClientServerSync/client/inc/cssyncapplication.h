/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CSSYNC_APPLICATION_H__
#define __CSSYNC_APPLICATION_H__

// INCLUDE FILES
#include <aknapp.h>

// CONSTANTS
// Uid for this application, this should match the one in the mmp file
const TUid KUidCSSyncApp = { 0xA00001FB };

// CLASS DECLARATION
/**
* CCSSyncApplication
*  An instance of CCSSyncApplication is the application part of the AVKON
*  application framework for the CSSync example application.
*/
class CCSSyncApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CAknApplication, AppDllUid.
        * Returns the application DLL UID value.
        * @return the UID of this Application/Dll.
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CAknApplication, CreateDocumentL.
        * Creates a CApaDocument object and return a pointer to it.
        * @return A pointer to the created document.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __CSSYNC_APPLICATION_H__


// End of File
