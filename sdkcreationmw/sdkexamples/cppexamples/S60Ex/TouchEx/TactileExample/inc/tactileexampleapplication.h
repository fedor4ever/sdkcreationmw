/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the main application class for the Tactile Example
*
*/


#ifndef __TACTILEEXAMPLEAPPLICATION_H__
#define __TACTILEEXAMPLEAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
 * CTactileExampleApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CTactileExampleApplication is the application part of the
 * AVKON application framework for the TactileExample example application.
 */
class CTactileExampleApplication : public CAknApplication
    {
// from base class CApaApplication
public:

    /**
     * From CApaApplication.
     * @return Application's UID (KUidTactileExampleApp).
     */
    TUid AppDllUid() const;


// from base class CApaApplication
protected:

    /**
     * From CApaApplication.
     * Creates CTactileExampleDocument document object. The returned
     * pointer in not owned by the CTactileExampleApplication object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
    };

#endif // __TACTILEEXAMPLEAPPLICATION_H__

// End of File

