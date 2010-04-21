/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#define TRACE_PREFIX "WSOCK: "
#include "wsock.h"
#include <utf.h>
#include <etelpckt.h>
#include <ApEngineConsts.h>
#include <CNetworkControllerBase.h>
#include "WinsockAgentFactory.h"
#include "WinsockProtocolFamily.h"
#include "WinsockInterfaceFactory.h"
#include "WinsockUtils.h"

_LIT(KProxyTableName,"Proxies");
static const TInt KDefaultProxyPort = 8080;

enum TProxyUpdateFlags
{
    TProxyUpdateUseProxy                = 0x00000001,
    TProxyUpdateServerName              = 0x00000002,
    TProxyUpdateProtocol                = 0x00000004,
    TProxyUpdatePort                    = 0x00000008,
    TProxyUpdateName                    = 0x00000010
};

enum TBearerUpdateFlags
{
    TBearerUpdateAgent                  = 0x00000001,
    TBearerUpdateSocketActivityTimeout  = 0x00000002,
    TBearerUpdateSessionClosedTimeout   = 0x00000004,
    TBearerUpdateSocketClosedTimeout    = 0x00000008
};

enum TServiceUpdateFlags
{
    TServiceUpdateNetworks              = 0x00000001,
    TServiceUpdateAuthPrompt            = 0x00000002,
    TServiceUpdateAPN                   = 0x00000004,
    TServiceUpdateAPType                = 0x00000008
};

enum TIAPUpdateFlags
{
    TIAPUpdateService                   = 0x00000001,
    TIAPUpdateServiceType               = 0x00000002,
    TIAPUpdateBearer                    = 0x00000004,
    TIAPUpdateBearerType                = 0x00000008,
    TIAPUpdateNetwork                   = 0x00000010
};

enum TWapIpBearerUpdateFlags
{
    TWapIpBearerUpdateIAP               = 0x00000001,
    TWapIpBearerUpdateWspOption         = 0x00000002,
    TWapIpBearerUpdateProxyPort         = 0x00000004,
    TWapIpBearerUpdateSecurity          = 0x00000008,
    TWapIpBearerUpdateGatewayAddress    = 0x00000010,
    TWapIpBearerUpdateProxyLoginName    = 0x00000020,
    TWapIpBearerUpdateProxyLoginPass    = 0x00000040
};

#ifdef _REALLY_DEBUG
extern "C" int __stdcall IsDebuggerPresent(void);
void WsockAssert(const TText* s, const TText* file, TInt line)
{
    TRACE1("Assertion failed: %s",s);
    TRACE1("File: %s",file);
    TRACE1("Line: %d",line);

    #ifdef WSOCK_ASSERT_BREAKPOINT
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
    #endif
}
TInt WsockAssertSuccess(TInt err, const TText* s, const TText* file, TInt line)
{
    if (err != KErrNone)
    {
        TRACE1("Unexpected error: %d",err);
        TRACE1("Expression: %s",s);
        TRACE1("File: %s",file);
        TRACE1("Line: %d",line);

        #ifdef WSOCK_ASSERT_BREAKPOINT
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
        #endif
    }
    return err;
}
void WsockLeave(TInt err, const TText* s, const TText* file, TInt line)
{
    TRACE2("Leaving with error %d (%s)",err,s);
    TRACE1("File: %s",file);
    TRACE1("Line: %d",line);

    #ifdef WSOCK_ASSERT_BREAKPOINT
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
    #endif
    User::Leave(err);
}
void WsockLeaveIfError(TInt err, const TText* s, const TText* file, TInt line)
{
    if (err != KErrNone)
    {
        TRACE1("Unexpected error: %d",err);
        TRACE1("Expression: %s",s);
        TRACE1("File: %s",file);
        TRACE1("Line: %d",line);

        #ifdef WSOCK_ASSERT_BREAKPOINT
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
        #endif
        User::Leave(err);
    }
}
#endif // _REALLY_DEBUG

// WSOCK lock
class WsockLock
{
private:
    TBool iLocked;
    RMutex iMutex;
public:
    WsockLock() : iLocked(EFalse) {}
    ~WsockLock() {
        ASSERT(!iLocked);
        if (iLocked) Unlock();
    }

    // Locks the mutex, returning KErrNone on success
    TInt Lock()
    {
        _LIT(KMutexName, "WsockLock");
        ASSERT(!iLocked);
        TInt err = iMutex.CreateGlobal(KMutexName, EOwnerThread);
        while (err == KErrAlreadyExists) {
            err = iMutex.OpenGlobal(KMutexName, EOwnerThread);
            if (err == KErrNone) {
                TRACE("opened existing CommDb mutex");
            } else {
                TRACE("couldn't either create or open CommDb mutex, retry");
                err = iMutex.CreateGlobal(KMutexName, EOwnerThread);
            }
        }
        ASSERT_SUCCESS(err);
        if (err == KErrNone) {
            iMutex.Wait();
            iLocked = ETrue;
        }
        return err;
    }

    // Locks the mutex, leaves on failure
    void LockL()
    {
        LEAVE_IF_ERROR(Lock());
    }

    // Locks the mutex, leaves on failure, sets up cleanup callback
    void LockLC()
    {
        LEAVE_IF_ERROR(Lock());
        CleanupStack::PushL(TCleanupItem(UnlockOnLeave,this));
    }

    // Releases the mutex
    void Unlock()
    {
        ASSERT(iLocked);
        if (iLocked) {
            iLocked = EFalse;
            iMutex.Signal();
            iMutex.Close();
        }
    }

    // Cleanup callback. Unlocks WsockLock
    static void UnlockOnLeave(TAny* aPtr)
    {
        WsockLock* lock = (WsockLock*)aPtr;
        lock->Unlock();
    }
};

// Cleanup callback. Rolls back the transaction
static void WsockRollbackOnLeave(TAny* aDb)
{
    CCommsDatabase* db = (CCommsDatabase*)aDb;
    db->RollbackTransaction();
}

// Begins CommDb transaction
static void WsockBeginCommDbTransactionLC(CCommsDatabase* aDb)
{
    TInt err = aDb->BeginTransaction();
    TInt retriesLeft = KRetryCount;
    while ((err == KErrLocked || err == KErrAccessDenied) && --retriesLeft)
    {
        User::After(KRetryWait);
        err = aDb->BeginTransaction();
    }
    LEAVE_IF_ERROR(err); // Leaves if error
    CleanupStack::PushL(TCleanupItem(WsockRollbackOnLeave,aDb));
}

