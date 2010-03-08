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


#ifndef HEADER_UI_DIALOGS
#define HEADER_UI_DIALOGS

#include "Driver.h"
#include <techview/eikdialg.h>

class CAgentDialog : public CEikDialog
	{
public:
	CAgentDialog(CDriver* aDriver);
private:	
	void       PreLayoutDynInitL();
	TBool      OkToExitL(TInt aKeycode);
private:
	CDriver* iDriver;
	};


#endif