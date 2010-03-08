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


#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <e32std.h>
#include <e32base.h>

_LIT(TEXT_MESSAGE, "text");
_LIT(BINARY_MESSAGE, "binary");

/**
* Simplified message class to store e.g. sms message data
*/
class CMessage : public CBase
    {
    public:
       /**
       * Basic C++ Constructor
       */
       CMessage();
       /**
       * Basic C++ Destructor
       */
	   virtual ~CMessage();
    public:
       /**
       * Address getter.
       * @return TPtrC descriptor containing address data
       */
       TPtrC getAddress();
       
	   /**
       * Timestamp getter.
       * @return TTime containing timestamp data
       */
       const TTime getTimestamp();
	   
	   /**
       * Type getter.
       * @return TPtrC descriptor containing type of the message data.
       *         i.e. "Text" or "Binary" 
       */
       TPtrC getType();
       
       /**
       * Payload length getter.
       * @return TUint containing length of the message payload
       */
       const TUint getPayloadLen();
       
       /**
       * Address data setter.
       * @param const TDesC& descriptor reference for address data
       */
       void setAddress(const TDesC& addr);
       
       /**
       * TimeStamp data setter.
       * @param TTime value for timestamp
       */
       void setTimeStamp(TTime time);
       
       /**
       * Type data setter.
       * @param const TDesC& descriptor reference for type data
       */
       void setType(const TDesC& type);
       
       /**
       * Payload data setter.
       * @param const TDesC& descriptor reference to payload data
       */
       void setPayload(const TDesC& aPayload);
       
       /**
       * Payload data getter.
       * @return TPtrC non modifiable descriptor to payload data
       */
       TPtrC getPayload(void);
       
       /**
       * RawMessage getter. Will create "RawMessage" formated message
       * Message is in the following format:
       * <receive>
	   * <smsmessage>
       * <address>address data</address>
       * <type>type data</type>
       * <timestamp>timestamp</timestamp>
       * <payload>data</payload>
       * </smsessage>
       * </receive>
       *
       * @return HBufC8* descriptor ponter to message data
       */
       HBufC8* getMessage();
       
       /**
       * RawMessage setter. Will instantiate message member variables
       * "RawMessage" must be in the following format:
       *
       * <smsmessage>
       * <address>address data</address>
       * <type>type data</type>
       * <timestamp>timestamp</timestamp>
       * <payload>data</payload>
       * </smsessage>
       *
       * @param const TDesC8& Reference to "RawMessge descriptor
       * @return TInt Retuns KErrNone if message was initialized successfully.
       */
       TInt setMessage(const TDesC8& aMessage);
    private:
       /**
       * Data value parser.
       * extracts data between aValue and aValueEnd tags.
       * @param const TDesC8& aValue, contains starting tag and
       *        const TDesC8& aValueEnd, contains ending tag.
       */
	   TPtrC8 ParseValueL(const TDesC8& aValue, const TDesC8& aValueEnd);
    
       /**
       * Creates RawMessage according to member variables and stores
       * it to iRawMessage.
       * @return Returns KErrNone if message can be created.
       *         Otherwise generic error code will be returned. 
       */
       TInt CreateRawMessage();
    private:
       HBufC8* iRawMessage;
	   HBufC* iAddress;
	   TTime iTimeStamp;
       HBufC* iType;
	   HBufC* iPayload;
    };
#endif