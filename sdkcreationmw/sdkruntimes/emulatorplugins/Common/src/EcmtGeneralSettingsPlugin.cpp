/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Scalable UI and Memory Size Configuration Plugin
*
*/



// INCLUDE FILES
#include "EcmtMessageEvent.h"
#include "EcmtGeneralSettingsPlugin.h"
#include "EcmtFile.h"
#include "EcmtSeekFile.h"
#include "EcmtOsUtils.h"

// CONSTANTS
const TInt KMaxEmuConfigMsgLen = 64;

_LIT8( KMemory, "MEMORY" );
_LIT8( KBlanco, " " );
_LIT8( KComma, "," );
_LIT8( KMemoryInIni, "MegabytesOfFreeMemory" );
_LIT8( KConfiguration, "configuration" );
_LIT8( KCrLf, "\r\n" );
_LIT( KCrLf16, "\r\n" );
_LIT( KLinePrefix, "configuration epoc_" );
_LIT( KLinePostfix, ".ini" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtGeneralSettingsPlugin::CEcmtGeneralSettingsPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtGeneralSettingsPlugin::CEcmtGeneralSettingsPlugin() :
    CEcmtCommonPluginBase( TUid::Uid( KGENERALSETTINGSPLUGIN_IMPL_UID ), TUid::Uid( KJAVA_GENERALSETTINGSPLUGIN_IMPL_UID ) )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtGeneralSettingsPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtGeneralSettingsPlugin::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtGeneralSettingsPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CEcmtGeneralSettingsPlugin::NewL()
    {
    CEcmtGeneralSettingsPlugin* self = new( ELeave ) CEcmtGeneralSettingsPlugin;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtGeneralSettingsPlugin::~CEcmtGeneralSettingsPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtGeneralSettingsPlugin::SendCurrentValues
// 
// -----------------------------------------------------------------------------
//
void CEcmtGeneralSettingsPlugin::SendCurrentValuesL( )
    {
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

	// Create a message.
	
   	TBuf8<KMaxEmuConfigMsgLen> memMsg;

	// Search for active .ini files.

	TInt found( 0 );
	TPtrC8 iniFileName;
	
	while ( seekFile.HasNext() )
		{
		iniFileName.Set( seekFile.NextLine() );
		
		TLex8 iniLexer( iniFileName );

		// Extract the next resolution.

		if ( !iniLexer.Eos() && ( iniLexer.NextToken() == KConfiguration ) )
			{
			// Extract the actual ini file name from the "configuration..." string.
		
			iniLexer.SkipSpace();

			// Extract resolution string if not empty line.
			
			if ( iniFileName.Length() - iniLexer.Offset() > 5 )
				{
				iniLexer.Mark(); // Is this...
				GetScalableIniFilename( iniLexer.NextToken(), buff );
				iniLexer.UnGetToMark(); // ... and this needed? Does NextToken() increment the lexer?

				iniLexer.SkipAndMark(5); // "epoc_"
				
				while ( !iniLexer.Eos() && ( iniLexer.Peek() != '.' ) ) // .ini
					{
					iniLexer.Inc();
					}
					
				found++;
				}
			}
			
		// Read settings from the first ini file.
		
		if ( found == 1 ) 
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
				
		    TPtrC8 line;
		   	line.Set( file.FindLine( KMemoryInIni()) );
		   	
		    if ( line.Length() == 0 )
		        {
		        return;
		        }

    		memMsg += KMemory;
    		memMsg += KBlanco;
		    
		    TLex8 lexer( line );
		    lexer.SkipCharacters();
		    memMsg += lexer.NextToken();
			}
		}

	if ( found > 0 ) 
		{
		// Send the message to the GUI.

	    CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, memMsg );
	    
	    if ( m )
	        {
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
// CEcmtGeneralSettingsPlugin::GetScalableIniFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtGeneralSettingsPlugin::GetScalableIniFilename( TPtrC8 aName, TDes& aBuff ) const
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
// CEcmtGeneralSettingsPlugin::GetEpocIniFilename
// 
// -----------------------------------------------------------------------------
//
void CEcmtGeneralSettingsPlugin::GetEpocIniFilename( TDes& aBuff ) const
    {
    aBuff.Copy( EcmtOsUtils::Epocroot() );
    _LIT( KEpocIni, "epoc32\\data\\epoc.ini" );

    aBuff += KEpocIni;
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtGeneralSettingsPlugin::GetIniLine
// 
// -----------------------------------------------------------------------------
//
void CEcmtGeneralSettingsPlugin::GetIniLine( TPtrC8 aName, TDes& aBuff ) const
    {
    TBuf<KMaxWin32Path> temp;
    
    temp.Copy( aName );
    aBuff.Copy( KLinePrefix );
    aBuff += temp;
    aBuff += KLinePostfix;
    aBuff += KCrLf16;
    }

// -----------------------------------------------------------------------------
// CEcmtGeneralSettingsPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CEcmtGeneralSettingsPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
    {
    const TPtrC8 m = iMessaging->Message( aEvent );
    TLex8 lexer( m );
    TPtrC8 defaultMem = lexer.NextToken();
    TPtrC8 defaultMemVal = lexer.NextToken();
    if ( /* ( defaultIni != KDefaultIni ) || */
    	 ( defaultMem != KMemory ) )
        {
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }

	/*
	* Move default ini file line to the top of EPOC.INI
	*/

    TBuf<KMaxWin32Path> buff;
    GetEpocIniFilename( buff );        

    REcmtFile epocFile( buff );
    epocFile.Read();

    if ( !epocFile.IsGood() )
        {
		ErrorReply( R_FILE_NOT_FOUND, buff );
        return;
        }
		
	// Go through all resolution ini files and write the memory setting.
	
    REcmtSeekFile seekFile( buff );
    seekFile.Read();

    if ( !seekFile.IsGood() )
        {
		ErrorReply( R_FILE_NOT_FOUND, buff );
        return;
        }

	seekFile.ResetPosition();
	
	TInt activeFiles( 0 );
	TPtrC8 iniFileName;
	
	while ( seekFile.HasNext() )
		{
		iniFileName.Set( seekFile.NextLine() );
		
		TLex8 iniLexer( iniFileName );

		if ( !iniLexer.Eos() && ( iniLexer.NextToken() == KConfiguration ) )
			{
			// Extract the actual ini file name from the "configuration..." string.
		
			GetScalableIniFilename( iniLexer.NextToken(), buff );

		    REcmtFile file( buff );
    		file.Read();
    		
		    if ( !file.IsGood() )
		        {
				ErrorReply( R_FILE_NOT_FOUND, buff );
		        return;
		        }
		        
		    activeFiles++;
		    
		    // Set Memory Capacity
		    TPtrC8 line( file.FindLine( KMemoryInIni()) );
		    if ( line.Length() == 0 )
		        {
		        ErrorReply( R_FILE_CORRUPTED, buff );
		        return;
		        }
		        
		    TBuf8< 64 > newLine;
		    newLine += KMemoryInIni;
		    newLine += KBlanco;
		    newLine += defaultMemVal;
		    newLine += KCrLf;

		    if ( !file.ReplaceLine( line, newLine ) )
		        {
				ErrorReply( R_INTERNAL_ERROR, KNullDesC );
		        return;
		        }

		    // Commint changes to epoc_XXxxYYY.ini
		    if ( !file.Write() )
		        {
				ErrorReply( R_FILE_WRITE_ERROR, buff );
		        return;
		        }
		        
		    file.Close();
		        
	    	SuccessReply();
			}
		}

    }

//  End of File  
