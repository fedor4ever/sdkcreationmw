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


#include "AgentEngine.h"
#include "CleanupTable.h"
#include "jdebug.h"


namespace MIDP
    {
namespace DBv2
    {
IMPORT_C MDatabase* GetDatabaseL(TUint32);
    } // end of namespace DB
    }


using namespace MIDP::DBv2;

#include "j2me/midp2/dbV2/StorePolicy.h"
_LIT(KDbFile,   "c:MIDP2SystemAMSStaticV2");

CAgentEngine::CAgentEngine(CDriver* aDriver):
	iDriver(aDriver)
	{
	iCleanupTable = new (ELeave)CCleanupTable(iDriver); //TODO calling new (ELeave) in constructor
	//Cleanup();
	}
	
CAgentEngine::~CAgentEngine ()
	{
	delete iCleanupTable;
	}

// start a MIDlet
void CAgentEngine::StartL(
        TInt aMsId, 
        TInt aId, 
        HBufC* aCmdLineOptions,
        TBool aDefer)
    {
	DEBUG("+CAgentEngine::StartL");
    MIDP::SystemAMS::RMIDP2Client client;
    User::LeaveIfError(client.Connect());
    User::LeaveIfError(client.Launch(aMsId, aId, aCmdLineOptions, aDefer ));
    client.Close();
	DEBUG("-CAgentEngine::StartL");
	}


// close a MIDlet
void CAgentEngine::CloseL(
        TInt aMsId, 
        TInt aId)
	{
	DEBUG("+CAgentEngine::CloseL");
    MIDP::SystemAMS::RMIDP2Client client;
    User::LeaveIfError(client.Connect());
    User::LeaveIfError(client.CloseMIDlet(aMsId, aId));
    client.Close();
	DEBUG("-CAgentEngine::CloseL");
	}

// list installed suite IDs
TInt CAgentEngine::ListSuiteIDsL(RArray<TUint32>& aResult)
    {
    
    // open
    const TUint32 KDBVersion = 0x0100;

    RDbs dbs;
    
    User::LeaveIfError(dbs.Connect());
    // ShareSession(dbs);
    RDbNamedDatabase db;
	User::LeaveIfError( db.Open(dbs, KDbFile(), KUidSSystemAMSDbPolicy) );

    MDatabase*         database   = GetDatabaseL(KDBVersion);
    MMIDletSuiteTable* suiteTable = database->MIDletSuiteTableL();

    suiteTable->OpenL(db);

    MMIDletSuiteTableIterator* i = suiteTable->IteratorLC();

    while (i->HasNext())
        {
        TUint32 id;
        TUint32 uid;
        TUint32 flags;
        TPtrC   vendor;
        TPtrC   name;
        TPtrC   version;
        TPtrC   protectionDomain;
        TPtrC   path;
        TPtrC   jadName;
        TPtrC   jarName;

        i->NextL(
               id,
               uid,
               flags,
               vendor,
               name,
               version,
               protectionDomain,
               path,
               jadName,
               jarName);
        aResult.AppendL(id);
        }
    CleanupStack::PopAndDestroy(); // i
    suiteTable->Close();
    delete database; // this will delete suite table
    db.Close();
    dbs.Close();
    return KErrNone;
    }

// retrieve the contents of jad 

