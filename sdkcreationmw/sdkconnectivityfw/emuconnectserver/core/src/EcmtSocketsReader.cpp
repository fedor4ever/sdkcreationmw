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
#include "EcmtSocketsReader.h"
#include "EcmtSocketsEngine.h"
#include "EcmtBiSocket.h"

#define ECMT_TRACE_PREFIX "CEcmtSocketsReader::"
#include "EcmtServerDebugTrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::CEcmtSocketsReader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtSocketsReader::CEcmtSocketsReader( CEcmtSocketsEngine& aEngine ) :
    CActive(EPriorityStandard),
    iEngine( aEngine ),
    iTmpBuffer( 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtSocketsReader::ConstructL()
    {
    CActiveScheduler::Add( this ); 
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtSocketsReader* CEcmtSocketsReader::NewL( CEcmtSocketsEngine& aEngine )
    {
    CEcmtSocketsReader* self = new( ELeave ) CEcmtSocketsReader( aEngine );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtSocketsReader::~CEcmtSocketsReader()
    {
    
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::SetSocket
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsReader::SetSocket( CEcmtBiSocket* aSocket )
    {
    iSocket = aSocket;
    if ( iSocket && iObserver )
        {
        iObserver->HandleCommsReaderNotify( EReady );
        }
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::ReaderReady
// 
// -----------------------------------------------------------------------------
//
TBool CEcmtSocketsReader::ReaderReady( ) const
    {
    return ( iSocket != 0 && !IsActive() );
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::IssueRead
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsReader::IssueRead( TDes8& aData )
    {
    TRACE("IssueRead");
    iClientBuffer = &aData;
    iClientBuffer->Zero( );

	// A socket may return less data than requested, so use iTmpBuffer descriptor
	// to keep track of how much data we have really received so that we can
	// issue the read again if do no receive all data at once.
    iTmpBuffer.Set( const_cast<TUint8*>( aData.Ptr() ), 0, aData.MaxLength() ); 
    iStatus = KRequestPending;
    iSocket->Read( iTmpBuffer, iStatus );
    SetActive( );
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::SetReaderObserver
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsReader::SetReaderObserver( MEcmtCommsReaderObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CEcmtSocketsReader::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsReader::DoCancel( )
    {
    iSocket->CancelRead( );
    }
    
// -----------------------------------------------------------------------------
// CEcmtSocketsReader::RunL
// 
// -----------------------------------------------------------------------------
//
void CEcmtSocketsReader::RunL( )
    {
    TRACE1("RunL, status = %d", iStatus.Int());
    if ( iStatus == KErrNone )
        {
        // Update client buffer descriptor.
        iClientBuffer->SetLength( iClientBuffer->Length() + iTmpBuffer.Length() );

        // Check whether we received all data.
        if ( iClientBuffer->Length() == iClientBuffer->MaxLength() )
        	{
        	iObserver->HandleCommsReaderNotify( EReady );
        	}
        else
        	{
        	iTmpBuffer.Set( const_cast<TUint8*>( iClientBuffer->Ptr() ) + iClientBuffer->Length(),
        					0,
        					iClientBuffer->MaxLength() - iClientBuffer->Length( ) );
			iSocket->Read( iTmpBuffer, iStatus );
			SetActive( );        					
        	}
        }
    else
        {
        iObserver->HandleCommsReaderNotify( EInError );
        iEngine.ReportError( iStatus.Int() );
        }
    }

//  End of File  
