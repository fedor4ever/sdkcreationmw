/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug trace management
*
*/


#ifndef ECMTSERVERDEBUG_H
#define ECMTSERVERDEBUG_H

// Uncomment the following line to enable debug output in those files
// that include this header. All debug code should be wrapped around
// #ifdef ECMT_RDEBUG ... #endif in order to keep the debug output from
// showing to the user in the released EcmtServer.

#if defined(_DEBUG) && !defined(__WINS__) && !defined(ECMT_RDEBUG)
//#  define ECMT_RDEBUG 1
#endif

#endif // ECMTSERVERDEBUG_H
