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

#ifndef NCIPLUGIN_H
#define NCIPLUGIN_H

#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"

class CMsgSvr;

class CNCIPlugin : public CBase, public MEcmtPlugin
	{
	public:
		~CNCIPlugin( );
		static MEcmtPlugin* NewL( );
		void BindMessagingL( MEcmtMessaging* aMessaging );
		
	private:
		CNCIPlugin();
		void ConstructL();
		
	private:
		TUid 				iUid;
		TUid 				iTargetUid;
		CMsgSvr* iMsgSvr;
		
	};
	
#endif

