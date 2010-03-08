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



#ifndef __GRAPHICS_APPLICATION_H__
#define __GRAPHICS_APPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// UID for the application; this should correspond
// to the uid defined in the mmp file
const TUid KUidGraphicsApp = { 0xA00001F8 };

// CLASS DECLARATION

/**
* CGraphicsApplication application class.
* An instance of CGraphicsApplication is the application part of the AVKON
* application framework for the CGraphicsApplication example application.
*/
class CGraphicsApplication : public CAknApplication
    {
    public: // from CAknApplication
        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID.
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes
        /**
        * From CApaApplication, CreateDocumentL.
        * Creates GraphicsApplication document object.
        * The returned pointer in not owned by the GraphicsApplication
        * object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __GRAPHICS_APPLICATION_H__

// End of File
