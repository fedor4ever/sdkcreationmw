/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header file declaring the RawIP Provisioning structures.
* 
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef WINSOCKAGENTHANDLER_H_
#define WINSOCKAGENTHANDLER_H_

#include <comms-infras/agentmessages.h>
#include <comms-infras/linkmessages.h>
#include <in_sock.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif

struct TWinsockInfo;

NONSHARABLE_CLASS(CWinsockAgentHandler) : public CAgentNotificationHandler
	{
public:
    CWinsockAgentHandler(const TWinsockInfo& aTunnelInfo);

private:
	virtual void ServiceStarted();
	virtual TInt NotificationFromAgent(TAgentToFlowEventType aEvent, TAny* aInfo);

private:
	// Pointer to Tunnel provisioning information in AccessPointConfig.  Should remain valid
	// whilst TunnelMCPr instance is valid - TunnelMCPr has a longer lifetime that this
	// Tunnel Agent Handler class (which TunnelMCPr created/deletes).
	const TWinsockInfo& iTunnelInfo;
	};

class TSigTunnelAddressParams : public Messages::TSignatureBase
/**
Parameters that communicate the IP Address that the Tunnel CFProtocol should use.

iIsUpdate is EFalse when the address is setup for the first time.  Address changes are
signified by iIsUpdate being set to ETrue.  This replaces the old notifications
EAgentToNifEventTypeSetAddress and EAgentToNifEventTypeUpdateAddress in the old
Networking architecture.
*/
	{
protected:
  	inline TSigTunnelAddressParams() {}
	explicit TSigTunnelAddressParams(const Messages::TNodeSignal::TMessageId& aMessageId,
			TBool aIsUpdate, TInetAddr aAddress, TInetAddr aNameSer1, TInetAddr aNameSer2)
		: Messages::TSignatureBase(aMessageId), iIsUpdate(aIsUpdate), iAddress(aAddress),
		iNameSer1(aNameSer1), iNameSer2(aNameSer2)
	    {}

public:
	DECLARE_MVIP_CTR( TSigTunnelAddressParams )
	DATA_VTABLE

	TInetAddr iAddress;
	TInetAddr iNameSer1;
	TInetAddr iNameSer2;
	TBool	iIsUpdate;

	};

template<TInt id, TInt32 realm>
struct TWinsockAddressMessage : public TSigTunnelAddressParams, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
/**
Template class specialising TCFUmtsIfMessageSig against the operation subid.

@internalComponent
*/
    {
	explicit TWinsockAddressMessage(
			TBool aIsUpdate, TInetAddr aAddress, TInetAddr aNameSer1, TInetAddr aNameSer2)
	    :TSigTunnelAddressParams(Messages::TNodeSignal::TMessageId(id, realm),
	    aIsUpdate, aAddress, aNameSer1, aNameSer2)
	    {}
    };

class TWinsockAgentMessage
    {
public:
    enum { ERealmId = 0x10281DF6 };

private:
    enum
    	{
    	ETunnelSetAddress  = Messages::KNullMessageId + 1,
    	};
public:
    typedef TWinsockAddressMessage<ETunnelSetAddress, TWinsockAgentMessage::ERealmId> TTunnelSetAddress;

    static void RegisterL();
    static void DeRegister();
	};


#endif