// Checks proxy settings in CommDb. Returns ETrue if transaction needs
// to be commited. If aProxyHost is NULL, does not check the proxy values,
// only makes sure that there is on and only one proxy record in CommDb.
// If we allow more than one record, Symbian HTTP framework gets confused
// what to use and more often than not uses the wrong one. Particularly,
// it appears to prefer proxy records associated with OutgoingGPRS service,
// ignoring LANService.
static TBool WsockCheckProxyL(CCommsDatabase* aDb,
                              TUint32 aServiceId,
                              TBool aUseProxy,
                              const TDesC* aProxyHost,
                              TUint aProxyPort)
{
    _LIT(KProxyProtocol,"http");
    _LIT(KProxyName, "Winsock Proxy Settings");
    const TDesC& TableName = KProxyTableName();
    const TDesC& ServiceType = KWinsockServiceType();
    TRACE1("examining %S table...",&TableName);
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    // Column names
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);
    TPtrC ISPColumn(PROXY_ISP);
    TPtrC UseProxyColumn(PROXY_USE_PROXY_SERVER);
    TPtrC ServiceTypeColumn(PROXY_SERVICE_TYPE);
    TPtrC ServerNameColumn(PROXY_SERVER_NAME);
    TPtrC ProtocolColumn(PROXY_PROTOCOL_NAME);
    TPtrC ProxyPortColumn(PROXY_PORT_NUMBER);

    // Delete everything except our record
    TInt err;
    TUint32 id = 0;
    TBool recordFound = EFalse;
    TBool databaseUpdated  = EFalse;
    if (view->GotoFirstRecord() == KErrNone)
    {
        TUint32 number = 0;
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TRAP(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                // We detect our record based on the values of "ISP"
                // and "ProxyServiceType" columns
                number = 0;
                TRAP(err, view->ReadUintL(ISPColumn,number));
                TRAP(err, view->ReadTextL(ServiceTypeColumn,text));
                TRACE4("  %S #%d -> %d,%S",&TableName,id,number,&text);
                if (err == KErrNone &&
                    text == ServiceType &&
                    number == aServiceId &&
                    !recordFound)
                {
                    TUint updateFlags = 0;
                    if (aProxyHost)
                    {
                        TBool useProxy = EFalse;
                        TRAP(err, view->ReadBoolL(UseProxyColumn,useProxy));
                        if (err != KErrNone || useProxy != aUseProxy)
                        {
                            updateFlags |= TProxyUpdateUseProxy;
                        }

                        HBufC* val = NULL;
                        TRAP(err, val = view->ReadLongTextLC(ServerNameColumn);
                        CleanupStack::Pop(val));
                        if (err != KErrNone || *val != *aProxyHost)
                        {
                            updateFlags |= TProxyUpdateServerName;
                        }
                        delete val;

                        TRAP(err, view->ReadUintL(ProxyPortColumn,number));
                        if (err != KErrNone || number != aProxyPort)
                        {
                            updateFlags |= TProxyUpdatePort;
                        }
                    }

                    TRAP(err, view->ReadTextL(NameColumn,text));
                    if (err != KErrNone || text != KProxyName())
                    {
                        updateFlags |= TProxyUpdateName;
                    }

                    TRAP(err, view->ReadTextL(ProtocolColumn,text));
                    if (err != KErrNone || text != KProxyProtocol())
                    {
                        updateFlags |= TProxyUpdateProtocol;
                    }

                    if (updateFlags)
                    {
                        TRACE3("updating %S #%d (flags: %08X)",&TableName,id,updateFlags);
                        LEAVE_IF_ERROR(view->UpdateRecord());

                        if (updateFlags & TProxyUpdateUseProxy)
                        {
                            TRAP(err,view->WriteBoolL(UseProxyColumn,aUseProxy));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TProxyUpdateServerName)
                        {
                            TRAP(err,view->WriteLongTextL(ServerNameColumn,*aProxyHost));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TProxyUpdateName)
                        {
                            TRAP(err,view->WriteTextL(NameColumn,KProxyName()));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TProxyUpdateProtocol)
                        {
                            TRAP(err,view->WriteTextL(ProtocolColumn,KProxyProtocol()));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TProxyUpdatePort)
                        {
                            TRAP(err,view->WriteUintL(ProxyPortColumn,aProxyPort));
                            ASSERT_SUCCESS(err);
                        }

                        LEAVE_IF_ERROR(view->PutRecordChanges());
                        databaseUpdated = ETrue;
                    }

                    recordFound = ETrue;
                    continue;
                }
            }
            TRACE4("  deleting proxy record: %S #%d -> %d,%S",&TableName,id,number,&text);
            VERIFY_SUCCESS(view->DeleteRecord());
            databaseUpdated = ETrue;
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Create a new entry if it doesn't exist
    if (!recordFound)
    {
        LEAVE_IF_ERROR(view->InsertRecord(id));

        const TDesC& host = aProxyHost ? (*aProxyHost) : KNullDesC();
        TRACE2("created %S #%d",&TableName,id);
        TRAP(err,view->WriteUintL(ISPColumn,aServiceId));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteTextL(ServiceTypeColumn,ServiceType));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteBoolL(UseProxyColumn, aUseProxy));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteTextL(NameColumn, KProxyName));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteTextL(ProtocolColumn, KProxyProtocol));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteUintL(ProxyPortColumn, aProxyPort));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteLongTextL(ServerNameColumn, host));
        ASSERT_SUCCESS(err);
        LEAVE_IF_ERROR(view->PutRecordChanges());
        databaseUpdated = ETrue;
    }

    CleanupStack::PopAndDestroy(view);
    return databaseUpdated;
}

// Shortcut for making sure that there's no garbage in the proxy settings table
static TBool WsockCheckProxyL(CCommsDatabase* aDb, TUint32 aServiceId)
{
    return WsockCheckProxyL(aDb, aServiceId, EFalse, NULL, KDefaultProxyPort);
}

