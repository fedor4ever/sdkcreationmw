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
* Description:  Symbian OS Server functionality
*
*/


// INCLUDE FILES
#include "EcmtServer.h"
#include "EcmtServerSession.h"
#include "EcmtServer.pan"
#include "EcmtUids.h"    
#include "EcmtSocketsEngine.h"
#include "EcmtBiSocket.h"
#include "EcmtClientServerCommon.h"
#include "EcmtServerObserver.h"
#include "EcmtServerDebugTrace.h"
#include "DebugLogging.h"

#ifdef __WINS__
#include "EcmtOsUtils.h"
#endif

// ERROR MESSAGES
_LIT( KEcmtBearerError, "Bearer error %d" );
_LIT( KEcmtEngineError, "Engine error %d" );
_LIT( KEcmtMessagingError, "Messaging error %d" );
_LIT( KEcmtObserverError, "Observer error %d" );


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CEcmtServer::CEcmtServer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CEcmtServer::CEcmtServer() : CServer2( EPriorityNormal )
    {
    }

// ---------------------------------------------------------------------------
// CEcmtServer::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CEcmtServer::ConstructL()
    {
    TBuf<KMaxClientMsgLen> errorBuf;

    TRAPD(err, iSocketFactory = LoadBearerL());
    
    __LOGWRITE1("CEcmtServer::ConstructL - Bearers load error:%d",err);

    if (err != KErrNone)
    {
        TRACE1("ConstructL: bearers load error %d", err);
	errorBuf.Format( KEcmtBearerError, err);
    }

    if (err == KErrNone)
    {
	TRAP(err, iCommsEngine = CEcmtSocketsEngine::NewL( *this, *iSocketFactory ));

	if (err != KErrNone)
	{
            TRACE1("ConstructL: CEcmtSocketsEngine error %d", err);
	    errorBuf.Format( KEcmtEngineError, err);
        }
    }

    if (err == KErrNone)
    {
	// For some reason, EcmtServer crashes on device if both Reader and 
	// Writer are passed in the constructor. They now have separate setter
	// methods for ARMV5.
	TRAP(err, iMessaging = CEcmtMessaging::NewL( *this ));
	if (err != KErrNone)
        {
            TRACE1("ConstructL: CEcmtMessaging error %d", err);
	    errorBuf.Format( KEcmtMessagingError, err);
        }
	else
	{
	    iMessaging->SetReader(&iCommsEngine->Reader());
	    iMessaging->SetWriter(&iCommsEngine->Writer());
        }
    }

    // Allow EcmtServer itself to receive messages.
    if (err == KErrNone)
    {
	TRAP(err, iMessaging->AddMessageEventObserverL( *this, TUid::Uid( KECMT_SERVER_UID ) ));

	if (err != KErrNone)
        {
            TRACE1("ConstructL: AddMessageEventObserverL error %d", err);
	    errorBuf.Format( KEcmtObserverError, err);
        }
    }

    // Load plugins

#ifdef __WINS__
    iLauncherMode = EcmtOsUtils::LauncherMode();
#endif

    REComSession::ListImplementationsL( TUid::Uid(KECMT_PLUGIN_IF_UID), iPluginInfoList);
    TRACE("ConstructL: starting server");
    StartL( KEcmtServerName );

    // Notify GUI of the server startup status.
    if ( err == KErrNone )
    {
	LoadPlugins( EStartupLoad );
	NotifyGui( KEcmtServerListening );
    }
    else 
    {
	NotifyGui( KEcmtServerError, errorBuf );
    }

#ifdef __WINS__
    // In emulator, Ecmt Server should always be running. If the session
    // counter is not increased, client applications using REcmt API would
    // cause Ecmt Server to exit upon client disconnect.
    //
    // NOTE that even though EcmtMenubar creates an REcmt session and never
    // disconnects it, the session does not get created until the first time
    // user pulls down the Tools menu. Something (such as HTTP traffic plugin)
    // can establish ans close the session before that has happened.
	IncrementSessions();
	
	SendIdleApplicationToBackground();
#endif
    }

