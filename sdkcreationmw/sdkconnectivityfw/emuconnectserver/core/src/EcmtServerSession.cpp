/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Server Session for Ecmt server
*
*/


// INCLUDE FILES
#include "EcmtClientServerCommon.h"
#include "EcmtServerSession.h"
#include "EcmtServer.h"
#include "EcmtUids.h"
#include "EcmtServerDebug.h"
#include <utf.h>

#define ECMT_TRACE_PREFIX "EcmtServerSession::"
#include "EcmtServerDebugTrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtServerSession::CEcmtServerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtServerSession::CEcmtServerSession( ) :
    CSession2( ),
    iClientUid( TUid::Null( ) )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtServerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtServerSession::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtServerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtServerSession* CEcmtServerSession::NewL( )
    {
    CEcmtServerSession* self = new( ELeave ) CEcmtServerSession( );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtServerSession::~CEcmtServerSession()
    {
    // decrement session count, so server may terminate if no longer needed
    const CEcmtServer* server = static_cast< const CEcmtServer* >( Server() );
    const_cast< CEcmtServer* >( server )->DecrementSessions();

    // Remove the message observer, if any.
    if ( iMessaging )
        {
        iMessaging->RemoveMessageEventObserver( *this, iClientUid );
        }
    }

// -----------------------------------------------------------------------------
// CEcmtServerSession::ServiceL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtServerSession::ServiceL( const RMessage2& aMessage )
{
    TRACE1("ServiceL: UID = %x", iClientUid.iUid );
    if ( !iMessaging ) {
        CEcmtServer* server = (CEcmtServer*)Server();
        iMessaging = server->MessagingIf();
    }

    switch ( aMessage.Function() )
    {
        // Client application sends it UID in order to receive messages.
        case EEcmtNotifyUid:
        {
            TRACE1("ServiceL: EEcmtNotifyUid: UID = %x" , aMessage.Int0());
            // The current implementation supports only one UID per client,
            // so one client is inable to listen to messages for multiple UIDs.
            //  If such functionality is required, this is the place to 
            // implement it. Just replace iClientUid with a list of UIDs.
            // Add/RemoveMessageEventObserver() methods are already compatible
            // with multiple UIDs per message listener.
            if ( iClientUid == TUid::Null() && iMessaging ) {
                iClientUid = TUid::Uid( aMessage.Int0() );
                iMessaging->AddMessageEventObserverL( *this, iClientUid );
            }

            aMessage.Complete( KErrNone );
            break;
        }

        // Client application requests to receive messages.
        case EEcmtRequestServerNotification:
        {
            TRACE("ServiceL: EEcmtRequestServerNotification");

            iClientNotifyMessage = aMessage;
            iClientWaitingForNotification = ETrue;

            if ( !iInitialClientNotificationSent )
            {
                iInitialClientNotificationSent = ETrue;
                const CEcmtServer* server = (CEcmtServer*)Server();
                NotifyClient( server->LastClientNotification() );
            }

            break;
        }

        // Client application cancels message request.
        case EEcmtCancelRequestServerNotification:
        {
            TRACE("ServiceL: EEcmtCancelRequestServerNotification");
            if ( iClientWaitingForNotification )
            {
                iClientNotifyMessage.Complete( KErrCancel );
                iClientWaitingForNotification = EFalse;
            }
            aMessage.Complete( KErrNone );        		
            break;
        }

        // Client application wants to send a text message to the 
        // Diagnostics log view.
        case EEcmtRequestSendText:
        {
            TRACE("ServiceL: EEcmtRequestSendText");
            SendTextL( aMessage );
            aMessage.Complete( KErrNone );
            break;
        }

        // Client application wants to send binary data to a UID.
        case EEcmtRequestSendData:
        {
            TRACE("ServiceL: EEcmtRequestSendData");
            SendDataL( aMessage );
            aMessage.Complete( KErrNone );
            break;
        }

        // Client application wants to do something illegal.
        default:
            TRACE("ServiceL: default");
            aMessage.Complete( KErrNotSupported );
            break;
     }
}

// -----------------------------------------------------------------------------
// CEcmtServerSession::NotifyClient
// 
// -----------------------------------------------------------------------------
void CEcmtServerSession::NotifyClient( const TDesC8& aNotification )
{
    TRACE1("NotifyClient: UID = %x", iClientUid.iUid );
    if ( iClientWaitingForNotification )
    {
        iClientWaitingForNotification = EFalse;
		
        TInt len = iClientNotifyMessage.GetDesMaxLength( 0 );
		
        if ( len < 0 )
        {
            iClientNotifyMessage.Panic( KCEcmtServer, EBadDescriptor );
        }
        else if ( len < aNotification.Length() )
        {
            iClientNotifyMessage.Complete( KErrNoMemory );
        }
        else
        {
            TRAPD( res, iClientNotifyMessage.WriteL( 0, aNotification ) );
            if ( res != KErrNone )
            {
                iClientNotifyMessage.Panic( KCEcmtServer, EBadDescriptor );
            }
            else
            {
                iClientNotifyMessage.Complete( KErrNone );
            }
        }
    }
}

// -----------------------------------------------------------------------------
// CEcmtServerSession::SendDataL
// 
// -----------------------------------------------------------------------------
void CEcmtServerSession::SendDataL( const RMessage2& aMessage )
{
    TRACE2("SendDataL: UID = %x, target = %x",iClientUid.iUid,aMessage.Int0());
    // Get length of client's text to send
    TInt dataLen = aMessage.GetDesLength( 1 );
    if ( dataLen < 0 ) 
    {
        aMessage.Panic( KCEcmtServer, EBadDescriptor );
    }

    // Get target plug-in's UID
    TUint uid = aMessage.Int0();

    // Read text from client's address space to our own buffer
    HBufC8* buff8 = HBufC8::NewLC( dataLen );
    TPtr8 ptr8 = buff8->Des();
    aMessage.ReadL( 1, ptr8 );
	
    // Allocate an Ecmt message that is long enough
    if (iMessaging)
    {
	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( TUid::Uid( uid ), ptr8 );
	
	User::LeaveIfNull( m );
	iMessaging->GetMessageSender()->SendMessage( m );
    }
    CleanupStack::PopAndDestroy( buff8 );
}


// -----------------------------------------------------------------------------
// CEcmtServerSession::SendTextL
// 
// -----------------------------------------------------------------------------
void CEcmtServerSession::SendTextL( const RMessage2& aMessage )
{
    TRACE2("SendTextL: UID = %x, msg = %s", iClientUid.iUid, aMessage.Ptr0());

	// Get length of client's text to send
	TInt unicodeLen = aMessage.GetDesLength( 0 );
	if ( unicodeLen < 0 ) 
		{
		TRACE1("SendTextL: Got unicode length %d", unicodeLen);
		aMessage.Panic( KCEcmtServer, EBadDescriptor );
		}

	// Read text from client's address space to our own buffer
	HBufC16* buff16 = HBufC16::NewLC( unicodeLen );
	TPtr16 ptr16 = buff16->Des();
	aMessage.ReadL( 0, ptr16 );

	// Check how long the text is when encoded as UTF8
	TPtr8 null( 0, 0 );
	TInt utf8len = CnvUtfConverter::ConvertFromUnicodeToUtf8( null, *buff16 );

	// Allocate an Ecmt message that is long enough
    if (iMessaging)
    {
	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( TUid::Uid(KJAVA_SYSTEMOUT_UID), utf8len );
	
	User::LeaveIfNull( m );
	
	TPtr8 ptr8 = m->Message();
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8, *buff16 );
	iMessaging->GetMessageSender()->SendMessage( m );
    }
    CleanupStack::PopAndDestroy( buff16 );
}

// -----------------------------------------------------------------------------
// CEcmtServerSession::GetClientUid
// 
// -----------------------------------------------------------------------------
TUid CEcmtServerSession::GetClientUid()
{
    return iClientUid;
}

// ---------------------------------------------------------------------------
// CEcmtServerSession::HandleNotifyL
// 
// ---------------------------------------------------------------------------
//
void CEcmtServerSession::HandleNotifyL( const CEcmtMessageEvent& aEvent )
    {
    NotifyClient( aEvent.Message() );
    }


//  End of File  