HBufC8* CAgentEngine::GetSuiteInfoL(TInt aMsId)
    {

    // open
    const TUint32 KDBVersion = 0x0100;

    RDbs dbs;
    
    User::LeaveIfError(dbs.Connect());
    // ShareSession(dbs);
    RDbNamedDatabase db;

	User::LeaveIfError(db.Open(dbs, KDbFile(), KUidSSystemAMSDbPolicy));

    MDatabase*         database   = GetDatabaseL(KDBVersion);
    MMIDletSuiteTable* suiteTable = database->MIDletSuiteTableL();

    suiteTable->OpenL(db);

    MMIDletSuiteTableIterator* i = suiteTable->IteratorLC();

    TUint32   id;
    TUint32   uid;
    TUint32   flags;
    TPtrC     vendor;
    TPtrC     name;
    TPtrC     version;
    TPtrC     protectionDomain;
    TPtrC     path;
    TPtrC     jadName;
    TPtrC     jarName;
    TBool     found = EFalse;
    TFileName jadPath;


    while (i->HasNext())
        {
        i->NextL(
               id,
               uid,
               flags,
               vendor,
               name,
               version,
               protectionDomain,
               path,
               jadName,
               jarName);
        if (id == aMsId)
            {
            found = ETrue;
            jadPath.Append(path);
            jadPath.Append(jadName);
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // i
    // clear up
    suiteTable->Close();
    delete database; // this will delete suite table
    db.Close();
    dbs.Close();
    if (!found)
        {
        User::Leave(KErrNotFound);
        }

    // load the file 

    HBufC8* jadContents = LoadJad8LC(iDriver, jadPath);

    CleanupStack::Pop(jadContents);

    return jadContents;
    }


HBufC* CAgentEngine::LoadJadL(
        CDriver* aDriver, 
        const TDesC& aFileName)
	{
	HBufC8* bytes    = LoadJad8LC(aDriver, aFileName);
    
    HBufC* contents = NULL;
	contents = HBufC::NewLC(bytes->Length());
	TPtr contentsPtr = contents->Des();

	User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(contentsPtr, *bytes));

	CleanupStack::Pop(contents);
	CleanupStack::PopAndDestroy(bytes);

	return contents;	
	}

HBufC8* CAgentEngine::LoadJad8LC(
        CDriver* aDriver, 
        const TDesC& aFileName)
	{
	RFile jadFile;
	TInt size = 0;

    User::LeaveIfError(jadFile.Open(aDriver->Fs(), aFileName, EFileRead));
	User::LeaveIfError(jadFile.Size(size));

	HBufC8* bytes    = HBufC8::NewLC(size);
	TPtr8   bytesDes = bytes->Des();

    User::LeaveIfError(jadFile.Read(0, bytesDes, size));

	jadFile.Close();
	return bytes;	
	}

#ifndef DEBUG_AGENT_ENGINE_SWI

HBufC* CAgentEngine::LoadManifestL(
        CDriver* aDriver, 
        const TDesC& aJarPath) // PRIVATE
	{
	_LIT8(KManifestName,   "META-INF/MANIFEST.MF");
	return LoadFromZipL(aDriver, aJarPath, KManifestName);
	}

HBufC* CAgentEngine::LoadFromZipL(
        CDriver* aDriver, 
        const TDesC& aJarPath, 
        const TDesC8& aFile) // PRIVATE
	{
	CZipFile* jarFile  = CZipFile::OpenLC(aDriver->Fs(), aJarPath);
	CZipFileMember* zipMember = NULL;
	TInt		    status;

	TRAP(status, zipMember = jarFile->GetMemberL(aFile));

	if ( status != KErrNone )
		{
		aDriver->Log( _L("Could not obtain zipEntry file from jar.") );
		User::Leave( status );
		}

	CleanupStack::PushL(zipMember);

	TUint32 zipEntrySize = zipMember->GetUncompressedSize();
	HBufC8* rawzipEntry  = HBufC8::NewL(zipEntrySize);

	CleanupStack::PushL(rawzipEntry);

	CZipFileMemberInputStream* is = jarFile->GetInputStreamL(*zipMember);

	CleanupStack::PushL(is);

	TPtr8 pointer(rawzipEntry->Des());

	is->ReadL(pointer, zipEntrySize);
	CleanupStack::PopAndDestroy(is);

	HBufC* zipEntry = HBufC::NewLC(zipEntrySize);

	TPtr8 rawzipEntryPtr(rawzipEntry->Des());
	TPtr  zipEntryPtr(zipEntry->Des());

	zipEntryPtr = _L("");

	jarFile->Close();
	CnvUtfConverter::ConvertToUnicodeFromUtf8(zipEntryPtr, rawzipEntryPtr);
    CleanupStack::Pop(zipEntry);
	CleanupStack::PopAndDestroy(rawzipEntry);
	CleanupStack::PopAndDestroy(zipMember);
	CleanupStack::PopAndDestroy(jarFile);
	return zipEntry;
	}

#endif
	
void CAgentEngine::Cleanup(void)
	{
	DEBUG("+CAgentEngine::Cleanup");
	iDriver->Log( _L("Agent performs cleanup"));
	TRAPD(err, iCleanupTable->CleanupL());
	DEBUG_INT("-CAgentEngine::Cleanup err=%d", err);
	}

void CAgentEngine::RemoveL(TInt aId)
	{
	DEBUG("+CAgentEngine::RemoveL");
	HBufC* name = HBufC::NewLC(256);
	HBufC* vendor = HBufC::NewLC(256);
	GetNameVendorL(aId, name, vendor);
	
	RemoveL(name,vendor);
	CleanupStack::PopAndDestroy(2);
	DEBUG("-CAgentEngine::RemoveL");
	}

TInt CAgentEngine::GetIdL(HBufC* aName, HBufC* aVendor)
	{
    // open
    const TUint32 KDBVersion = 0x0100;

    RDbs dbs;

    User::LeaveIfError(dbs.Connect());

    RDbNamedDatabase db;

	User::LeaveIfError(db.Open(dbs, KDbFile(), KUidSSystemAMSDbPolicy));

    MDatabase*         database   = GetDatabaseL(KDBVersion);
    MMIDletSuiteTable* suiteTable = database->MIDletSuiteTableL();
    
    suiteTable->OpenL(db);

    MMIDletSuiteTableIterator* i = suiteTable->IteratorLC();

    TBool   found = EFalse;
    TUint32 id;

    while (i->HasNext())
        {
        
        TUint32 uid;
        TUint32 flags;
        TPtrC   vendor;
        TPtrC   name;
        TPtrC   version;
        TPtrC   protectionDomain;
        TPtrC   path;
        TPtrC   jadName;
        TPtrC   jarName;

        i->NextL(
               id,
               uid,
               flags,
               vendor,
               name,
               version,
               protectionDomain,
               path,
               jadName,
               jarName);
        if ((*aName == name) && (*aVendor == vendor))
            {
            found = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // i
    suiteTable->Close();
    delete database; // this will delete suite table
    db.Close();
    dbs.Close();
    if (!found)
        {
        User::Leave(KErrNotFound);
        }
    return id;
	}
	
void CAgentEngine::GetNameVendorL(TInt aId, HBufC*& aNameOut, HBufC*& aVendorOut)
	{
    // open
    const TUint32 KDBVersion = 0x0100;

    RDbs dbs;

    User::LeaveIfError(dbs.Connect());

    RDbNamedDatabase db;

	User::LeaveIfError(db.Open(dbs, KDbFile(), KUidSSystemAMSDbPolicy));

    MDatabase*         database   = GetDatabaseL(KDBVersion);
    MMIDletSuiteTable* suiteTable = database->MIDletSuiteTableL();

    suiteTable->OpenL(db);

    MMIDletSuiteTableIterator* i = suiteTable->IteratorLC();

    TBool found = EFalse;

    while (i->HasNext())
        {
        TUint32 id;
        TUint32 uid;
        TUint32 flags;
        TPtrC   vendor;
        TPtrC   name;
        TPtrC   version;
        TPtrC   protectionDomain;
        TPtrC   path;
        TPtrC   jadName;
        TPtrC   jarName;

        i->NextL(
               id,
               uid,
               flags,
               vendor,
               name,
               version,
               protectionDomain,
               path,
               jadName,
               jarName);
        if (id == aId)
            {
            found = ETrue;
            aNameOut   = name.AllocL();
            aVendorOut = vendor.AllocL();
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // i
    delete database; // this will delete suite table
    db.Close();
    dbs.Close();
    if (!found)
        {
        User::Leave(KErrNotFound);
        }
	}

TUid CAgentEngine::FindMIDletUidL(TUint32 aMsId)
	{
    // open
    const TUint32 KDBVersion = 0x0100;

    RDbs dbs;
    
    User::LeaveIfError(dbs.Connect());
    // ShareSession(dbs);

    RDbNamedDatabase db;
	User::LeaveIfError(db.Open(dbs, KDbFile(), KUidSSystemAMSDbPolicy));

    MDatabase*         database   = GetDatabaseL(KDBVersion);
    MMIDletSuiteTable* suiteTable = database->MIDletSuiteTableL();

    suiteTable->OpenL(db); 

	MMIDletSuiteTableIterator* i = suiteTable->IteratorLC();
	
	TUint32 id;
    TUint32 UID;
    TUint32 flags;
    TPtrC   vendor;
    TPtrC   name;
    TPtrC   version;
    TPtrC   protectionDomain;
    TPtrC   path;
    TPtrC   jadName;
    TPtrC   jarName;
    TBool   found = EFalse;

    while (i->HasNext())
        {
        i->NextL(
               id,
               UID,
               flags,
               vendor,
               name,
               version,
               protectionDomain,
               path,
               jadName,
               jarName);
        if (id == aMsId)
            {
            found = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // i
	
	if (!found)
		{
		User::Leave(KErrNotFound);
		}
	// extract UID from path
	TParse pathParser;
	
	pathParser.Set(path, NULL, NULL);
	pathParser.PopDir();
	TInt startindex = pathParser.DriveAndPath().Length() + 1; // gobble '['

	TPtrC uidptr = path.Mid(startindex, 8);
	
	TLex lex(uidptr);
	TUint32 val;
	User::LeaveIfError(lex.Val(val, EHex));

	TUid uid = TUid::Uid(val);
	
    // clear up
    suiteTable->Close();
    delete database; // this will delete suite table
    db.Close();
    dbs.Close();
    return uid;
	}

// to be called by Install and Remove implementations
// so that we can clean up properly

void CAgentEngine::SuiteInstalled(TInt aId)
	{
	TRAP_IGNORE(iCleanupTable->SuiteInstalledL(aId));
	}
	
// to be called by Install and Remove implementations
// so that we can clean up properly

void CAgentEngine::SuiteRemoved(TInt aId)
	{
	TRAP_IGNORE(iCleanupTable->SuiteRemovedL(aId));
	}


