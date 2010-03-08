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


#include "AmsEngine.h"
#include "jdebug.h"


namespace MIDP
    {
namespace DBv2
    {
extern MDatabase* GetDatabaseL(TUint32);
    } // end of namespace DB
    }


using namespace MIDP::DBv2;


CAmsEngine::CAmsEngine (CDriver* aDriver):
	CAgentEngine(aDriver)
	{
	}
	
CAmsEngine::~CAmsEngine ()
	{
	}


// jar and jad
TInt CAmsEngine::InstallL(
        HBufC* aJadFile, 
        HBufC* aJarFile, 
        TBool aTrusted)
	{
	DEBUG("+CAmsEngine::InstallL");
	_LIT(KUtf8, "utf-8");
	HBufC* jadContents = NULL;
	HBufC* manifestContents = NULL;
	//RFs fs = iDriver->Fs();

    MIDP::SystemAMS::RInstallerClient client;

    User::LeaveIfError(client.Connect());

	// for trusted MIDlets/certs
	CX509Certificate* cert = NULL;
	TUint32 chainNumber = 0;

	// MIDlet suite ID will be returned by the client
	TUint32 msID = 0;

	// load manifest
	manifestContents = LoadManifestL(iDriver, *aJarFile);
	CleanupStack::PushL(manifestContents);

	jadContents = LoadJadL(iDriver, *aJadFile);
	CleanupStack::PushL(jadContents);

    // prepare phase
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
		
	RFile jarfile;
	CleanupClosePushL(jarfile);
	User::LeaveIfError(jarfile.Open(fs,*aJarFile,EFileRead|EFileShareReadersOnly));		
			
	RFile jadfile;
	CleanupClosePushL(jadfile);
	if (aJadFile->Length() != 0)
		{
		User::LeaveIfError(jadfile.Open(fs,*aJadFile,EFileRead|EFileShareReadersOnly));
		}

	if ( aTrusted )
		{
	DEBUG("install as Trusted");
        User::LeaveIfError(
    		client.PrepareToInstallTrustedL(
				*cert,
				0, 
				chainNumber,
				*aJadFile,
				KUtf8,
				*jadContents,
				*manifestContents,
				*aJarFile,
				msID,
				jadfile,
				jarfile,
				EFalse
				));
		}
	else 
		{
		// untrusted
	DEBUG("install as Untrusted");
        User::LeaveIfError(
    		client.PrepareToInstallUntrustedL(
				*aJadFile,
				KUtf8,
				*jadContents,
				*manifestContents,
				*aJarFile,
				msID,
				jadfile,
				jarfile,
				EFalse  
				));
		}
	CleanupStack::PopAndDestroy(3); // jadfile, jarfile, fs

	// Debug Agent uses download directory as target / MIDlet home dir
    // no further processing needed, just complete install

	_LIT(KDummy, "");
    User::LeaveIfError(
            client.InstallCompleted(
				  msID,
				  *aJadFile,
				  *aJarFile,
				  KDummy));

	// delete jadContents, manifestContents
	CleanupStack::PopAndDestroy(2);

    client.Close();

	// all done
	SuiteInstalled(msID);
	DEBUG("-CAmsEngine::InstallL");
	return msID;
	}

// remove by name and vendor
void CAmsEngine::RemoveL(
        HBufC* aName, 
        HBufC* aVendor)
	{
	DEBUG("+CAmsEngine::RemoveL");
    TInt suiteId = GetIdL(aName, aVendor);
    MIDP::SystemAMS::RInstallerClient client;
    User::LeaveIfError(client.Connect());
    User::LeaveIfError(client.Uninstall(*aVendor, *aName, ETrue )); /* aForceShutdown = ETrue */
    client.Close();
	SuiteRemoved(suiteId);
	DEBUG("-CAmsEngine::RemoveL");

	}

