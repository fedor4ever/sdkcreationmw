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


#include "SocketReader.h"

CSocketReader* CSocketReader::NewL(RSocket* aSocket)
	{
	CSocketReader* object = new (ELeave) CSocketReader(aSocket);
	CleanupStack::PushL(object);
	CActiveScheduler::Add(object);
	object->ConstructL();
	CleanupStack::Pop(object);
	return object;
	}

CSocketReader::CSocketReader(RSocket* aSocket)
: CActive(EPriorityStandard), iSocket(aSocket)
	{
	}

CSocketReader::~CSocketReader()
	{
	}

void CSocketReader::ConstructL()
    {
    }

// read a byte from the socket
TUint8 CSocketReader::ReadL(void)
	{
	TUint8 ret = 0;
    TPtr8 ptr(&ret, 0, 1);
    iStatus = KRequestPending;
    iSocket->Read(ptr, iStatus);
    WaitForRequest();
    User::LeaveIfError(iStatus.Int());
    return ret;
	}

// sync read an int fro the socket
TInt CSocketReader::ReadIntL(void)
	{
	TInt ret = 0;
    TPtr8 ptr((TUint8*)(&ret), 0, 4);
    iStatus = KRequestPending;
    iSocket->Read(ptr, iStatus);
    WaitForRequest();
    User::LeaveIfError(iStatus.Int());
    return ByteOrder::Swap32(ret);
	}

TInt16 CSocketReader::ReadShortL(void)
    {
	TInt16 ret = 0;
    TPtr8 ptr((TUint8*)(&ret), 0, 2);
    iStatus = KRequestPending;
    iSocket->Read(ptr, iStatus);
    WaitForRequest();
    User::LeaveIfError(iStatus.Int());
    return (TInt16)ByteOrder::Swap16((TUint16)ret);
    }

// read aLen bytes and store in aBuff
TInt CSocketReader::Read8L(TDes8& aBuff)
	{
    TInt initialLen = aBuff.Length();
    iStatus = KRequestPending;
    iSocket->Read(aBuff, iStatus);
    WaitForRequest();
    if ( iStatus.Int() < 0 )
        {
        return iStatus.Int();
        }
    return aBuff.Length() - initialLen;
	}

// read UTF8 formatted 8 bit data. This method calls ReadShortL 
// and then calls Read8L. No conversion is performed on actual data
// and the result is UTF8 encoded.
TInt CSocketReader::ReadUtf8L(HBufC8*& aBuff)
	{
    TInt16 len = ReadShortL();
    aBuff = HBufC8::NewLC(len);
    // this appears to be necessary as aBuff max length
    // is pretty much a random number (see HBufC::NewLC docs)
    TPtr8 ptr((TUint8*)(aBuff->Ptr()),0,len);
    Read8L(ptr);
    CleanupStack::Pop(aBuff);
    (aBuff->Des()).SetLength(ptr.Length());
    return 4 + aBuff->Length();
    }

// read UTF8 data and convert to UCS-2.
TInt CSocketReader::ReadUtfL(HBufC*& aBuff)
	{
    HBufC8* buf;
    TInt res = ReadUtf8L(buf);
    CleanupStack::PushL(buf);
    // convert
    aBuff = HBufC::NewLC(res);
    TPtr ptr = aBuff->Des();
    User::LeaveIfError(
        CnvUtfConverter::ConvertToUnicodeFromUtf8(
           ptr, *buf));
    CleanupStack::Pop(aBuff);
    CleanupStack::PopAndDestroy(buf);
    return res;
	}

void CSocketReader::DoCancel()
    {
    iSocket->CancelRead();
    iWait.AsyncStop();
    }

void CSocketReader::RunL()
    {
    iWait.AsyncStop();
    }

void CSocketReader::WaitForRequest()
    {
    if (iStatus == KRequestPending)
        {
        SetActive();
        iWait.Start();
        }
    else
        {
        // Seemingly useless assignment statement below performs very 
        // important function. It clears TRequestStatus::ERequestPending
        // bit in TRequestStatus::iFlags field, which otherwise panics
        // the scheduler. The next statement eats the signal.
        iStatus = iStatus.Int();
        User::WaitForRequest(iStatus);
        }
    }
