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


#ifndef __CHATBTSERVICE_SEARCHER_PAN__
#define __CHATBTSERVICE_SEARCHER_PAN__

#include <e32std.h>

/** ChatBtServiceSearcher application panic codes */
enum TChatBtServiceSearcherPanics 
    {
    EChatBtServiceSearcherNextRecordRequestComplete = 1,
    EChatBtServiceSearcherAttributeRequestResult,
    EChatBtServiceSearcherAttributeRequestComplete,
    EChatBtServiceSearcherProtocolRead
    };


/** Panic Category */
_LIT( KPanicServiceSearcher, "ServiceSearcher" );


inline void Panic( TChatBtServiceSearcherPanics aReason )
    { 
    User::Panic( KPanicServiceSearcher, aReason );
    }

#endif // __CHATBTSERVICE_SEARCHER_PAN__

// End of File