// Makes sure our IAP is in the Network table
static TBool WsockCheckNetworkL(CCommsDatabase* aDb, TUint32& aNetworkId)
{
    _LIT(KNetworkTable,"Network");
    const TDesC& TableName = KNetworkTable();
    TRACE1("examining %S table...",&TableName);
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    // Column names
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);

    // Walk the table
    TInt err;
    TUint32 id = 0;
    TBool found = EFalse;
    TBool needCommit = EFalse;
    if (view->GotoFirstRecord() == KErrNone)
    {
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TRAPD(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                TRAP(err, view->ReadTextL(NameColumn,text));
                TRACE3("  %S #%d -> %S",&TableName,id,&text);
                if (err == KErrNone && text == KWinsockAPName)
                {
                    if (!found)
                    {
                        found = ETrue;
                    }
                    else
                    {
                        TRACE1("  deleting record #%d",id);
                        LEAVE_IF_ERROR(view->DeleteRecord());
                        needCommit = ETrue;
                    }
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Create a new entry if it doesn't exist
    if (!found)
    {
        LEAVE_IF_ERROR(view->InsertRecord(id));
        TRACE2("created %S #%d",&TableName,id);
        TRAP(err,view->WriteTextL(NameColumn, KWinsockAPName));
        ASSERT_SUCCESS(err);
        LEAVE_IF_ERROR(view->PutRecordChanges());
        needCommit = ETrue;
    }

    aNetworkId = id;
    CleanupStack::PopAndDestroy(view);
    return needCommit;
}

// Finds our records in CommDb, creating one if necessary. Returns ETrue
// if transaction needs to be commited.
static TBool WsockCheckBearerTypeL(CCommsDatabase* aDb, TUint32& aBearerId)
{
    const TDesC& TableName = KWinsockBearerType();
    TRACE1("examining %S table...",&TableName);
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    TBuf<KCommsDbSvrMaxFieldLength> agent;
    agent.Copy(KWinsockAgent);
    agent.Append(_L(".agt"));

    // Column names
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);
    TPtrC IfNameColumn(IF_NAME);
    TPtrC AgentColumn(AGENT_NAME);
    TPtrC SocketActivityTimeoutColumn(LAST_SOCKET_ACTIVITY_TIMEOUT);
    TPtrC SessionClosedTimeoutColumn(LAST_SESSION_CLOSED_TIMEOUT);
    TPtrC SocketClosedTimeoutColumn(LAST_SOCKET_CLOSED_TIMEOUT);

    // Walk the table
    TInt err;
    TBool needCommit = EFalse;
    if (view->GotoFirstRecord() == KErrNone)
    {
        TUint32 number = 0;
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TUint32 id = 0;
            TRAPD(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                // We detect our record based on the value of the "IfName"
                // column, it must point to our interface.
                TRAP(err, view->ReadTextL(IfNameColumn,text));
                TRACE3("  %S #%d -> %S",&TableName,id,&text);
                if (err == KErrNone && text == KWinsockInterface)
                {
                    aBearerId = id;
                    TUint updateFlags = 0;
                    TRAP(err, view->ReadTextL(AgentColumn,text));
                    if (err != KErrNone || text != agent)
                    {
                       updateFlags |= TBearerUpdateAgent;
                    }
                    TRAP(err, view->ReadUintL(SocketActivityTimeoutColumn,number));
                    if (err != KErrNone || number != KWinsockSocketActivityTimeout)
                    {
                       updateFlags |= TBearerUpdateSocketActivityTimeout;
                    }
                    TRAP(err, view->ReadUintL(SessionClosedTimeoutColumn,number));
                    if (err != KErrNone || number != KWinsockSessionClosedTimeout)
                    {
                       updateFlags |= TBearerUpdateSessionClosedTimeout;
                    }
                    TRAP(err, view->ReadUintL(SocketClosedTimeoutColumn,number));
                    if (err != KErrNone || number != KWinsockSocketClosedTimeout)
                    {
                       updateFlags |= TBearerUpdateSocketClosedTimeout;
                    }

                    if (updateFlags)
                    {
                        needCommit = ETrue;
                        TRACE3("updating %S #%d (flags: %08X)",&TableName,id,updateFlags);
                        LEAVE_IF_ERROR(view->UpdateRecord());

                        if (updateFlags & TBearerUpdateAgent)
                        {
                            TRAP(err,view->WriteTextL(AgentColumn,agent));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TBearerUpdateSocketActivityTimeout)
                        {
                            TRAP(err,view->WriteUintL(SocketActivityTimeoutColumn,
                                KWinsockSocketActivityTimeout));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TBearerUpdateSessionClosedTimeout)
                        {
                            TRAP(err,view->WriteUintL(SessionClosedTimeoutColumn,
                                KWinsockSessionClosedTimeout));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TBearerUpdateSocketClosedTimeout)
                        {
                            TRAP(err,view->WriteUintL(SocketClosedTimeoutColumn,
                                KWinsockSocketClosedTimeout));
                            ASSERT_SUCCESS(err);
                        }

                        LEAVE_IF_ERROR(view->PutRecordChanges());
                    }

                    // Done
                    CleanupStack::PopAndDestroy(view);
                    return needCommit;
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Create a new entry
    LEAVE_IF_ERROR(view->InsertRecord(aBearerId));

    TRACE2("created %S #%d",&TableName,aBearerId);
    TRAP(err,view->WriteTextL(NameColumn,KWinsockBearerName));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(IfNameColumn,KWinsockInterface));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(AgentColumn,agent));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(SocketActivityTimeoutColumn, KWinsockSocketClosedTimeout));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(SessionClosedTimeoutColumn, KWinsockSessionClosedTimeout));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(SocketClosedTimeoutColumn, KWinsockSocketClosedTimeout));
    ASSERT_SUCCESS(err);

    // These must be present in the LANBearer table, otherwise
    // PutRecordChanges fails because these fields are mandatory
    _LIT(something,"foo");
    TRAP(err,view->WriteTextL(TPtrC(LAN_BEARER_LDD_NAME),something));
    TRAP(err,view->WriteTextL(TPtrC(LAN_BEARER_PDD_NAME),something));

    LEAVE_IF_ERROR(view->PutRecordChanges());
    CleanupStack::PopAndDestroy(view);
    return ETrue;
}

