/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ecom.h>
#include <ImplementationProxy.h>
#include "EcmtNCIPlugin.h"
#include "EcmtNCIPluginUids.h"
#include "EcmtMessageEvent.h"
#include "MsgSvr.h"

CNCIPlugin::CNCIPlugin() : 
	iUid( TUid::Uid( KECMTNCIPLUGIN_IMPL_UID ) )
	{
	}
	
CNCIPlugin::~CNCIPlugin()
	{
	delete iMsgSvr;
	}
	
void CNCIPlugin::ConstructL()
	{
	iMsgSvr = CMsgSvr::NewL();
	}

MEcmtPlugin* CNCIPlugin::NewL( )
	{
    CNCIPlugin* self = new( ELeave ) CNCIPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

void CNCIPlugin::BindMessagingL( MEcmtMessaging* /*aMessaging*/ )
	{
	// No need for messaging
	}
	
/**
*	Bulk code needed by ECom and Symbian OS frameworks
*/
const TImplementationProxy ImplementationTable[] = 
	{
		{ {KECMTNCIPLUGIN_IMPL_UID}, CNCIPlugin::NewL}
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	

