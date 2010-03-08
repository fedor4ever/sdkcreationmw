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
* Description:  Abstract base for common plugins
*
*/



// INCLUDE FILES
#include "EcmtCommonPluginBase.h"
#include <bautils.h>
#include <barsc.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtCommonPluginBase::CEcmtCommonPluginBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtCommonPluginBase::CEcmtCommonPluginBase( TUid aUid, TUid aTargetUid ) : iUid( aUid ), iTargetUid( aTargetUid )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtCommonPluginBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtCommonPluginBase::ConstructL()
    {
    }

   
// Destructor
CEcmtCommonPluginBase::~CEcmtCommonPluginBase()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtCommonPluginBase::BindMessagingL
// 
// -----------------------------------------------------------------------------
//
void CEcmtCommonPluginBase::BindMessagingL( MEcmtMessaging* aMessaging )
    {
    iMessaging = aMessaging;
    iMessaging->AddMessageEventObserverL( *this, iUid );
    iMessageSender = iMessaging->GetMessageSender( );
    
    TRAPD( err, SendCurrentValuesL() );
    }

// -----------------------------------------------------------------------------
// CEcmtCommonPluginBase::AppendResourceStringL
// 
// -----------------------------------------------------------------------------
//
HBufC8* CEcmtCommonPluginBase::ReadResourceStringL( RFs& aFs, TInt aResourceId )
	{
	TFileName resFileName( _L("z:\\system\\libs\\plugins\\EcmtCommonPlugin.rsc") );
	BaflUtils::NearestLanguageFile( aFs, resFileName );
	RResourceFile resFile;
	resFile.OpenL( aFs, resFileName );
	CleanupClosePushL( resFile );
	resFile.ConfirmSignatureL();
	HBufC8* s = resFile.AllocReadL( aResourceId );
	CleanupStack::PopAndDestroy( );
	return s;
	}

// -----------------------------------------------------------------------------
// CEcmtCommonPluginBase::ErrorReply
// 
// -----------------------------------------------------------------------------
//
void CEcmtCommonPluginBase::ErrorReply( TInt aResourceId, const TDesC& aInfo )
    {
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtCommonPluginBase::ErrorReply" ) );
#endif
    RFs fs;
    TInt retval = fs.Connect();
    if ( retval == KErrNone )
    	{
		HBufC8* resString = 0;
    	TRAPD( err, resString = ReadResourceStringL( fs, aResourceId ) );
       	fs.Close();

		if ( err != KErrNone )
			{
			return;
			}

	    _LIT8( KErrorReply, "ERROR " );
	    _LIT8( KBlanco, " " );
    	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid,
    														KErrorReply().Length() +
    														resString->Length() + 
    														KBlanco().Length() + 
    														aInfo.Length() );
		if ( m )
			{
#ifdef ECMT_RDEBUG
			RDebug::Print( _L( "EcmtCommonPluginBase::ErrorReply: Sending a message" ) );
#endif
			TPtr8 msgData = iMessaging->Message( *m );
			msgData += KErrorReply;
			msgData += *resString;
			msgData += KBlanco;
			msgData.Append( aInfo );
	        iMessageSender->SendMessage( m );
    	    }
    	delete resString;
    	}
	}

// -----------------------------------------------------------------------------
// CEcmtCommonPluginBase::SuccessReply
// 
// -----------------------------------------------------------------------------
//
void CEcmtCommonPluginBase::SuccessReply( )
    {
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtCommonPluginBase::SuccessReply" ) );
#endif
    _LIT8( KOkReply, "OK" );
    
    CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, KOkReply() );
    if ( m )
        {
#ifdef ECMT_RDEBUG
		RDebug::Print( _L( "EcmtCommonPluginBase::SuccessReply: Sending a message" ) );
#endif
        iMessageSender->SendMessage( m );
        }
    }

//  End of File  
