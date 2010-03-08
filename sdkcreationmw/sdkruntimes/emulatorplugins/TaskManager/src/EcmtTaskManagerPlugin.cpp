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
* Description:  Emulator TaskManager Plugin
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <ImplementationProxy.h>
#include <hal.h>
#include <hal_data.h>
#include <apgwgnam.h>
#include <utf.h>
#include <e32math.h>
#include "EcmtTaskManagerPlugin.h"
#include "EcmtTaskManagerPluginUids.h"
#include "EcmtMessageEvent.h"

/**
TaskManager configuration message (from epdt->ecmt)
---------------------------------------------------
<update><b><update-val><b><type><type-val>
where:
<b>			= blank character 
<update>	= UPDATE
<update-val>= Number from 0 to 5 inclusive. 0=pause, 1...5 update speed
			  with 1=lowest 5=highest
<type>		= TYPE
<type-val>	= 0=applications data, 1=threads data

TaskManager diagnostic data message (from ecmt->epdt)
-----------------------------------------------------
<cpu><b><uptime><b><usage><b>
<drives><b><drive-A><b><drive-size><b><drive-free><;>...<drive-Z><b><drive-size><b><drive-free><;><b>
<ram><b><free-RAM><total-RAM><total-ROM><b>
<type><b><name><b><id><b><mem-used><b><mem-reserved><b><stack><b><cpu-time><;>

where:
<b>			= blank character 
<cpu>		= CPU
<uptime>	= time since boot in microseconds
<usage>		= CPU usage percentage, 0...100

<drives>	= DRIVES
<drive-A>	= A
<drive-size>= total capacity of drive
<drive-free>= current free capacity on drive
<drive-B>	= B
...
<drive-Z>	= Z

<ram>		= RAM
<free-RAM>	= current amount of free RAM
<total-RAM>	= total RAM on device
<total-ROM>	= total ROM on device

<type>		= APPLICATIONS or THREADS
<name>		= name of application/thread (contains no blanks, no ';')
<id>		= id of thread or id of primary thread of application
<mem-used>	= amount of memory used by application/thread
<mem-reserved> = amount of memory reserved for application/thread
<stack>		= stack size of application/thread
<cpu-time>	= thread/application CPU usage in microseconds

*/

// CONSTANTS
const TInt KDefaultInterval = 5*1000*1000;
const TInt KMovingAverageLength = 5;
const TInt KCpuSamplerInterval = 1000000/2;
const TMessageChannel KChannelId = 1;

_LIT( KMatchAll, "*" );
_LIT8( KSuccess, "OK" );
_LIT8( KError, "ERROR" );

_LIT8( KUpdateSpeedKey, "UPDATE" );
_LIT8( KTaskTypeKey, "TYPE" );
const TInt KUpdateSpeedChoices = 5;
const TInt KUpdateSpeedTable[ KUpdateSpeedChoices ] =
		{ 15*1000*1000,		// once every 15 sec
		  5*1000*1000, 		// once every 5 sec
		  1*1000*1000, 		// once every 1 sec
		  5*100*1000,		// once every 50 ms (twice a second)
		  100*1000			// once every 10 ms (ten times a second)
		};


// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CounterProc : Callback function called from CPeriodic
// 
// -----------------------------------------------------------------------------
TInt CounterProc(TAny* aInt)
	{
	// Increment the TUint counter value
	(*(TUint*)aInt)++;
	// Return 1 to keep CPeriodic calling us back
	return 1;
	}

