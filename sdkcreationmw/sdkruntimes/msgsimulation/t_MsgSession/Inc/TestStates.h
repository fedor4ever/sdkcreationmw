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


#ifndef TEST_STATES_H
#define TEST_STATES_H

class CMessageSession;
class CMessageListener;
class CSmsMessage;

typedef enum TTestState
{ 
	EStarting, 
	ETextMessageSent, 
	ETextMessageReceived, 
	EBinaryMessageSent, 
	EBinaryMessageReceived,
	EDone
};

class CTest : public CActive
{

	TTestState iState;

	CMessageSession *iSession;
	CMessageListener *iListener;

	CSmsMessage *CreateMessageL(TBool aBinary);

public:
	CTest();
	
	void RunL();
	void DoCancel();
	
	TTestState GetState() const { return iState; }

	TRequestStatus *Start();
};

#endif /* TEST_STATES_H */
