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


#ifndef HEADER_TCP_SERVER_HANDLER
#define HEADER_TCP_SERVER_HANDLER

#include <e32base.h>
#include <es_sock.h>

#include "SocketReader.h"
#include "SeiCommands.h"
class CDriver;
	
class CTcpServerHandler : public CActive
	{
public:

	enum TStage
		{
		EInitial = 0,
		ECreating = 1,
		EReading = 2,
		EProcessing = 3,
		EWriting = 4,
		EComplete = 5,
		EErrored = 6
		};

	static CTcpServerHandler* NewL(CDriver* aDriver);
	~CTcpServerHandler();

	RSocket& InitSocketL(RSocketServ& aServer);
	RSocket* Socket();
	void ProcessRequest();

	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();


	// auth
	TBool IsAuthenticated();
	void SetAuthenticated(TBool aAuthenticated);
	
private:
	// construction
	CTcpServerHandler(CDriver* aDriver);
	void ConstructL();

	// RunL stages
	void CreateCommandL(void);
	void ReadCommandL(void);
	void ProcessCommandL(void);
	void WriteResponseL(void);

    void ProcessError(TInt aError, const TDesC& aDesc);
	void Reschedule();

private:
	TInt            iState;
	TInt            iErroredState;
	TInt            iProcessingStatus;
	RSocket         iSocket;
	CSocketReader*  iReader;
	CSocketWriter*  iWriter;
	CSeiCommand*    iCommand;
	TInt            iCommandCode;
	TPtr8			iCommandPtr;
    CDriver*        iDriver;
	TBool 			iAuthenticated;
	};

#endif