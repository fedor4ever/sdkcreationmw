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


#ifndef HEADER_UI_SEI_CONNECTOR
#define HEADER_UI_SEI_CONNECTOR


#include <in_sock.h>
#include <e32base.h>
#include <es_sock.h>

class CDriver;
class CTcpServerHandler;
	
class CSEIConnector : public CActive
	{
public:

	enum TStage
		{
		EGetByName = 0,
		ESocketConnect = 1,
		EProcessRequest = 2,
		EErrored = 3
		};

	static CSEIConnector* NewL(CDriver* aDriver);
	~CSEIConnector();

	void MakeOutgoingConnectionL(const TDesC& aHost, TInt aPort);

	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	// construction
	CSEIConnector(CDriver* aDriver);
	void ConstructL();

	// RunL stages
	void GetByNameL(const TDesC& aHost, TInt aPort);
	void ConnectSocketL(void);
	void ProcessRequestL();		
	void Reschedule();

private:
	TInt            iState;
	RSocket         iSocket;
	RSocketServ	iSocketServer;
	CDriver*        iDriver;
	RHostResolver	iResolver;
	TInetAddr	iAddress;
	TNameEntry iEntry;
	TUint16*	iHost;
	TInt iPort;
	CTcpServerHandler* iHandler;
	};

#endif