// ---------------------------------------------------------------------------
// CEcmtServer::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CEcmtServer* CEcmtServer::NewLC()
    {
    CEcmtServer* self = new( ELeave ) CEcmtServer;

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CEcmtServer::~CEcmtServer
// Destructor
// ---------------------------------------------------------------------------
//
CEcmtServer::~CEcmtServer()
    {
    TRACE("~CEcmtServer");
    // Delete all plugins and destroy the list
    iPluginList.ResetAndDestroy();
    // Tell ECom we deleted our plugins   
    for ( TInt i(0); i < iPluginDtorUidList.Count(); i++ )
        {
    	REComSession::DestroyedImplementation( iPluginDtorUidList[ i ] );
        }

    iPluginDtorUidList.Reset();
    
    // If not all plugins were loaded, then iPluginInfo list has not yet
    // been deleted
    if ( !iAllPluginsLoaded )
        {
        iPluginInfoList.ResetAndDestroy();
        }
   
    delete iMessaging;
    delete iCommsEngine;
    delete iSocketFactory;
    REComSession::DestroyedImplementation( iSocketFactoryDtorUid );
    REComSession::FinalClose();
    TRACE("~CEcmtServer done. Good bye, cruel world!");
    TRACE_DEINIT();
    }

// ---------------------------------------------------------------------------
// CEcmtServer::LoadBearerL
// 
// ---------------------------------------------------------------------------
//
CEcmtBiSocketFactory* CEcmtServer::LoadBearerL()
	{
	__LOGWRITE("CEcmtServer::LoadBearerL()");
	
    const TUid KEcmtBearerIfUid = { KECMT_BEARER_IF_UID };
    RImplInfoPtrArray bearerInfoList;
    CleanupClosePushL( bearerInfoList );

    REComSession es = REComSession::OpenL();
    es.ListImplementationsL( KEcmtBearerIfUid, bearerInfoList );

    TInt count = bearerInfoList.Count();
    TRACE1("LoadBearerL found %d bearer(s)",count);
    if ( count == 0 )
    	{
        PanicServer( ENoBearers );
    	}
    	
    TUid implUid = TUid::Null();
    	
#ifndef __WINS__
	HBufC* defaultBearer = DefaultBearerL();
	
    //If default bearer was not found, load Bluetooth bearer.    
    if( !defaultBearer )	
        {
        defaultBearer = HBufC::NewLC( KEcmtBluetooth().Length() );
        defaultBearer->Des().Append( KEcmtBluetooth() );        
        __LOGWRITE("CEcmtServer::LoadBearerL() - No default bearer, use Bluetooth" );     
        }
    else 
        {     
        CleanupStack::PushL( defaultBearer );        
        __LOGWRITE1("CEcmtServerLoadBearerL() - Default bearer: %d", implUid.iUid);        
        }
    for(TInt i=0; i<count; ++i)
    	{
    	TPtrC bearerNamePtr = bearerInfoList[i]->DisplayName();
    	if( bearerNamePtr.Find( *defaultBearer ) != KErrNotFound )
    	    {
    	    //Default bearer name found from one Ecom implementation name.
    	    implUid = bearerInfoList[i]->ImplementationUid();
    	    __LOGWRITE1("CEcmtServerLoadBearerL() - Default bearer implementation found: %d", implUid.iUid);    	
    	    break;    
    	    }    		
    	}
    
    //Error situation, we didn't found bearer based on name, load first implementation
    if( implUid.iUid == NULL )
        {
        implUid = bearerInfoList[0]->ImplementationUid();
    	__LOGWRITE1("CEcmtServerLoadBearerL() - Error, default bearer implementation not found. Loading: %d", implUid.iUid);  
        }
        	
   	CleanupStack::PopAndDestroy( defaultBearer );

#else //__WINS__ defined 

    implUid = bearerInfoList[0]->ImplementationUid();      

#endif // #ifndef __WINS__

    bearerInfoList.ResetAndDestroy();
    CleanupStack::Pop( );

    TAny* impl = es.CreateImplementationL( implUid, iSocketFactoryDtorUid );
    __LOGWRITE("CEcmtServer::LoadBearerL() - Implementation created");
    es.Close();
    return static_cast<CEcmtBiSocketFactory*> (impl);
	}

// ---------------------------------------------------------------------------
// CEcmtServer::DefaultBearerL
// 
// ---------------------------------------------------------------------------
//
HBufC* CEcmtServer::DefaultBearerL() const
	{	        	
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );
    
    RFile file;
    
    TInt err=file.Open( fsSession, KEcmtDefBearerFileName, EFileStreamText|EFileWrite|EFileShareAny );
    if( err != KErrNone )
    	{
    	CleanupStack::PopAndDestroy();
    	return NULL;
    	}
    	
    CleanupClosePushL( file );
    TBuf8<20> bearerData;      
    file.Read(0, bearerData);
    
    CleanupStack::PopAndDestroy( 2 );
    
    if( bearerData.Length() < 1 )
        return NULL;
    
    _LIT8( KLinebreak, "\n" );
    TInt offset = bearerData.Find( KLinebreak() );
    
    HBufC* buf = NULL;
    TInt length = bearerData.Length();
    
    if( offset != KErrNotFound )
        {
        TInt newLength = length-( length-offset );
        buf = HBufC::NewL( newLength );
        buf->Des().Copy( bearerData.Left( newLength ) );
        }
    else
        {
        buf = HBufC::NewL( length );
        buf->Des().Copy( bearerData );
        }
    
    return buf;
	}


