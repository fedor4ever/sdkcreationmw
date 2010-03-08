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


#ifndef TIMEOUTIMER_H
#define TIMEOUTIMER_H

#include <e32base.h>

class CMsgCommandConnection;

/**
* Connection timout CallBack Timer
*/
class CTimeOutTimer: public CTimer
	{
public:
	static CTimeOutTimer* NewL(
        const TInt aPriority,
        CMsgCommandConnection* aConnection);
	~CTimeOutTimer();

protected:
    CTimeOutTimer(const TInt aPriority);
	void ConstructL(CMsgCommandConnection* aConnection);
    virtual void RunL();

private:
	CMsgCommandConnection* iCommandConnection;
	};

#endif // TIMEOUTIMER_H
