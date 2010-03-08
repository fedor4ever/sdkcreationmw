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
* Description:  Class implementing tool core and Symbian OS Server functionality
*
*/



#ifndef ECMTSERVER_H
#define ECMTSERVER_H

//  INCLUDES
#include "EcmtPlugin.h"
#include "EcmtServer.pan"
#include "EcmtSocketsEngine.h"
#include "EcmtMessaging.h"

#include <e32svr.h>
#include <ecom.h>

//Needed to send idle to backgound
#include <e32wins.h>
#include <apgtask.h>
#include <w32std.h>

// FORWARD DECLARATIONS
class CEcmtBiSocketFactory;

// CONSTANTS
const TInt KMaxClientMsgLen = 64;

/**
*  @defgroup EcmtCore Core
*  The core component is responsible for: 
*  <ul>
*  <li>initialization and control of the system,</li>
*  <li>message delivery and message queuing services,</li>
*  <li>initializing and controlling the network connection,</li>
*  </ul>
*/

// CLASS DECLARATION

/**
*  Core controller of Ecmt.
*  
*  This class handles the loading of plugins and creates/owns the other system
*  components.
*
*  @ingroup EcmtCore
*/
class CEcmtServer : public CServer2, 
                    public MEcmtCommsEngineObserver,
                    public MEcmtMessagingObserver,
					public MEcmtMessageEventObserver
    {
    public:  // Constructors and destructor
    
	    /**
	    * Two-phased constructor.
	    */
	    static CEcmtServer* NewLC();
	    
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtServer();

    public: // New functions
    
	    /**
	    * Main function of server thread for RThread::Create. See TThreadFunction.
	    *
	    * @param aPtr defined from TThreadFunction but not used
	    */
        static TInt ThreadFunction( TAny* aPtr );
        
        /**
        * Session counting for automatic unloading (public for CEcmtServerSession)
        */
        void IncrementSessions();
        /**
        * Session counting for automatic unloading (public for CEcmtServerSession)
        */
        void DecrementSessions();
        
        /**
        *
        */
		const TDesC8& LastClientNotification( ) const;        

        /**
        *
        */
        MEcmtMessaging* MessagingIf();
            
    public: // Functions from base classes 
        /**
        * From MEcmtCommsEngineObserver
        */
        void HandleCommsEngineNotify( TEcmtCommsObjectStatus aStatus, TInt aCode );

        /**
        * From MEcmtMessagingObserver
        */
        void HandleMessagingNotify( TInt aCode );

		/**
		* From MecmtMessageEventObserver
		*/ 
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );

    protected:    // Functions from base classes 
    
	    /**
	    * Inherited from CServer for error processing
	    *
	    * @param aError the leave code reported.
	    * @return KErrNone if leave is handled.
	    */
	    TInt RunError(TInt aError);

	private:

	    /**
	    * C++ default constructor.
	    */
	    CEcmtServer();

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();
    
    	/**
    	* Second stage startup for the server thread called from
    	* ThreadFunction inside a trap harness.
    	*/
        static void ThreadMainL();

        /**
        * Panic the client (typically due to an invalid request)
        * @param aMessage the message channel to the client
        * @param aReason the reason code for the panic.
        */
        static void PanicClient( const RMessage2& aMessage, TEcmtServPanic aReason );

        /**
        * Panic the server if startup fails
        * @param aPanic the panic code
          */
        static void PanicServer( TEcmtServPanic aPanic );
        
        /**
        * Initialize cleanupstack for heap checking. See SOS C++ for Mobibile... p. 677
        */
        static void InitCleanupStack();
        
		/**
		* Plugins can be loaded either at core startup or when connection
		* is established
		*/    
    	enum TPluginLoadMode
			{
			EStartupLoad,
			EConnectLoad
			};
		/**
		* Plugin can specify in their opaque_data field of the .rss file some
		* parameters that are represented with the following enum.
		*/
		enum TPluginOpaqueData
			{
			/// +1 = Load plugin already at server startup
			ELoadAtStartup = 1,	
			/// +2 = Skip plugin in launcher mode
			ESkipInLauncherMode = 2
			};
		/**
        *
        */
        void LoadPlugins( TPluginLoadMode aMode );
        MEcmtPlugin* LoadPluginL( TUid aPluginUid );
        
        HBufC* DefaultBearerL() const;
        
        /**
        *
        */
        void NotifyGui( const TDesC& aFirstLine );
        void NotifyGui( const TDesC& aFirstLine, TDesC& aSecondLine );
        
        /**
        *
        */
        CEcmtBiSocketFactory* LoadBearerL();
        
        /**
        * The following function sends Idle app to background,
        * making the application being launched from command line
        * seen in the foreground
        * The function has nothing to do with EcmtCore
        * The reasos why it's here are:
        * * I couldn't find any better place
        * * Ecmt starts as one of the last applications, after idle
        *   thus it can send idle to backround	
        */
        void SendIdleApplicationToBackground();
        
    private:    // From CServer
    	/**
    	* Inherited from CServer for client session management
    	*
    	* @param aVersion version information supplied by the client
    	* @return pointer to the newly created server-side client session
    	*/
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& ) const;

    private:    // Data
        RImplInfoPtrArray		iPluginInfoList;
        RArray< TUid >			iPluginDtorUidList;
        RPointerArray< MEcmtPlugin > iPluginList;
        CEcmtBiSocketFactory*		iSocketFactory;
        CEcmtSocketsEngine*		iCommsEngine;
        CEcmtMessaging*			iMessaging;
        TBool					iAllPluginsLoaded;
        TBool					iLauncherMode;
        TInt					iSessionCount;
        TBuf8<KMaxClientMsgLen>	iClientNotification;
        TUid					iSocketFactoryDtorUid;
    };

#endif      // ECMTSERVER_H   
        
// End of File