// ---------------------------------------------------------------------------
// CEcmtServer::LoadPlugins
// 
// ---------------------------------------------------------------------------
void CEcmtServer::LoadPlugins( TPluginLoadMode aMode )
    {
    TRACE("LoadPlugins");
    ASSERT(!iAllPluginsLoaded);

	for ( TInt i(0); i < iPluginInfoList.Count(); i++ )
        {
    	TRACE2("LoadPlugins: i = %d, uid = %08x" , i,
            iPluginInfoList[ i ]->ImplementationUid() );

    	TLex8 lex( iPluginInfoList[ i ]->OpaqueData() );
    	TInt opaqueData;
    	lex.Val( opaqueData );

    	if ( iLauncherMode && ( opaqueData & ESkipInLauncherMode ) ||
        	aMode == EStartupLoad && !( opaqueData & ELoadAtStartup ) ||
        	aMode == EConnectLoad &&  ( opaqueData & ELoadAtStartup ) )
            {
            // Do not load a plugin if:
            // 1) We are in launcher mode and ESkipInLauncherMode is specified,
            // 2) Core is starting and ELoadAtStartup has not been specified
            // 3) We got a connection and ELoadAtStartup is specified (because
            //    the plugin was already loaded.
            TRACE1("LoadPlugins: Plugin %d NOT loaded", i );
            }
    	else
            {
        	TRAPD(err, LoadPluginL(iPluginInfoList[i]->ImplementationUid()));
                if (err == KErrNone)
                    {
                    TRACE1("LoadPlugins: Plugin %d loaded", i );
                    }
                else
                    {
                    TRACE2("LoadPlugins: Plugin %d load failed, err %d",i,err);
                    }
            }
        }

    if ( aMode == EConnectLoad )
        {
        iAllPluginsLoaded = ETrue;
        // Free iPluginInfoList since it is no longer needed
        iPluginInfoList.ResetAndDestroy();
    	TRACE("LoadPlugins: All plugins loaded");
        }
	else 
        {
    	TRACE("LoadPlugins: All plugins NOT loaded");
        }
    }

// ---------------------------------------------------------------------------
// CEcmtServer::LoadPluginL
// Loads a single plugin. Leaves on failure
// ---------------------------------------------------------------------------
MEcmtPlugin* CEcmtServer::LoadPluginL( TUid aPluginUid )
{
    TUid dtorUid;
    TAny* impl = REComSession::CreateImplementationL( aPluginUid, dtorUid );
    MEcmtPlugin* plugin = static_cast< MEcmtPlugin* > ( impl );
    TInt err = iPluginList.Append(plugin);

    if (err == KErrNone)
    {
        err = iPluginDtorUidList.Append(dtorUid);
	
        if (err != KErrNone)
        {
            iPluginList.Remove(iPluginList.Count()-1);
        }
    }
    
    if (err != KErrNone)
    {
        delete plugin;
	
    	REComSession::DestroyedImplementation( dtorUid );
        LEAVE(err);
    }
    else
    {
        TRAP(err, plugin->BindMessagingL( iMessaging ));

        if (err != KErrNone)
        {
            iPluginDtorUidList.Remove(iPluginDtorUidList.Count()-1);
            iPluginList.Remove(iPluginList.Count()-1);
	    
            delete plugin;
	    
            REComSession::DestroyedImplementation( dtorUid );
            LEAVE(err);
        }
    }
	
    return plugin;
}

// ---------------------------------------------------------------------------
// CEcmtServer::HandleCommsEngineNotify
// 
// ---------------------------------------------------------------------------
void CEcmtServer::HandleCommsEngineNotify( TEcmtCommsObjectStatus aStatus, TInt /*aCode*/ )
    {
    TRACE("HandleCommsEngineNotify");

    if ( aStatus == EReady )
        {
        if ( !iAllPluginsLoaded )
            {
            TRAPD( err, LoadPlugins( EConnectLoad ) );
            if (err != KErrNone)
                {
        	TRACE1("HandleCommsEngineNotify: error = %d", err);
                }
            }

        NotifyGui( KEcmtServerConnected );
        }
    else
        {
        NotifyGui( KEcmtServerDisconnected );
        }
    }