// Finds our records in CommDb, creating one if necessary. Returns ETrue
// if transaction needs to be commited.
static TBool WsockCheckServiceTypeL(CCommsDatabase* aDb, TUint32& aServiceId)
{
    const TDesC& TableName = KWinsockServiceType();
    TRACE1("examining %S table...",&TableName);
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    // We will need the list of networks and the agent ame
    TBuf<KCommsDbSvrMaxFieldLength> networks;
    CWinsockProtocolFamily::ProtocolNamesfownetwork(networks);

    // Column names
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);
    TPtrC APNColumn(GPRS_APN);
    TPtrC APTypeColumn(GPRS_AP_TYPE);
    TPtrC NetworksColumn(SERVICE_IF_NETWORKS);
    TPtrC AuthPromptColumn(ISP_IF_PROMPT_FOR_AUTH);

    // Walk the table
    TInt err;
    if (view->GotoFirstRecord() == KErrNone)
    {
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TUint32 id = 0;
            TRAPD(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                TRAP(err, view->ReadTextL(NameColumn,text));
                TRACE3("  %S #%d -> %S",&TableName,id,&text);
                if (err == KErrNone && text == KWinsockServiceName)
                {
                    // This is our record. Make sure it looks right.
                    aServiceId = id;
                    TBool authPrompt = EFalse;
                    TInt updateFlags = 0;
                    TRAP(err, view->ReadTextL(NetworksColumn,text));
                    if (err != KErrNone || text != networks)
                    {
                        updateFlags |= TServiceUpdateNetworks;
                    }
                    // The following one is not present in the LANService
                    // table. Ignore it if it's not there.
                    TRAP(err, view->ReadBoolL(AuthPromptColumn,authPrompt));
                    if (err == KErrNone && authPrompt)
                    {
                        updateFlags |= TServiceUpdateAuthPrompt;
                    }

                    // The above statement also applies to APN column
                    TBuf<KCommsDbSvrMaxFieldLength> text;
                    TRAP(err, view->ReadTextL(APNColumn,text));
                    if (err == KErrNone && text != KWinsockAPName)
                    {
                        updateFlags |= TServiceUpdateAPN;
                    }

#ifndef WSOCK_LAN_SERVICE
                    // This one is important. If this number isn't set, our
                    // access point almost (!) never shows up in the list of
                    // access points, at least if we use "OutgoingGPRS"
                    // service type (hence the #ifdef).
                    TUint32 n = 0;
                    TRAP(err, view->ReadUintL(APTypeColumn, n));
                    if (err != KErrNone || n != KWinsockAPType)
                    {
                        updateFlags |= TServiceUpdateAPType;
                    }
#endif // !WSOCK_LAN_SERVICE

                    TBool update = (updateFlags != 0);
                    if (update)
                    {
                        TRACE3("updating %S #%d (flags: %08X)",&TableName,id,updateFlags);
                        LEAVE_IF_ERROR(view->UpdateRecord());

                        if (updateFlags & TServiceUpdateNetworks)
                        {
                            TRAP(err,view->WriteTextL(NetworksColumn,networks));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TServiceUpdateAuthPrompt)
                        {
                            TRAP(err,view->WriteBoolL(AuthPromptColumn,EFalse));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TServiceUpdateAPN)
                        {
                            TRAP(err,view->WriteTextL(APNColumn,KWinsockAPName));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TServiceUpdateAPType)
                        {
                            TRAP(err,view->WriteUintL(APTypeColumn,KWinsockAPType));
                            ASSERT_SUCCESS(err);
                        }

                        LEAVE_IF_ERROR(view->PutRecordChanges());
                    }

                    // Done
                    CleanupStack::PopAndDestroy(view);
                    return update;
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);

        TRACE1("  did not find %S",&KWinsockServiceName);
    }

    // Create a new entry
    LEAVE_IF_ERROR(view->InsertRecord(aServiceId));

    TRACE2("created %S #%d",&TableName,aServiceId);
    TRAP(err,view->WriteTextL(NameColumn,KWinsockServiceName));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(NetworksColumn,networks));
    ASSERT_SUCCESS(err);

    // Ignore the errors below - LANService table does not have these entries.
    // Only OutgoingGPRS does. But we try anyway, in case if Symbian unifies
    // the formats of the service tables in CommDb.
    TRAP(err,view->WriteBoolL(AuthPromptColumn,EFalse));
    TRAP(err,view->WriteTextL(APNColumn,KWinsockAPName));
    TRAP(err,view->WriteUintL(APTypeColumn,KWinsockAPType));
    TRAP(err,view->WriteUintL(TPtrC(GPRS_PDP_TYPE),KWinsockPdpType));

    // We don't use these fields, but they are mandatory.
    TRAP(err,view->WriteBoolL(TPtrC(SERVICE_IP_ADDR_FROM_SERVER),EFalse));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteBoolL(TPtrC(SERVICE_IP_DNS_ADDR_FROM_SERVER),EFalse));
    ASSERT_SUCCESS(err);

    LEAVE_IF_ERROR(view->PutRecordChanges());
    CleanupStack::PopAndDestroy(view);
    return ETrue;
}

