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
* Description:  Active object performing read operations on a socket
*
*/



// INCLUDE FILES
#include "EcmtSocketsWriter.h"
#include "EcmtSocketsEngine.h"
#include "EcmtBiSocket.h"

#define ECMT_TRACE_PREFIX "CEcmtSocketsEngine::"
#include "EcmtServerDebugTrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::CEcmtSocketsWriter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtSocketsWriter::CEcmtSocketsWriter( CEcmtSocketsEngine& aEngine ) :
    CActive(EPriorityStandard),
    iEngine( aEngine )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtSocketsWriter::ConstructL()
    {
    CActiveScheduler::Add( this ); 
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtSocketsWriter* CEcmtSocketsWriter::NewL( CEcmtSocketsEngine& aEngine )
    {
    CEcmtSocketsWriter* self = new( ELeave ) CEcmtSocketsWriter( aEngine );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtSocketsWriter::~CEcmtSocketsWriter()
    {
    
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::SetSocket
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsWriter::SetSocket( CEcmtBiSocket* aSocket )
    {
    iSocket = aSocket;
    if ( iSocket && iObserver )
        {
        iObserver->HandleCommsWriterNotify( EReady );
        }
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::WriterReady
// 
// -----------------------------------------------------------------------------
//
TBool CEcmtSocketsWriter::WriterReady( ) const
    {
    return ( iSocket != 0 && !IsActive() );
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::IssueWrite
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsWriter::IssueWrite( const TDesC8& aData )
    {
    TRACE("EcmtSocketsWriter::IssueWrite");
    iSocket->Write( aData, iStatus );
    SetActive( );
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::SetWriterObserver
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsWriter::SetWriterObserver( MEcmtCommsWriterObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsWriter::DoCancel( )
    {
    iSocket->CancelWrite( );
    }
    
// -----------------------------------------------------------------------------
// CEcmtSocketsWriter::RunL
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsWriter::RunL( )
    {
    TRACE1("RunL: status = %d", iStatus.Int());
    if ( iStatus == KErrNone )
        {
        iObserver->HandleCommsWriterNotify( EReady );
        }
    else
        {
        iObserver->HandleCommsWriterNotify( EInError );
        iEngine.ReportError( iStatus.Int() );
        }
    }

//  End of File  
