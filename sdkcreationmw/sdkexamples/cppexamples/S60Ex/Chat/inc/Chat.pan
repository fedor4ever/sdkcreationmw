/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CHAT_PAN__
#define __CHAT_PAN__

/** Chat application panic codes */
enum TChatPanics 
    {
    EChatInvalidControlIndex = 1,
    EChatInvalidLogicState,
    EChatUnableToDisconnect
    };


/** Panic Category */
_LIT( KPanicChat, "Chat" );
_LIT( KPanicBTServiceAdvertiser, "BTServiceAdvertiser" );


inline void Panic( TChatPanics aReason )
    { 
    User::Panic( KPanicChat, aReason );
    }


#endif // __CHAT_PAN__

// End of File
