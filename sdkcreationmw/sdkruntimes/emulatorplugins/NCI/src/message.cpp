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



#include "message.h"
#include "message.hrh"
#include "utf.h"

_LIT8(KTimeFormat, "YYYYMMDD:HHMMSS.MMMMMM");
_LIT(KTimeFormatStr, "%04d%02d%02d:%02d%02d%02d.%06d");

CMessage::CMessage()
    {
    }

CMessage::~CMessage()
    {
	delete iAddress;
	delete iPayload;
	delete iType;
    delete iRawMessage;
    }

TPtrC CMessage::getAddress()
    {
    return iAddress->Des();
    }

void CMessage::setAddress(const TDesC& addr)
    {
	iAddress = addr.Alloc();
    }

const TTime CMessage::getTimestamp()
    {  
	return iTimeStamp;
    }

void CMessage::setType(const TDesC& type)
    {
	iType = type.Alloc();
    }

TPtrC CMessage::getType()
    {
    return iType->Des();
    }


void CMessage::setTimeStamp(TTime timeStamp)
    {
    iTimeStamp = timeStamp;
    }

const TUint CMessage::getPayloadLen()
    {
	return (TUint) iPayload->Length();	
    }

TPtrC CMessage::getPayload()
    {
    return iPayload->Des();
    }

void CMessage::setPayload(const TDesC& aPayload)
    {
	iPayload = aPayload.Alloc();
    }


HBufC8* CMessage::getMessage()
    {
    if(iRawMessage == NULL)
        {
        //Build RawMessage
        CreateRawMessage();
        }
    return iRawMessage->Alloc();
    }

TInt CMessage::setMessage(const TDesC8& aMessage)
    {
    //delete old rawmessage
    delete iRawMessage;
    TRAPD(err, iRawMessage = aMessage.Alloc());
    switch (err)
        {
        case KErrNone:
            break;
        default:
            User::Leave(err);
        }
   
    //address part
    TPtrC8 addr = ParseValueL(KBeginAddress, KEndAddress);
    iAddress = HBufC::NewL(addr.Length());
    TPtr addrPtr = iAddress->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(
        addrPtr, addr);

    // type part
    TPtrC8 type = ParseValueL(KBeginType, KEndType);
    iType = HBufC::NewL(type.Length());
    TPtr typePtr = iType->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(
        typePtr, type);

    // payload
    TPtrC8 payload = ParseValueL(KBeginPayload, KEndPayload);
    iPayload = HBufC::NewL(payload.Length());
    TPtr payloadPtr = iPayload->Des();
    if(typePtr.Match(BINARY_MESSAGE))
        {
        // convert data from 8 to 16 bit 
        payloadPtr.Copy(payload);
        }
    else
        { // test message payload can be converted
        CnvUtfConverter::ConvertToUnicodeFromUtf8(
        payloadPtr, payload);
         }
    // timestamp
    // time stamp should be in this format YYYYMMDD:HHMMSS.MMMMMM
    TPtrC8 timeStamp8= ParseValueL(KBeginTimestamp, KEndTimestamp);
    HBufC* timeStampBuf = HBufC::NewL(timeStamp8.Length());
    TPtr timePtr = timeStampBuf->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(
        timePtr, timeStamp8);
    
    iTimeStamp.Set(timePtr);

    return KErrNone;
    }

// parse method
TPtrC8 CMessage::ParseValueL(const TDesC8& aValue, const TDesC8& aValueEnd)
    {
    
    // begin and end offset
    TInt begin = 0;
    User::LeaveIfError(begin = iRawMessage->Find(aValue));
    
    begin += aValue.Length();
    
    TInt length = 0;
    User::LeaveIfError(length = iRawMessage->Find(aValueEnd));
    length -= begin;

    return iRawMessage->Mid(begin, length);
    }

