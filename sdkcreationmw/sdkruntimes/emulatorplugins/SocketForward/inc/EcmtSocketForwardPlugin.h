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


#ifndef ECMTSOCKETFORWARDPLUGIN_H
#define ECMTSOCKETFORWARDPLUGIN_H

#include <e32base.h>
#include <w32std.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"
#include "SymbianSocket.h"

/**
*  @ingroup EcmtPlugins
*/
class CSocketForwardPlugin : public CActive, public MEcmtPlugin, public MEcmtMessageEventObserver
	{
	public:
		~CSocketForwardPlugin( );
		static MEcmtPlugin* NewL( );
		void BindMessagingL( MEcmtMessaging* aMessaging );
		void Remove( CSymbianSocket* aSocket );

	protected:
		// from CActive
		void	RunL();
		TInt	RunError( TInt aError );
		void	DoCancel();
		void	WritePortNumberL( TInt aPortNumber );
		
	private:
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );
		CSocketForwardPlugin();
		void ConstructL();
		void AcceptL();
		
	private:
    	enum TPluginState
    	{
    		EIdle,
			EAccepting
		};
	
		//Cannot be a reference since it is given only after construction
		MEcmtMessaging*					iMessaging;
		TUid							iUid;
		TPluginState					iState;
		RSocketServ						iSocketServer;
		RSocket							iSocket;
		RSocket*						iBlankSocket;
        RPointerArray< CSymbianSocket> 	iSocketList;
        RFs								iFs;
        RFile							iLogFile;
	};
#endif

