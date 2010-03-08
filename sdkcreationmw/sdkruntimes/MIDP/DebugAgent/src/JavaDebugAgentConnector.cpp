/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <es_enum.h>
#include <ApUtils.h>
#include "TcpServerHandler.h"
#include "JavaDebugAgentLog.h"
#include "JavaDebugAgentSettings.h"
#include "JavaDebugAgentConnector.h"

const TInt KMaxInetAddrBufSize = 64;

#define LOG(text)           iDriver->Log()->Log(_S(text))
#define LOG1(text,p1)       iDriver->Log()->LogFormat(_S(text),p1)
#define LOG2(text,p1,p2)    iDriver->Log()->LogFormat(_S(text),p1,p2)

CJavaDebugAgentConnector::
CJavaDebugAgentConnector(CJavaDebugAgentDriver* aDriver, TInt aApId) :
CActive(EPriorityStandard),
iDriver(aDriver),
iApId(aApId)
{
    CActiveScheduler::Add(this);
}

CJavaDebugAgentConnector::~CJavaDebugAgentConnector()
{
    Cancel();
    iResolver.Close();
    iConnection.Close();
    delete iHandler;
}

void CJavaDebugAgentConnector::ConnectL()
{
    if (iState == EIdle)
    {
        TDesC* host = iDriver->GetPCHost();
        if (host && host->Length() > 0)
        {
            // NOTE: iApId is apparently so called "WAP id". It's different
            // from Internet AP id, which is what TCommDbConnPref wants.
            TUint32 iap = KNoAccessPoint;
            TBuf<KCommsDbSvrMaxFieldLength> apName;

            if (iApId != KNoAccessPoint)
            {
                // We will need the connection
                User::LeaveIfError(iConnection.Open(SocketServ(), KAfInet));

                // Get IAP id and connection name
                CCommsDatabase* db = CCommsDatabase::NewL();
                CleanupStack::PushL(db);
                CApUtils* apUtils = CApUtils::NewLC(*db);
                iap = apUtils->IapIdFromWapIdL(iApId);
                apUtils->NameL(iApId, apName);
                CleanupStack::PopAndDestroy(2); // apUtils, db
            }
            if (iap == KNoAccessPoint || AlreadyConnected(iap))
            {
                DoResolveL();
            }
            else
            {
                // Initiate the connection
                LOG1("Starting %S connection...",&apName);
                iState = EStarting;
                iStatus = KRequestPending;
                SetActive();

                iConnPref.SetIapId(iap);
                iConnPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
                iConnection.Start(iConnPref, iStatus);
            }
        }
        else
        {
            iState = EFailed;
            iDriver->Log(_L("PC host is not set!"));
            User::LeaveIfError(KErrArgument);
        }
    }
}

void CJavaDebugAgentConnector::DoResolveL()
{
    TDesC* host = iDriver->GetPCHost();
    TInt err = iAddress.Input(*host);
    if (err != KErrNone)
    {
        User::LeaveIfError(iResolver.Open(SocketServ(), KAfInet, 
            KProtocolInetTcp));

        iState = EResolving;
        iStatus = KRequestPending;
        SetActive();

        LOG1("Resolving %S",host);
        iResolver.GetByName(*host, iNameEntry, iStatus);
    }
    else
    {
        DoConnectL();
    }
}

void CJavaDebugAgentConnector::DoConnectL()
{
    TInt port = iDriver->GetPCPort();
    TBuf<KMaxInetAddrBufSize> addrBuf;
    iAddress.OutputWithScope(addrBuf);
    LOG2("Connecting to %S:%d", &addrBuf, port);

    iHandler = CTcpServerHandler::NewL(iDriver);
    iSocket = &iHandler->InitSocketL(SocketServ());
    User::LeaveIfError(iSocket->Open(SocketServ(), KAfInet, KSockStream,
        KProtocolInetTcp));

    iState = EConnecting;
    iStatus = KRequestPending;
    SetActive();

    iAddress.SetPort(port);
    iSocket->Connect(iAddress, iStatus);
}

TBool CJavaDebugAgentConnector::AlreadyConnected(TUint32 aIapId)
{
    TUint count = 0;
    if (iConnection.EnumerateConnections(count) == KErrNone)
    {
        TPckgBuf<TConnectionInfo> connInfo;
        for (TUint i=1; i <= count; ++i)
        {
            iConnection.GetConnectionInfo(i, connInfo);
            if (connInfo().iIapId == aIapId)
            {
                return ETrue;
            }
        }
    }
    return EFalse;
}

void CJavaDebugAgentConnector::RunL()
{
    TInt err = iStatus.Int();
    switch (iState)
    {
    case EStarting:
        if (err == KErrNone)
        {
            DoResolveL();
        }
        else
        {
            iState = EFailed;
            LOG1("Connection error %d",err);
            iDriver->ConnectionFailed();
        }
        break;

    case EResolving:
        if (err == KErrNone)
        {
            iAddress = iNameEntry().iAddr;
            DoConnectL();
        }
        else
        {
            iState = EFailed;
            LOG1("Host name resolution error %d",err);
            iDriver->ConnectionFailed();
        }
        break;

    case EConnecting:
        if (err == KErrNone)
        {
            iState = EConnected;

            // The ProcessRequest() call transfers ownership of the
            // CTcpServerHandler object to the driver in a rather
            // obscure way... Clear the  pointer so that we don't
            // deallocate it twice.
            CTcpServerHandler* handler = iHandler;
            iHandler = NULL;
            handler->ProcessRequest();
        }
        else
        {
            iState = EFailed;
            LOG1("Connection failed, error %d",err);
            iDriver->ConnectionFailed();
        }
        break;
    }
}

TInt CJavaDebugAgentConnector::RunError(TInt aError)
{
    iState = EFailed;
    LOG1("Error %d",aError);
    iDriver->ConnectionFailed();
    return KErrNone;
}

void CJavaDebugAgentConnector::DoCancel()
{
    TRequestStatus *status;
    LOG("Canceled");
    switch (iState)
    {
    case EStarting:
        // How to cancel connection? Will just wait
        break;

    case EResolving:
        iResolver.Cancel();
        break;

    case EConnecting:
        iSocket->CancelConnect();
        break;

    default:
		status = (&iStatus);
		User::RequestComplete(status, KErrCancel);
        break;
    }
}
