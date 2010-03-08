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


#ifndef MSGSVRENG_H
#define MSGSVRENG_H

#include <e32std.h>
#include <e32base.h>
#include <in_sock.h>

const TUint KSvrSocketMaxConnections = 1;
const TUint KSvrPort = 8181;


class CMessage;
class MMsgCommandObserver;
class CMsgCommandConnection;
class CMsgSvrEng : public CActive
    {
    public:
        CMsgSvrEng();
        virtual ~CMsgSvrEng();
        void ConstructL(MMsgCommandObserver* aNotify);
        static CMsgSvrEng* NewL(MMsgCommandObserver* aNotify);
        static CMsgSvrEng* NewLC(MMsgCommandObserver* aNotify);
    public:
        TInt StartL();
    private:
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        void WaitForConnection();
        void DestroyConnectionL();
        void CloseConnection();
        void ShutDownServer();
    public:
        enum TServerState
	        {
		    ENotStarted,
            EWaitingForConnections,
            EClosingConnection,
            EShuttingDownServer,
            EErrorRecovery
	        };
    private:
        MMsgCommandObserver* iNotify;
        CMsgCommandConnection* iConnection;
        RConnection iRConnection;
        RSocket		iSocket;
        RSocket* 	iCommandSocket;
	    RSocketServ iSocketServ;
        TServerState iServerState;
    private:
        friend class CMsgCommandConnection;
        friend class CMsgScheduler;
    };
#endif // MSGSVRENG_H