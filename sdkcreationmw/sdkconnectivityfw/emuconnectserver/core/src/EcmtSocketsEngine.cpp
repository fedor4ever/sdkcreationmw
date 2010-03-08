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
* Description:  Communication engine using sockets
*
*/


#include "EcmtSocketsEngine.h"
#include "EcmtSocketsReader.h"
#include "EcmtSocketsWriter.h"
#include "EcmtBiSocket.h"

#define ECMT_TRACE_PREFIX "CEcmtSocketsEngine::"
#include "EcmtServerDebugTrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::CEcmtSocketsEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtSocketsEngine::CEcmtSocketsEngine( MEcmtCommsEngineObserver& aObserver, 
                                        CEcmtBiSocketFactory& aFactory ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iSocketFactory( aFactory )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::ConstructL()
    {
    CActiveScheduler::Add( this ); 

    iListeningSocket = iSocketFactory.NewSocketL();
    iSocketsReader = CEcmtSocketsReader::NewL( *this );
    iSocketsWriter = CEcmtSocketsWriter::NewL( *this );
    
    LEAVE_IF_ERROR(iListeningSocket->Open(EFalse));
    LEAVE_IF_ERROR(iListeningSocket->Bind());
    LEAVE_IF_ERROR(iListeningSocket->Listen(0));

    // start accept
    this->AcceptL( );
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtSocketsEngine* CEcmtSocketsEngine::NewL( MEcmtCommsEngineObserver& aObserver,
                                              CEcmtBiSocketFactory& aFactory  )
    {
    CEcmtSocketsEngine* self = new( ELeave ) CEcmtSocketsEngine( aObserver, aFactory );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtSocketsEngine::~CEcmtSocketsEngine()
    {
    iEngineState = EExiting;
    Cancel();
    ResetConnection( );

    delete iSocketsReader;
    delete iSocketsWriter;
    
    if ( iListeningSocket )
        {
        iListeningSocket->Close( );
        delete iListeningSocket;
        }
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::ResetConnection
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::ResetConnection( )
    {
    if ( iEngineState != EExiting )
        {
        ChangeState( EClosing );
        }
    iSocketsReader->Cancel();
    iSocketsWriter->Cancel();
    if ( iSocket )
        {
        iSocket->Close( );
        }

    iSocketsReader->SetSocket( 0 );
    iSocketsWriter->SetSocket( 0 );

    delete iSocket;
    iSocket = 0;
    
    if ( iEngineState != EExiting )
        {
        Cancel();
        this->AcceptL( );
        }
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::AcceptL
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::AcceptL( )
    {
    TRACE("AcceptL");
    iSocket = iSocketFactory.NewSocketL();
    LEAVE_IF_ERROR(iSocket->Open(ETrue));

	iStatus = KRequestPending;
    iListeningSocket->Accept( *iSocket, iStatus );
    ChangeState( EAccepting );

    SetActive( );
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::Writer
// 
// -----------------------------------------------------------------------------
//
MEcmtCommsWriter& CEcmtSocketsEngine::Writer( ) const
    {
    return *iSocketsWriter;
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::Reader
// 
// -----------------------------------------------------------------------------
//
MEcmtCommsReader& CEcmtSocketsEngine::Reader( ) const
    {
    return *iSocketsReader;
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::ParameterString
// 
// -----------------------------------------------------------------------------
//
const TPtrC CEcmtSocketsEngine::ParameterString( ) const
    {
    return iListeningSocket ? iListeningSocket->ParameterString( ) : KNullDesC().Ptr();
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::AddressFamily
// 
// -----------------------------------------------------------------------------
//
const TPtrC CEcmtSocketsEngine::AddressFamily( ) const
    {
    return iListeningSocket ? iListeningSocket->AddressFamily( ) : KNullDesC().Ptr();
    }
   

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::DoCancel( )
    {
    iListeningSocket->CancelAccept( );    
    }
    
// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::RunL
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::RunL( )
    {
    TRACE1("RunL: status = %d", iStatus.Int());

    if ( iStatus == KErrNone )
        {
        iSocketsReader->SetSocket( iSocket );
        iSocketsWriter->SetSocket( iSocket );
        ChangeState( EConnected );
        }
    else
        {
        if ( iEngineState != EExiting )
            {
            ResetConnection( );
            }
        }
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::ChangeState
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::ChangeState( TSocketsEngineState aNewState )
    {
    if ( aNewState == EConnected )
        {
        iObserver.HandleCommsEngineNotify( EReady, KErrNone );
        }

    iEngineState = aNewState;
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsEngine::ReportError
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsEngine::ReportError( TInt aError )
    {
    iObserver.HandleCommsEngineNotify( EInError, aError );
    ResetConnection( );
    }

//  End of File  
