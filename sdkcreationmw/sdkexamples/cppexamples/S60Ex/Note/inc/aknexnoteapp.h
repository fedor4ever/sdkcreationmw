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


#ifndef __AKNEXNOTEAPP_H__
#define __AKNEXNOTEAPP_H__

// INCLUDES
#include <Aknapp.h>


// CONSTANTS
// UID of the application
const TUid KUidAknExNote = { 0xE0000230 }; 


// CLASS DECLARATION

/**
* CAknExNoteApp application class.
* Provides factory to create concrete document object.
*/
class CAknExNoteApp : public CAknApplication
    {
    private: // From CApaApplication

        /**
        * CreateDocumentL
        * Creates CAknExNoteDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * AppDllUid
        * Returns application's UID (KUidAknExNote).
        * @return The value of KUidAknExNote.
        */
        TUid AppDllUid() const;

    };

#endif  //__AKNEXNOTEAPP_H__

// End of File
