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



#ifndef _C_SYS_TIMER_H_
#define _C_SYS_TIMER_H_

// Includes
#include <e32base.h>

// Derive user class from this
class CSysTimer_if
	{
friend class CSysTimer;
protected:
	// The num parameter is the same value as give to CSysTimer's
	// constructor. If you have several timers, use this toidentify
	// which timer timed out
	virtual void Timer(int num) = 0;
	};

// Main class definition
class CSysTimer  : public CTimer
{
protected:
	IMPORT_C CSysTimer(TInt aPriority);
	IMPORT_C void ConstructL(CSysTimer_if* client, int num);
public:
	IMPORT_C static CSysTimer* NewL(CSysTimer_if* client
		, int num							// Enter here number to be passed to 
											// CSysTimer_if::Timer
											// The point of this number is that 
											// it is possible for user class to 
											// own several CSysTimers
		, TInt aPriority = EPriorityHigh);

	IMPORT_C virtual ~CSysTimer();

	IMPORT_C void RunL();

	IMPORT_C void Seconds(int s=1);			// Launch after second(s)

	IMPORT_C void MilliSeconds(int s=1);	// Launch after millisecond(s)

	IMPORT_C void MicroSeconds(int s=1);	// Launch after microsecond(s)

protected:
	CSysTimer_if*	iClient;				// Parent
	int				iUserNum;				// Number to be pased to parent
};

#endif // _C_SYS_TIMER_H_
