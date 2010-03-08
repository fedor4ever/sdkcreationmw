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


#ifndef HEADER_TCP_SERVER
#define HEADER_TCP_SERVER

#include <e32base.h>
#include <es_sock.h>

#include "Driver.h"
#include "TcpServerHandler.h"

class CTcpServer : public CActive
	{
public:

	// construct/destruct
	static CTcpServer* NewLC(CDriver* aDriver);

	void StartListeningL(
		const TDesC& aProtocol, 
		const TSockAddr& aAddr, 
		TInt aQLength);

	~CTcpServer();

	CDriver* Driver() { return iDriver; }


	void RunL();
	void DoCancel();

private:
	void ConstructL();
	void AcceptNextConnectionL();

	CTcpServer(CDriver* aDriver);

	TSockAddr			iListenAddr;
	RSocket				iListener;
    RSocketServ			iServer;
	CTcpServerHandler*	iHandler;
	CDriver*			iDriver;
	};

#endif