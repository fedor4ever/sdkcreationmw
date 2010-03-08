/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class representing the messages and message events of the system
*
*/



// INCLUDE FILES
#include "EcmtMessageEvent.h"
#include "EcmtMessageFormat.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::CEcmtMessageEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtMessageEvent::CEcmtMessageEvent() :
    iPtr( 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::SetHeader
// 
// -----------------------------------------------------------------------------
//
void CEcmtMessageEvent::SetHeader( )
	{
    iPtr.Append( _L( "0x" ));
    iPtr.AppendNumFixedWidthUC( iPtr.MaxLength(), EHex, KEcmtMessageHexDigits );
    iPtr.Append( _L( "0x" ));
    iPtr.AppendNumFixedWidthUC( iDest.iUid, EHex, KEcmtMessageHexDigits );
    iPtr.LowerCase();
	}

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtMessageEvent::ConstructL( TUid aDest, const TDesC8& aData )
    {
    TInt len = aData.Length() + KEcmtMessageHeaderLength;
    iData = (TUint8*)User::AllocL( len );
    iPtr.Set( iData, 0, len );
    iDest = aDest;
	SetHeader();
    iPtr.Append( aData );
    }

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtMessageEvent* CEcmtMessageEvent::NewL( TUid aDest, const TDesC8& aData )
    {
    CEcmtMessageEvent* me = new( ELeave ) CEcmtMessageEvent;
    
    CleanupStack::PushL( me );
    me->ConstructL( aDest, aData );
    CleanupStack::Pop();

    return me;
    }

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::New
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEcmtMessageEvent* CEcmtMessageEvent::New( TUid aDest, TInt aDataLen )
    {
    CEcmtMessageEvent* me = 0;

    TRAPD( err, me = new( ELeave ) CEcmtMessageEvent );

    if ( err == KErrNone )
        {
        me->iData = (TUint8*)User::Alloc( aDataLen );
        me->iPtr.Set( me->iData, 0, aDataLen );
        me->iDest = aDest;
        
        if ( !me->iData )
            {
            delete me;
            me = 0;
            }
        }
    return me;
    }

    
// Destructor
CEcmtMessageEvent::~CEcmtMessageEvent()
    {
    if ( iData )
    	{
        User::Free( iData );
    	}
    }

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::Uid
// 
// -----------------------------------------------------------------------------
//
const TUid CEcmtMessageEvent::Uid( ) const
    {
    return iDest;
    }

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::Message
// 
// -----------------------------------------------------------------------------
//
const TPtrC8 CEcmtMessageEvent::Message( ) const
    {
    TPtr8 m( iData+KEcmtMessageHeaderLength, 
    		 iPtr.MaxLength()-KEcmtMessageHeaderLength, 
    		 iPtr.MaxLength()-KEcmtMessageHeaderLength );
    return m;
    }

// -----------------------------------------------------------------------------
// CEcmtMessageEvent::Message
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TPtr8 CEcmtMessageEvent::Message( )
    {
    TPtr8 m( iData+KEcmtMessageHeaderLength, 
    		 0,
    		 iPtr.MaxLength()-KEcmtMessageHeaderLength );
    SetHeader();
    iPtr.SetLength( iPtr.MaxLength() );
    return m;
    }


//  End of File  
