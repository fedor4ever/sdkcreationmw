/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ui/DebugAgentApplication.h"
#include "ui/DebugAgentDocument.h"

#include <eikstart.h>

const TUid KUidDebugAgentApp = { 0x10202065 }; 


TUid CDebugAgentApplication::AppDllUid() const 
    {
    return KUidDebugAgentApp;
    }

CApaDocument* CDebugAgentApplication::CreateDocumentL() 
    {
    return CDebugAgentDocument::NewL(*this);
    }



LOCAL_C CApaApplication* NewApplication()
    {
    return new CDebugAgentApplication;
    }

GLDEF_C TInt E32Main()
    {
	TInt rc = EikStart::RunApplication(NewApplication);
    return rc;
    }