// ---------------------------------------------------------------------------
// CEcmtServer::HandleMessagingNotify
// 
// ---------------------------------------------------------------------------
void CEcmtServer::HandleMessagingNotify( TInt aCode )
    {
    TRACE("HandleMessagingNotify");

    if ( aCode != KErrNone )
        {
        iCommsEngine->ResetConnection();
        }
    }

// ---------------------------------------------------------------------------
// CEcmtServer::NewSessionL
// Inherited from CServer2 for client session management
// ---------------------------------------------------------------------------
//
CSession2* CEcmtServer::NewSessionL( const TVersion& aVersion, 
                                     const RMessage2& ) const
{
    TRACE("NewSessionL");

    // Check we're the right version
    if (!User::QueryVersionSupported(TVersion(KEcmtServerMajorVersionNumber,
                                              KEcmtServerMinorVersionNumber,
                                              KEcmtServerBuildVersionNumber ),
                                              aVersion))
    {
        LEAVE(KErrNotSupported);
    }

    // Make new session
    CEcmtServerSession* session = CEcmtServerSession::NewL( );
    const_cast< CEcmtServer* >(this)->IncrementSessions();
    return session;
}

// ---------------------------------------------------------------------------
// CEcmtServer::RunError
// Inherited from CServer2 for client session management
// ---------------------------------------------------------------------------
TInt CEcmtServer::RunError( TInt aError )
    {
    if (aError == KErrBadDescriptor)
        {
        // A bad descriptor error implies a badly programmed client, so panic it;
        // otherwise report the error to the client
        PanicClient( Message(), EBadDescriptor );
        }
    else
        {
        Message().Complete( aError );
        }

    // The leave will result in an early return from CServer2::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    return KErrNone;    // Handled the error fully
    }

// ---------------------------------------------------------------------------
// CEcmtServer::ThreadMainL
// Second stage startup for the server thread called from
// ThreadFunction inside a trap harness.
// ---------------------------------------------------------------------------
//
void CEcmtServer::ThreadMainL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( activeScheduler ) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Construct our server
    CEcmtServer::NewLC();

    // Signal sempahore created by client (if any)
    RSemaphore semaphore;

    if ( semaphore.OpenGlobal( KEcmtSemaphoreName ) == KErrNone )
        {
    	semaphore.Signal( );
    	semaphore.Close( );
        }

    // Start handling requests 
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2, activeScheduler );
    }

// ---------------------------------------------------------------------------
// CEcmtServer::InitCleanupStack
// Initialize cleanupstack for heap checking. See SOS C++ for Mobile... p. 677
// ---------------------------------------------------------------------------
void CEcmtServer::InitCleanupStack()
    {
    TRAP_IGNORE(
        for ( TInt i=0; i<20; i++ ) 
            {
            CleanupStack::PushL( ( TAny* ) 0 );
            }
        CleanupStack::Pop( 20 );
        );
    }

// ---------------------------------------------------------------------------
// CEcmtServer::ThreadFunction
// Startup function for RThread/RProcess::Create. Cannot leave.
// ---------------------------------------------------------------------------

TInt CEcmtServer::ThreadFunction( TAny* /*aNone*/ )
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

#ifdef _DEBUG
    InitCleanupStack();
#endif
    
    __UHEAP_MARK;
    
    if ( !( cleanupStack ) )
        {
    	PanicServer( ECreateTrapCleanup );
        }

    TRAPD( err, ThreadMainL() );
    if (err != KErrNone)
        {
    	PanicServer( ESrvCreateServer );
        }

    delete cleanupStack;
    cleanupStack = NULL;

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CEcmtServer::PanicClient
// Panic the client (typically due to an invalid request)
// ---------------------------------------------------------------------------
void CEcmtServer::PanicClient( const RMessage2& aMessage, TEcmtServPanic aReason )
    {
    TRACE1("PanicClient %d",aReason);
    aMessage.Panic( KCEcmtServer, aReason );
    }

// ---------------------------------------------------------------------------
// CEcmtServer::PanicServer
// Panic the server if startup fails
// ---------------------------------------------------------------------------
void CEcmtServer::PanicServer( TEcmtServPanic aPanic )
    {
    TRACE1("PanicServer %d", aPanic);
    User::Panic(KCEcmtServer, aPanic);
    }
