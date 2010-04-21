// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tunnelnif.cpp
// 
//

#include <es_mbuf.h>
//#include <flogger.h>

#include <agenterrors.h>

//#include "tunnelnifvar.h"
#include "winsockBinders.h"
#include <comms-infras/es_protbinder.h>
//#include "TunnelAgentHandler.h"
#include <comms-infras/commsdebugutility.h>
/*
 * This sections defines a whole load of constants etc... not very exciting
 */
#if defined (_DEBUG) 
	#define LOG(a) a
	_LIT(KTunnelIfLogFolder, "Winsock");
	_LIT(KTunnelIfLogFile, "Winsock");
#else
	#define LOG(a)
#endif

const TUint KConnNifConfig = (1 | KConnReadUserDataBit);

CWinsockNcp::CWinsockNcp(CWinsockFlow& aFlow)
    : iFlow(&aFlow)
    {
    }

//
// from ESock::MLowerControl
//

TInt CWinsockNcp::GetName(TDes& aName)
/**
Return binder name.

Called from upper layer.

@param aName return descriptor filled in with binder name 
*/
	{
	aName.Copy(Info()->iIfName);  //  Need to Written for Winsock Interface name  
	return KErrNone;
	}

TInt CWinsockNcp::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
	return KErrNotSupported;
	}

//
// Utility functions called from CTunnelFlow
//

MLowerDataSender* CWinsockNcp::Bind(MUpperDataReceiver* aUpperReceiver, MUpperControl* aUpperControl)
/**
 * Binds TCP/IP protocol to Flow
 *
 * @param aUpperReceiver A pointer to Upper layer Receive class
 * @param aUpperControl A pointer to Upper layer control class
 */
	{
   // CWinsockNcpLog::Printf(_L("CWinsockNcp:\tBind()"));

	iUpperReceiver = aUpperReceiver;
	iUpperControl = aUpperControl;
	return this;
	}

void CWinsockNcp::Unbind(MUpperDataReceiver* aUpperReceiver, MUpperControl* aUpperControl)
    {
	//CWinsockNcpLog::Printf(_L("CWinsockNcp:\tUnbind()"));

	(void)aUpperReceiver;
	(void)aUpperControl;
	ASSERT(aUpperReceiver == iUpperReceiver);
	ASSERT(aUpperControl == iUpperControl);
	iUpperReceiver = NULL;
	iUpperControl = NULL;
	}

void CWinsockNcp::StartSending()
/**
 * Indicates to the protocol layer that the NIF is ready to send packets.
 *
 * @param aProtocol A pointer to a protocol
 */
	{
	//CWinsockNcpLog::Printf(_L("CWinsockNcp:\tStartSending()"));

	// Default implementation.
	// Uses iProtocol instead aProtocol.
	iUpperControl->StartSending();
	}

TBool CWinsockNcp::MatchesUpperControl(const ESock::MUpperControl* aUpperControl)
	{
	return iUpperControl == aUpperControl;
	}

MLowerDataSender::TSendResult CWinsockNcp::Send(RMBufChain& aPdu)
    {
   // CWinsockNcpLog::Printf(_L("Illegal CWinsockNcp::Send Called, Dropping packet"));
    aPdu.Free();
	return MLowerDataSender::ESendAccepted;
    }

// ===================================================================================
//
//	NCP4 specific functions
//

CWinsockNcp4::CWinsockNcp4(CWinsockFlow& aFlow)
    : CWinsockNcp(aFlow)
    {
    }

CWinsockNcp4* CWinsockNcp4::ConstructL(CWinsockFlow& aFlow)
    {
  //  CWinsockNcpLog::Printf(_L("CWinsockNcp4::ConstructL()"));
    
    CWinsockNcp4* self = new(ELeave) CWinsockNcp4(aFlow);
    CleanupStack::PushL(self);

 //   CWinsockNcpLog::Printf(_L("CWinsockNcp4::ConstructL() - Created Nif %S"), &self->Info()->iIfName);
    CleanupStack::Pop(self);
    return self;
    }

TInt CWinsockNcp4::GetConfig(TBinderConfig& aConfig)
	{
    TBinderConfig4* config = TBinderConfig::Cast<TBinderConfig4>(aConfig);
    
   	if(config == NULL)
   		{
   		return KErrNotSupported;
   		}

	config->iFamily = KAfInet;		/* KAfInet - selects TBinderConfig4 */
	
	config->iInfo.iFeatures = KIfCanBroadcast | KIfCanMulticast;		/* Feature flags */
	config->iInfo.iMtu = KWinsockMtu;							/* Maximum transmission unit. */
	config->iInfo.iRMtu = KWinsockMtu;						/* Maximum transmission unit for receiving. */
	config->iInfo.iSpeedMetric = 0;							/* approximation of the interface speed in Kbps. */
	
	config->iNetMask.SetAddress(0);							/* IP netmask. */
	config->iBrdAddr.SetAddress(0);							/* IP broadcast address. */
	
	// do this only if we have a valid address
	if(iLocalAddress == 0)
		{
		TInetAddr unspec(KAFUnspec);
		config->iAddress = unspec;
		config->iDefGate = unspec;
		}
	else
		{
		config->iAddress.SetAddress(iLocalAddress);		/* Interface IP address. */
		config->iDefGate.SetAddress(iLocalAddress);		/* IP default gateway or peer address (if known). */
		config->iNameSer1 = iNameSer1;					/* IP primary name server (if any). */
		config->iNameSer2 = iNameSer2;					/* IP secondary name server (if any). */
		}
		
	return KErrNone;
    }

