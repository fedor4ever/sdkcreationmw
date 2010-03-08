/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for route plugin message receiver
*
*/

#ifndef ECMTSIMPSYPLUGIN_H
#define ECMTSIMPSYPLUGIN_H

//  INCLUDES
#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"

// CLASS DECLARATION

/**
*  Provides a message interceptor and handler for messages coming from the
*  SimPsyPlugin (Route) in epdt (previously ecmt)
*  
*  @lib ?library
*  @since ?Series60_version
*  @ingroup EcmtPlugins
*/
class CSimPsyPlugin : public CBase, public MEcmtPlugin, public MEcmtMessageEventObserver
	{
	public:
		/**
        * Destructor.
        */
		~CSimPsyPlugin( );

		/**
        * Two-phased constructor.
        */
		static MEcmtPlugin* NewL( );

		/**
        * Binds this object into a messaging interface.
        * @since ?Series60_version
        * @param MEcmtMessaging* object to bind to
        */
		void BindMessagingL( MEcmtMessaging* aMessaging );
		
	private:
		/**
        * Handles a notify message; the main functionality in this class.
        * The message should be from the Route plugin and is used to rewrite
        * the SimPSY configuration file.
        * @since ?Series60_version
        * @param const CEcmtMessageEvent& message event
        */	
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );
		
		/**
        * C++ default constructor.
        */
		CSimPsyPlugin();
		
		/**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

	private:
		//Cannot be a reference since it is given only after construction
		MEcmtMessaging* 	iMessaging;
		MEcmtMessageSender*	iMessageSender;
		TUid 				iUid;
		TUid 				iTargetUid;
	};
#endif

