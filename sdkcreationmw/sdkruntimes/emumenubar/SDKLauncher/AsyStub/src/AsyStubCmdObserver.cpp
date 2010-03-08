/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include "AsyStubCmdObserver.h"
#include "ASYStubMainServiceBase.h"
#include "AsyStubCmdHandlerBase.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::CAsyStubCmdObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAsyStubCmdObserver::CAsyStubCmdObserver( CASYStubCmdHandlerBase* aASYStubCmdHandler ) : 
    CTimer( CActive::EPriorityHigh )
    {

    CActiveScheduler::Add( this );
    iASYStubCmdHandler = aASYStubCmdHandler;

    }

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAsyStubCmdObserver::ConstructL()
    {
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAsyStubCmdObserver* CAsyStubCmdObserver::NewL( CASYStubCmdHandlerBase* aASYStubCmdHandler )
    {
    CAsyStubCmdObserver* self = new (ELeave) CAsyStubCmdObserver( aASYStubCmdHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();    
   
    return self;
    }
    
// Destructor
CAsyStubCmdObserver::~CAsyStubCmdObserver()
    {
    }

// -----------------------------------------------------------------------------
// CAsyStubCmdObserver::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAsyStubCmdObserver::RunL()
    {  
    TAccValueTypeTBool aCmdResponse;
    aCmdResponse.iValue	= ETrue;
    iASYStubCmdHandler->ProcessResponseL( aCmdResponse );       				   
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
    
//  End of File
