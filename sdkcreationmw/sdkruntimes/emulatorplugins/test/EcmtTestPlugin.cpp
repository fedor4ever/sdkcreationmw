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
#include "EcmtTestPlugin.h"
#include "EcmtTestPluginUids.h"
#include "EcmtMessageEvent.h"

TUid CTestPlugin::GetUid()
	{
	return iUid;
	}

CTestPlugin::CTestPlugin() : 
	CTimer( EPriorityStandard ),
	iUid( TUid::Uid( KTESTPLUGIN_IMPL_UID ) ),
	iTargetUid( TUid::Uid( KJAVA_TESTPLUGIN_IMPL_UID ) ),
	iInterval( 5000000 )
	{
	
	}
	
CTestPlugin::~CTestPlugin()
	{
	
	}
	
void CTestPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
	iMessaging->AddMessageEventObserverL( *this, iUid );
	iMessageSender = iMessaging->GetMessageSender( );
	}

void CTestPlugin::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);	
	After( iInterval );
	}

MEcmtPlugin* CTestPlugin::NewL( )
	{
    CTestPlugin* self = new( ELeave ) CTestPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}
	
void CTestPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
	{
	const TPtrC8 m = iMessaging->Message( aEvent );
	TLex8 lexer( m );
	
	TInt newInterval;
	if ( lexer.Val( newInterval ) == KErrNone && newInterval > 1000 )
		{
		iInterval = newInterval;
		}
	}
	
void CTestPlugin::RunL( )
	{
    if (iStatus == KErrNone)
    	{
    	TBuf8<256> msg;
    	
    	msg.Append( _L("Count is now: ") );
    	msg.AppendNum( iCount++ );
	msg.Append( _L("\n\r") );
    	
		CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, msg );
		if ( m )
			{
			iMessageSender->SendMessage( m );
			}
		After( iInterval );
    	}
	}


/**
*	Bulk code needed by ECom and Symbian OS frameworks
*/
const TImplementationProxy ImplementationTable[] = 
	{
		{ {KTESTPLUGIN_IMPL_UID}, CTestPlugin::NewL}
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
