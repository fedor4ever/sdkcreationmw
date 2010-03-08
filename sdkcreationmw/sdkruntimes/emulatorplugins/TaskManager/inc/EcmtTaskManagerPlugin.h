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
* Description:  Task Manager Plugin
*
*/


#ifndef ECMTTASKMANAGERPLUGIN_H
#define ECMTTASKMANAGERPLUGIN_H

#include <e32base.h>
#include <w32std.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"
#include "MovingAverage.h"

/**
*  @ingroup EcmtPlugins
*/
class CTaskManagerPlugin : public CTimer, public MEcmtPlugin, public MEcmtMessageEventObserver
	{
	public:
		~CTaskManagerPlugin( );
		static MEcmtPlugin* NewL( );
		void BindMessagingL( MEcmtMessaging* aMessaging );

	protected:
		// from CActive
		void RunL();
		TInt RunError( TInt aError );
		
	private:
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );
		CTaskManagerPlugin();
		void ConstructL();
		void CpuInfoL( CBufFlat* aBuff );
		void DriveInfoL( CBufFlat* aBuff );
		void RamInfoL( CBufFlat* aBuff );
		void ApplicationsL( CBufFlat* aBuff );
		void ThreadsL( CBufFlat* aBuff );
		TInt ExtractAppCaption( TDes& aName, CBufFlat* aBuff );
		void ThreadInfoL( RThread& aThread, CBufFlat* aBuff, TBool aOpenOK );
		void RemoveSpecialCharacters( TDes8& aBuff );
		void SendTaskDataL( );
		void SendReply( const TDesC8& aReply );
		static TInt SampleCpuUsage( TAny* aThis );
		void DoSampleCpuUsage();

	private:
    	enum ETaskType
    	{
			EApplications,
			EThreads
		};
	
		//Cannot be a reference since it is given only after construction
		MEcmtMessaging*				iMessaging;
		MEcmtMessageSender*			iMessageSender;
		TTimeIntervalMicroSeconds32 iInterval;
		TUid						iUid;
		TUid						iTargetUid;
		RFs							iFs;
		RWsSession					iWs;
		ETaskType					iTaskType;
		CMovingAverage<TReal>*		iMovingAverage;
		RThread						iCounterThread;
		TUint						iCounter;
		TUint						iPrevCounterValue;
		CPeriodic*					iCpuSampler;
	};
#endif

