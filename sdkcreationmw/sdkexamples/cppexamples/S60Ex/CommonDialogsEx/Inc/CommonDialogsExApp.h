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
* Description:  Declares main application class.
*
*/


#ifndef CCOMMONDIALOGSEXAPP_H
#define CCOMMONDIALOGSEXAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidDLLTest = { 0xA000027B };

// CLASS DECLARATION

/**
* CCommonDialogsExApp application class.
* Provides factory to create concrete document object.
* 
*/
class CCommonDialogsExApp : public CAknApplication
    {
    
    private:

        /**
        * From CApaApplication, creates CDLLTestDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidDLLTest).
        * @return The value of KUidDLLTest.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

