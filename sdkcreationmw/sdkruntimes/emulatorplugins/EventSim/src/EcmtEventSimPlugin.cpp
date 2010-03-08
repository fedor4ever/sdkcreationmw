/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event simulation plugin
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <ImplementationProxy.h>
#include "EcmtEventSimPlugin.h"
#include "EcmtEventSimPluginUids.h"
#include "EcmtMessageEvent.h"

#include "EventWrapper.h" // for setting events
#include "utf.h" // for CnvUtfConverter




// ============================= LOCAL FUNCTIONS ===============================
  // none

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CEventSimPlugin::CEventSimPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEventSimPlugin::CEventSimPlugin() : 
	iUid( TUid::Uid( KEVENTSIMPLUGIN_IMPL_UID ) ), 
	iTargetUid( TUid::Uid( KJAVA_KEVENTSIMPLUGIN_IMPL_UID ) ),
	CTimer( EPriorityStandard ),
	iInterval( KDefaultInterval )
	{
	}

// -----------------------------------------------------------------------------
// CEventSimPlugin::~CEventSimPlugin
// Destructor
// -----------------------------------------------------------------------------
//
CEventSimPlugin::~CEventSimPlugin()
	{
	Cancel();
	delete iWrapper;
	}

// -----------------------------------------------------------------------------
// CEventSimPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEventSimPlugin::ConstructL()
	{
	iWrapper = CEventWrapper::NewL( this );
	CTimer::ConstructL();
	CActiveScheduler::Add( this );
	}


// -----------------------------------------------------------------------------
// CEventSimPlugin::NewL
// Two-phased constructor; initiates the object and the "leave" functionality
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CEventSimPlugin::NewL( )
	{
    CEventSimPlugin* self = new( ELeave ) CEventSimPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// -----------------------------------------------------------------------------
// CEventSimPlugin::RunL
// 
// -----------------------------------------------------------------------------
//
void CEventSimPlugin::RunL()
	{
	// if request has completed, check wrapper status
	if ( iStatus == KErrNone )
    	{
    	TRAPD(err, 	HandleStatusChangeL());
    	switch(err)
    		{
			case KErrNone:
				//this is ok
				break;
			case KErrNotSupported:
				//TODO error handling
				break;
			default:
				User::Leave( err );
			}		
    			
		After( iInterval );
    	}
	}
	
// -----------------------------------------------------------------------------
// CEventSimPlugin::RunError
// 
// -----------------------------------------------------------------------------
//
TInt CEventSimPlugin::RunError( TInt /*aError*/ )
	{
	
	After( iInterval );
	return KErrNone;
	}	
	
// -----------------------------------------------------------------------------
// CEventSimPlugin::BindMessagingL
// Binds to the messaging interface to start handling messages
// -----------------------------------------------------------------------------
//
void CEventSimPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
	iMessaging->AddMessageEventObserverL( *this, iUid );
	iMessageSender = iMessaging->GetMessageSender( );
	RunL(); // start timer by runner
	}

// -----------------------------------------------------------------------------
// CEventSimPlugin::HandleNotifyL
// Handles the actual notify event, ie. the message coming from the epdt plugin
// -----------------------------------------------------------------------------
//
void CEventSimPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
	{	
	RDebug::Print(_L("CEventSimPlugin::HandleNotifyL"));
	const TPtrC8 m = iMessaging->Message( aEvent );
	
	TBuf <256> message;
	
	// convert message to unicode
	TInt result = CnvUtfConverter::ConvertToUnicodeFromUtf8( message, m );
	
  	TLex lexer ( message );
  	TPtrC id = lexer.NextToken();
  	TPtrC parameters = lexer.Remainder();
    		
  	// get id to int
  	TLex tmpEventId ( id );
  	TInt eventId;
  	result = tmpEventId.Val( eventId );
  	if ( result == KErrNone )
  		{
  		// pass message to wrapper  
	  	TRAP(result, iWrapper->SetEventL( eventId, parameters ));
  		}
  	

	}


// -----------------------------------------------------------------------------
// CEventSimPlugin::SendDataL
// Sends data to EPDT 
// -----------------------------------------------------------------------------
//
void CEventSimPlugin::SendDataL( CBufFlat* aMsg )
	{
	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, aMsg->Ptr(0) );
	if ( m )
		{
		iMessageSender->SendMessage( m, KChannelId );
		}
	}
	

// -----------------------------------------------------------------------------
// CEventWrapper::HandleStatusChangeL
// 
// -----------------------------------------------------------------------------
//
void CEventSimPlugin::HandleStatusChangeL()
	{
	if ( iWrapper )
		{
		
		// get status from wrapper
		CBufFlat* msg = CBufFlat::NewL( 512 );
		CleanupStack::PushL( msg );
		iWrapper->GetWrapperStatusL( msg );
		SendDataL( msg );
		
		CleanupStack::PopAndDestroy( msg );
		
		}
	}

/**
*	Bulk code needed by ECom and Symbian OS frameworks
*/


//GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//	{
//	return(KErrNone);
//	}
	
const TImplementationProxy ImplementationTable[] = 
	{
		{ {KEVENTSIMPLUGIN_IMPL_UID}, CEventSimPlugin::NewL}
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}	

	
// End of File