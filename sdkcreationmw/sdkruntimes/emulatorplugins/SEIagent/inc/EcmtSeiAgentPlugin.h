/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Socket Forward Plugin
*
*/


#ifndef ECMTSEIAGENTPLUGIN_H
#define ECMTSEIAGENTPLUGIN_H

#include <e32base.h>
#include <w32std.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"
#include "SocketContainerIf.h"

class CSocketListener;
class CSymbianSocket;

/**
*  @ingroup EcmtPlugins
*/
class CSeiAgentPlugin : public MEcmtPlugin, public MEcmtMessageEventObserver
	{
	public:
		~CSeiAgentPlugin( );
		static MEcmtPlugin* NewL( );
		void BindMessagingL( MEcmtMessaging* aMessaging );
		

	private:
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );
		CSeiAgentPlugin();
		void ConstructL();
		
		void StartSeiSlaveL(TDesC& aArguments);
		void KillSeiSlaveL();
		
		TInt FindFreePortL();
		
	
		/**
        * iSocketServ, the socket server.
        */
        RSocketServ iSocketServ;
        
        
        RProcess iProcess;
        
        
		MEcmtMessaging*						iMessaging;
		TUid								iUid;

	};
#endif

