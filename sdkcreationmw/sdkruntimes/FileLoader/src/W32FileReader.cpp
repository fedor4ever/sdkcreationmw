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


// INCLUDE FILES
#include "ResourceHandle.h"
#include "W32FileConnection.h"
#include "W32FileReader.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CW32FileReader::CW32FileReader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CW32FileReader::CW32FileReader(HANDLE aFh, TInt aChunkSize, CW32FileConnection* aFileConnection) 
    : CActive( CActive::EPriorityHigh ), 
      iFh(aFh), m_chunkSize( aChunkSize ), m_fileConnection( aFileConnection ),
      m_chunkBufPtr( NULL, 0 ), m_stopReading( EFalse )
{
    CActiveScheduler::Add( this );
}

// -----------------------------------------------------------------------------
// CW32FileReader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CW32FileReader::ConstructL()
{
    m_chunkBuffer = HBufC8::NewL( m_chunkSize );
    //
    m_chunkBufPtr.Set( (TUint8*)m_chunkBuffer->Des().Ptr(), m_chunkSize, m_chunkSize );
}

// -----------------------------------------------------------------------------
// CW32FileReader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CW32FileReader* CW32FileReader::NewL(HANDLE aFh, TInt aChunkSize, CW32FileConnection* aFileConnection) 
{
    CW32FileReader* self = new( ELeave ) CW32FileReader( aFh, aChunkSize, aFileConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

    
// Destructor
CW32FileReader::~CW32FileReader()
{
    Cancel();
    delete m_chunkBuffer;
}

// -----------------------------------------------------------------------------
// CW32FileReader::StartReading
// 
// 
// -----------------------------------------------------------------------------
//
void CW32FileReader::StartReading()
{
    //
    m_filePos = 0;
    // start reading
    ReadFile();
}

// -----------------------------------------------------------------------------
// CW32FileReader::StopReading
// 
// 
// -----------------------------------------------------------------------------
//
void CW32FileReader::StopReading()
{
    m_stopReading = ETrue;
}

// -----------------------------------------------------------------------------
// CW32FileReader::GetChunkBuffer
// 
// 
// -----------------------------------------------------------------------------
//
void CW32FileReader::GetChunkBuffer( TPtrC8& aChunkBuffer )
{
    aChunkBuffer.Set( m_chunkBufPtr );
}


// -----------------------------------------------------------------------------
// CW32FileReader::DoCancel
// 
// 
// -----------------------------------------------------------------------------
//
void CW32FileReader::DoCancel()
{
/*    if (IsActive()) {
        m_file.ReadCancel(iStatus);
    }    */
}

// -----------------------------------------------------------------------------
// CW32FileReader::RunL
// 
// 
// -----------------------------------------------------------------------------
//
void CW32FileReader::RunL()
{
    // do not send the chunk back to the transaction if the trans has been cancelled
    if( !m_stopReading ) {
        switch( iStatus.Int() )
        {
            case KErrNone:
            {
                TInt bytesRead = m_chunkBufPtr.Length();
                // 0 means we read beyond the file 
                if( bytesRead ) {
                    // let the observer know. 
                    m_fileConnection->response();
                    // If it is not Browser content, reading the file
                    // is canceled in CW32FileConnection::download().
                    if( !m_stopReading ) {
                        // fix file pos
                        m_filePos+=bytesRead;
                        // keep on reading
                        ReadFile();
                    }
                }
                else {
                    // let the observer know
                    m_fileConnection->complete(KErrNone);
                }
                break;
            }
            default:
            {
                //
                m_fileConnection->complete( iStatus.Int() );
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// CW32FileReader::ReadFile
// 
// 
// -----------------------------------------------------------------------------
//
void CW32FileReader::ReadFile() 
{
  TUint32   bytesRead = -1; 
  TInt    readStatus = KErrNone;
  TUint8* buf = &m_chunkBufPtr[0];
  if( !m_stopReading ) {
      SetActive();
			::ReadFile(iFh, buf, m_chunkSize, &bytesRead, NULL);
  }
  m_chunkBufPtr.SetLength(bytesRead);
  readStatus =  Emulator::LastError();
  TRequestStatus* status = &iStatus;
  if( !m_stopReading ) {
  User::RequestComplete(status, readStatus);
 }
}


//  End of File   
