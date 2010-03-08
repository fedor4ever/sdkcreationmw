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


#ifndef HEADER_DEBUG_AGENT_UI_DOCUMENT
#define HEADER_DEBUG_AGENT_UI_DOCUMENT

#include <eikdoc.h>


/**
Document class.
Creates App UI.
*/
class CDebugAgentDocument : public CEikDocument
    {

	public:
		static CDebugAgentDocument* NewL(CEikApplication& aApp);
		~CDebugAgentDocument();

	private:
		CDebugAgentDocument(CEikApplication& aApp);
		void ConstructL();
		CEikAppUi*  CreateAppUiL();

    };

#endif