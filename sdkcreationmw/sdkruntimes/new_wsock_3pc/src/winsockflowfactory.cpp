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
// Implements the factory class which is used to instantiate the WINSOCK NIF.
// 
//

/**
 @file
*/

#include "WinsockFlowFactory.h"
#include "WinsockFlow.h"
#include "../inc/winsockUid.h"
#include <es_mbuf.h>
//#include "bttlog.h"

using namespace ESock;

// =====================================================================================
//
// RawIP Flow Factory
//

CWinsockFlowFactory* CWinsockFlowFactory::NewL(TAny* aConstructionParameters)
/**
Constructs a Default SubConnection Flow Factory

@param aConstructionParameters construction data passed by ECOM

@returns pointer to a constructed factory
*/
	{
	
		CWinsockFlowFactory* ptr = new (ELeave) CWinsockFlowFactory(TUid::Uid(KWinsockFlowUid), *(reinterpret_cast<CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	return ptr;
	}


CWinsockFlowFactory::CWinsockFlowFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
	: CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
/**
Default SubConnection Flow Factory Constructor

@param aFactoryId ECOM Implementation Id
@param aParentContainer Object Owner
*/
	{
	}


CSubConnectionFlowBase* CWinsockFlowFactory::DoCreateFlowL(ESock::CProtocolIntfBase* aProtocol, ESock::TFactoryQueryBase& aQuery)
	{
    const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
    CWinsockFlow* s = new (ELeave) CWinsockFlow(*this, query.iSCprId, aProtocol);
    return s;
	}

