// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tunnelnif.h 
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef WINSOCKBINDERS_H__
#define WINSOCKBINDERS_H__

#include <comms-infras/nifif.h>
#include <in6_if.h>
#include <eui_addr.h>	// TE64Addr
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_nodemessages.h>
#include "winsockflow.h"				// for CTunnelNcp::Info()

using namespace ESock;

const TInt KWinsockMtu = 1500;

class CWinsockFlow;

NONSHARABLE_CLASS(CWinsockNcp) : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl
	{
public:
	// from MLowerControl
	virtual TInt GetName(TDes& aName);
	virtual TInt BlockFlow(ESock::MLowerControl::TBlockOption /*aOption*/);

	// from MLowerDataSender
	virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aPdu);

	// Utility functions called from CTunnelFlow
	ESock::MLowerDataSender* Bind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);
	void Unbind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);
	TBool MatchesUpperControl(const ESock::MUpperControl* aUpperControl);
	void StartSending();

protected:
	CWinsockNcp(CWinsockFlow& aFlow);
	inline const TWinsockInfo* Info();

protected:
    CWinsockFlow* iFlow;
	
    TInetAddr iNameSer1;
	TInetAddr iNameSer2;

	ESock::MUpperControl* iUpperControl;
	ESock::MUpperDataReceiver* iUpperReceiver;
	};

// ======================================================================================

NONSHARABLE_CLASS(CWinsockNcp4) : public CWinsockNcp
	{
public:
	static CWinsockNcp4* ConstructL(CWinsockFlow& aLink);

	// from MLowerDataSender
	virtual TInt GetConfig(TBinderConfig& aConfig);
	virtual TInt Control(TUint, TUint, TDes8&);
	//

	virtual TInt Notification(TWinsockAgentMessage::TTunnelSetAddress& aMessage);

private:
    CWinsockNcp4(CWinsockFlow& aLink);

	TUint32 iLocalAddress;
	};

// ======================================================================================

NONSHARABLE_CLASS(CWinsockNcp6) : public CWinsockNcp
	{
public:
	static CWinsockNcp6* ConstructL(CWinsockFlow& aLink);

	// from MLowerDataSender
	virtual TInt GetConfig(TBinderConfig& aConfig);
	virtual TInt Control(TUint, TUint, TDes8&);
	//

	virtual TInt Notification(TWinsockAgentMessage::TTunnelSetAddress& aMessage);

private:
    CWinsockNcp6(CWinsockFlow& aLink);

	TInetAddr iLocalAddress;
	};

// ======================================================================================

class CWinsockNcpLog : public CBase
	{
public:
	static void Write(const TDesC& aDes);
	static void Printf(TRefByValue<const TDesC> aFmt, ...);
	};

//
// Inline functions
//

inline const TWinsockInfo* CWinsockNcp::Info()
	{
	return iFlow->Info();
	}

#endif
