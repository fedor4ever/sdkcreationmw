/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NCILOGGER_H
#define NCILOGGER_H

#include <e32std.h>


// Debugging headers
#include <e32svr.h>
#include <flogger.h>


// Strings for flogger directory and file names
_LIT( KLogDirectory, "NCIMsgServer" );
_LIT( KLogFileName, "NCIMsgServer.txt" );

inline void Debug( const TDesC& aMsg )
	{
	// Print to log file
	RFileLogger::Write( KLogDirectory, KLogFileName, EFileLoggingModeAppend, aMsg );

	// Print to debugger (WINS only)
	RDebug::Print( aMsg );
	}



inline void DebugString( const TDesC& aMsg, const TDesC& aString )
	{
	// Print to log file
	RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend,
							 aMsg, &aString );

	// Print to debugger (WINS only)
	RDebug::Print( aMsg, &aString );
	}



inline void DebugInt( const TDesC& aMsg, TInt aNum )
	{
	// Print to log file
	RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend,
							  aMsg, aNum );


	RDebug::Print( aMsg, aNum );

	}



inline void DebugInt2( const TDesC& aMsg, TInt aNum, TInt aNum2 )
	{
	// Print to log file
	RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend,
							  aMsg, aNum, aNum2 );

	// Print to debugger (WINS only)
	RDebug::Print( aMsg, aNum, aNum2 );
	}


#define NCI_DEBUG( msg ) Debug( msg )
#define NCI_DEBUG_STR( msg, string ) DebugString( msg , string )
#define NCI_DEBUG_INT( msg, num ) DebugInt( msg , num )
#define NCI_DEBUG_INT2( msg, num, num2 ) DebugInt2(  msg , num, num2 )
#define NCI_DEBUG_ERR( msg, num ) if ( num < KErrNone ) DebugInt(  msg, num )



#endif // NCILOGGER_H