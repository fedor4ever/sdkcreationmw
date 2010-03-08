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
* Description:  Class implementing messaging functionality
*
*/


// INCLUDE FILES
#include <e32debug.h>
#include "EcmtMessaging.h"
#include "EcmtMessageEvent.h"    
#include "EcmtServerDebug.h"

#define ECMT_TRACE_PREFIX "EcmtMessaging::"
#include "EcmtServerDebugTrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtMessaging::CEcmtMessaging
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtMessaging::CEcmtMessaging( MEcmtMessagingObserver& aObserver ) : 
    iObserver( aObserver ),
    iMessageQue( _FOFF( CEcmtMessageEvent, iLink ) ),
    iPayload( 0, 0 ),
    iReceiverState( EIdle )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtMessaging* CEcmtMessaging::NewL( MEcmtMessagingObserver& aObserver )
    {
    CEcmtMessaging* self = new( ELeave ) CEcmtMessaging( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

void CEcmtMessaging::SetReader( MEcmtCommsReader* aReader ) 
    {
    iCommsReader = aReader;
    iCommsReader->SetReaderObserver( *this );
    }

void CEcmtMessaging::SetWriter( MEcmtCommsWriter* aWriter )
    {
    iCommsWriter = aWriter;
    iCommsWriter->SetWriterObserver( *this );
    }
    
// Destructor
CEcmtMessaging::~CEcmtMessaging()
    {
    TSglQueIter<CEcmtMessageEvent> iter( iMessageQue );

    while ( CEcmtMessageEvent* m = iter++ )
        {
        delete m;
        }
        
    iObserverUidList.Reset();
    iObserverList.Reset();

    delete iIncomingMsg;
    }


// -----------------------------------------------------------------------------
// CEcmtMessaging::AddMessageObserver
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::AddMessageEventObserverL( MEcmtMessageEventObserver& aObserver, TUid aUid )
    {
    TRACE1("AddMessageEventObserverL: uid = %x", aUid);

    // Add uid to the uid list
    LEAVE_IF_ERROR(iObserverUidList.Append(aUid));
    
    // Add observer interface to observer list
    TInt r = iObserverList.Append( &aObserver );
    
    if ( r != KErrNone )
    	{
	    // If adding to observer list fails, remove also uid from uid list
            iObserverUidList.Remove( iObserverUidList.Count() - 1 );
            LEAVE(r);
    	}
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::SetMessageObserver
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::RemoveMessageEventObserver( MEcmtMessageEventObserver& aObserver, TUid aUid )
    {
    TRACE1("RemoveMessageEventObserver: uid = %x", aUid);

   TInt i;

   for ( i = 0; i < iObserverUidList.Count(); i++ )
       {
       if ( ( iObserverUidList[i] == aUid ) &&
            ( iObserverList[i] == &aObserver ) )
           {
           iObserverUidList.Remove( i );
           iObserverList.Remove( i );
           }
       }
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::GetMessageSender
//
// -----------------------------------------------------------------------------
//
MEcmtMessageSender* CEcmtMessaging::GetMessageSender( )
    {
    return this;
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::SendMessage
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::SendMessage( CEcmtMessageEvent* aMessage )
{
    if ( aMessage )
    {
        TRACE1("SendMessage: Message available for UID %x", aMessage->iDest );

        // If the message is internal, ie. sent to an Ecmt Server plug-in,
        // it's delivered immediately past the message queue.
        for ( TInt i(0); i < iObserverUidList.Count(); i++ )
        {
            if ( iObserverUidList[i] == aMessage->iDest )
            {
                TRAP_IGNORE( iObserverList[i]->HandleNotifyL( *aMessage ) );
                TRACE1("SendMessage: Found internal listener for %x", 
                    aMessage->iDest);
            }
        }

        // If no message channel specified, just add the message to the queue.
        if ( aMessage->iChannelId == 0 )
        {
            iMessageQue.AddFirst( *aMessage );
        }
        // Otherwise overwrite possible previous message with same Uid and
        // message channel with the new message, execpt if the previous 
        // message is the last one in the queue, because that message is
        // currently being written to network.
        else
        {
            TSglQueIter<CEcmtMessageEvent> iter( iMessageQue );
            CEcmtMessageEvent* q;

            while (aMessage && (q = iter++) != NULL && !iMessageQue.IsLast(q))
            {
                if ( q->iDest == aMessage->iDest &&
                     q->iChannelId == aMessage->iChannelId )
                {
                    User::Free( q->iData );        // Free old message data
                    q->iData = aMessage->iData;	   // Replace old data with new
                    q->iPtr.Set( aMessage->iPtr ); // Update iPtr information

                    TRACE2("SendMessage: Found a previous message for %x/%d", 
                           aMessage->iDest, aMessage->iChannelId);

                    // Remove pointers to data from the new MessageEvent, so
                    // that q->iData and iPtr don't get deleted with aMessage.
                    // Delete the new MessageEvent.
                    aMessage->iData = 0;
                    aMessage->iPtr = TPtr8( 0, 0 );
                    delete aMessage;
                    aMessage = NULL;
                }
            }
            
            // If no previous message found, add the new message to queue normally.
            if ( aMessage )
            {
                iMessageQue.AddFirst( *aMessage );
            }
        }

        // If writer is ready, issue write command
        if ( iCommsWriter->WriterReady() )
        {
            CEcmtMessageEvent* message = iMessageQue.Last( );
            iCommsWriter->IssueWrite( message->iPtr );
            iMessageIsOutgoing = ETrue;

            TRACE("SendMessage: Wrote message from queue to socket.");
        }

    }
    else 
    {
        TRACE("SendMessage: Message NOT available");
    }
}

// -----------------------------------------------------------------------------
// CEcmtMessaging::SendMessage
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::SendMessage( CEcmtMessageEvent* aMessage, TMessageChannel aChannelId )
	{
	if ( aMessage )
		{
		aMessage->iChannelId = aChannelId;
		SendMessage( aMessage );
		}
	}
    
// -----------------------------------------------------------------------------
// CEcmtMessaging::NewMessageEvent
//
// -----------------------------------------------------------------------------
//
CEcmtMessageEvent* CEcmtMessaging::NewMessageEvent( TUid aDest, const TDesC8& aData )
    {
    CEcmtMessageEvent* m = 0;
    TRAPD( err, m = CEcmtMessageEvent::NewL( aDest, aData ) );
    return err == KErrNone ? m : 0;
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::NewMessageEvent
//
// -----------------------------------------------------------------------------
//
CEcmtMessageEvent* CEcmtMessaging::NewMessageEvent( TUid aDest, TInt aDataLen )
    {
    return CEcmtMessageEvent::New( aDest, aDataLen + KEcmtMessageHeaderLength );
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::Message
//
// -----------------------------------------------------------------------------
//
const TPtrC8 CEcmtMessaging::Message( const CEcmtMessageEvent& aMessage )
    {
    return aMessage.Message();
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::Message
//
// -----------------------------------------------------------------------------
//
TPtr8 CEcmtMessaging::Message( CEcmtMessageEvent& aMessage )
    {
    return aMessage.Message();
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::ParseInt
//
// -----------------------------------------------------------------------------
//
TBool CEcmtMessaging::ParseInt( const TPtrC8 aStr, TUint32& aVal )
    {
    TLex8 lexer( aStr );
    
    if ( !( lexer.Get() == '0' && lexer.Get() == 'x' ) )
        {
        return EFalse;
        }
    return lexer.Val( aVal, EHex ) == KErrNone ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::ParseHeader
//
// -----------------------------------------------------------------------------
//
TBool CEcmtMessaging::ParseHeader( TUint32& aLen, TUint32& aUid )
    {
    TPtrC8 strMsgLen( &iHeader[0], KEcmtMessageHexNumberLength );
    TPtrC8 strUid( &iHeader[KEcmtMessageHexNumberLength], KEcmtMessageHexNumberLength );
    
    return ParseInt( strMsgLen, aLen ) && ParseInt( strUid, aUid ) && aLen < KMaxTInt/2;
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::ResetReceiver
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::ResetReceiver( )
    {
    iReceiverState = EIdle;
    delete iIncomingMsg;
    iIncomingMsg = 0;
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::HandleCommsReaderNotify
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::HandleCommsReaderNotify( TEcmtCommsObjectStatus aStatus )
    {
    TRACE1("HandleCommsReaderNotify: status = %d", aStatus);
    if ( aStatus == EInError )
        {
        TRACE("HandleCommsReaderNotify: error");
        ResetReceiver();
        }

    else switch ( iReceiverState )
        {
        case EIdle:
            {
            TRACE("HandleCommsReaderNotify: idle");
            iCommsReader->IssueRead( iHeader );
            iReceiverState = EReceivingHeader;
            break;
            }

        case EReceivingHeader:
            {
            TRACE("HandleCommsReaderNotify: receiving header");
            TUint32 msgLen, uid;
            iHeader.SetLength( KEcmtMessageHeaderLength );
            
            if ( !ParseHeader( msgLen, uid ) )
                {
                TRACE("HandleCommsReaderNotify: parse error");
                // Protocol error, inform controller
                iObserver.HandleMessagingNotify( KErrGeneral );
                ResetReceiver();
                }
            else if ( ( iIncomingMsg = CEcmtMessageEvent::New( TUid::Uid( uid ), msgLen ) ) == 0 )
                {
                TRACE1("HandleCommsReaderNotify: insufficient memory (UID %x)", uid );
                // Not enough memory, inform controller
                iObserver.HandleMessagingNotify( KErrTooBig );
                ResetReceiver();
                }
            else
                {
                TRACE1("HandleCommsReaderNotify: UID %x", uid);
                iIncomingMsg->iPtr.Append( iHeader );
                iIncomingMsg->iPtr.SetLength( msgLen );
                
                iPayload.Set( iIncomingMsg->iData + KEcmtMessageHeaderLength, 
                			  0, 
                			  msgLen - KEcmtMessageHeaderLength );
                			  
                iCommsReader->IssueRead( iPayload );
                iReceiverState = EReceivingBody;
                }
            break;
            }
        
        case EReceivingBody:
            {
            TRACE1("HandleCommsReaderNotify: receiving body (UID %x)" , iIncomingMsg->iDest );
            for ( TInt i(0); i < iObserverUidList.Count(); i++ )
                {
                if ( iObserverUidList[i] == iIncomingMsg->iDest )
                    {
                    TRAP_IGNORE( iObserverList[i]->HandleNotifyL( *iIncomingMsg ) );
                    }
                }
            delete iIncomingMsg;
            iIncomingMsg = 0;

            iCommsReader->IssueRead( iHeader );
            iReceiverState = EReceivingHeader;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CEcmtMessaging::HandleCommsWriterNotify
//
// -----------------------------------------------------------------------------
//
void CEcmtMessaging::HandleCommsWriterNotify( TEcmtCommsObjectStatus aStatus )
    {
    switch (aStatus)
        {
            case EReady:
                if (iMessageIsOutgoing)
                    {
                        CEcmtMessageEvent* message = iMessageQue.Last();
                        iMessageQue.Remove(*message);
                        delete message;
                        iMessageIsOutgoing = EFalse;
                    }
                if (!iMessageQue.IsEmpty())
                    {
                        iCommsWriter->IssueWrite(iMessageQue.Last()->iPtr);
                        iMessageIsOutgoing = ETrue;
                    }
                break;

            case EInError:
                ResetReceiver();
                break;
        }
    }

//  End of File  
