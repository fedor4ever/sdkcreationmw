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


// INCLUDE FILES
#include <ChatEx.rsg>
#include "ChatBase.h"
#include "Chat.pan"
#include "Log.h"
#include <StringLoader.h>


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatBase::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatBase* CChatBase::NewL( MLog& aLog )
    {
    CChatBase* self = NewLC( aLog );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CChatBase::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatBase* CChatBase::NewLC( MLog& aLog )
    {
    CChatBase* self = new ( ELeave ) CChatBase( aLog );
    CleanupStack::PushL( self );
    self->ConstructL ();
    return self;
    }

// ----------------------------------------------------------------------------
// CChatBase::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CChatBase::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CChatBt::CChatBt()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatBase::CChatBase( MLog& aLog )
    : CActive( CActive::EPriorityStandard ),
    iState( EWaitingToGetDevice ),
    iLog( aLog ),
    iServerMode( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CChatBase::~CChatBase()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatBase::~CChatBase()
    {
  
    }
    
// ----------------------------------------------------------------------------
// CChatBase::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CChatBase::DoCancel()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CChatBase::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CChatBase::RunL()
    {
    // no implementation required
    }
    
// ----------------------------------------------------------------------------
// CChatBase::IsServer()
// True if the acting as server.
// ----------------------------------------------------------------------------
//
TBool CChatBase::Server()
    {
    return iServerMode;
    }    

// ----------------------------------------------------------------------------
// CChatBase::SetServer()
// 
// ----------------------------------------------------------------------------
//
void CChatBase::SetServer( TBool aServerMode )
    {
    iServerMode = aServerMode;
    }          

// ----------------------------------------------------------------------------
// CChatBase::SetState()
// 
// ----------------------------------------------------------------------------
//
void CChatBase::SetState( TChatState aState )
  {
  iState = aState;
  }

// ----------------------------------------------------------------------------
// CChatBase::State()
// 
// ----------------------------------------------------------------------------
//    
TInt CChatBase::State()
  {
  return iState;
  }

// ----------------------------------------------------------------------------
// CChatBase::IsReadyToSendMessage()
// True if the client can send a message.
// ----------------------------------------------------------------------------
//
TBool CChatBase::IsReadyToSendMessage()
    {
    return ( State() == EConnected );
    }

// ----------------------------------------------------------------------------
// CChatBase::IsConnected()
// True if the client can send a message.
// ----------------------------------------------------------------------------
//
TBool CChatBase::IsConnected()
    {
    return ( ( State() == EConnected )||( State() == ESendingMessage ) );
    }
    

// ----------------------------------------------------------------------------
// CChatBase::IsConnecting()
// True if is establishing a connection.
// ----------------------------------------------------------------------------
//
TBool CChatBase::IsConnecting()
    {
    return ( ( State() == EGettingDevice )
        ||
        ( State() == EGettingService )
        ||
        ( State() == EGettingConnection )
        || 
        ( State() == EConnecting )  
        );
    }

// ----------------------------------------------------------------------------
// CChatBase::IsSendingMessage()
// True if the client is connected.
// ----------------------------------------------------------------------------
//
TBool CChatBase::IsSendingMessage()
    {
    return ( State() == ESendingMessage );
    }




