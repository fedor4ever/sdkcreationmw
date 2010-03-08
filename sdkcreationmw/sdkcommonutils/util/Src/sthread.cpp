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


// Includes
#include <e32std.h>
#include "sthread.h"

// Symbian OS version of SThreadRec
typedef struct SThreadRec
	{
	RThread thread;
	}SMutexStruct;

/****************************************************************************
*
*****************************************************************************
*
*
*
****************************************************************************/
SThread sthread_create(sthread_function fp, void* context)
	{
	SThread p = new SMutexStruct;

	TBuf<30> name;
	TUint nro = 0;
	nro += (TUint)p;
	nro +=(TUint)context;
	nro += User::TickCount();
	name.AppendFormat(_L("SThread %i"),p+User::TickCount());

	int err = -1;
	while(err != 0)
		{
		err = p->thread.Create(
			name					// Name
			, fp					// Startup function
			, KDefaultStackSize		// Stacksize
			, KMinHeapSize			// Min heap size
			, 10000					// Max heap
			, context);				// Paratmeter to thread function

		switch(err)
			{
			case KErrNone:
				p->thread.Resume();
				return p;
			
			// Hitting same again is a very small propability, yet possible
			case KErrAlreadyExists:
				nro += User::TickCount();
				name.SetLength(0);
				name.AppendFormat(_L("SThread %i"),p+User::TickCount());
				break;

			default:
				return 0;
				break;
			}

		}
	return 0;	
	}
/****************************************************************************
*
*****************************************************************************
*
*
*
****************************************************************************/
void sthread_destroy(SThread s)
	{
	s->thread.Kill(KErrNone);
	delete s;
	}