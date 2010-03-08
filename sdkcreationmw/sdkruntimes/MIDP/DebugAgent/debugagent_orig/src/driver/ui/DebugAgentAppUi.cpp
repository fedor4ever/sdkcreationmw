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


#include <eikenv.h>
#include <eikproc.h>
#include <eikfnlab.h>
#include <eiktbar.h>
#include <eikapp.h>
#include <eikdialg.h>
#include <eikcfdlg.h>
#include <eikon.rsg>
#include <coeutils.h>
#include <datastor.h>
#include <eikmfne.h>
#include <eikmenup.h>

#include "ui/DebugAgentAppUi.h"
#include "ui/DebugAgentDocument.h"
#include "ui/DebugAgentView.h"

#include "ui/Dialogs.h"

#include <eikon.rsg> // needed for file browse dialog
#include <DebugAgent.rsg>

void CDebugAgentAppUi::ConstructL() 
    {
	BaseConstructL();
	iDebugAgentView = CDebugAgentView::NewL(ClientRect(),*this);
	AddToStackL(iDebugAgentView,ECoeStackPriorityDefault,ECoeStackFlagStandard);	
	}

CDebugAgentAppUi::~CDebugAgentAppUi() 
    {
    RemoveFromStack(iDebugAgentView);
	delete iDebugAgentView;
	}

void CDebugAgentAppUi::HandleCommandL(TInt aCommand) 
    {	
	switch(aCommand)
		{
		case EEikCmdExit:
			iDebugAgentView->Shutdown();
			Exit();
			break;

		case EDebugAgentClearScreen:
			ClearScreen();
			break;			
		
		case EDebugAgentStartServer:
			iDebugAgentView->StartL();
			break;

		case EDebugAgentConnect:
			iDebugAgentView->MakeOutgoingConnectionL();
			break;

		case EDebugAgentStopServer:
			iDebugAgentView->Shutdown();
			break;
			
		case EDebugAgentSetAgent:
			CAgentDialog* dialog1 = new (ELeave) CAgentDialog(iDebugAgentView->Driver());
			dialog1->ExecuteLD(R_DEBUGAGENT_AGENT_SETTINGS_DIALOG);
			break;
			
		}
	}


void CDebugAgentAppUi::ClearScreen()
	{
	iDebugAgentView->Clear();
	}




