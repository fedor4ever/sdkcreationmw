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

CTestCommand* CTestCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CTestCommand* object = new (ELeave) CTestCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CTestCommand::CTestCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CTestCommand::~CTestCommand()
    {
    }

void CTestCommand::ConstructL()
    {
    }

void CTestCommand::DoReadL()
    {
    // test 1 byte order
    TInt test1 = iReader->ReadIntL();
    if ( test1 != 0x01020304 )
        {
        _LIT(KTest1Fail, "Test 1: FAIL");
        TPtrC t1fail (KTest1Fail);
        iDriver->Log(t1fail);
        User::Leave(-20001);
        }
    _LIT(KTest1OK, "Test 1: OK");
    TPtrC t1ok (KTest1OK);
    iDriver->Log(t1ok);

    // test 2 byte order
    TInt test2 = iReader->ReadShortL();
    if ( test2 != 0x0506 )
        {
        _LIT(KTest2Fail, "Test 2: FAIL");
        TPtrC t2fail (KTest2Fail);
        iDriver->Log(t2fail);
        User::Leave(-20002);
        }
    _LIT(KTest2OK, "Test 2: OK");
    TPtrC t2ok (KTest2OK);
    iDriver->Log(t2ok);

    // test 3 byte 
    TUint8 test3 = iReader->ReadL();
    if ( test3 != 0x5a )
        {
        _LIT(KTest3Fail, "Test 3: FAIL");
        TPtrC t3fail (KTest3Fail);
        iDriver->Log(t3fail);
        User::Leave(-20003);
        }
    _LIT(KTest3OK, "Test 3: OK");
    TPtrC t3ok (KTest3OK);
    iDriver->Log(t3ok);

    HBufC8* buff;
    iReader->ReadUtf8L(buff);
    TPtr8 ptr = buff->Des();
    iDriver->Log(ptr, EFalse);
    delete buff;

    }

void CTestCommand::DoExecuteL()
    {
    _LIT(KTxtTestCommandExecute, "TestCommand::DoExecuteL");
    TPtrC ptr(KTxtTestCommandExecute);
    iDriver->Log(ptr);
    }

void CTestCommand::DoWriteL()
    {
    iWriter->WriteIntL(KErrNone);

    // test 8 byte order
    iWriter->WriteIntL(0x01020304);

    // test 9 utf
    _LIT(KTxtTestCommandWrite, "TestCommand::DoWriteL");
    TPtrC ptr(KTxtTestCommandWrite);
    iDriver->Log(ptr);
    iWriter->WriteUtfL(ptr);
    }
