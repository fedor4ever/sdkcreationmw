/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Socket Forward Plugin
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <in_sock.h>
#include <ImplementationProxy.h>
#include <utf.h>
#include "EcmtSeiAgentPlugin.h"
#include "EcmtSeiAgentPluginUids.h"
#include "EcmtMessageEvent.h"
#include "EcmtMidpDebugProtocol.h"
#include "DebugLogging.h"

// CONSTANTS
const TUint KMaxFileNameLength = 55;

const TInt KMaxAcceptMsgLen = 32;
const TInt KMaxMsgPrefixLen = 16;

_LIT8( KStart, "START" );

_LIT8( KMsg, "MSG %d" );
_LIT8( KAcceptMsg, "ACCEPT %d %d" );
_LIT8( KBlanco, " " );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSeiAgentPlugin::CSeiAgentPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSeiAgentPlugin::CSeiAgentPlugin() : 
	iUid( TUid::Uid( KSEIAGENTPLUGIN_IMPL_UID ) )
	{
	}

// -----------------------------------------------------------------------------
// CSeiAgentPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CSeiAgentPlugin::NewL( )
	{
    CSeiAgentPlugin* self = new( ELeave ) CSeiAgentPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// Destructor
CSeiAgentPlugin::~CSeiAgentPlugin()
	{
	RDebug::Print( _L( "EcmtSeiAgentPlugin::~" ) );

	}
	
// -----------------------------------------------------------------------------
// CSeiAgentPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSeiAgentPlugin::ConstructL()
	{
	RDebug::Print( _L( "EcmtSeiAgentPlugin::ConstructL" ) );


	

	}




// -----------------------------------------------------------------------------
// CSeiAgentPlugin::BindMessagingL
// 
// -----------------------------------------------------------------------------
//
void CSeiAgentPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
	iMessaging->AddMessageEventObserverL( *this, iUid );
	}
	
// -----------------------------------------------------------------------------
// CSeiAgentPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CSeiAgentPlugin::HandleNotifyL(const CEcmtMessageEvent& aEvent)
{
    const TPtrC8 msg = iMessaging->Message(aEvent);
    if (msg.Length() >= ECMT_MIDP_DEBUG_HEADER_SIZE) 
    {
      
        TUint opcode = msg[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET];
       
        TUint sessionId = (msg[ECMT_MIDP_DEBUG_HEADER_CID_OFFSET] << 24) | 
            (msg[ECMT_MIDP_DEBUG_HEADER_CID_OFFSET+1] << 16) |
            (msg[ECMT_MIDP_DEBUG_HEADER_CID_OFFSET+2] << 8) |
            (msg[ECMT_MIDP_DEBUG_HEADER_CID_OFFSET+3]);
       
        TUint seq = (msg[ECMT_MIDP_DEBUG_HEADER_SEQ_OFFSET] << 24) | 
            (msg[ECMT_MIDP_DEBUG_HEADER_SEQ_OFFSET+1] << 16) |
            (msg[ECMT_MIDP_DEBUG_HEADER_SEQ_OFFSET+2] << 8) |
            (msg[ECMT_MIDP_DEBUG_HEADER_SEQ_OFFSET+3]);

        TInt index = -1;
       
        switch (opcode)
         {
        case ECMT_MIDP_DEBUG_OPCODE_OPEN:
        
        		
				/*Test code*/
			//	_LIT(KTestPortNumber, "2889");
	
				TUint portNumber = ( (msg[ECMT_MIDP_DEBUG_HEADER_SIZE]<<8) |
									(msg[ECMT_MIDP_DEBUG_HEADER_SIZE+1]) );
									
				

				TBuf<KMaxFileNameLength> arguments;
				
				
				arguments.NumUC(portNumber);
				
				
				//FindFreePortL();


				StartSeiSlaveL(arguments);
            break;
                
        case ECMT_MIDP_DEBUG_OPCODE_CLOSE:
        
        		KillSeiSlaveL();

            break;

        case ECMT_MIDP_DEBUG_OPCODE_SEND:
        	break;
        }
    }
}

// -----------------------------------------------------------------------------
// CSeiAgentPlugin::StartSeiSlaveL
// 
// -----------------------------------------------------------------------------
//
void CSeiAgentPlugin::StartSeiSlaveL(TDesC& aArguments)
	{
	
	_LIT(KDebugAgentFullFileName, "z:\\sys\\bin\\JDbgAgt.exe");



 	TBufC<KMaxFileNameLength> exeFileName(KDebugAgentFullFileName);
 

	
	TInt err=iProcess.Create(exeFileName, aArguments );


	iProcess.Resume();
	
        
	
	}

// -----------------------------------------------------------------------------
// CSeiAgentPlugin::KillSeiSlaveL
// 
// -----------------------------------------------------------------------------
//
void CSeiAgentPlugin::KillSeiSlaveL()
	{
	

	iProcess.Kill(0);

	iProcess.Close();

        
	
	}

	
// -----------------------------------------------------------------------------
// CSeiAgentPlugin::FindFreePortL
// 
// -----------------------------------------------------------------------------
//
TInt CSeiAgentPlugin::FindFreePortL()
	{
	
	
	
	// Open channel to Socket Server
    User::LeaveIfError( iSocketServ.Connect() );
	
    RSocket trialSocket;
    
	TInt portNumber=2080;
	TInt err;
		
	/*Test code
	
	RSocket testSocket;	
	
	User::LeaveIfError( testSocket.Open( iSocketServ,
                                          KAfInet,
                                          KSockStream,
                                          KProtocolInetTcp ) );

	
	testSocket.SetLocalPort( portNumber + 5 );
	
	 Test code end
	*/

	do
	{
	
		User::LeaveIfError( trialSocket.Open( iSocketServ,
                                          KAfInet,
                                          KSockStream,
                                          KProtocolInetTcp ) );
                                          
		portNumber += 5;
		
		if(portNumber>=65535)
		{
			User::Leave(KErrCouldNotConnect);
		}                                          
	
		err=trialSocket.SetLocalPort( portNumber );
		

		
	}while(err != KErrNone );
		
	
	trialSocket.Close();
	
	/*testSocket.Close(); /*Test code*/
	
	__LOGWRITE1("CSeiAgentPlugin::FindFreePortL, port found: %d", portNumber);		
		
	return portNumber;		


    
	return 0;
		
	}
	
// ============================ OTHER FUNCTIONS ===============================
	
/**
 *	Bulk code needed by ECom and Symbian OS frameworks
 */

// Fix for RVCT Error #144: a value of type "MEcmtPlugin *(*)()" cannot be 
// used to initialize an entity of type "TProxyNewLPtr"
static TAny* CreateSeiAgentPluginL()
{
  return CSeiAgentPlugin::NewL();
}

const TImplementationProxy ImplementationTable[] = 
{
  { {KSEIAGENTPLUGIN_IMPL_UID}, CreateSeiAgentPluginL }
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
  aCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
  return ImplementationTable;
}
	
