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


#ifndef ECMTSEIFORWARDPLUGIN_H
#define ECMTSEIFORWARDPLUGIN_H

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
class CSeiForwardPlugin : public MEcmtPlugin, public MEcmtMessageEventObserver, public MSocketContainer
	{
	public:
		~CSeiForwardPlugin( );
		static MEcmtPlugin* NewL( );
		void BindMessagingL( MEcmtMessaging* aMessaging );
		
	public:
		void Remove( CSymbianSocket* aSocket );
		void ForwardL( CSymbianSocket* aSocket, TPtrC8 aMessage );
		void ForwardL( CSocketListener* aListener, TPtrC8 aMessage );

	private:
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );
		CSeiForwardPlugin();
		void ConstructL();
		void ConnectL( TInt aChannel, TInt aPort );
		void ListenL( TInt aChannel );
		void SendMessageL( TInt aChannel, TPtrC8 aMessage );
		void ReceiveMessageL( TInt aChannel, TPtrC8 aMessage );
		void AcceptMessageL( TInt aChannel, TInt aPort );
		
	private:
    	enum TPluginState
    	{
    		EIdle,
			EAccepting
		};
	
		MEcmtMessaging*						iMessaging;
		TUid								iUid;
		RSocketServ							iSocketServer;
		RPointerArray< CSocketListener >	iListenerList;
        RPointerArray< CSymbianSocket >		iSocketList;
        RArray< TInt >						iListenerChannelList;
        RArray< TInt >						iSocketChannelList;
	};
#endif

