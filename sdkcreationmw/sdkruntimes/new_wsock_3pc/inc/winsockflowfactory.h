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
// Defines the factory class which is used to instantiate the RAW IP.
// 
//

/**
 @file
*/

#ifndef WINSCOKFLOWFACTORY_H__
#define WINSCOKFLOWFACTORY_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_protflow.h>
#include "WinsockUid.h"



/**
Winsock Flow Implementation UID  : Find the 
*/
const TInt KWinsockFlowImplUid = KWinsockFlowUid;


class CWinsockFlowFactory : public ESock::CSubConnectionFlowFactoryBase
	{
public:
	static CWinsockFlowFactory* NewL(TAny* aConstructionParameters);

protected:
	CWinsockFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);
	virtual ESock::CSubConnectionFlowBase* DoCreateFlowL(ESock::CProtocolIntfBase* aProtocol, ESock::TFactoryQueryBase& aQuery);

private:
	//CBttLogger* iTheLogger;		// only here to allow use of _LOG_LnCn() macros within factory
	};

#endif
