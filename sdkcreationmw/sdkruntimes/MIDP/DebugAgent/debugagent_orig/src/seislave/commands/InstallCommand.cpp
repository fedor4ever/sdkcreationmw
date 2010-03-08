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


#include "SeiCommands.h"

CInstallCommand* CInstallCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CInstallCommand* object = new (ELeave) CInstallCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CInstallCommand::CInstallCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    iDirID = 0;
    iInstallationResult = KErrNone;
    }

CInstallCommand::~CInstallCommand()
    {
    if ( iJadPath != NULL )
        {
        delete iJadPath;
        }
    if ( iJarPath != NULL )
        {
        delete iJarPath;
        }
    }

void CInstallCommand::ConstructL()
    {
    }

void CInstallCommand::DoReadL()
    {
    _LIT(KInstallInitMessage, "Installation starting");
    iDriver->Log(KInstallInitMessage);
    // trusted: 0 - untrusted, non-zero - trusted
    TUint8 trusted = iReader->ReadL();
    if ( trusted )
        {
        iTrusted = ETrue;
        }
    else 
        {
        iTrusted = EFalse;
        }
    
    // download jad and jar
    DownloadJadL();
    // download jar
    DownloadJarL();
    }

void CInstallCommand::DoExecuteL()
    {
    TRAP(
    	iInstallationResult, 
    	iId = iDriver->Engine()->InstallL(
    		iJadPath, iJarPath, iTrusted));
    }

_LIT(KInstallCompleteMessage, "Installation complete");
_LIT(KInstallErrorMessage, "Installation error : %d");

void CInstallCommand::DoWriteL()
    {
    iWriter->WriteIntL(iInstallationResult);
    iWriter->WriteIntL(iId);
	if ( iInstallationResult == KErrNone )
		{
	    iDriver->Log(KInstallCompleteMessage);
		}
	else 
		{
		TBuf<64> errorMsg;
		errorMsg.Format(KInstallErrorMessage, iInstallationResult);
		iDriver->Log(errorMsg);
		}
    }


void CInstallCommand::DownloadJadL()
    {
    HBufC* jadFileName;
    iReader->ReadUtfL(jadFileName);
    // save jad to a file
    iJadPath = NextFilePathL(*jadFileName);
    RFile jfile;
    User::LeaveIfError(jfile.Create( iDriver->Fs(), *iJadPath, EFileWrite|EFileShareExclusive ));
    HBufC8* jadContents;
    iReader->ReadUtf8L(jadContents);
    jfile.Write( *jadContents );
    jfile.Close();
    delete jadFileName;
    delete jadContents;
    }

void CInstallCommand::DownloadJarL()
    {
    HBufC* jarFileName;
    iReader->ReadUtfL(jarFileName);
    TInt32 jarSize = iReader->ReadIntL();
    // 1k buffer
    iJarPath = NextFilePathL(*jarFileName);

    HBufC8* buf = HBufC8::NewLC(1024);

    RFile jfile;
    User::LeaveIfError(jfile.Create( iDriver->Fs(), *iJarPath, EFileWrite|EFileShareExclusive ));
    
    TInt read = 0;

    while ( read < jarSize )
        {
        TInt remaining = jarSize - read;
        TInt buffSize = (remaining<1024)?remaining:1024;
        TPtr8 ptr((TUint8*)(buf->Ptr()),0,buffSize);
        TInt num = iReader->Read8L(ptr);
        if ( num < 0 ) 
            {
            break;
            }
        read += num;
        jfile.Write( ptr );
        }

    CleanupStack::PopAndDestroy(buf);
    delete jarFileName;
    jfile.Close();
    }

HBufC* CInstallCommand::NextFilePathL(const TDesC& aType)
    {
    _LIT(KBaseDebugDir, "c:\\midp2\\jdebugagent\\");
    _LIT(KSeparator, "\\");

    HBufC* nextFile = HBufC::NewLC(256);
    TPtr ptr = nextFile->Des();
    ptr.Append(KBaseDebugDir);

    if ( iDirID == 0 )
        {
        TTime time;
        time.UniversalTime();
        iDirID = time.Int64() / 1000;
        }

    ptr.AppendNum(iDirID);
    ptr.Append(KSeparator);
    ptr.Append(aType);

    // ensure directory
    RFs& fs = iDriver->Fs();
    fs.MkDirAll(ptr);
   
    CleanupStack::Pop(nextFile);
    return nextFile;
    }
