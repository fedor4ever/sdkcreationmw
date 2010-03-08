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
#include <bt_sock.h>
#include <StringLoader.h>
#include <ChatEx.rsg>

#include "ChatServiceAdvertiser.h"
#include "ChatProtocolConstants.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatServiceAdvertiser* CChatServiceAdvertiser::NewL()
    {
    CChatServiceAdvertiser* self = CChatServiceAdvertiser::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatServiceAdvertiser* CChatServiceAdvertiser::NewLC()
    {
    CChatServiceAdvertiser* self = new ( ELeave ) CChatServiceAdvertiser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::CChatServiceAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatServiceAdvertiser::CChatServiceAdvertiser()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::~CChatServiceAdvertiser()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatServiceAdvertiser::~CChatServiceAdvertiser()
    {
    delete iServiceName;
    delete iServiceDescription;
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CChatServiceAdvertiser::ConstructL()
    {
    iServiceName = StringLoader::LoadL ( R_CHAT_SERVICE_NAME );
    iServiceDescription = StringLoader::LoadL ( R_CHAT_SERVICE_DESCRIPTION );
    }

// -----------------------------------------------------------------------------
// CMessageServiceAdvertiser::BuildProtocolDescriptionL()
// Builds the protocol description.
// -----------------------------------------------------------------------------
//
void CChatServiceAdvertiser
::BuildProtocolDescriptionL( CSdpAttrValueDES* aProtocolDescriptor, TInt aPort )
    {
    TBuf8<1> channel;
    channel.Append( ( TChar )aPort );

    aProtocolDescriptor
      ->StartListL()
        ->BuildDESL()
        ->StartListL()   //  Details of lowest level protocol
        ->BuildUUIDL( KL2CAP )
        ->EndListL()

        ->BuildDESL()
        ->StartListL()
        ->BuildUUIDL( KRFCOMM )
        ->BuildUintL( channel )
        ->EndListL()
      ->EndListL();
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::ServiceName()
// ----------------------------------------------------------------------------
//
const TDesC& CChatServiceAdvertiser::ServiceName()
    {
    return *iServiceName;
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::ServiceDescription()
// ----------------------------------------------------------------------------
//
const TDesC& CChatServiceAdvertiser::ServiceDescription()
    {
    return *iServiceDescription;
    }

// ----------------------------------------------------------------------------
// CChatServiceAdvertiser::ServiceClass()
// ----------------------------------------------------------------------------
//
TInt CChatServiceAdvertiser::ServiceClass()
    {
    return KServiceClass;
    }

// End of File
