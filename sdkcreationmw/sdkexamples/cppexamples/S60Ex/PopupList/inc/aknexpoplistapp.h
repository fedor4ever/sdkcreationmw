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


#ifndef __AKNEXPOPLISTAPP_H__
#define __AKNEXPOPLISTAPP_H__


// INCLUDES
#include <AknApp.h>


// CONSTANTS
// UID of the application 
const TUid KUidAknExPopList = { 0xA0000231 };


// CLASS DECLARATION
/**
* CAknExPopListApp application class.
* Provides factory to create concrete document object.
* 
*/
class CAknExPopListApp : public CAknApplication
    {
    
    private: // From CApaApplication

        /**
        * CreateDocumentL.
        * Creates CAknExNoteDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        

        /**
        * AppDllUid
        * Returns application's UID (KUidAknExPopList).
        * @return The value of KUidAknExPopList.
        */
        TUid AppDllUid() const;
    };

#endif // __AKNEXPOPLISTAPP_H__

// End of File

