/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares main application class
*
*/




#ifndef MYVIEWAPP_H
#define MYVIEWAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application

const TUid KUidMyView = { 0xE1EF0018  };


// CLASS DECLARATION

/**
* CMyViewApp application class.
* Provides factory to create concrete document object.
* 
*/
class CMyViewApp : public CAknApplication
    {
    
    private:

        /**
        * From CApaApplication, creates CMyViewDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidMyView).
        * @return The value of KUidMyView.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

