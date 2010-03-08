/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ui/DebugAgentDocument.h"
#include "ui/DebugAgentAppUi.h"

#include <eikenv.h>
#include <eikproc.h>

CDebugAgentDocument* CDebugAgentDocument::NewL(CEikApplication& aApp)
	{
	CDebugAgentDocument* self=new (ELeave) CDebugAgentDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CDebugAgentDocument::CDebugAgentDocument(CEikApplication& aApp)
: CEikDocument(aApp)
    {}

void CDebugAgentDocument::ConstructL()
	{}

CDebugAgentDocument::~CDebugAgentDocument()
    {}

CEikAppUi* CDebugAgentDocument::CreateAppUiL() 
    {
    return new (ELeave) CDebugAgentAppUi;
    }


