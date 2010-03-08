/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client interface for EcmtServer
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "EcmtClient.h"
#include "EcmtUids.h"
#include "EcmtClientServerCommon.h"
#include "EcmtServerListener.h"

_LIT(KEcmtServerFilename, "\\sys\\bin\\EcmtCore.exe");
const TInt KServerStartTimeout = 30000000; // 30 sec
const TInt KServerCheckTimeout = 1000000;  // 1 sec

// ============================= LOCAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// StartServer
// Starts the Ecmt server if it is not yet running.
// ---------------------------------------------------------------------------
static TInt StartServer()
{
    TFindServer findServer( KEcmtServerName );
    TFullName name;
    // Try to find Ecmt server
    TInt result = findServer.Next( name );

    if ( result != KErrNone )
    {
        // Server not yet running, so must launch it.
        // Create a semaphore that will be signaled by server when
        // it has completed its "boot" sequence.
        RSemaphore semaphore;
        result = semaphore.CreateGlobal( KEcmtSemaphoreName, 0 );

        if ( result == KErrNone || result == KErrAlreadyExists )
        {
            // Semaphore created succesfully (or it existed already)
            // so launch the server process.
            RProcess server;

            const TUid KEcmtServerUid = { KECMT_SERVER_UID };
            const TUidType serverUid( KNullUid, KNullUid, KEcmtServerUid );

            result = server.Create(KEcmtServerFilename, KNullDesC, serverUid);
            if (result == KErrNone)
            {
                server.Resume();

                TTime now, deadline;
                now.HomeTime();
                deadline.HomeTime();
                deadline += TTimeIntervalMicroSeconds32(KServerStartTimeout);
                result = KErrTimedOut;
                while (result == KErrTimedOut && now < deadline)
                {
                    result = (server.ExitType() == EExitPending) ?
                        semaphore.Wait(KServerCheckTimeout) : 
                        KErrGeneral;
                }

                server.Close();
            }
        }
        else
        {
            semaphore.Wait(KServerStartTimeout);
        }

        semaphore.Close();
    }
    return result;
}

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// REcmt::REcmt
// ---------------------------------------------------------------------------
EXPORT_C REcmt::REcmt( ) : iServerListener(0)
{
}

// ---------------------------------------------------------------------------
// REcmt::~REcmt
// ---------------------------------------------------------------------------
EXPORT_C REcmt::~REcmt( )
{
    delete iServerListener;
}

// ---------------------------------------------------------------------------
// REcmt::Connect
// ---------------------------------------------------------------------------
EXPORT_C TInt REcmt::Connect( )
{
    // Start server (unless it is already running)
    TInt error = StartServer();

    if (KErrNone == error)
    {
        error = CreateSession( KEcmtServerName,
                               TVersion( KEcmtServerMajorVersionNumber,
                                         KEcmtServerMinorVersionNumber,
                                         KEcmtServerBuildVersionNumber ) );
    }
    return error;
}

// ---------------------------------------------------------------------------
// REcmt::Close
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::Close( )
{
    if ( iServerListener )
    {
        delete iServerListener;
        iServerListener = 0;
    }
    RHandleBase::Close();
}

// ---------------------------------------------------------------------------
// REcmt::SetServerObserver
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::SetServerObserverL( MEcmtServerObserver* /* aObserver */ )
{
    User::Leave( KErrNotSupported );
}

// ---------------------------------------------------------------------------
// REcmt::SetServerObserver
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::SetServerObserverL( TUid aUid, MEcmtServerObserver* aObserver )
{
    iServerListener = new (ELeave) CEcmtServerListener( aObserver, *this ); 

    TIpcArgs messageParameters( aUid.iUid );
    SendReceive( EEcmtNotifyUid, messageParameters ); // ignore return code
}

// ---------------------------------------------------------------------------
// REcmt::RequestServerNotification
// ---------------------------------------------------------------------------
void REcmt::RequestServerNotification( TDes8& aDes, TRequestStatus& aStatus )
{
    TIpcArgs messageParameters( &aDes );
    SendReceive( EEcmtRequestServerNotification, messageParameters, aStatus );
}

// ---------------------------------------------------------------------------
// REcmt::CancelRequestServerMessage
// ---------------------------------------------------------------------------
void REcmt::CancelRequestServerNotification( )
{
    SendReceive( EEcmtCancelRequestServerNotification, TIpcArgs( ) );
}

// ---------------------------------------------------------------------------
// REcmt::Send
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::Send( const TUint uid, const TDesC8& aData )
{
    TIpcArgs messageParameters( uid, &aData );
    SendReceive( EEcmtRequestSendData, messageParameters );
}

// ---------------------------------------------------------------------------
// REcmt::Write
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::Write( const TDesC16& aText )
{
    TIpcArgs messageParameters( &aText );
    SendReceive( EEcmtRequestSendText, messageParameters );
}

// ---------------------------------------------------------------------------
// REcmt::WriteFormat
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::WriteFormat( TRefByValue<const TDesC16> aFmt, ... )
{
    VA_LIST list;
    VA_START( list, aFmt );

    TBuf16<KFormatMaxSize> buf;
    buf.FormatList( aFmt, list );
    Write( buf );
}

// ---------------------------------------------------------------------------
// REcmt::GetServer
// Returns thread local instance of REcmt client
// ---------------------------------------------------------------------------
REcmt* REcmt::GetServer()
{
    // Create a new REcmt reference for the static write functions, if a
    // reference has not been created earlier.
    REcmt* r = static_cast<REcmt*>(UserSvr::DllTls(KECMT_SERVER_UID));
    if (!r)
    {
        r = new REcmt();
        if (r)
        {
            TInt err = UserSvr::DllSetTls(KECMT_SERVER_UID, r);
            if (err == KErrNone)
            {
                r->Connect();
                if (err == KErrNone)
                {
                    return r;
                }

                // Cleanup
                UserSvr::DllSetTls(KECMT_SERVER_UID, NULL);
            }
            delete r;
            r = NULL;
        }
    }
    
    return r;
}

// ---------------------------------------------------------------------------
// REcmt::SWrite
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::SWrite(const TDesC16& aText)
{
    REcmt* r = GetServer();
    if (r)
    {
        r->Write(aText);
    }
}

// ---------------------------------------------------------------------------
// REcmt::SWriteFormat
// ---------------------------------------------------------------------------
EXPORT_C void REcmt::SWriteFormat(TRefByValue<const TDesC16> aFmt, ...)
{
    REcmt* r = GetServer();
    if (r)
    {
        VA_LIST list;
        VA_START( list, aFmt );
        r->WriteFormat(aFmt, list);
    }
}

// -----------------------------------------------------------------------------
// REcmt::SClose
// Deallocates thread local instance of REcmt client
// -----------------------------------------------------------------------------
EXPORT_C void REcmt::SClose()
{
    // Don't use GetServer() here because it may create a new REcmt instance,
    // which would be exactly opposite to what we are trying to do here.
    REcmt* r = static_cast<REcmt*>(UserSvr::DllTls(KECMT_SERVER_UID));
    if (r)
    {
        r->Close();
        delete r;
        UserSvr::DllSetTls(KECMT_SERVER_UID, NULL);
    }
}

//  End of File  