// -----------------------------------------------------------------------------
// CounterThread : thread function for auxiliary counter thread
//
// -----------------------------------------------------------------------------
TInt CounterThread(TAny* aParam)
	{
	CTrapCleanup* pC=CTrapCleanup::New();
	CActiveScheduler* pS=new CActiveScheduler;
	CActiveScheduler::Install(pS);

	CPeriodic* idle = CPeriodic::NewL(CActive::EPriorityStandard);
 	idle->Start( 1, 1, TCallBack( CounterProc, aParam ) );
	pS->Start();
	delete idle;
	delete pS;
	delete pC;
	return 0;
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::CTaskManagerPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTaskManagerPlugin::CTaskManagerPlugin() : 
	CTimer( EPriorityStandard ),
	iInterval( KDefaultInterval ),
	iUid( TUid::Uid( KTASKMANAGERPLUGIN_IMPL_UID ) ),
	iTargetUid( TUid::Uid( KJAVA_TASKMANAGERPLUGIN_IMPL_UID ) ),
	iTaskType( EApplications )
	{
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::ConstructL()
	{
	User::LeaveIfError( iFs.Connect() );
	User::LeaveIfError( iWs.Connect() );
	iMovingAverage = CMovingAverage<TReal>::NewL( KMovingAverageLength );
	CTimer::ConstructL();
	iCpuSampler = CPeriodic::NewL( CActive::EPriorityStandard );
	iCpuSampler->Start( KCpuSamplerInterval, KCpuSamplerInterval, 
					   TCallBack(SampleCpuUsage, this ) );
	
	TName threadName( _L( "EcmtCounterThread" ) );
	// Append a random number to make it unique. This avoids problems
	// when core is started/stopped/started in emulator where the old
	// thread may still be around.
	threadName.AppendNum( Math::Random(), EHex );

	User::LeaveIfError( iCounterThread.Create( threadName, CounterThread, 
					   0x1000, 0x1000, 0x100000, &iCounter ) );
	iCounterThread.SetPriority( EPriorityAbsoluteVeryLow );
	iCounterThread.Resume();
	CActiveScheduler::Add( this );	
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CTaskManagerPlugin::NewL( )
	{
    CTaskManagerPlugin* self = new( ELeave ) CTaskManagerPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// Destructor
CTaskManagerPlugin::~CTaskManagerPlugin()
	{
	Cancel();
	if ( iCounterThread.ExitType() == EExitPending )
		{
		iCounterThread.Kill(0);
		}
	delete iCpuSampler;
	delete iMovingAverage;
	iWs.Close();
	iFs.Close();
	}
	
// -----------------------------------------------------------------------------
// CTaskManagerPlugin::BindMessagingL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
	iMessaging->AddMessageEventObserverL( *this, iUid );
	iMessageSender = iMessaging->GetMessageSender( );
	After( iInterval );
	SendTaskDataL();
	}
	
// -----------------------------------------------------------------------------
// CTaskManagerPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
	{
	const TPtrC8 m = iMessaging->Message( aEvent );
	TLex8 lexer( m );
	
	// Message example: UPDATE 5 TYPE 0
	// -> update speed = 5, 0==monitor applications (1 would be threads)
	TInt updateSpeedVal(-1), taskTypeVal(-1); 
	TPtrC8 updateSpeedKey = lexer.NextToken();
	lexer.SkipSpace();
	lexer.Val( updateSpeedVal );
	TPtrC8 taskTypeKey = lexer.NextToken();
	lexer.SkipSpace();
	lexer.Val( taskTypeVal );
	if ( updateSpeedKey == KUpdateSpeedKey && taskTypeKey == KTaskTypeKey )
		{
		if ( updateSpeedVal >= 0 && updateSpeedVal <= KUpdateSpeedChoices )
			{
			Cancel();
			if ( updateSpeedVal != 0)
				{
				// Send new message immediately to refresh screen and
				// update iInterval for subsequent schedules
				iInterval = KUpdateSpeedTable[updateSpeedVal-1];
				After( 0 );
				}
			}
		// Thread info no longer supported, since it cannot be provided in 3.0
		// iTaskType = taskTypeVal == 0 ? EApplications : EThreads;
		SendReply( KSuccess() );
		}
	else
		{
		SendReply( KError );
		}
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::RunError
// Called by active scheduler if our RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CTaskManagerPlugin::RunError( TInt /*aError*/ )
	{
	// Reissue timer tick and return KErrNone to keep us running.
	After( iInterval );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::SendTaskDataL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::SendTaskDataL()
	{
	CBufFlat* msg = CBufFlat::NewL( 512 );
	CleanupStack::PushL( msg );

	CpuInfoL( msg );
	DriveInfoL( msg );
	RamInfoL( msg );
	if ( iTaskType == EApplications )
		{
    	ApplicationsL( msg );
		}
	//else	/*  Thread info not supported since it cannot be gotten in 3.0
	//	{
    //	ThreadsL( msg );
	//	}

	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, msg->Ptr(0) );
	if ( m )
		{
		// Send all task data messages on a single channel id so that
		// they do not get buffered unecessarily.
		iMessageSender->SendMessage( m, KChannelId );
		}
	CleanupStack::PopAndDestroy( msg );
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::RunL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::RunL( )
	{
    if (iStatus == KErrNone)
    	{
    	SendTaskDataL();
		After( iInterval );
    	}
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::CpuInfoL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::CpuInfoL( CBufFlat* aBuff )
	{
	_LIT8( KCpuInfoFormat, "CPU %u %u " );
	TBuf8<256> string;

	int period;
	HAL::Get(HALData::ESystemTickPeriod, period);
	TReal real = iMovingAverage->Value();
	TInt val = (TInt)real;
	string.Format( KCpuInfoFormat, User::TickCount() * period, val );
	aBuff->InsertL( aBuff->Size(), string );
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::SampleCpuUsage
// This is a static method so that it can be called from CPeriodic. It just calls
// the real member function DoSamplCpuUsage.
// -----------------------------------------------------------------------------
//
TInt CTaskManagerPlugin::SampleCpuUsage(TAny* aThis)
	{
	((CTaskManagerPlugin*)aThis)->DoSampleCpuUsage();
	return 1;
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::DoSampleCpuUsage
// This is called from the static SampleCpuUsage method
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::DoSampleCpuUsage()
	{
	TUint countedTicks = iCounter;
	iCounter = 0;
	
	TUint curTickCount = User::TickCount();
	TUint realTicks = curTickCount - iPrevCounterValue;
	iPrevCounterValue = curTickCount;

	if ( countedTicks > realTicks ) 
		countedTicks = realTicks;
	
	TReal usage = ( ( TReal )( realTicks-countedTicks)  / realTicks ) * 100;
	
	if ( usage < 0 ) 
		{
		usage = 0;
		}
	else if ( usage > 100 ) 
		{
		usage = 100;
		}
	
	iMovingAverage->Insert( usage );
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::ApplicationsL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::ApplicationsL( CBufFlat* aBuff )
	{
	_LIT8( KApplications, "APPLICATIONS " );
	aBuff->InsertL( aBuff->Size(), KApplications );
	
	TInt wgId(0);
	CApaWindowGroupName::FindByCaption( KMatchAll, iWs, wgId );
	while( wgId != KErrNotFound )
		{
		TFullName name;
		
		if ( iWs.GetWindowGroupNameFromIdentifier( wgId, name ) == KErrNone &&
             ExtractAppCaption( name, aBuff ) == KErrNone )
			{
			TThreadId threadId;
			iWs.GetWindowGroupClientThreadId( wgId, threadId );
			RThread thread;
			TBool openOK = ( thread.Open( threadId ) == KErrNone ? ETrue : EFalse );
			ThreadInfoL( thread, aBuff, openOK );
			thread.Close();
			}
		CApaWindowGroupName::FindByCaption( KMatchAll, iWs, wgId );
		}
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::ExtractAppCaption
// Extract caption from window group name
// Format is: "status\0uid\0caption\0docname"
// -----------------------------------------------------------------------------
//
TInt CTaskManagerPlugin::ExtractAppCaption( TDes& aName, CBufFlat* aBuff )
	{
	TInt end = aName.LocateReverse( 0 );
	if ( end > 0 )
		{
		TInt start = aName.Left( end ).LocateReverse( 0 );
		if ( start > 0 )
			{
			TPtrC16 appName16 = aName.Mid( start + 1,end - start - 1 );
			TBuf8< sizeof(TFullName)/2+1 > appName8;
			CnvUtfConverter::ConvertFromUnicodeToUtf8( appName8, appName16 );
			RemoveSpecialCharacters( appName8 );
			aBuff->InsertL( aBuff->Size(), appName8 );
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::ThreadsL
// 
// -----------------------------------------------------------------------------
//
/*  Thread info not supported since it cannot be gotten in 3.0
void CTaskManagerPlugin::ThreadsL( CBufFlat* aBuff )
	{
	_LIT8( KThreads, "THREADS " );
	aBuff->InsertL( aBuff->Size(), KThreads );
	
	TFindThread find( KMatchAll );

	TFullName name16;
	TBuf8< sizeof( TFullName )/2 > name8;
	while( find.Next( name16 ) == KErrNone )
		{
		CnvUtfConverter::ConvertFromUnicodeToUtf8( name8, name16 );
		RemoveSpecialCharacters( name8 );
		aBuff->InsertL( aBuff->Size(), name8 );
		RThread thread;
		TBool openOK = ( thread.Open( name16 ) == KErrNone ? ETrue : EFalse );
		ThreadInfoL( thread, aBuff, openOK );
		thread.Close();
		}
	}
*/
// -----------------------------------------------------------------------------
// CTaskManagerPlugin::ThreadInfoL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::ThreadInfoL( RThread& aThread, CBufFlat* aBuff, TBool aOpenOK )
	{
	_LIT8( KThreadInfoFormat, " %u %d %d %d %Ld;" );
	_LIT8( KThreadInfoNA," -1 -1 -1 -1 -1;" );

	TBuf8<256> string( KThreadInfoNA );

	// Query info only if thread opened OK and thread has not died.
	// In other case, just output N/A info to avoid corrupting message format.
	if ( aOpenOK && aThread.ExitType() == EExitPending )
		{
		TThreadId id = aThread.Id();
		TInt allocSize(-1), heapSize(-1), stackSize(-1);
		/* 
		RHeap* heap = aThread.Heap();
		if ( heap )	// On HW heap does not open
			{
			heap->AllocSize( allocSize );
			}
		aThread.GetRamSizes( heapSize, stackSize );
		*/
	
		TTimeIntervalMicroSeconds cpuTime(-1);
		if ( aThread.GetCpuTime( cpuTime ) != KErrNone )
			{
			cpuTime = -1;
			}
		string.Format( KThreadInfoFormat, (TUint)id, allocSize, heapSize, stackSize, cpuTime.Int64() );
		}
	aBuff->InsertL( aBuff->Size(), string );
	}
	
// -----------------------------------------------------------------------------
// CTaskManagerPlugin::DriveInfoL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::DriveInfoL( CBufFlat* aBuff )
	{
	_LIT8( KDriveInfo, "DRIVES " );

	aBuff->InsertL( aBuff->Size(), KDriveInfo );
	
	TBuf8<256> string;
	TDriveList drivelist;
	User::LeaveIfError( iFs.DriveList( drivelist ) );
	
	for ( TDriveNumber i(EDriveA); i <= EDriveZ; i = static_cast<TDriveNumber>(i+1) )
		{
		if ( drivelist[i] )
			{
	    	TVolumeInfo volInfo;
			TInt err = iFs.Volume( volInfo, i );
			TChar driveLetter;
			iFs.DriveToChar( i, driveLetter );

			string.Zero();
			string.Append( driveLetter );
			string.Append( ' ' );
			if ( err == KErrNone )
				{
// RFs::Volume() can't retrieve z drive size on device, so use HAL on device and 
// RFs::Volume on emulator environment. For D drive, RFs::Volume returns TVolumeInfo
// that have same size as the hard drive where d drive is mapped. It is not a bug, 
// it is a feature. This D drive size has nothing to do with RAM. In device this 
// issue is differently: D drive is reserved from RAM disk and RFs::Volume returned 
// TVolumeInfo is comparable with RAM sizes (D should be smaller than RAM, because 
// in device some RAM is reserved also for kernel).
#ifndef __WINS__
				if( i == EDriveZ )
				    {
				    TInt totalRom(0);
	                HAL::Get( HALData::EMemoryROM, totalRom );    
	                string.AppendNum( totalRom );
				    string.Append( ' ' );
				    // it is not possible to retrieve free ROM
				    string.AppendNum( 0 );				    
				    }
				else
				    {
    				// can't use "format" since it does not support TInt64
    				string.AppendNum( volInfo.iSize );
    				string.Append( ' ' );
    				string.AppendNum( volInfo.iFree );    				
                    }
#else                                
			    // can't use "format" since it does not support TInt64
    			string.AppendNum( volInfo.iSize );
    			string.Append( ' ' );
    			string.AppendNum( volInfo.iFree );       		
#endif
                string.Append( ';' );
    		    aBuff->InsertL( aBuff->Size(), string );
				}
			}
		}

	_LIT8( KBlanco, " " );	
	aBuff->InsertL( aBuff->Size(), KBlanco );
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::RamInfoL
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::RamInfoL( CBufFlat* aBuff )
	{
	_LIT8( KRamInfo, "RAM " );
	aBuff->InsertL( aBuff->Size(), KRamInfo );

	TInt totalRam(0), freeRam(0), totalRom(0);
	HAL::Get( HALData::EMemoryRAMFree, freeRam );
	HAL::Get( HALData::EMemoryRAM, totalRam );
	
	// It should be possible to retrieve total ROM size using HAL under WINS 
	// (on device it works), but it returns zero for ROM/z drive size, 
	// so use RFs and TVolumeInfo under WINS and HAL under ARM:	
#ifdef __WINS__
    TDriveList drivelist;
	User::LeaveIfError( iFs.DriveList( drivelist ) );	
	if ( drivelist[EDriveZ] )
		{
    	TVolumeInfo volInfo;
		TInt err = iFs.Volume( volInfo, EDriveZ );
		if ( err == KErrNone )
			{				
			totalRom = volInfo.iSize;
			}
		}
#else // On device use HAL
	HAL::Get( HALData::EMemoryROM, totalRom );
#endif //_WINS_

	_LIT8( KRamInfoFormat, "%u %u %u " );

	TBuf8<256> string;
	string.Format( KRamInfoFormat, freeRam, totalRam, totalRom );
	aBuff->InsertL( aBuff->Size(), string );
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::RemoveSpecialCharacters
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::RemoveSpecialCharacters( TDes8& aBuff )
	{
	TInt i;
	// Change all blancos to underscore (to avoid problems in parsing message)
	while ( ( i = aBuff.Locate( ' ' ) ) != KErrNotFound )
		{
		aBuff[i] = '_';
		}

	// Change all ';' to underscore (to avoid problems in parsing message)
	while ( ( i = aBuff.Locate( ';' ) ) != KErrNotFound )
		{
		aBuff[i] = '_';
		}
	}

// -----------------------------------------------------------------------------
// CTaskManagerPlugin::SendReply
// 
// -----------------------------------------------------------------------------
//
void CTaskManagerPlugin::SendReply( const TDesC8& aReply )
	{
    CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, aReply );
    if ( m )
        {
        iMessageSender->SendMessage( m );
        }
	}

// ============================ OTHER FUNCTIONS ===============================
	
/**
 *	Bulk code needed by ECom and Symbian OS frameworks
 */

// Fix for RVCT Error #144: a value of type "MEcmtPlugin *(*)()" cannot be 
// used to initialize an entity of type "TProxyNewLPtr"
static TAny* CreateTaskManagerPluginL()
{
    return CTaskManagerPlugin::NewL();
}

const TImplementationProxy ImplementationTable[] = 
{
    { {KTASKMANAGERPLUGIN_IMPL_UID}, CreateTaskManagerPluginL }
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}
	