// ---------------------------------------------------------------------------
// CEcmtServer::IncrementSessions
// Sesison counting
// ---------------------------------------------------------------------------
void CEcmtServer::IncrementSessions()
    {
    iSessionCount++;
    }

// ---------------------------------------------------------------------------
// CEcmtServer::DecrementSessions
// Sesison counting
// ---------------------------------------------------------------------------
void CEcmtServer::DecrementSessions()
    {
    iSessionCount--;
    
    // When last client closes session, we shut ourselves down
    if ( iSessionCount <= 0 )
        {
    	CActiveScheduler::Stop();
        }
    }

// ---------------------------------------------------------------------------
// CEcmtServer::NotifyGui
// 
// ---------------------------------------------------------------------------
void CEcmtServer::NotifyGui( const TDesC& aFirstLine )
    {
    TBuf<KMaxClientMsgLen> buf;

    buf.Append( iCommsEngine->AddressFamily() );
    buf.Append( ' ' );
    buf.Append( iCommsEngine->ParameterString() );

    NotifyGui( aFirstLine, buf );
    }

// ---------------------------------------------------------------------------
// CEcmtServer::NotifyGui
// 
// ---------------------------------------------------------------------------
void CEcmtServer::NotifyGui( const TDesC& aFirstLine, TDesC& aSecondLine )
{
    TRACE("EcmtServer::NotifyGui");

    iClientNotification.Zero();
    iClientNotification.Append( aFirstLine );
    iClientNotification.Append( '\t' );
    iClientNotification.Append( aSecondLine );

#ifdef ECMT_RDEBUG
    TBuf<KMaxClientMsgLen> buf;
    buf.Copy(iClientNotification);
    TRACE1("NotifyGui: message = %S", &buf);
#endif

    iSessionIter.SetToFirst();
    
    CEcmtServerSession* session = static_cast<CEcmtServerSession*>( iSessionIter++ );
    
    while ( session )
    {
        TRACE1("NotifyGui: session UID = %x", session->GetClientUid().iUid );
        session->NotifyClient( iClientNotification );
        session = static_cast<CEcmtServerSession*>( iSessionIter++ );
    }
}

// ---------------------------------------------------------------------------
// CEcmtServer::LastGuiNotification
// 
// ---------------------------------------------------------------------------
const TDesC8& CEcmtServer::LastClientNotification( ) const
    {
   return iClientNotification;
    }

// ---------------------------------------------------------------------------
// CEcmtServer::MessagingIf
// 
// ---------------------------------------------------------------------------
MEcmtMessaging* CEcmtServer::MessagingIf( )
    {
	return iMessaging;
    }

// ---------------------------------------------------------------------------
// CEcmtServer::HandleNotifyL
// 
// ---------------------------------------------------------------------------
//
void CEcmtServer::HandleNotifyL( const CEcmtMessageEvent& /* aEvent */ )
    {
    TRACE("HandleNotifyL");
    // Handle messages meant for Ecmt Server core.
    }

//
//
//        * The following function sends Idle app to background,
//        * making the application being launched from command line
//        * seen in the foreground
//        * The function has nothing to do with EcmtCore
//        * The reasos why it's here are:
//        * * I couldn't find any better place
//        * * Ecmt starts as one of the last applications, after idle
//        *   thus it can send idle to backround	

void CEcmtServer::SendIdleApplicationToBackground()
{

#ifdef __WINS__
 /* The code for possible future use:
    const char* autorun = EmulatorAutoRun();
    TPtrC8 appName = TPtrC8((const TText8*)autorun);
    HBufC16* buf16 = HBufC16::NewL(100);
	//buf16->Des().Copy(buf8->Des());
	buf16->Des().Copy(appName);
	*/
	if(EmulatorAutoRun())
	{
	    RWsSession wsSession;
	    wsSession.Connect();
	    TApaTaskList list(wsSession);
	    //We could identify idle by app. caption
	    //although Uid is preferrable
	    //because of localisation
	   	//_LIT(idleNameLit, "Idle");
		//TBufC<25> idleName(idleNameLit);
		// TApaTask task = list.FindApp(idleName);
	    TUid idleUid;
	    idleUid.iUid = 0x101FD64C;
	    //list.CycleTasks(idleUid);
		TApaTask task = list.FindApp(idleUid);
		task.SendToBackground();           
	}
#endif

}


// ---------------------------------------------------------------------------
// Server module only export for startup. Different between WINS and target. 
// ---------------------------------------------------------------------------
TInt E32Main()
    {
    TRACE("E32Main");
    return CEcmtServer::ThreadFunction( NULL );
    }
    
    
//  End of File  