// Finds our record in WAPAccessPoint table, creating one if necessary.
// Returns ETrue if transaction needs to be commited.
static TBool WsockCheckWapAccessPointL(CCommsDatabase* aDb, TUint32& aWapApId)
{
    TPtrC TableName(WAP_ACCESS_POINT);
    TRACE1("examining %S table...",&TableName);
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    // Column names
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);
    TPtrC CurrentBearerColumn(WAP_CURRENT_BEARER);

    // Walk the table
    TInt err;
    TUint32 id = 0;
    TBool found = EFalse;
    TBool commit = EFalse;
    if (view->GotoFirstRecord() == KErrNone)
    {
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TRAPD(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                TRAP(err, view->ReadTextL(NameColumn,text));
                TRACE3("  %S #%d -> %S",&TableName,id,&text);
                if (err == KErrNone && text == KWinsockAPName)
                {
                    if (!found)
                    {
                        aWapApId = id;
                        found = ETrue;
                        TRAP(err, view->ReadTextL(CurrentBearerColumn,text));
                        if (err != KErrNone || text != KWinsockWapBearer)
                        {
                            TRACE1("  updating \"%S\"",&CurrentBearerColumn);
                            LEAVE_IF_ERROR(view->UpdateRecord());
                            TRAP(err,view->WriteTextL(CurrentBearerColumn,
                                                      KWinsockWapBearer));
                            ASSERT_SUCCESS(err);
                            LEAVE_IF_ERROR(view->PutRecordChanges());
                            commit = ETrue;
                        }
                    }
                    else
                    {
                        TRACE1("  deleting record #%d",id);
                        LEAVE_IF_ERROR(view->DeleteRecord());
                        commit = ETrue;
                    }
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Create a new entry if it doesn't exist
    if (!found)
    {
        LEAVE_IF_ERROR(view->InsertRecord(id));
        TRACE2("created %S #%d",&TableName,id);
        TRAP(err,view->WriteTextL(NameColumn, KWinsockAPName));
        ASSERT_SUCCESS(err);
        TRAP(err,view->WriteTextL(CurrentBearerColumn, KWinsockWapBearer));
        ASSERT_SUCCESS(err);
        LEAVE_IF_ERROR(view->PutRecordChanges());
        aWapApId = id;
        commit = ETrue;
    }

    CleanupStack::PopAndDestroy(view);
    return commit;
}

// Finds our records in WAPIPBearer table, creating one if necessary.
// Returns ETrue if transaction needs to be commited.
static TBool WsockCheckWapIpBearerL(CCommsDatabase* aDb,
                                    TUint32 aWapApId,
                                    TUint32 aIapId)
{
    const TDesC& TableName = KWinsockWapBearer();
    TRACE1("examining %S table...",&TableName);
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    // Column names
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);
    TPtrC AccessPointIdColumn(WAP_ACCESS_POINT_ID);
    TPtrC IAPColumn(WAP_IAP);
    TPtrC ProxyPortColumn(WAP_PROXY_PORT);
    TPtrC WspOptionColumn(WAP_WSP_OPTION);
    TPtrC SecurityColumn(WAP_SECURITY);
    TPtrC GatewayAddressColumn(WAP_GATEWAY_ADDRESS);
    TPtrC ProxyLoginNameColumn(WAP_PROXY_LOGIN_NAME);
    TPtrC ProxyLoginPassColumn(WAP_PROXY_LOGIN_PASS);

    // Walk the table
    TInt err;
    TUint32 id = 0;
    if (view->GotoFirstRecord() == KErrNone)
    {
        do
        {
            // This one must exist
            TRAPD(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                // We detect our record based on WAP access point id
                TUint32 wap = 0;
                TRAPD(err, view->ReadUintL(AccessPointIdColumn, wap));
                TRACE3("  %S #%d -> %d", &TableName, id, wap);
                if (err == KErrNone && wap == aWapApId)
                {
                    TUint32 n = 0;
                    TBool b = EFalse;
                    TUint updateFlags = 0;
                    TBuf<KCommsDbSvrMaxFieldLength> text;
                    TBool commit = EFalse;

                    TRAPD(err, view->ReadUintL(IAPColumn, n));
                    if (err != KErrNone || n != aIapId)
                    {
                        updateFlags |= TWapIpBearerUpdateIAP;
                    }
                    TRAP(err, view->ReadUintL(WspOptionColumn, n));
                    if (err != KErrNone)
                    {
                        updateFlags |= TWapIpBearerUpdateWspOption;
                    }
                    TRAP(err, view->ReadUintL(ProxyPortColumn, n));
                    if (err != KErrNone)
                    {
                        updateFlags |= TWapIpBearerUpdateProxyPort;
                    }
                    TRAP(err, view->ReadBoolL(SecurityColumn, b));
                    if (err != KErrNone)
                    {
                        updateFlags |= TWapIpBearerUpdateSecurity;
                    }
                    TRAP(err, view->ReadTextL(GatewayAddressColumn,text));
                    if (err != KErrNone || text != KWinsockGatewayAddress)
                    {
                        updateFlags |= TWapIpBearerUpdateGatewayAddress;
                    }
                    TRAP(err, view->ReadTextL(ProxyLoginNameColumn,text));
                    if (err != KErrNone)
                    {
                        updateFlags |= TWapIpBearerUpdateProxyLoginName;
                    }
                    TRAP(err, view->ReadTextL(ProxyLoginPassColumn,text));
                    if (err != KErrNone)
                    {
                        updateFlags |= TWapIpBearerUpdateProxyLoginPass;
                    }

                    TBool update = (updateFlags != 0);
                    if (updateFlags)
                    {
                        // Update the existing table
                        TRACE3("updating %S #%d (flags: %08X)",&TableName,id,updateFlags);
                        LEAVE_IF_ERROR(view->UpdateRecord());
                        if (updateFlags & TWapIpBearerUpdateIAP)
                        {
                            TRACE1("  updating \"%S\"",&IAPColumn);
                            TRAP(err,view->WriteUintL(IAPColumn, aIapId));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TWapIpBearerUpdateWspOption)
                        {
                            TRACE1("  updating \"%S\"",&WspOptionColumn);
                            TRAP(err,view->WriteUintL(WspOptionColumn,0));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TWapIpBearerUpdateProxyPort)
                        {
                            TRACE1("  updating \"%S\"",&ProxyPortColumn);
                            TRAP(err,view->WriteUintL(ProxyPortColumn,0));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TWapIpBearerUpdateSecurity)
                        {
                            TRACE1("  updating \"%S\"",&SecurityColumn);
                            TRAP(err,view->WriteBoolL(SecurityColumn,EFalse));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TWapIpBearerUpdateGatewayAddress)
                        {
                            TRACE1("  updating \"%S\"",&GatewayAddressColumn);
                            TRAP(err,view->WriteTextL(GatewayAddressColumn,
                                                      KWinsockGatewayAddress));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TWapIpBearerUpdateProxyLoginName)
                        {
                            TRACE1("  updating \"%S\"",&ProxyLoginNameColumn);
                            TRAP(err,view->WriteTextL(ProxyLoginNameColumn,
                                                      KNullDesC));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TWapIpBearerUpdateProxyLoginPass)
                        {
                            TRACE1("  updating \"%S\"",&ProxyLoginPassColumn);
                            TRAP(err,view->WriteTextL(ProxyLoginPassColumn,
                                                      KNullDesC));
                            ASSERT_SUCCESS(err);
                        }
                        LEAVE_IF_ERROR(view->PutRecordChanges());
                        commit = ETrue;
                    }

                    // Done
                    CleanupStack::PopAndDestroy(view);
                    return commit;
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Create a new entry
    LEAVE_IF_ERROR(view->InsertRecord(id));

    TRACE2("created %S #%d",&TableName,id);
    TRAP(err,view->WriteTextL(NameColumn,KWinsockBearerName));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(AccessPointIdColumn, aWapApId));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(IAPColumn, aIapId));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(ProxyPortColumn, 0));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(WspOptionColumn, 0));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteBoolL(SecurityColumn, EFalse));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(GatewayAddressColumn,KWinsockGatewayAddress));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(ProxyLoginNameColumn,KNullDesC));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(ProxyLoginPassColumn,KNullDesC));
    ASSERT_SUCCESS(err);

    LEAVE_IF_ERROR(view->PutRecordChanges());
    CleanupStack::PopAndDestroy(view);
    return ETrue;
}

