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
#ifndef ECMTTESTPLUGIN_H
#define ECMTTESTPLUGIN_H

#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"

/**
*  @ingroup EcmtPlugins
*/
class CTestPlugin : public CTimer, public MEcmtPlugin, public MEcmtMessageEventObserver
	{
	public:
		~CTestPlugin( );
		TUid GetUid( );
		static MEcmtPlugin* NewL( );
		void BindMessagingL( MEcmtMessaging* aMessaging );
		
	protected:
		void RunL();

	private:
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );
		CTestPlugin();
		void ConstructL();

	private:
		//Cannot be a reference since it is given only after construction
		MEcmtMessaging* 	iMessaging;
		MEcmtMessageSender*	iMessageSender;
		TUid 				iUid;
		TUid 				iTargetUid;
		TUint				iCount;
		TTimeIntervalMicroSeconds32 iInterval;
		
	};

#endif