TInt CMessage::CreateRawMessage()
    {
    TRAPD(err, 
    // count length reguired;
    TInt length(0);
    // command
    length += TPtrC8(KBeginReceive).Length();
    // message type
    length += TPtrC8(KBeginSmsMessage).Length();
    // address
    length += TPtrC8(KBeginAddress).Length();
    length += iAddress->Length();
    length += TPtrC8(KEndAddress).Length();
    // payload type
    length += TPtrC8(KBeginType).Length();
    length += iType->Length();
    length += TPtrC8(KEndType).Length();
    // timestamp
    length += TPtrC8(KBeginTimestamp).Length();
    length += TPtrC8(KTimeFormat).Length();
    length += TPtrC8(KEndTimestamp).Length();
    // payload
    length += TPtrC8(KBeginPayload).Length();
    length += iPayload->Length();
    length += TPtrC8(KEndPayload).Length();
    // message type end
    length += TPtrC8(KEndSmsMessage).Length();
    // command end
    length += TPtrC8(KEndReceive).Length();

    // create raw message buffer
    iRawMessage = HBufC8::NewL(length);
    TPtr8 ptrRaw = iRawMessage->Des();
    
    ptrRaw.FillZ();
    // fill message message 
    ptrRaw.Append(KBeginReceive);
    ptrRaw.Append(KBeginSmsMessage);
    //address
    ptrRaw.Append(KBeginAddress);
    HBufC8* addrBuf8 = HBufC8::NewL(iAddress->Length());
    CleanupStack::PushL(addrBuf8);
    TPtr8 addrPtr8 = addrBuf8->Des();
    TPtr addrPtr = iAddress->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(addrPtr8, addrPtr);
    ptrRaw.Append(addrPtr8);
    CleanupStack::PopAndDestroy(); // addrBuf8
    ptrRaw.Append(KEndAddress);
    //type
    ptrRaw.Append(KBeginType);
    HBufC8* typeBuf8 = HBufC8::NewL(iType->Length());
    CleanupStack::PushL(typeBuf8);
    TPtr8 typePtr8 = typeBuf8->Des();
    TPtr typePtr = iType->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(typePtr8, typePtr);
    ptrRaw.Append(typePtr8);
    CleanupStack::PopAndDestroy(); // typeBuf8
    ptrRaw.Append(KEndType);
    //timestamp
    ptrRaw.Append(KBeginTimestamp);
    //need timestamp in formated descriptor
    HBufC* timeBuf = HBufC::NewL(TPtrC8(KTimeFormat).Length());
    CleanupStack::PushL(timeBuf);
    TPtr timePtr = timeBuf->Des();
    
    // time in good format
    TDateTime dateTime;
    dateTime = iTimeStamp.DateTime();
    
    timePtr.Format(KTimeFormatStr,
        dateTime.Year(),
        dateTime.Month(),
        dateTime.Day(),
        dateTime.Hour(),
        dateTime.Minute(),
        dateTime.Second(),
        dateTime.MicroSecond());

    HBufC8* timeBuf8 = HBufC8::NewL(timePtr.MaxLength());
    CleanupStack::PushL(timeBuf8);
    TPtr8 timePtr8 = timeBuf8->Des();
    
    CnvUtfConverter::ConvertFromUnicodeToUtf8(timePtr8, timePtr);
    ptrRaw.Append(timePtr8);
    CleanupStack::PopAndDestroy(2); //timeBuf, timeBuf8
    ptrRaw.Append(KEndTimestamp);
    //payload
    ptrRaw.Append(KBeginPayload);
    HBufC8* payloadBuf8 = HBufC8::NewL(iPayload->Length());
    CleanupStack::PushL(payloadBuf8);
    TPtr8 payloadPtr8 = payloadBuf8->Des();
    TPtr payloadPtr = iPayload->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(payloadPtr8, payloadPtr);
    ptrRaw.Append(payloadPtr8);
    CleanupStack::PopAndDestroy(); // payloadBuf8
    ptrRaw.Append(KEndPayload);
    //end smsmessage
    ptrRaw.Append(KEndSmsMessage);
    ptrRaw.Append(KEndReceive);
	);
	
	return err;
    }

