/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the main application class for the Haptics Example
*
*/


#ifndef C_HAPTICSEXAMPLEAPPLICATION_H
#define C_HAPTICSEXAMPLEAPPLICATION_H

#include <aknapp.h>

/**
 * CHapticsExampleApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CHapticsExampleApplication is the application part of the
 * AVKON application framework for the Haptics Example application.
 */
class CHapticsExampleApplication : public CAknApplication
    {
public:

    /**
     * From CApaApplication.
     * @return Application's UID (KUidHapticsExampleApp).
     */
    TUid AppDllUid() const;


// from base class CApaApplication
protected:

    /**
     * From CApaApplication.
     * Creates CHapticsExampleDocument document object. The returned
     * pointer in not owned by the CHapticsExampleApplication object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
    };

#endif // C_HAPTICSEXAMPLEAPPLICATION_H

// End of File

