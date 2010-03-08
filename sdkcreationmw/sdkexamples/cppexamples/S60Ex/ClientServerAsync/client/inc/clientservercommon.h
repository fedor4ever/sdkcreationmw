/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CLIENTSERVERCOMMON_H__
#define __CLIENTSERVERCOMMON_H__

// INCLUDE FILES
#include <e32base.h>

// CONSTANTS
_LIT( KTimeServerName,"AsyncTimeServer" ); // Server name
_LIT( KTimeServerSemaphoreName, "AsyncTimeServerSemaphore" );

// The server version. A version must be specified when
// creating a session with the server.
const TUint KTimeServMajorVersionNumber=0;
const TUint KTimeServMinorVersionNumber=1;
const TUint KTimeServBuildVersionNumber=1;

// DATA TYPES
// Opcodes used in message passing between client and server
enum TTimeServRqst
    {
    ETimeServRequestTime,
    ETimeServCancelRequestTime
    };

// Opcodes used by server to indicate which asynchronous service
// has completed
enum TTimeServRqstComplete
    {
    ETimeServRequestTimeComplete = 1
    };

#endif // __CLIENTSERVERCOMMON_H__

// End of file