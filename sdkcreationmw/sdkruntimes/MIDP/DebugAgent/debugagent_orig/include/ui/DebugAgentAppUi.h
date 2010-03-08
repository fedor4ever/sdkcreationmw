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



#ifndef HEADER_DEBUG_AGENT_UI_APPUI
#define HEADER_DEBUG_AGENT_UI_APPUI

#include <eikappui.h>

#include "ui/DebugAgent.hrh"

class CDebugAgentView;

/**
Application UI class.
Creates view.
Created database.
Deals with menu commands.
*/
class CDebugAgentAppUi : public CEikAppUi
    {
	public:
		void ConstructL();
		virtual ~CDebugAgentAppUi();
		
	private: 
		//from CEikAppUi
		void HandleCommandL(TInt aCommand);

	private:
		void ClearScreen();

	private:
		CDebugAgentView*		iDebugAgentView;
	};


#endif