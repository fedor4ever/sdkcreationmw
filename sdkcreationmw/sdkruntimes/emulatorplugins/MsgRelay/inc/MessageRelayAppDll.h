/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

class CFolderObserver;
class CMessageWriter;
class CMessageRelay;

class CMessageRelayAppDll : public CBase
{

public:
	virtual ~CMessageRelayAppDll();
	static CMessageRelayAppDll* NewL();
	
private:
	CMessageRelayAppDll();
	void ConstructL();

private:
	CFolderObserver		*iFolderObserver;
	CMessageWriter		*iMessageWriter;
	CMessageRelay		*iMessageRelay;
};
