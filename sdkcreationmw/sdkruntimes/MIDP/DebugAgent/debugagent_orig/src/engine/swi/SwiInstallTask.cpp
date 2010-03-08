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
* Description: 
*
*/

#include <javaregistry.h>
#include <javaregistryentry.h>
#include <javaregistrypackageentry.h>
#include <javaregistryapplicationentry.h>
#include <apgicnfl.h>
#include <apgcli.h>
#include <apgaplst.h>
#include <apacmdln.h>

#include <j2me/systemams/client/midp2/MIDP2Client.h>
//end Pechantikov, SDK

#include <e32std.h>
#include <MIDPInstallationLaunchInfo.h>
#include "SwiInstallTask.h"
#include "ifeui.h"
#include <ecom/implementationproxy.h>
#include "SWInstDefs.h"


using namespace Java::Installer::Ife;
using namespace Java;
class CJavaRegistry;
class CJavaRegistryPackageEntry;
class CJavaRegistryEntry;
//class CIfeUi;
typedef CIfeUi* (*NEWLP)();
//#include "jdebug.h"




CSwiInstallTask::CSwiInstallTask(
	CDriver* aDriver, CPassiveLogger* aLogger,
	HBufC* aJad, HBufC* aJar) :
	CSwiTask(aDriver, aLogger), iJad(aJad), iJar(aJar)
	{
	}
	
CSwiInstallTask::~CSwiInstallTask()
	{
	}

void CSwiInstallTask::DoTaskL(void)
	{
	//	DEBUG("+CSwiInstallTask::DoTaskL");
	// Alternative solution: there seem to be a bug in installerClient
	//therefore we can't use it now:
	iInstaller.Connect();
	 //TInstallOptions options;
	 //iInstallOptions.iDrive =  'c';//iConfig->InstallDrive();
	 //options.iUntrusted = ( iConfig->InstallUntrusted() ? EPolicyAllowed : EPolicyNotAllowed );
	 iInstallOptions.iUntrusted = EPolicyAllowed;
	 iInstallOptions.iOverwrite = EPolicyAllowed;
     //TPckgBuf<TInstallOptions> optionsPckg( iInstallOptions );
	 //TInstallOptionsPckg pkg( iInstallOptions );
	 iInstallPckg = iInstallOptions;
	 TInt len = iInstallPckg.Length();
	 iInstaller.SilentInstall( iStatus, *iJad, iInstallPckg );
		 
	//	DEBUG("-CSwiInstallTask::DoTaskL");
	/*
	User::LeaveIfError(iIfeLib.Load(_L("ifeui.dll")));
	
	typedef TImplementationProxy* (*TInstantiationL)(TInt&);
	TInstantiationL proxy = 
	 REINTERPRET_CAST(TInstantiationL, iIfeLib.Lookup(1));
	 
	TInt numOfImplementations = 0;
	TImplementationProxy* implementationTable = proxy(numOfImplementations);
	//ife is installer front end
	iNewLp=(NEWLP)implementationTable[0].iNewLFuncPtr;
	iIfe = iNewLp();
	
	iFs.Connect();
	iJadFile.Open(iFs, *iJad, EFileRead);
	
	TInstallReq installReq;
	TInstallOptions opt;
	opt.iOverwrite = EPolicyAllowed;

    //installReq.iCharsetId;
    installReq.iMIME = KJadMIMEType;
   	//Test code:
	//iTimer.CreateLocal(); 
	//iTimer.After(iStatus,3000000); 
	//iStatus = KRequestPending;
	
	//TUid* my_id = &TUid::TUid(0);
	
	iIfe->SilentInstallL( iJadFile,//RFile& aFile,
                             installReq,//TInstallReq& aInstallParams, 
                             false,//TBool aIsDRM, 
                             opt,//SwiUI::TInstallOptions& aOptions,
                             iStatus);//,//TRequestStatus& aStatus
                             //&iInstallPkgUid );
   */

//	DEBUG("-CSwiInstallTask::DoTaskL");
}

void CSwiInstallTask::DoTaskCleanupL(void)
{
	iIfeLib.Close();
	//CleanupStack::PopAndDestroy(iIfe);
	iJadFile.Close();
	iFs.Close();
	
	iInstaller.Close();
	/*
	//Pechatnikov:
//Ugly hack for now, ask management why they need it NOW to work somehow:
    Java::CJavaRegistry* registry = Java::CJavaRegistry::NewLC();
    Java::CJavaRegistryEntry* entry = registry->RegistryEntryL( iInstallPkgUid );
    CleanupStack::PushL( entry );
    
    Java::CJavaRegistryPackageEntry* pentry = (Java::CJavaRegistryPackageEntry*)entry;

    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

	//I think in case of SDK launch index should be always 0
	TInt index = 0;
    Java::CJavaRegistryEntry* eentry = pentry->EmbeddedEntryByNumberL( index );
    if(eentry)
    {
        CleanupStack::PushL( eentry );
			
		TInt a = 1;	        
	     if(a==1)
	     {
	     	//CApaCommandLine* commandLine=CApaCommandLine::NewLC();
				
			//commandLine->SetCommandL(EApaCommandOpen);//EApaCommandOpen
			//commandLine->SetExecutableNameL(_L("StubMIDP2RecogExe.exe"));
			//commandLine->SetDocumentNameL(eentry->Name());//L""
			//commandLine->SetOpaqueDataL(*aOpaqueData);
			
			 //session.StartApp( *commandLine );		
			 //CleanupStack::PopAndDestroy( commandLine );
			 
		     MIDP::SystemAMS::RMIDP2Client client;
		    User::LeaveIfError(client.Connect());
		    _LIT(KArgs, "-vp 21000");
		//    User::LeaveIfError(client.Launch(aMsId, aId, aCmdLineOptions, aDefer ));
			HBufC* args = HBufC::NewL(1);
	    	client.Launch( (iInstallPkgUid.iUid), ( (eentry->Uid()).iUid ), args  , false );
		    client.Close();

	     }
	     else
	     {
	        TUid muid = eentry->Uid();
	        TThreadId threadid;
	        // start the selected application
	        User::LeaveIfError( session.StartDocument( KNullDesC, muid, threadid) );
	     }

	     CleanupStack::PopAndDestroy( eentry );
    }
    
    CleanupStack::PopAndDestroy( &session );    
    CleanupStack::PopAndDestroy( entry ); 
    CleanupStack::PopAndDestroy( registry ); 
    
    //end Pechatnikov	
	*/
}
