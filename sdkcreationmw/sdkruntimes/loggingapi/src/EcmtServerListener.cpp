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
* Description:  Helper class for REcmt
*
*/



// INCLUDE FILES
#include "EcmtClient.h"
#include "EcmtServerObserver.h"
#include "EcmtServerListener.h"


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CEcmtServerListener::CEcmtServerListener
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CEcmtServerListener::CEcmtServerListener( MEcmtServerObserver* aObserver, REcmt& aServer ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iServer( aServer )
{
    CActiveScheduler::Add(this);
    iServer.RequestServerNotification( iBuff, iStatus );
    SetActive();
}

// Destructor
CEcmtServerListener::~CEcmtServerListener()
{
    Cancel();
}


// ---------------------------------------------------------------------------
// CEcmtServerListener::RunL
//
// ---------------------------------------------------------------------------
//
void CEcmtServerListener::RunL()
{
    if ( iStatus == KErrNone )
    {
        iBuff2.Copy(iBuff);
        iServer.RequestServerNotification( iBuff, iStatus );
        iObserver->HandleEcmtServerNotification( iBuff2, KErrNone );
        SetActive();
    }
    else
    {
        iBuff2.Zero();
        iObserver->HandleEcmtServerNotification( iBuff2, iStatus.Int() );
    }
}

// ---------------------------------------------------------------------------
// CEcmtServerListener::DoCancel
//
// ---------------------------------------------------------------------------
void CEcmtServerListener::DoCancel()
{
    iServer.CancelRequestServerNotification();
}

//  End of File  
