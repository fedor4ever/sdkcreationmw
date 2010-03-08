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



#ifndef __CCEAPP_H__
#define __CCEAPP_H__

// INCLUDES
#include <aknapp.h>
#include "CLFExampleUID.hrh"
#include "CCEDocument.h"

// CONSTANTS
const TUid KUidCE = { KCEUID3 };

/**
*  CLF Example Application.
*/
class CCEApp : public CAknApplication
    {
    private:    // Functions from base classes

        /**
        * From CEikApplication. Creates the Document.
        */
        inline CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication. Get Uid of the application.
        */
        inline TUid AppDllUid() const;

    };

CApaDocument* CCEApp::CreateDocumentL()
    {
    return( CCEDocument::NewL( *this ) );
    }

TUid CCEApp::AppDllUid() const
    {
    return KUidCE;
    }


#endif // __CCEAPP_H__

// End of File
