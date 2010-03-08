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
#include "EcmtMsgRelayPlugin.h"
#include "EcmtMsgRelayPluginUids.h"
#include "EcmtMessageEvent.h"
#include "MessageRelayAppDll.h"

CMsgRelayPlugin::CMsgRelayPlugin() : 
	iUid( TUid::Uid( KECMTMSGRELAYPLUGIN_IMPL_UID ) )
	{
	}
	
CMsgRelayPlugin::~CMsgRelayPlugin()
	{
	delete iMessageRelayAppDll;
	}
	
void CMsgRelayPlugin::ConstructL()
	{
	iMessageRelayAppDll = CMessageRelayAppDll::NewL();
	}

MEcmtPlugin* CMsgRelayPlugin::NewL( )
	{
    CMsgRelayPlugin* self = new( ELeave ) CMsgRelayPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

void CMsgRelayPlugin::BindMessagingL( MEcmtMessaging* /*aMessaging*/ )
	{
	// No need for messaging
	}
	
/**
*	Bulk code needed by ECom and Symbian OS frameworks
*/

// Fix for RVCT Error #144: a value of type "MEcmtPlugin *(*)()" cannot
// be used to initialize an entity of type "TProxyNewLPtr"
static TAny* CreateMsgRelayPluginL()
{
    return CMsgRelayPlugin::NewL();
}

const TImplementationProxy ImplementationTable[] = 
{
    { {KECMTMSGRELAYPLUGIN_IMPL_UID}, CreateMsgRelayPluginL }
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}