TInt CWinsockNcp4::Control(TUint aLevel, TUint aName, TDes8& aOption)
    {
    CWinsockNcpLog::Printf(_L("CWinsockNcp4::Control(aLevel %x, aName %x, ...)"), aLevel, aName);

    if (aLevel == KCOLInterface)
	{
	if (aName == KConnNifConfig)
	    {
	    TInetIfConfig& opt = *(TInetIfConfig*)aOption.Ptr();
	    iLocalAddress = opt.iAddress.Address();
	    iNameSer1 = opt.iNameSer1;
	    iNameSer2 = opt.iNameSer2;
	    return KErrNone;
	    }
	}
    return KErrNotSupported;
    }

TInt CWinsockNcp4::Notification(TWinsockAgentMessage::TTunnelSetAddress& aMessage)
    {
    iLocalAddress = aMessage.iAddress.Address() ;
    iNameSer1 = aMessage.iNameSer1;
    iNameSer2 = aMessage.iNameSer2;
    if ( aMessage.iIsUpdate )
		{
		ASSERT(iUpperControl);
		iUpperControl->Error(KErrLinkConfigChanged);
//	        iLink->Notify()->BinderLayerDown((CNifIfBase*)this, KErrLinkConfigChanged, MNifIfNotify::EReconnect);
		}
	return KErrNone;
    }

// ===================================================================================
//
//	NCP6 specific functions
//

CWinsockNcp6::CWinsockNcp6(CWinsockFlow& aFlow)
    : CWinsockNcp(aFlow)
    {
    }

CWinsockNcp6* CWinsockNcp6::ConstructL(CWinsockFlow& aLink)
    {
    CWinsockNcpLog::Printf(_L("CWinsockNcp6::ConstructL()"));
    
    CWinsockNcp6* self = new(ELeave) CWinsockNcp6(aLink);
    CleanupStack::PushL(self);

   // CWinsockNcpLog::Printf(_L("CWinsockNcp::ConstructL() - Created Nif %S"), &self->Info()->iIfName);
    CleanupStack::Pop(self);
    return self;
    }

TInt CWinsockNcp6::GetConfig(TBinderConfig& aConfig)
/**
Return the configuration information for the binder (e.g. IP address)

Called from upper layer.

@param aConfig base class of structure to fill in with configuration information.
*/
	{
	if(TBinderConfig::Cast<TBinderConfig6>(aConfig) == NULL)
		{
		return KErrNotSupported;
		}

	TBinderConfig6& config = static_cast<TBinderConfig6&>(aConfig);
		
	config.iFamily = KAfInet6;		/* KAfInet - selects TBinderConfig4 */
	
	config.iInfo.iFeatures = KIfCanBroadcast | KIfCanMulticast;		/* Feature flags */
	config.iInfo.iMtu = KWinsockMtu;							/* Maximum transmission unit. */
	config.iInfo.iRMtu = KWinsockMtu;						/* Maximum transmission unit for receiving. */
	config.iInfo.iSpeedMetric = 0;							/* approximation of the interface speed in Kbps. */
	
	config.iLocalId = iLocalAddress;
	//opt.iRemoteId.SetAddress(iLocalAddress);
	// Setup static DNS address if required
	if (!iNameSer1.IsUnspecified())
		{
		config.iNameSer1 = iNameSer1;
		if (!iNameSer1.IsUnspecified())
			config.iNameSer2 = iNameSer2;
		}

	return KErrNone;
    }

TInt CWinsockNcp6::Control(TUint aLevel, TUint aName, TDes8& aOption)
    {
  //  CWinsockNcpLog::Printf(_L("CWinsockNcp6::Control(aLevel %x, aName %x, ...)"), aLevel, aName);

	if (aLevel == KCOLInterface)
		{
		if (aName == KConnNifConfig)
			{
			TSoInet6IfConfig& opt = *(TSoInet6IfConfig*)aOption.Ptr();
			iLocalAddress = opt.iLocalId;
			iNameSer1 = opt.iNameSer1;
			iNameSer2 = opt.iNameSer2;
			return KErrNone;
			}
		}

    return KErrNotSupported;
    }

TInt CWinsockNcp6::Notification(TWinsockAgentMessage::TTunnelSetAddress& aMessage)
    {
    iLocalAddress = aMessage.iAddress;
    iNameSer1 = aMessage.iNameSer1;
    iNameSer2 = aMessage.iNameSer2;
    if ( aMessage.iIsUpdate )
		{
		ASSERT(iUpperControl);
		iUpperControl->Error(KErrLinkConfigChanged);
        // iLink->Notify()->BinderLayerDown((CNifIfBase*)this, KErrLinkConfigChanged, MNifIfNotify::EReconnect);
		}
	return KErrNone;
    }


void CWinsockNcpLog::Write(const TDesC& aDes)
//
// Write aText to the log
//
    {
    LOG(RFileLogger::Write(KTunnelIfLogFolder(), KTunnelIfLogFile(), EFileLoggingModeAppend, aDes);)
	(void)aDes;
    }

void CWinsockNcpLog::Printf(TRefByValue<const TDesC> aFmt,...)
//
// Write a mulitple argument list to the log, trapping and ignoring any leave
//
    {
	/*LOG(
		VA_LIST list;
		VA_START(list,aFmt);
		RFileLogger::WriteFormat(KTunnelIfLogFolder(), KTunnelIfLogFile(), EFileLoggingModeAppend, aFmt, list);
		)*/
	(void)aFmt;
    }




