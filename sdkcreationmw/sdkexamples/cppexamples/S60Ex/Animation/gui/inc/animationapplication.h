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


#ifndef __ANIMATIONAPPLICATION_H__
#define __ANIMATIONAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID for the application; this should match the uid defined in the mmp file.
static const TUid KUidAnimationApp = { 0xE000022D };

// CLASS DECLARATION

/**
* CAnimationApplication application class.
* An instance of CAnimationApplication is the application part of the AVKON
* application framework for the Animation example application.
*/
class CAnimationApplication : public CAknApplication
    {
    public: // from CAknApplication

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID.
        */
        TUid AppDllUid() const;

    protected:   // New functions

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CAnimationApplication document object.
        * The returned pointer in not owned by the CAnimationApplication
        * object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __ANIMATIONAPPLICATION_H__

// End of File
