// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology
 @prototype
*/


#ifndef WINSOCK_AGENTMCPR_H
#define WINSOCK_AGENTMCPR_H

#include <comms-infras/coremcpr.h>
#include <comms-infras/nifagt.h>
#include <comms-infras/agentmessages.h>
#include <metadatabase.h>		// for TMDBElementId
//#include "agentmcpr.h"
#include <comms-infras/agentmcpr.h>
#include <comms-infras/ss_commsdataobject.h>
#include <comms-infras/ss_nodemessages_legacy.h>
#include <comms-infras/ss_nodemessages_legacy_internal.h>
#include <comms-infras/ss_legacyinterfaces.h>

class TIp6Addr;
class CAgentNotificationHandler;
class CAvailabilitySubscriber;
class CAgentQueryConnSettingsImpl;
class CAgentProvisionInfo;

namespace ESock
	{
	class CCommsDatIapView;
	}


/*class CWinsockMetaConnectionProvider : public CCoreMetaConnectionProvider, private ESock::MLinkMCprLegacyDataAccessApiExt,
    public ITFHIERARCHY_LINK_2(CWinsockMetaConnectionProvider, CCoreMetaConnectionProvider,
		ESock::MQueryConnSettingsApiExt,
    	ESock::MLinkMCprLegacyDataAccessApiExt)*/
class CWinsockMetaConnectionProvider : public CAgentMetaConnectionProvider
	{
//public:
	/*typedef ITFHIERARCHY_LINK_2(CWinsockMetaConnectionProvider, CCoreMetaConnectionProvider,
		ESock::MQueryConnSettingsApiExt,
    	ESock::MLinkMCprLegacyDataAccessApiExt) TIfStaticFetcherNearestInHierarchy;*/
    //typedef CWinsockMetaConnectionProviderFactory FactoryType;
public:
    static CWinsockMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
	IMPORT_C ~CWinsockMetaConnectionProvider();

	void ReturnInterfacePtrL(ESock::MLinkMCprLegacyDataAccessApiExt*& aInterface);
	void ReturnInterfacePtrL(ESock::MQueryConnSettingsApiExt*& aInterface);
	
	// Methods for retrieving and decoding CommsDat fields containing IPv4/v6 addresses.
	//
	// These methods could have been in CCommsDatIapView, but that would have meant ESockSvr
	// linking against insock.lib (IP specific and takes up RAM, even if ESockSvr isn't used
	// for IP at all).  If CCommsDatIapView is moved out of ESockSvr, then these
	// methods should be moved into CCommsDatIapView.
	IMPORT_C static void GetIp4AddrL(ESock::CCommsDatIapView* aIapView, CommsDat::TMDBElementId aElementId, TUint32& aAddr);
	IMPORT_C static TInt GetIp4Addr(ESock::CCommsDatIapView* aIapView, CommsDat::TMDBElementId aElementId, TUint32& aAddr);
	IMPORT_C static void GetIp6AddrL(ESock::CCommsDatIapView* aIapView, CommsDat::TMDBElementId aElementId, TIp6Addr& aAddr);
	IMPORT_C static TInt GetIp6Addr(ESock::CCommsDatIapView* aIapView, CommsDat::TMDBElementId aElementId, TIp6Addr& aAddr);

	
	// From MLinkCprLegacyDataAccessExtApi Interface
	
#if 0
	IMPORT_C virtual void GetIntSettingL(const TDesC& aSettingName, TUint32& aValue, ESock::MPlatsecApiExt* aPlatsecItf);
	IMPORT_C virtual void GetBoolSettingL(const TDesC& aSettingName, TBool& aValue, ESock::MPlatsecApiExt* aPlatsecItf);
	IMPORT_C virtual void GetDes8SettingL(const TDesC& aSettingName, TDes8& aValue, ESock::MPlatsecApiExt* aPlatsecItf);
	IMPORT_C virtual void GetDes16SettingL(const TDesC& aSettingName, TDes16& aValue, ESock::MPlatsecApiExt* aPlatsecItf);
	IMPORT_C virtual void GetLongDesSettingL(const TDesC& aSettingName, HBufC*& aValue, ESock::MPlatsecApiExt* aPlatsecItf);
#endif 
	// Utility method
	TMDBElementId MapFieldNameL(const TDesC& aSettingName, ESock::MPlatsecApiExt* aPlatsecItf);
    TMDBElementId SecondChanceModemBearerLegacyMapFieldNameL(const TDesC& aSettingName, ESock::MPlatsecApiExt* aPlatsecItf);

protected:
	const CAgentProvisionInfo* AgentProvisionInfo();

	IMPORT_C static void CleanupCloseIapView(TAny* aThis);

	 CWinsockMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);

	IMPORT_C ESock::CCommsDatIapView* OpenIapViewL();
	IMPORT_C ESock::CCommsDatIapView* OpenIapViewLC();
	IMPORT_C void CloseIapView();
	IMPORT_C ESock::CCommsDatIapView* IapView();
	void DestroyIapView();

	IMPORT_C void SetAgentNotificationHandlerL(CAgentNotificationHandler* aNotificationHandler);
    IMPORT_C void ConstructL();

    // Messages::ANode Interface - To be implemented by the deriving MCPr
    // virtual TInt ReceivedL(Messages::TSignatureBase& aCFMessage);

	IMPORT_C void StartAvailabilityMonitoringL(const Messages::TNodeCtxId& aAvailabilityActivity);
	IMPORT_C void CancelAvailabilityMonitoring();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
    void ProvisionAgentInfoL ();
    CCredentialsConfig* CreateCredentialsInfoL();

private:
    ESock::CCommsDatIapView* iIapView;
    TBool iAgentMCPrDoneWithIapView:1;
    TInt32 iIapViewRefCount;

public:
#ifdef _DEBUG
	CAvailabilitySubscriber* iAvailabilitySubscriber;
#else
	TAny* iReserved; //Place holder to avoid BC between udeb/urel
#endif

	
protected:
    CAgentQueryConnSettingsImpl* iQueryConnSettingsImpl;
	};


#endif
// SYMBIAN_AGENTMCPR_H

