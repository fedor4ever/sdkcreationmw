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


#ifndef OcrExampleAPP_H
#define OcrExampleAPP_H

// INCLUDES
#include <aknapp.h>
#include "OcrExampleUids.h"
// CONSTANTS

// CLASS DECLARATION

/**
* COcrExampleApp application class.
* Provides factory to create concrete document object.
*
*/
class COcrExampleApp : public CAknApplication
    {

    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates COcrExampleDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, returns application's UID (KUidOcrExample).
        * @return The value of KUidOcrExample.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File