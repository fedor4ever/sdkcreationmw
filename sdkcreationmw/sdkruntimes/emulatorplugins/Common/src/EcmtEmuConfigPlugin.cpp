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
* Description:  Emulator Configuration Plugin
*
*/



// INCLUDE FILES
#include "EcmtMessageEvent.h"
#include "EcmtEmuConfigPlugin.h"
#include "EcmtFile.h"
#include "EcmtSeekFile.h"
#include "EcmtOsUtils.h"

// CONSTANTS
const TInt KMaxEmuConfigMsgLen = 64;

_LIT8( KJustInTime, "JUSTINTIME" );
_LIT8( KPanics, "PANICS" );
_LIT8( KDebugInfo, "DEBUGINFO" );
_LIT8( KConfiguration, "configuration" );
_LIT8( KBlanco, " " );
_LIT8( KCrLf, "\r\n" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::CEcmtEmuConfigPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtEmuConfigPlugin::CEcmtEmuConfigPlugin() :
    CEcmtCommonPluginBase( TUid::Uid( KEMUCONFIGPLUGIN_IMPL_UID ), TUid::Uid( KJAVA_EMUCONFIGPLUGIN_IMPL_UID ) )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CEcmtEmuConfigPlugin::NewL()
    {
    CEcmtEmuConfigPlugin* self = new( ELeave ) CEcmtEmuConfigPlugin;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtEmuConfigPlugin::~CEcmtEmuConfigPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::SendCurrentValues
// 
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::SendCurrentValuesL( )
    {
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtEmuConfigPlugin::SendCurrentValuesL" ) );
#endif
    // Search for the first active configuration file name in epoc.ini.
    
    TBuf<KMaxWin32Path> buff;
    GetEpocIniFilename( buff );        
    REcmtSeekFile seekFile( buff );
    
    seekFile.Read();

    if ( !seekFile.IsGood() )
        {
		ErrorReply( R_FILE_NOT_FOUND, buff );
        return;
        }

	seekFile.ResetPosition();
	
	TBool found( EFalse );
	TPtrC8 iniFileName;
	
	while ( seekFile.HasNext() )
		{
		iniFileName.Set( seekFile.NextLine() );
		
		TLex8 iniLexer( iniFileName );

		if ( !iniLexer.Eos() && ( iniLexer.NextToken() == KConfiguration ) )
			{
			// Extract the actual ini file name from the "configuration..." string.
		
			GetScalableIniFilename( iniLexer.NextToken(), buff );
			found = ETrue;
			}
		}
			
	if ( found ) 
		{
		// Open file for reading.
		
	    REcmtFile file( buff );
   		file.Read();
	    if ( !file.IsGood() )
	        {
			ErrorReply( R_FILE_NOT_FOUND, buff );
	        return;
	        }
			
		// Read configuration.
			
    	TBuf8<KMaxEmuConfigMsgLen> msg;
	    msg += KJustInTime;
	    msg += KBlanco;
	    msg.Append( User::JustInTime() ? '1' : '0' );
	    
	    msg += KBlanco;
	    msg += KPanics;
	    msg += KBlanco;
	    GetErrRdFilename( buff );
	    REcmtFile panicFile( buff );
	    msg.Append( panicFile.Exists( ) ? '1' : '0' );
	    
	    msg += KBlanco;
	    msg += KDebugInfo;
	    msg += KBlanco;
	    GetEpocWindFilename( buff );
	    REcmtFile epocwindFile( buff );
	    msg.Append( epocwindFile.Exists() && epocwindFile.ReadOnly( ) ? '0' : '1' );

		// Create a message for sending to the GUI.

	    CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, msg );
	    if ( m )
	        {
#ifdef ECMT_RDEBUG
			RDebug::Print( _L( "EcmtEmuConfigPlugin::SendCurrentValuesL: Sending a message" ) );
#endif
	        iMessageSender->SendMessage( m );
	        }
		}
	else
		{
		ErrorReply( R_NO_ACTIVE_INI_FILES, KNullDesC );
        return;
		}
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::GetEpocIniFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::GetEpocIniFilename( TDes& aBuff ) const
    {
    aBuff.Copy( EcmtOsUtils::Epocroot() );
    _LIT( KEpocIni, "epoc32\\data\\epoc.ini" );

    aBuff += KEpocIni;
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::GetScalableIniFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::GetScalableIniFilename( TPtrC8 aName, TDes& aBuff ) const
    {
    TBuf<KMaxWin32Path> temp;
    
    temp.Copy( aName );
    aBuff.Copy( EcmtOsUtils::Epocroot() );
    _LIT( KEpocIni, "epoc32\\data\\" );

    aBuff += KEpocIni;
    aBuff += temp;
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::GetErrRdFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::GetErrRdFilename( TDes& aBuff ) const
    {
    aBuff.Copy( EcmtOsUtils::Epocroot() );

    #ifdef __WINSCW__
    _LIT( KErrRd, "epoc32\\winscw\\c\\resource\\ErrRd" );
    #else
    _LIT( KErrRd, "epoc32\\wins\\c\\resource\\ErrRd" );
    #endif

    aBuff += KErrRd;
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::GetEpocWindFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::GetEpocWindFilename( TDes& aBuff ) const
    {
    aBuff.Copy( EcmtOsUtils::TempDir() );
    _LIT( KEpocWind, "epocwind.out" );

    aBuff += KEpocWind;
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtEmuConfigPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CEcmtEmuConfigPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
    {
    const TPtrC8 m = iMessaging->Message( aEvent );

    TLex8 lexer( m );
    TPtrC8 justInTime = lexer.NextToken();
    TPtrC8 justInTimeVal = lexer.NextToken();
    TPtrC8 panics = lexer.NextToken();
    TPtrC8 panicsVal = lexer.NextToken();
    TPtrC8 debug = lexer.NextToken();
    TPtrC8 debugVal = lexer.NextToken();

    if ( justInTime != KJustInTime || 
         panics != KPanics || 
         debug != KDebugInfo )
        {
#ifdef ECMT_RDEBUG
		RDebug::Print( _L( "EcmtEmuConfigPlugin: Invalid message" ) );
#endif
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }

    /*
     * Handle JUST_IN_TIME
     */
    
    User::SetJustInTime( justInTimeVal[0] == '1' ? ETrue : EFalse );    
    
    /*
     * Handle ErrRd
     */
    
    TBuf<KMaxWin32Path> buff;

    GetErrRdFilename( buff );

    REcmtFile panicFile( buff );
    TBool success( ETrue );

    if ( panicsVal[0] == '0' && panicFile.Exists( ) )
	{
	success = panicFile.Delete( );
	}
    else if ( panicsVal[0] == '1' && !panicFile.Exists( ) )
	{
	success = panicFile.Create( );
	}

    if ( !success )
	{
	ErrorReply( R_FILE_WRITE_ERROR, buff );
	return;
	}
    
    /*
     * Handle EpocWnd.out
     */
    
    // Set debuginfo
    GetEpocWindFilename( buff );

    REcmtFile epocwindFile( buff );

    if ( debugVal[0] == '0' && !epocwindFile.ReadOnly( ) )
	{
	if ( epocwindFile.Exists( ) )
	    {
	    epocwindFile.Delete( );
	    }

	epocwindFile.Create();
	success = epocwindFile.SetReadOnly( );
	}
    else if ( debugVal[0] == '1' && epocwindFile.Exists( ) && epocwindFile.ReadOnly( ) )
	{
	success = epocwindFile.ResetReadOnly( );
	}

    if ( !success )
	{
	ErrorReply( R_FILE_WRITE_ERROR, buff );
	return;
	}
    
    SuccessReply();
    return;
    }

//  End of File  
