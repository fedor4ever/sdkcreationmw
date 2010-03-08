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

#ifndef __OPENLIB_H__
#define __OPENLIB_H__

#ifndef __SYMBIAN32__

/* IMPORT_C is Symbian OS only stuff*/
#define IMPORT_C __declspec(dllexport)

#else	/* __SYMBIAN32__*/

#include <e32def.h>

#endif



#endif /* __OPENLIB_H__ */