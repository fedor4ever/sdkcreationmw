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


#ifndef HEADER_SWI_ENGINE
#define HEADER_SWI_ENGINE

#include "AgentEngine.h"

class CSwiTask;

class CSwiEngine : public CAgentEngine
	{
public:
	CSwiEngine(CDriver* aDriver);
	virtual ~CSwiEngine();

public:
	// from CAgentEngine
	// jar and jad
	TInt InstallL(HBufC* aJad, HBufC* aJar, TBool aTrusted);
	// remove by name and vendor
	void RemoveL(HBufC* aName, HBufC* aVendor);


	TInt RunInNewThreadL(CSwiTask* aTask);
	static TInt ThreadFunction(CSwiTask* aTask);

	HBufC* FindInJadLC(HBufC* aJad, const TDesC& aProp);

private:
	static void ThreadFunctionL(CSwiTask* aTask);
	};

#endif