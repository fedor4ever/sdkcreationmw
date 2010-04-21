/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#define TRACE_PREFIX "WSOCK: Protocol: "
#include "wsock.h"
#include "WinsockProtocol.h"
#include "WinsockProtocolFamily.h"
#include "WinsockHostResolver.h"
#include "WinsockServProvider.h"
#include "WinsockUtils.h"

CWinsockProtocol::CWinsockProtocol(const TWinProtocolDesc* aProtocolDesc) :
iProtocolDesc(aProtocolDesc)
{
    TRACE3("[%08X] created %s %S protocol",this,
        (iProtocolDesc->iAddrFamily == KAfInet6) ? _S("IPv6") : _S("IPv4"),
        &iProtocolDesc->iName);
}

CWinsockProtocol::~CWinsockProtocol()
{
    iReadPending.Reset();
    if (iIpHlpApi)
    {
        BEGIN_WIN32();
        FreeLibrary((HMODULE)iIpHlpApi);
        END_WIN32();
    }
    TRACE2("[%08X] deleted %S protocol",this,&iProtocolDesc->iName);
}

TBool CWinsockProtocol::IsStreamProtocol() const
{
    return (iProtocolDesc->iSockType == KSockStream);
}

TBool CWinsockProtocol::IsDatagramProtocol() const
{
    return (iProtocolDesc->iSockType == KSockDatagram);
}

void CWinsockProtocol::DataSent(TUint aBytes)
{
    if (iWinsockInterface)
    {
        iWinsockInterface->DataSent(aBytes);
    }
    else if (iIfNotify)
    {
        iIfNotify->NotifyDataSent(KNifEntireConnectionSubConnectionId,aBytes);
    }
}

void CWinsockProtocol::DataReceived(TUint aBytes)
{
    if (iWinsockInterface)
    {
        iWinsockInterface->DataReceived(aBytes);
    }
    else if (iIfNotify)
    {
        iIfNotify->NotifyDataReceived(KNifEntireConnectionSubConnectionId,aBytes);
    }
}

WinsockIpAdapterInfo* CWinsockProtocol::GetAdapterInfo()
{
    if (!iIpHlpApi && iIpHlpApi != INVALID_HANDLE_VALUE)
    {
        BEGIN_WIN32();
        iIpHlpApi = LoadLibraryA("IPHLPAPI.DLL");
        if (iIpHlpApi)
        {
            iGetAdapterInfo = (GetAdaptersInfoProc)GetProcAddress((HMODULE)
                              iIpHlpApi, "GetAdaptersInfo");
            if (iGetAdapterInfo)
            {
                END_WIN32();
            }
            else
            {
                FreeLibrary((HMODULE)iIpHlpApi);
                END_WIN32();
                iIpHlpApi = INVALID_HANDLE_VALUE;
                TRACE("failed to find GetAdaptersInfo in IPHLPAPI.DLL");
            }
        }
        else
        {
            END_WIN32();
            iIpHlpApi = INVALID_HANDLE_VALUE;
            TRACE1("failed to load IPHLPAPI.DLL, err %d",GetLastError());
        }
    }

    WinsockIpAdapterInfo* info = NULL;
    if (iGetAdapterInfo)
    {
        TUint len = 0;
        BEGIN_WIN32();
        TUint err = iGetAdapterInfo(NULL, &len);
        END_WIN32();
        if (err == ERROR_BUFFER_OVERFLOW && len > 0)
        {
            info = (WinsockIpAdapterInfo*)User::Alloc(len);
            if (info)
            {
                BEGIN_WIN32();
                err = iGetAdapterInfo(info, &len);
                END_WIN32();
                if (err != ERROR_SUCCESS)
                {
                    TRACE1("GetAdapterInfo err %d",err);
                    User::Free(info);
                    info = NULL;
                }
            }
        }
    }
    return info;
}

// Notifies CWinsockProtocol that there are unread data in a particular
// socket.
void CWinsockProtocol::SetReadPending(CWinsockServProvider* aSocket, 
                                      TBool aPending)
{
    TInt index = iReadPending.FindInAddressOrder(aSocket);
    if (aPending)
    {
        if (index < 0)
        {
            iReadPending.InsertInAddressOrder(aSocket);
            TRACE2("[%08X] %d socket(s) have unread data",this,
                iReadPending.Count());
        }
    }
    else
    {
        if (index >= 0)
        {
            iReadPending.Remove(index);
            TRACE2("[%08X] %d socket(s) have unread data",this,
                iReadPending.Count());
        }
    }
}

// Checks if there are any unread data in any socket associated with this
// protocol.
TBool CWinsockProtocol::ReadPending() const
{
    return (iReadPending.Count() > 0);
}

// CProtocolBase
CServProviderBase* CWinsockProtocol::NewSAPL(TUint DEBUG_ONLY(aSockType))
{
    TRACE3("[%08X] NewSAPL(%d) for %S",this,aSockType,&iProtocolDesc->iName);
    return CWinsockServProvider::NewL(this);
}

CHostResolvProvdBase* CWinsockProtocol::NewHostResolverL()
{
    return CWinsockHostResolver::NewL(iProtocolDesc);
}

void CWinsockProtocol::Identify(TServerProtocolDesc* aProtocolDesc) const
{
    *aProtocolDesc = *iProtocolDesc;
}

TInt CWinsockProtocol::GetOption(TUint aLevel, TUint aName, TDes8& aOption, 
                                 CProtocolBase* aSourceProtocol)
{
    if(aLevel == KNifOptLevel)
    {
        if(aName == KNifOptGetNifIfUser)
        {
            TNifIfUser ifuser;
            ifuser() = this;
            aOption.Copy(ifuser);
            return KErrNone;
        }
    }
    return CProtocolBase::GetOption(aLevel, aName, aOption, aSourceProtocol);
}

// MNifIfUser
void CWinsockProtocol::IfUserBindFailure(TInt /*aResult*/, TAny* /*aId*/)
{
    TRACE1("[%08X] IfUserBindFailure",this);
}

void CWinsockProtocol::IfUserNewInterfaceL(CNifIfBase* aIf, TAny* /*aId*/)
{
    TRACE1("[%08X] IfUserNewInterfaceL",this);
    ASSERT(aIf && aIf->Notify());
    if (aIf) 
    {
        iWinsockInterface = NULL;
        iIfNotify = aIf->Notify();
        CWinsockInterface* mayBeWinsockInterface = (CWinsockInterface*)aIf;
        if (mayBeWinsockInterface->Signature() == KWinsockInterfaceSignature)
        {
            iWinsockInterface = mayBeWinsockInterface;
        }
    }
}

void CWinsockProtocol::IfUserInterfaceDown(TInt /*aResult*/, 
                                           CNifIfBase* DEBUG_ONLY(aIf))
{
    TRACE1("[%08X] IfUserInterfaceDown",this);
    ASSERT(aIf && iIfNotify && (aIf->Notify() == iIfNotify));
    iIfNotify = NULL;
    iWinsockInterface = NULL;
}

void CWinsockProtocol::IfUserOpenNetworkLayer()
{
    TRACE1("[%08X] IfUserOpenNetworkLayer",this);
    Open();
}

void CWinsockProtocol::IfUserCloseNetworkLayer()
{
    TRACE1("[%08X] IfUserCloseNetworkLayer",this);
    Close();
}

CProtocolBase* CWinsockProtocol::IfUserProtocol()
{
    return this;
}

TBool CWinsockProtocol::IfUserIsNetworkLayerActive()
{
    TRACE1("[%08X] IfUserIsNetworkLayerActive",this);
    return ETrue;
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
