/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains thread panics
*
*/


#ifndef __AUDIOSTREAM_PAN__
#define __AUDIOSTREAM_PAN__

#include <e32std.h>

inline void Panic(TInt aPanicCode)
	{
	_LIT(KApplicationName, "AudioStream");
	User::Panic(KApplicationName, aPanicCode);
	}

inline void PanicIfError(TInt aPanicCode)
	{
	if (aPanicCode != KErrNone)
		{
		Panic(aPanicCode);
		}
	}

#endif // __AUDIOSTREAM_PAN__

