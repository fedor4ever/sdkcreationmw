/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements CEikApplication and is used in the 
*                launching of the application
*
*/


#ifndef LBSREFERENCEAPP_H
#define LBSREFERENCEAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// UID of the application
const TUid KUidLBSReference = { 0xA0000189 };

// CLASS DECLARATION

/**
* CLbsReferenceApp application class.
* Provides factory to create concrete document object.
*
*/
class CLbsReferenceApp : public CAknApplication
    {
    private:

        /**
        * From CApaApplication, returns application's UID (KUidLBSReference).
        * @return The value of KUidLBSReference.
        */
        TUid AppDllUid() const;

        /**
        * From CApaApplication, creates CLBSReferenceDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif //LBSREFERENCEAPP_H

// End of File

