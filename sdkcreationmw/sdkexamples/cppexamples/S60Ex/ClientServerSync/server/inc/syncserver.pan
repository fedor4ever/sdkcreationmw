/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __SYNCSERVER_PAN__
#define __SYNCSERVER_PAN__

/** Panic Category */
_LIT( KCSSyncServer, "CSSyncServer" );

/** SyncServer panic codes */
enum TTimeServPanic
    {
    EBadRequest,
    EBadDescriptor,
    ESrvCreateServer,
    ECreateTrapCleanup
    };

#endif // __SYNCSERVER_PAN__

// End of File