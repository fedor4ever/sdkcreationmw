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
* Description:  Pan plugin
*
*/



// INCLUDE FILES
#include "EcmtMessageEvent.h"
#include "EcmtPanPlugin.h"
#include "EcmtFile.h"
#include "EcmtOsUtils.h"
#include <emulator.h>
#undef SendMessage

// CONSTANTS
const TInt KMaxPanMsgLen = 64;

_LIT8( KBtCom, "BTCOM" );
_LIT8( KHci, "HCI" );
_LIT8( KIrdaCom, "IRDACOM" );

_LIT8( KBtPort, "port=" );
_LIT8( KHciDll, "hcidllfilename=" );
_LIT8( KIrdaPort, "irPhysicalComPort=" );

_LIT8( KHciDllH4, "hci_h4.dll" );
_LIT8( KHciDllBcsp, "hci_bcsp.dll" );
_LIT8( KHciDllUsb, "hci_usb.dll" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::CEcmtPanPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtPanPlugin::CEcmtPanPlugin() :
    CEcmtCommonPluginBase( TUid::Uid( KPANPLUGIN_IMPL_UID), TUid::Uid( KJAVA_PANPLUGIN_IMPL_UID) )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtPanPlugin::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CEcmtPanPlugin::NewL()
    {
    CEcmtPanPlugin* self = new( ELeave ) CEcmtPanPlugin;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtPanPlugin::~CEcmtPanPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::SendCurrentValues
// 
// -----------------------------------------------------------------------------
//
void CEcmtPanPlugin::SendCurrentValuesL( )
    {
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtPanPlugin::SendCurrentValuesL" ) );
#endif
    TBuf<KMaxWin32Path> buf;
    TBuf8<KMaxPanMsgLen> msg;
	TPtrC8 line;
	TLex8 lexer;
	
	/*
	* Handle bt.esk
	*/
    GetBtEskFilename( buf );
    
    REcmtFile btFile( buf );
    btFile.Read();
    if ( !btFile.IsGood() )
        {
        return;
        }

    line.Set( btFile.FindLine( KBtPort ) );
    if ( line.Length() == 0 )
        {
        return;
        }

    msg.Append( KBtCom );
    msg.Append( ' ' );

    lexer.Assign( line );
    lexer.SkipCharacters();
    msg.Append( lexer.NextToken() );
    msg.Append( ' ' );
    
    line.Set( btFile.FindLine( KHciDll ) );
    if ( line.Length() == 0 )
        {
        return;
        }

    msg.Append( KHci );
    msg.Append( ' ' );
    lexer.Assign( line );
    lexer.SkipCharacters();
    TPtrC8 dll( lexer.NextToken() );
    if ( dll.CompareF( KHciDllBcsp ) == 0 )
        {
        msg.Append( '0' );
        }
    else if ( dll.CompareF( KHciDllH4 ) == 0 )
        {
        msg.Append( '1' );
        }
    else if ( dll.CompareF( KHciDllUsb ) == 0 )
        {
        msg.Append( '2' );
        }
    else
        {
        msg.Append( '-1' );
        }

	/*
	* Handle irda.esk
	*/
    GetIrdaEskFilename( buf );
    
    REcmtFile irdaFile( buf );
    irdaFile.Read();
    if ( !irdaFile.IsGood() )
        {
        return;
        }

    line.Set( irdaFile.FindLine( KIrdaPort ) );
    if ( line.Length() == 0 )
        {
        return;
        }

    msg.Append( ' ' );
    msg.Append( KIrdaCom );
    msg.Append( ' ' );

    lexer.Assign( line );
    lexer.SkipCharacters();
    msg.Append( lexer.NextToken() );

	/*
	* Send values
	*/
    CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, msg );
    if ( m )
        {
#ifdef ECMT_RDEBUG
		RDebug::Print( _L( "EcmtPanPlugin::SendCurrentValuesL: Sending a message" ) );
#endif
        iMessageSender->SendMessage( m );
        }
    }

// -----------------------------------------------------------------------------
// Shared by GetBtEskFilename and GetIrdaEskFilename
// -----------------------------------------------------------------------------
static void GetEskFilename( TDes& aBuff, const TDesC& aFileName )
{
    TInt maxLen = aBuff.MaxLength();
    aBuff.SetLength(maxLen);
    ::GetModuleFileName(NULL, &(aBuff[0]), maxLen);
    aBuff[aBuff.MaxLength() - 1] = 0;
    aBuff.SetLength(User::StringLength(aBuff.Ptr()));
    aBuff.SetLength(aBuff.LocateReverse('\\') + 1);
    aBuff.Append(_L( "z\\private\\101f7989\\ESock\\"));
    aBuff.Append(aFileName);
    aBuff.ZeroTerminate();
}

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::GetBtEskFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtPanPlugin::GetBtEskFilename( TDes& aBuff ) const
    {
    GetEskFilename(aBuff, _L("bt.bt.esk"));
    }

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::GetIrdaEskFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtPanPlugin::GetIrdaEskFilename( TDes& aBuff ) const
    {
    GetEskFilename(aBuff, _L("ir.irda.esk"));
    }

// -----------------------------------------------------------------------------
// CEcmtPanPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CEcmtPanPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
    {
    const TPtrC8 m = iMessaging->Message( aEvent );
    TLex8 lexer( m );
    
    TPtrC8 btCom = lexer.NextToken();
    TPtrC8 btComVal = lexer.NextToken();
    TPtrC8 hci = lexer.NextToken();
    TPtrC8 hciVal = lexer.NextToken();
    TPtrC8 irdaCom = lexer.NextToken();
    TPtrC8 irdaComVal = lexer.NextToken();
    
    if ( btCom != KBtCom || hci != KHci || irdaCom != KIrdaCom )
        {
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }
        
    _LIT8( KMinusOne, "-1" );
    if ( btComVal != KMinusOne && btComVal == irdaComVal )
    	{
    	ErrorReply( R_IRDA_BT_ERROR, KNullDesC );
    	return;
    	}

    TBuf<KMaxWin32Path> buff;
    TPtrC8 line;

	/*
	* Handle bt.esk
	*/
    GetBtEskFilename( buff );        
    REcmtFile btFile( buff );
    btFile.Read();
    if ( !btFile.IsGood() )
        {
		ErrorReply( R_FILE_NOT_FOUND, buff );
        return;
        }
    
    // Set port number    
    line.Set( btFile.FindLine( KBtPort ) );
    if ( line.Length() == 0 )
        {
        ErrorReply( R_FILE_CORRUPTED, buff );
        return;
        }
        
    TBuf8< 64 > newLine;
    newLine.Append( KBtPort );
    newLine.Append( _L(" ") );
    newLine.Append( btComVal );
    newLine.Append( _L("\r\n") );
    
    if ( !btFile.ReplaceLine( line, newLine ) )
        {
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }

    // Set hci dll
    line.Set( btFile.FindLine( KHciDll ) );
    if ( line.Length() == 0 )
        {
        ErrorReply( R_FILE_CORRUPTED, buff );
        return;
        }
    
    newLine.Zero();        
    newLine.Append( KHciDll );
    newLine.Append( _L(" ") );
    if ( hciVal[0] == '0' )
        {
        newLine.Append( KHciDllBcsp );
        }
    else if ( hciVal[0] == '1' )
        {
        newLine.Append( KHciDllH4 );
        }
    else if ( hciVal[0] == '2' )
        {
        newLine.Append( KHciDllUsb );
        }
    else
        {
        newLine.Append( KHciDllH4 );
        }
    newLine.Append( _L("\r\n") );
    
    if ( !btFile.ReplaceLine( line, newLine ) )
        {
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }
    if ( !btFile.Write() )
        {
		ErrorReply( R_FILE_WRITE_ERROR, buff );
        return;
        }

	/*
	* Handle irda.esk
	*/
    GetIrdaEskFilename( buff );        
    REcmtFile irdaFile( buff );
    irdaFile.Read();
    if ( !irdaFile.IsGood() )
        {
		ErrorReply( R_FILE_NOT_FOUND, buff );
        return;
        }
    
    // Set port number    
    line.Set( irdaFile.FindLine( KIrdaPort ) );
    if ( line.Length() == 0 )
        {
        ErrorReply( R_FILE_CORRUPTED, buff );
        return;
        }
        
    newLine.Zero();        
    newLine.Append( KIrdaPort );
    newLine.Append( _L(" ") );
    newLine.Append( irdaComVal );
    newLine.Append( _L("\r\n") );
    
    if ( !irdaFile.ReplaceLine( line, newLine ) )
        {
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }
    if ( !irdaFile.Write() )
        {
		ErrorReply( R_FILE_WRITE_ERROR, buff );
        return;
        }

    SuccessReply();
    }

//  End of File  
