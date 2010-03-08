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


#include <aknapp.h>
#include <akndoc.h>
#include <eikstart.h>
#include "JavaDebugAgentAppUi.h"
#include "JavaDebugAgentUid.h"

// Classes required by the application framework

class CJavaDebugAgentApp : public CAknApplication
{
public:
    TUid AppDllUid() const;
protected:
    CApaDocument* CreateDocumentL();
};

class CJavaDebugAgentDoc : public CAknDocument
{
public:
    CJavaDebugAgentDoc(CEikApplication& aApp);
    CEikAppUi* CreateAppUiL();
};

// Implementation

TUid CJavaDebugAgentApp::AppDllUid() const
{
    return KUidJavaDebugAgentApp;
}

CApaDocument* CJavaDebugAgentApp::CreateDocumentL()
{
    return new(ELeave)CJavaDebugAgentDoc(*this);
}

CJavaDebugAgentDoc::CJavaDebugAgentDoc(CEikApplication& aApp) : 
CAknDocument(aApp)
{
}

CEikAppUi* CJavaDebugAgentDoc::CreateAppUiL()
{
    return new(ELeave)CJavaDebugAgentAppUi;
}

// Startup

LOCAL_C CApaApplication* NewJavaDebugAgentApp()
{
    return new CJavaDebugAgentApp;
}

GLDEF_C TInt E32Main()
{
    return EikStart::RunApplication(NewJavaDebugAgentApp);
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
