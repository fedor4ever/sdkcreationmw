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



#include "ui/Dialogs.h"
#include <techview/eikbutb.h>
#include <techview/eikchkbx.h>

#include <DebugAgent.rsg>
#include "ui/DebugAgent.hrh"

// CAgentDialog
CAgentDialog::CAgentDialog(CDriver* aDriver) :
	iDriver(aDriver)
	{
	}

void CAgentDialog::PreLayoutDynInitL()
	{
	TRAPD(e,iDriver->LoadPrefsL());
	// autoclean
	CEikCheckBox* chkbox = STATIC_CAST(CEikCheckBox*,Control(EStartupAutoClean));
	TBool isauto = iDriver->IsAutoCleanEnabled();
	CEikButtonBase::TState state;
	if ( isauto )
		{
		state = CEikButtonBase::ESet;
		}
	else 
		{
		state = CEikButtonBase::EClear;
		}
	chkbox->SetState(state);
	
	// filelog
	CEikCheckBox* filelogcb = STATIC_CAST(CEikCheckBox*,Control(EDebugAgentFileLog));
	TBool isfilelog = iDriver->IsFileLog();
	CEikButtonBase::TState flstate;
	if ( isfilelog )
		{
		flstate = CEikButtonBase::ESet;
		}
	else 
		{
		flstate = CEikButtonBase::EClear;
		}
	filelogcb->SetState(state);

	// pin
	CEikCheckBox* pincb = STATIC_CAST(CEikCheckBox*,Control(EDebugAgentUsePin));
	TBool ispin = iDriver->IsUsePin();
	CEikButtonBase::TState pinstate;
	if ( ispin )
		{
		pinstate = CEikButtonBase::ESet;
		}
	else 
		{
		pinstate = CEikButtonBase::EClear;
		}
	pincb->SetState(pinstate);

	// local port
	CEikEdwin* edwin = STATIC_CAST(CEikEdwin*,Control(EDebugAgentPort));
	HBufC* buf = HBufC::NewLC(10);
	TPtr bptr =buf->Des();
	bptr.AppendNum(iDriver->GetAgentPort());
	edwin->SetTextL(buf);
	CleanupStack::PopAndDestroy(1);

	// PC port
	edwin = STATIC_CAST(CEikEdwin*,Control(EPCPort));
	buf = HBufC::NewLC(10);
	bptr = buf->Des();
	bptr.AppendNum(iDriver->GetPCPort());
	edwin->SetTextL(buf);
	CleanupStack::PopAndDestroy(1);

	// PC port
	edwin = STATIC_CAST(CEikEdwin*,Control(EPCHost));
	buf = HBufC::NewLC(15);
	bptr = buf->Des();
	bptr.Append(iDriver->GetPCHost()->Des());
	edwin->SetTextL(buf);
	CleanupStack::PopAndDestroy(1);
	}

TBool CAgentDialog::OkToExitL(TInt /*aKeycode*/)
	{
	// autoclean
	CEikCheckBox* chkbox = STATIC_CAST(CEikCheckBox*,Control(EStartupAutoClean));
	CEikButtonBase::TState state = chkbox->State();
	if ( state == CEikButtonBase::ESet )
		{
		iDriver->SetAutoCleanEnabled(ETrue);
		}
	else
		{
		iDriver->SetAutoCleanEnabled(EFalse);
		}
		
	// filelog
	CEikCheckBox* filelogcb = STATIC_CAST(CEikCheckBox*,Control(EDebugAgentFileLog));
	CEikButtonBase::TState flstate = filelogcb->State();
	if ( flstate == CEikButtonBase::ESet )
		{
		iDriver->SetFileLog(ETrue);
		}
	else
		{
		iDriver->SetFileLog(EFalse);
		}
		
	// autoclean
	CEikCheckBox* pincb = STATIC_CAST(CEikCheckBox*,Control(EDebugAgentUsePin));
	CEikButtonBase::TState pinstate = pincb->State();
	if ( pinstate == CEikButtonBase::ESet )
		{
		iDriver->SetUsePin(ETrue);
		}
	else
		{
		iDriver->SetUsePin(EFalse);
		}
		
	// local port
	CEikEdwin* edwin = STATIC_CAST(CEikEdwin*,Control(EDebugAgentPort));
	
	HBufC* hport = edwin->GetTextInHBufL();
	CleanupStack::PushL(hport);
	
	TInt port;
	TLex lex(*hport);
	TInt err = lex.Val(port);

	CleanupStack::PopAndDestroy(hport);

	if ( err != KErrNone || port<1024 || port > 65500)
		{
		return EFalse;
		}

	iDriver->SetAgentPort(port);

	// PC port
	edwin = STATIC_CAST(CEikEdwin*,Control(EPCPort));
	hport = edwin->GetTextInHBufL();
	CleanupStack::PushL(hport);
	TLex lex2(*hport);
	err = lex2.Val(port);
	CleanupStack::PopAndDestroy(hport);
	if ( err != KErrNone || port<1024 || port > 65500)
		{
		return EFalse;
		}
	iDriver->SetPCPort(port);
	
	// PC host
	edwin = STATIC_CAST(CEikEdwin*,Control(EPCHost));
	hport = edwin->GetTextInHBufL();
	iDriver->SetPCHostL(hport->Des());

	TRAPD(e,iDriver->SavePrefsL());
	return ETrue;
	}


