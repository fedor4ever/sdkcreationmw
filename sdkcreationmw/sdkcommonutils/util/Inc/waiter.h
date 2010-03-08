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

#ifndef _WAITER_H_
#define _WAITER_H_

// Includes
#include <e32base.h>

// Interface class definition
class CWaiter_if 
	{
friend class CWaiter;
private:
	virtual void now(
		int idx) = 0;		// Idx is the same number as given in 
							// CWaiter::ConstructL
	};

// Main class definition
class CWaiter : public CActive 
{
protected:
	IMPORT_C CWaiter(TInt aPriority);

	IMPORT_C void ConstructL(CWaiter_if* cb, int idx);							
public:
	IMPORT_C ~CWaiter();

	// Use this to contruct objects. idx is users index value that will
	// be passed to users interface. This is useful if user has several 
	// CWaiter object's waiting. Give them different idx values to identify
	// which was the one that signaled
	IMPORT_C static CWaiter* NewL(CWaiter_if* cb
		, int idx=0		
		, TInt aPrioiry = EPriorityNormal);	// Priority of this active object

	// Mandatory for CActive derived classes
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();

	// Start waiting asynchronously: now will be called when it is time
	IMPORT_C void Wait();

	// Complete waiting
	IMPORT_C void Complete();

protected:
	RMutex			iMutex;
	CWaiter_if*		iCb;		// Pointer to parent
	TThreadId		iOwner;	// Master thread's id
	int				iUserIdx;

};

#endif //_WAITER_H_
