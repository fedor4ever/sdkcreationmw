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
#include "ChatServiceSearcher.h"
#include "ChatBtServiceSearcher.pan"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CChatServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatServiceSearcher* CChatServiceSearcher::NewL( MLog& aLog )
    {
    CChatServiceSearcher* self = CChatServiceSearcher::NewLC( aLog );
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CChatServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatServiceSearcher* CChatServiceSearcher::NewLC( MLog& aLog )
    {
    CChatServiceSearcher* self = new ( ELeave ) CChatServiceSearcher( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::CChatServiceSearcher()
// Construcor.
// ----------------------------------------------------------------------------
//
CChatServiceSearcher::CChatServiceSearcher( MLog& aLog )
: CChatBtServiceSearcher( aLog ),
  iServiceClass( KServiceClass ),
  iPort( -1 )
    {
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::~CChatServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatServiceSearcher::~CChatServiceSearcher()
    {
    iProtocolArray.Close();
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CChatServiceSearcher::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CChatServiceSearcher::ServiceClass() const
    {
    return iServiceClass;
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::ProtocolList()
// The list of Protocols required by the service.
// ----------------------------------------------------------------------------
//
 RArray <TChatSdpAttributeParser::TChatSdpAttributeNode>& CChatServiceSearcher
::ProtocolList()
    {
    TChatSdpAttributeParser::TChatSdpAttributeNode attrib;
    attrib.SetCommand( TChatSdpAttributeParser::ECheckType );
    attrib.SetType( ETypeDES );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TChatSdpAttributeParser::ECheckType );
    attrib.SetType( ETypeDES );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TChatSdpAttributeParser::ECheckValue );
    attrib.SetType( ETypeUUID );
    attrib.SetValue( KL2CAP );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TChatSdpAttributeParser::ECheckEnd );
    iProtocolArray.Append( attrib );
  
    attrib.SetCommand( TChatSdpAttributeParser::ECheckType );
    attrib.SetType( ETypeDES );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TChatSdpAttributeParser::ECheckValue );
    attrib.SetType( ETypeUUID );
    attrib.SetValue( KRFCOMM );
    iProtocolArray.Append( attrib );
  
    attrib.SetCommand( TChatSdpAttributeParser::EReadValue );
    attrib.SetType( ETypeUint );
    attrib.SetValue( KRfcommChannel );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TChatSdpAttributeParser::ECheckEnd );
    iProtocolArray.Append( attrib );
  
    attrib.SetCommand( TChatSdpAttributeParser::ECheckEnd );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TChatSdpAttributeParser::EFinished );
    iProtocolArray.Append( attrib );
  
    return iProtocolArray;
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CChatServiceSearcher::FoundElementL( TInt aKey, CSdpAttrValue& aValue )
    {
    __ASSERT_ALWAYS( aKey == static_cast<TInt>( KRfcommChannel ), 
        Panic( EChatBtServiceSearcherProtocolRead ) );
    iPort = aValue.Uint();
    }

// ----------------------------------------------------------------------------
// CChatServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CChatServiceSearcher::Port()
    {
    return iPort;
    }

// End of File
