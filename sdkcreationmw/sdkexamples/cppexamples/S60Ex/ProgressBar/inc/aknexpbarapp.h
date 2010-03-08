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



#ifndef __AKNEXPBARAPP_H__
#define __AKNEXPBARAPP_H__


// INCLUDES
#include <aknapp.h>


// CONSTANTS
// UID of the application
const TUid KUidAknExPbar = { 0xA00001F9 };

// CLASS DECLARATION

/**
* CAknExPbarApp application class.
* Provides factory to create concrete document object.
* 
*/
class CAknExPbarApp : public CAknApplication
    {
    private:  // From CApaApplication

        /**
        * CreateDocumentL.
        * Creates CAknExNoteDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * AppDllUid
        * Returns application's UID (KUidAknExPbar).
        * @return The value of KUidAknExPbar.
        */
        TUid AppDllUid() const;
    };

#endif // __AKNEXPBARAPP_H__

// End of File