static TBool WsockCheckConnPreferencesL(CCommsDatabase* aDb, TUint32 aIapId)
{
    TRACE("examining Connection Preferences...");
    CCommsDbConnectionPrefTableView *view = aDb->OpenConnectionPrefTableLC();

    CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref myPrefs;
    myPrefs.iBearer.iIapId      = aIapId;
    myPrefs.iDirection          = ECommDbConnectionDirectionOutgoing;
    myPrefs.iDialogPref         = ECommDbDialogPrefPrompt;
    myPrefs.iRanking            = 1;
    myPrefs.iBearer.iBearerSet  = KCommDbBearerCSD
                                | KCommDbBearerWcdma
                                | KCommDbBearerLAN
                                | KCommDbBearerVirtual;
    // Walk the table
    TInt err;
    if (view->GotoFirstRecord() == KErrNone)
    {
        do
        {
            CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref prefs;
            TRAP(err, view->ReadConnectionPreferenceL(prefs));
            if (err == KErrNone)
            {
                // check, whether this is our record
                if (prefs.iBearer.iIapId == myPrefs.iBearer.iIapId)
                {
                    TBool update = EFalse;

                    // This is our record. Make sure it looks right.
                    if (prefs.iDialogPref != myPrefs.iDialogPref)
                    {
                        TRACE("updating DialogPref");
                        view->UpdateDialogPrefL(myPrefs.iDialogPref);
                        update = ETrue;
                    }
                    if (prefs.iRanking != myPrefs.iRanking)
                    {
                        TRACE("updating Ranking");
                        view->ChangeConnectionPreferenceRankL(myPrefs.iRanking);
                        update = ETrue;
                    }
                    if (prefs.iBearer.iBearerSet != myPrefs.iBearer.iBearerSet)
                    {
                        TRACE("updating Bearer");
                        view->UpdateBearerL(myPrefs.iBearer);
                        update = ETrue;
                    }
                    CleanupStack::PopAndDestroy(view);
                    return update;
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Create a new entry
    view->InsertConnectionPreferenceL(myPrefs);
    TRACE("created connection preferences");
    CleanupStack::PopAndDestroy(view);
    return ETrue;
}

// Finds our records in CommDb, creating one if necessary.
void WsockCheckCommDbL(TUint32& aIapId,
                       TUint32& aNetworkId,
                       TUint32& aServiceId,
                       TUint32& aBearerId)
{
    TRACE("checking CommDb");
    CCommsDatabase* db = CCommsDatabase::NewL();
    CleanupStack::PushL(db);
    WsockBeginCommDbTransactionLC(db);
    TBool commit = EFalse;
    TUint32 number = 0;
    TInt err;

    // This must be 2
    TPtrC ConnectionAttempts(CONNECTION_ATTEMPTS);
    TRAP(err, db->GetGlobalSettingL(ConnectionAttempts, number));
    if (err != KErrNone || number < 2)
    {
        TRACE1("Resetting ConnectionAttempts (%d) to 2",number);
        TRAP(err, db->SetGlobalSettingL(ConnectionAttempts, 2));
        ASSERT_SUCCESS(err);
        commit = ETrue;
    }

    // Not sure if we need this check...
    _LIT(KTsy, "PHONETSY");
    TFileName tsy;
    TPtrC BearerAvailabilityCheckTsy(BEARER_AVAILABILITY_CHECK_TSY);
    TRAP(err, db->GetGlobalSettingL(BearerAvailabilityCheckTsy, tsy));
    if (err != KErrNone || tsy.CompareF(KTsy))
    {
        TRACE1("Updating BearerAvailabilityCheckTSY (%S)",&tsy);
        TRAP(err, db->SetGlobalSettingL(BearerAvailabilityCheckTsy, KTsy));
        ASSERT_SUCCESS(err);
        commit = ETrue;
    }

    // Check a few other things
    if (WsockCheckServiceTypeL(db, aServiceId)) commit = ETrue;
    if (WsockCheckBearerTypeL(db, aBearerId)) commit = ETrue;
    if (WsockCheckNetworkL(db, aNetworkId)) commit = ETrue;

    // Initialize descriptors
    TPtrC TableName(IAP);
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);
    TPtrC IAPNetworkColumn(IAP_NETWORK);
    TPtrC IAPServiceColumn(IAP_SERVICE);
    TPtrC IAPServiceTypeColumn(IAP_SERVICE_TYPE);
    TPtrC IAPBearerColumn(IAP_BEARER);
    TPtrC IAPBearerTypeColumn(IAP_BEARER_TYPE);

    // Open the IAP table
    CCommsDbTableView* view = db->OpenTableLC(TableName);

    // Find our access point
    TRACE("examining APs...");
    if (view->GotoFirstRecord() == KErrNone)
    {
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TUint32 id = 0;
            TRAP(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                TRAP(err, view->ReadTextL(NameColumn,text));
                TRACE3("  %S #%d -> %S",&TableName,id,&text);
                if (err == KErrNone && text == KWinsockAPName)
                {
                    aIapId = id;
                    TUint updateFlags = 0;
                    TRAP(err, view->ReadUintL(IAPServiceColumn,number));
                    if (err != KErrNone || number != aServiceId)
                    {
                       updateFlags |= TIAPUpdateService;
                    }
                    TRAP(err, view->ReadTextL(IAPServiceTypeColumn,text));
                    if (err == KErrNone && text != KWinsockServiceType)
                    {
                        updateFlags |= TIAPUpdateServiceType;
                    }
                    TRAP(err, view->ReadUintL(IAPBearerColumn,number));
                    if (err != KErrNone || number != aBearerId)
                    {
                       updateFlags |= TIAPUpdateBearer;
                    }
                    TRAP(err, view->ReadTextL(IAPBearerTypeColumn,text));
                    if (err == KErrNone && text != KWinsockBearerType)
                    {
                        updateFlags |= TIAPUpdateBearerType;
                    }
                    TRAP(err, view->ReadUintL(IAPNetworkColumn,number));
                    if (err != KErrNone || number != aNetworkId)
                    {
                       updateFlags |= TIAPUpdateNetwork;
                    }

                    if (updateFlags)
                    {
                        // Update the existing table
                        TRACE2("updating AP \"%S\" (flags: %08X)",&KWinsockAPName(), updateFlags);
                        LEAVE_IF_ERROR(view->UpdateRecord());

                        if (updateFlags & TIAPUpdateService)
                        {
                            TRACE2("  updating \"%S\": %u",&IAPServiceColumn, aServiceId);
                            TRAP(err,view->WriteUintL(IAPServiceColumn,aServiceId));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TIAPUpdateServiceType)
                        {
                            TRACE2("  updating \"%S\": %S",&IAPServiceTypeColumn, &KWinsockServiceType);
                            TRAP(err,view->WriteTextL(IAPServiceTypeColumn,KWinsockServiceType));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TIAPUpdateBearer)
                        {
                            TRACE2("  updating \"%S\": %u",&IAPServiceTypeColumn, aBearerId);
                            TRAP(err,view->WriteUintL(IAPBearerColumn,aBearerId));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TIAPUpdateBearerType)
                        {
                            TRACE2("  updating \"%S\": %S",&IAPBearerTypeColumn, &KWinsockBearerType);
                            TRAP(err,view->WriteTextL(IAPBearerTypeColumn,KWinsockBearerType));
                            ASSERT_SUCCESS(err);
                        }
                        if (updateFlags & TIAPUpdateNetwork)
                        {
                            TRACE2("  updating \"%S\": %u",&IAPNetworkColumn, aNetworkId);
                            TRAP(err,view->WriteUintL(IAPNetworkColumn,aNetworkId));
                            ASSERT_SUCCESS(err);
                        }

                        LEAVE_IF_ERROR(view->PutRecordChanges());
                        commit = ETrue;
                    }

                    TUint32 wap = 0;
                    if (WsockCheckWapAccessPointL(db,wap)) commit = ETrue;
                    if (WsockCheckWapIpBearerL(db,wap,aIapId)) commit = ETrue;
                    if (WsockCheckConnPreferencesL(db, aIapId)) commit = ETrue;
                    if (commit)
                    {
                        TRACE("commit CommDb transaction");
                        LEAVE_IF_ERROR(db->CommitTransaction());
                    }

                    // Done
                    CleanupStack::PopAndDestroy(view);
                    CleanupStack::Pop(); // WsockRollbackOnLeave
                    CleanupStack::PopAndDestroy(db);

                    TRACE4("selected AP=%d, network=%d, service=%d, bearer=%d",
                        aIapId,aNetworkId,aServiceId,aBearerId);
                    return;
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    // Find any valid location id
    TUint32 locationId = 1;
    CCommsDbTableView* locationView = db->OpenTableLC(TPtrC(LOCATION));
    if (locationView->GotoFirstRecord() == KErrNone)
    {
        TRAPD(err, locationView->ReadUintL(IdColumn, locationId));
    }
    CleanupStack::PopAndDestroy(locationView);

    // Create new IAP entry
    err = view->InsertRecord(aIapId);
    ASSERT_SUCCESS(err);

    TRACE1("created AP #%d",aIapId);
    TRAP(err,view->WriteTextL(NameColumn,KWinsockAPName));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(IAPServiceColumn,aServiceId));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(IAPServiceTypeColumn,KWinsockServiceType));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(IAPBearerColumn,aBearerId));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteTextL(IAPBearerTypeColumn,KWinsockBearerType));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(IAPNetworkColumn,aNetworkId));
    ASSERT_SUCCESS(err);

    // These must be present in the LANBearer table, otherwise
    // PutRecordChanges fails because these fields are mandatory
    _LIT(something,"foo");
    TRAP(err,view->WriteUintL(TPtrC(IAP_NETWORK_WEIGHTING),0));
    ASSERT_SUCCESS(err);
    TRAP(err,view->WriteUintL(TPtrC(IAP_LOCATION),locationId));
    ASSERT_SUCCESS(err);

    LEAVE_IF_ERROR(view->PutRecordChanges());
    CleanupStack::PopAndDestroy(view);
    CleanupStack::Pop(); // WsockRollbackOnLeave

    TUint32 wap = 0;
    WsockCheckWapAccessPointL(db, wap);
    WsockCheckWapIpBearerL(db, wap, aIapId);
    WsockCheckConnPreferencesL(db, aIapId);

    TRACE("commit CommDb transaction");
    LEAVE_IF_ERROR(db->CommitTransaction());
    CleanupStack::PopAndDestroy(db);
    TRACE4("selected AP=%d, network=%d, service=%d, bearer=%d",
        aIapId,aNetworkId,aServiceId,aBearerId);
}

// Creating necessary entries in CommDB if necessary.
static TInt WsockCheckCommDb()
{
    TUint32 apId, apNetwork, apService, apBearer;

    // Make sure Winsock is initialized
    static TBool initWinsock = ETrue;
    if (initWinsock) {
        WSADATA wsaData;
        TRACE("initializing Winsock");
        WSAStartup(MAKEWORD(1,1),&wsaData);
        initWinsock = EFalse;
    }

    WsockLock lock;
    lock.Lock();
    TRAPD(err, WsockCheckCommDbL(apId, apNetwork, apService, apBearer));
    lock.Unlock();

    if (err == KErrNone)
    {
        TRACE("CommDB check successful");
    }
    else
    {
        TRACE1("CommDB check failed, err %d",err);
    }
    return err;
}

// Deletes Wsock access points
static TBool WsockDeleteApsL(CCommsDatabase* aDb)
{
    TBool needCommit = EFalse;

    // Initialize descriptors
    TPtrC TableName(IAP);
    TPtrC IdColumn(COMMDB_ID);
    TPtrC NameColumn(COMMDB_NAME);

    // Open the IAP table
    CCommsDbTableView* view = aDb->OpenTableLC(TableName);

    // Find our access points
    TRACE("looking for APs to delete...");
    if (view->GotoFirstRecord() == KErrNone)
    {
        TUint32 number = 0;
        TBuf<KCommsDbSvrMaxFieldLength> text;
        do
        {
            // This one must exist
            TUint32 id = 0;
            TRAPD(err, view->ReadUintL(IdColumn, id));
            if (err == KErrNone)
            {
                TRAP(err, view->ReadTextL(NameColumn,text));
                TRACE3("  %S #%d -> %S",&TableName,id,&text);
                if (err == KErrNone && text == KWinsockAPName)
                {
                    TRACE1("deleting AP #%d",id);
                    LEAVE_IF_ERROR(view->DeleteRecord());
                    needCommit = ETrue;
                }
            }
        }
        while (view->GotoNextRecord() == KErrNone);
    }

    CleanupStack::PopAndDestroy(view);
    return needCommit;
}

// Configures wsock settings, leaves on failure
static void WsockSetConfigL(TBool aUseWsock, TBool aUseProxy,
                            const TDesC* aHost, TUint aPort)
{
    // Synchronize access to the database
    WsockLock lock;
    lock.LockLC();

    // Figure out the service id - we will need it
    CCommsDatabase* db = CCommsDatabase::NewL();
    CleanupStack::PushL(db);
    WsockBeginCommDbTransactionLC(db);
    TUint32 serviceId;
    TBool commit = WsockCheckServiceTypeL(db, serviceId);

    // Now that we know the service id, we can update the proxy settings
    if (WsockCheckProxyL(db,serviceId,aUseProxy,aHost,aPort)) commit = ETrue;
    if (!aUseWsock && WsockDeleteApsL(db)) commit = ETrue;
    if (commit)
    {
        TRACE("commit CommDb transaction");
        LEAVE_IF_ERROR(db->CommitTransaction());
    }
    CleanupStack::Pop(); // WsockRollbackOnLeave
    CleanupStack::PopAndDestroy(db);

    if (aUseWsock)
    {
        // Final touch
        TUint32 apId, networkId, bearerId;
        TRAPD(err, WsockCheckCommDbL(apId, networkId, serviceId, bearerId));
    }

    CleanupStack::PopAndDestroy(); // WsockLock::UnlockOnLeave
}

// Reads Wsock settings, leaves on failure
static void WsockQueryConfigL(TBool& aUseProxy, HBufC*& aHost, TUint& aPort)
{
    // Default values
    aUseProxy = EFalse;
    aHost = NULL;
    aPort = KDefaultProxyPort;

    // Synchronize access to the database
    WsockLock lock;
    lock.LockLC();

    // Figure out the service id - we will need it
    CCommsDatabase* db = CCommsDatabase::NewL();
    CleanupStack::PushL(db);
    WsockBeginCommDbTransactionLC(db);
    TUint32 serviceId;
    TBool commit = EFalse;
    if (WsockCheckServiceTypeL(db, serviceId)) commit = ETrue;
    if (WsockCheckProxyL(db, serviceId)) commit = ETrue;

    // At this point there should be only one record in the proxy table
    const TDesC& TableName = KProxyTableName();
    const TDesC& ServiceType = KWinsockServiceType();
    CCommsDbTableView* view = db->OpenTableLC(TableName);

    // Column names
    TPtrC UseProxyColumn(PROXY_USE_PROXY_SERVER);
    TPtrC ProxyPortColumn(PROXY_PORT_NUMBER);
    TPtrC ServerNameColumn(PROXY_SERVER_NAME);

    // Walk the table and deleted everything except our settings
    TInt status = KErrNotFound;
    if (view->GotoFirstRecord() == KErrNone)
    {
        status = KErrNone;
        TUint32 number = KDefaultProxyPort;
        view->ReadBoolL(UseProxyColumn,aUseProxy);
        view->ReadUintL(ProxyPortColumn, number);
        aPort = number;
        aHost = view->ReadLongTextLC(ServerNameColumn);
        CleanupStack::Pop(aHost);
    }

    if (commit)
    {
        TRACE("commit CommDb transaction");
        LEAVE_IF_ERROR(db->CommitTransaction());
    }

    CleanupStack::PopAndDestroy(view);
    CleanupStack::Pop();            // WsockRollbackOnLeave
    CleanupStack::PopAndDestroy(db);
    CleanupStack::PopAndDestroy();  // WsockLock::UnlockOnLeave

    LEAVE_IF_ERROR(status);
}

// Entry point #1
extern "C"EXPORT_C CProtocolFamilyBase * WsockCreateProtocolFamily()
{
    TRACE("WsockCreateProtocolFamily");
    VERIFY_SUCCESS(WsockCheckCommDb());
    return new CWinsockProtocolFamily(KAfInet);
}

// Entry point #2
extern "C" EXPORT_C CNifAgentFactory* WsockCreateAgentFactory()
{
    TRACE("WsockCreateAgentFactory");
    return new CWinsockAgentFactory;
}

// Entry point #3
extern "C" EXPORT_C CNifIfFactory* WsockCreateInterfaceFactory()
{
    TRACE("WsockCreateInterfaceFactory");
    return new CWinsockInterfaceFactory;
}

// Entry point #4
extern "C" EXPORT_C TInt WsockQueryConfiguration(TBool& aUseProxy,
                                                 HBufC*& aProxyHost,
                                                 TUint& aProxyPort)
{
    TRACE("WsockQueryProxy");
    TRAPD(err,WsockQueryConfigL(aUseProxy,aProxyHost,aProxyPort));
    if (err == KErrNone)
    {
        TRACE3("WsockQueryConfig: %d,%S:%d",aUseProxy,aProxyHost,aProxyPort);
    }
    else
    {
        TRACE1("configuration query failed, err %d",err);
    }
    return err;
}

// Entry point #5
extern "C" EXPORT_C TInt WsockSetConfiguration(TBool aUseWsock,
                                               TBool aUseProxy,
                                               const TDesC* aProxyHost,
                                               TUint aProxyPort)
{
    TRACE4("WsockSetConfig(%d,%d,%S:%d)",aUseWsock,aUseProxy,
        aProxyHost,aProxyPort);
    TRAPD(err,WsockSetConfigL(aUseWsock, aUseProxy, aProxyHost, aProxyPort));
    if (err == KErrNone)
    {
        TRACE("configuration updated ok");
    }
    else
    {
        TRACE1("configuration update failed, err %d",err);
    }
    return err;
}

// Entry point #6
extern "C"EXPORT_C CProtocolFamilyBase * WsockCreateProtocolFamily6()
{
    TRACE("WsockCreateProtocolFamily6");
    VERIFY_SUCCESS(WsockCheckCommDb());
    return new CWinsockProtocolFamily(KAfInet6);
}

#ifndef EKA2
// Required function
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
{
    return(KErrNone);
}
#endif // !EKA2

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
