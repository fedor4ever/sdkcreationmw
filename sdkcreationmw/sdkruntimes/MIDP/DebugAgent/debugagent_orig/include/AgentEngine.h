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



#ifndef HEADER_MAGENT_ENGINE
#define HEADER_MAGENT_ENGINE

#include <e32base.h>
#include <d32dbms.h>
#include <utf.h>
#include "Driver.h"
#include "CleanupTable.h"
#include <j2me/systemams/client/midp2/MIDP2Client.h>
#include <j2me/systemams/client/installer/InstallerClient.h>

#include <j2me/midp2/dbV2/MDatabaseV2.h>
#include "j2me/midp2/dbV2/MMIDletSuiteTableV2.h"
#include "j2me/midp2/dbV2/MMIDletSuiteTableIteratorV2.h"

#include "ZipFile.h"
#include "ZipFileMember.h"
#include "ZipFileMemberInputStream.h"

class CAgentEngine : public CBase
	{
public:
	CAgentEngine (CDriver* aDriver);
	virtual ~CAgentEngine ();
public:
	// jar and jad
	virtual TInt InstallL(HBufC* aJad, HBufC* aJar, TBool aTrusted) = 0;
	// remove by name and vendor
	virtual void RemoveL(HBufC* aName, HBufC* aVendor) = 0;
	// remove by id
	void RemoveL(TInt aId);
	// start a MIDlet
	void StartL(TInt aMsId, TInt aId, HBufC* aCmdLineOptions, TBool aDefer);
	// close a MIDlet
	void CloseL(TInt aMsId, TInt aId);
    // list installed suites
    TInt ListSuiteIDsL(RArray<TUint32>& aResult);
    // retrieve the contents of jad 
    HBufC8* GetSuiteInfoL(TInt aMsId);
    // retrieve midlet suite UID
    TUid FindMIDletUidL(TUint32 aMsId);
	// remove all temporary installs and data
	void Cleanup(void);


public:
	// to be called by Install and Remove implementations
	// so that we can clean up properly
	void SuiteInstalled(TInt aId);
	// to be called by Install and Remove implementations
	// so that we can clean up properly
	void SuiteRemoved(TInt aId);


public:
	
	// utilities
	TInt GetIdL(HBufC* aName, HBufC* aVendor);

	void GetNameVendorL(TInt aId, HBufC*& aNameOut, HBufC*& aVendorOut);
	/**
	*	Load the jad file from aJadPath into aContents.
	*
	*	@param aJadPath the path to the jad file
	*	@return If successfull, returns newly allocated HBufC8. This heap 
	*			buffer is not on cleanup stack and is managed by the caller.
	*	@leave  If unsuccessfull, leaves with  one of the Symbian OS wide 
	*			error codes.
	*/
	static HBufC* LoadJadL(CDriver* aDriver, const TDesC& aJadPath);

    // UTF8 version
	static HBufC8* LoadJad8LC(CDriver* aDriver, const TDesC& aJadPath);

	/**
	*	Load the manifest file from the jar file aJarPath into aContents.
	*
	*	@param aJarPath the path to the jar file
	*	@return If successfull, returns newly allocated HBufC8. This heap 
	*			buffer is not on cleanup stack and is managed by the caller.
	*	@leave  If unsuccessfull, leaves with  one of the Symbian OS wide 
	*			error codes.
	*/
	static HBufC* LoadManifestL(CDriver* aDriver, const TDesC& aJarPath);

	static HBufC* LoadFromZipL(CDriver* aDriver, const TDesC& aJarPath, const TDesC8& aFile); // PRIVATE

public:
	CDriver*		iDriver;
	CCleanupTable*	iCleanupTable;
	};

#endif