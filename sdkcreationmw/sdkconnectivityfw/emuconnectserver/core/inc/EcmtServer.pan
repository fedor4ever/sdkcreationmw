/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ECMTSERVER_PAN
#define ECMTSERVER_PAN

#include <e32base.h>

/** Panic Category */
_LIT( KCEcmtServer, "CEcmtServer" );

/** EcmtServer panic codes 
*  @ingroup EcmtServer
*/
enum TEcmtServPanic
    {
    EBadRequest = 1,
    EBadDescriptor = 2,
    ESrvCreateServer = 3,
    EMainSchedulerError = 4,
    ECreateTrapCleanup = 5,
    EReqAlreadyPending = 6,
    EPluginsLoadedTwice = 7,
    ENoBearers = 8
    };

#endif // ECMTSERVER_PAN
