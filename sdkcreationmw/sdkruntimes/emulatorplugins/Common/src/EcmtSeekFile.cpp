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
* Description:  Utility class for file manipulation
*
*/



// INCLUDE FILES
#include "EcmtSeekFile.h"
#include <windows.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// REcmtSeekFile::REcmtSeekFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
REcmtSeekFile::REcmtSeekFile( TDesC& aFileName ) :
    REcmtFile( aFileName )
    {
    }
    
// -----------------------------------------------------------------------------
// REctmSeekFile::~REcmtSeekFile
// Release resources in case we are an automatic variable on stack
// -----------------------------------------------------------------------------
REcmtSeekFile::~REcmtSeekFile()
    {
    }

// -----------------------------------------------------------------------------
// REcmtSeekFile::ResetPosition
// 
// -----------------------------------------------------------------------------
//
void REcmtSeekFile::ResetPosition( )
	{
    TPtr8 x( iData, iLen, iLen );
    
    iLexer.Assign( x );
	}

// -----------------------------------------------------------------------------
// REcmtSeekFile::SkipLines
// 
// -----------------------------------------------------------------------------
//
void REcmtSeekFile::SkipLines( TInt aLines )
	{
	TInt skipped(0);
	
	while ( !iLexer.Eos() && iLexer.Get() != '\n' && ( skipped < aLines) )
      	{
      	skipped++;
    	}
	}

// -----------------------------------------------------------------------------
// REcmtSeekFile::NextLine
// 
// -----------------------------------------------------------------------------
//
const TPtrC8 REcmtSeekFile::NextLine( )
	{
    TPtrC8 line;

	if ( !iLexer.Eos() )
		{
	    iLexer.Mark();
        
	    while ( !iLexer.Eos() && iLexer.Get() != '\n' )
    	  	{
      		}

    	line.Set( iLexer.MarkedToken() );
        iLexer.Mark();

        return line;
        }
    else
        {
        return KNullDesC8();
        }
	}

// -----------------------------------------------------------------------------
// REcmtSeekFile::HasNext
// 
// -----------------------------------------------------------------------------
//
TBool REcmtSeekFile::HasNext( )
	{
	if ( iLexer.Eos() )
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

//  End of File  
