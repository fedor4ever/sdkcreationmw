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
* Description:  Simulation PSY plugin
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <ImplementationProxy.h>
#include "EcmtSimPsyPlugin.h"
#include "EcmtSimPsyPluginUids.h"
#include "EcmtMessageEvent.h"

// Parts of the PSY Simulator
#include "EPos_TPosSimPsySettings.h"
#include <EPos_RPosSettingsFile.h>


// -----------------------------------------------------------------------------
// CSimPsyPlugin::CSimPsyPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CSimPsyPlugin::CSimPsyPlugin() : 
	iUid( TUid::Uid( KSIMPSYPLUGIN_IMPL_UID ) ),
	iTargetUid( TUid::Uid( KJAVA_SIMPSYPLUGIN_IMPL_UID ) )
	{
	}

// Destructor
CSimPsyPlugin::~CSimPsyPlugin()
	{
	}

// -----------------------------------------------------------------------------
// CSimPsyPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CSimPsyPlugin::ConstructL()
	{
	}


// -----------------------------------------------------------------------------
// CSimPsyPlugin::NewL
// Two-phased constructor; initiates the object and the "leave" functionality
// -----------------------------------------------------------------------------
MEcmtPlugin* CSimPsyPlugin::NewL( )
	{
    CSimPsyPlugin* self = new( ELeave ) CSimPsyPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// -----------------------------------------------------------------------------
// CSimPsyPlugin::BindMessagingL
// Binds to the messaging interface to start handling messages
// -----------------------------------------------------------------------------
void CSimPsyPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
	iMessaging->SetMessageEventObserverL( *this, iUid );
	iMessageSender = iMessaging->GetMessageSender( );
	}

// -----------------------------------------------------------------------------
// CSimPsyPlugin::HandleNotifyL
// Handles the actual notify event, ie. the message coming from the epdt plugin
// -----------------------------------------------------------------------------
void CSimPsyPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
	{
	const TPtrC8 m = iMessaging->Message( aEvent );
	TLex8 lexer( m );

	// Start handling the message and hopefully notifying the PSY simulator
	
	TPtrC8 msgDataId = lexer.NextToken();
	TPtrC8 filename = lexer.NextToken();
	
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	// configuration file
	_LIT( KPSySettingsFile, "c:\\system\\data\\simulationpsySettings");
	TPosSimPsySettings settings;
	
	// The filename required is actually the full path name, which is hardcoded
	// to SimPsySimulator anyway, so we'll get that from the java-side
	TFileName EpocFilename;
	EpocFilename.Copy( filename );
	
	settings.iFileName = EpocFilename;
	User::LeaveIfError(
		RPosSettingsFile<TPosSimPsySettings>::WriteSettings(
			fs, KPSySettingsFile, settings ) );

	CleanupStack::PopAndDestroy( 1 ); // fs


    _LIT8( KOkReply, "OK" );
    CEcmtMessageEvent* reply = iMessaging->NewMessageEvent( iUid, KOkReply() );
    if ( reply )
        {
        iMessageSender->SendMessage( reply );
        }
	}
	
/**
*	Bulk code needed by ECom and Symbian OS frameworks
*/

GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return(KErrNone);
	}
	
const TImplementationProxy ImplementationTable[] = 
	{
		{ {KSIMPSYPLUGIN_IMPL_UID}, CSimPsyPlugin::NewL}
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
