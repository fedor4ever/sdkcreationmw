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
#include "EcmtFile.h"
#include <windows.h>

// For some reason this isn't always defined in WinDef.h
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

// CONSTANTS
_LIT8( KEmpty, "" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// REcmtFile::REcmtFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
REcmtFile::REcmtFile( TDesC& aFileName ) :
    iGood(0), 
    iData(0),
    iMemHandle(0),
    iLen(0),
    iMaxLen(0),
    iHandle(0),
    iName( aFileName )
    {
    }
    
// -----------------------------------------------------------------------------
// REcmtFile::Read
// 
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::Read( )
    {
    iGood = EFalse;
    // Use native Win32 file api to be able to read outside \epoc32 tree
    // and not to consume emulator memory pool.
    iHandle = CreateFile( iName.Ptr(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
             
    if ( iHandle != INVALID_HANDLE_VALUE )
        {
        DWORD s = GetFileSize( iHandle, 0 );
        iMaxLen = ( ( s/0x1000 ) + 2 ) * 0x1000;        //reserve additional space to allow editing
        
        if ( (iMemHandle = GlobalAlloc( GPTR, iMaxLen ) ) != 0 && (iData = (unsigned char*)GlobalLock( iMemHandle ) ) != 0 )
            {
            iGood = ReadFile( iHandle, iData, s, &iLen, (OVERLAPPED*)0 );
            }
        }
    return iGood;
    }

    
// -----------------------------------------------------------------------------
// REctmFile::~REcmtFile
// Release resources in case we are an automatic variable on stack
// -----------------------------------------------------------------------------
REcmtFile::~REcmtFile()
    {
    Close( );
    }

// -----------------------------------------------------------------------------
// REcmtFile::Close
// Implemented to support CleanupClosePushL in leaving methods
// -----------------------------------------------------------------------------
void REcmtFile::Close( )
	{
    if ( iHandle != 0 && iHandle != INVALID_HANDLE_VALUE )
        {
        CloseHandle( iHandle );
        iHandle = 0;
        }
    if ( iData )
        {
        GlobalUnlock( iData );
        iData = 0;
		}
    if ( iMemHandle )
        {
        GlobalFree( iMemHandle );
        iMemHandle = 0;
        }
	}

// -----------------------------------------------------------------------------
// REcmtFile::Exist
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::Exists( )
    {
    HANDLE h = CreateFile( iName.Ptr(), GENERIC_READ, FILE_SHARE_READ, NULL,
             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
    CloseHandle( h );
    return h != INVALID_HANDLE_VALUE;    
    }

// -----------------------------------------------------------------------------
// REcmtFile::Create
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::Create( )
    {
    HANDLE h = CreateFile( iName.Ptr(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL,
             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    
    CloseHandle( h );
    return h != INVALID_HANDLE_VALUE;    
    }

// -----------------------------------------------------------------------------
// REcmtFile::Delete
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::Delete( )
    {
    return DeleteFile( iName.Ptr() );
    }

// -----------------------------------------------------------------------------
// REcmtFile::ReadOnly
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::ReadOnly( )
    {
    DWORD attrib = GetFileAttributes( iName.Ptr() );
    // undefined if file didn't exits
    return attrib & FILE_ATTRIBUTE_READONLY;
    }

// -----------------------------------------------------------------------------
// REcmtFile::SetReadOnly
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::SetReadOnly( )
    {
    DWORD attrib = GetFileAttributes( iName.Ptr() );
    return SetFileAttributes( iName.Ptr(), attrib | FILE_ATTRIBUTE_READONLY );
    }

// -----------------------------------------------------------------------------
// REcmtFile::ResetReadOnly
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::ResetReadOnly( )
    {
    DWORD attrib = GetFileAttributes( iName.Ptr() );
    return SetFileAttributes( iName.Ptr(), attrib & !(FILE_ATTRIBUTE_READONLY ) );
    }
    
// -----------------------------------------------------------------------------
// REcmtFile::Write
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::Write( )
    {
    DWORD written(0);
    BOOL retval(0);

    DWORD pos = SetFilePointer( iHandle, 0, 0, FILE_BEGIN );
    if ( pos != INVALID_SET_FILE_POINTER && SetEndOfFile( iHandle ) )
        {
        retval = WriteFile( iHandle, iData, iLen, &written, (OVERLAPPED*)0 );
        }
        
    return retval && written == iLen;
    }


// -----------------------------------------------------------------------------
// REcmtFile::FindLine
// 
// -----------------------------------------------------------------------------
//
const TPtrC8 REcmtFile::FindLine( const TDesC8& aFind )
    {
    TPtr8 x( iData, iLen, iLen );
    TLex8 lexer( x );
    
    TBool found(EFalse);
    TPtrC8 line;

    while( !found && !lexer.Eos() )
        {
        lexer.Mark();
        while( lexer.Peek() && lexer.Get() != '\n' )
            {
            }
        line.Set( lexer.MarkedToken() );
        if ( line.Length() >= aFind.Length() && line[0] != '#' )
            {
            TPtr8 keyword( iData + lexer.MarkedOffset(), aFind.Length(), aFind.Length() );
            if ( keyword.CompareF( aFind ) == 0 )
                {
                found = ETrue;
                }
            }
        }
    if ( found )
        {
        return line;
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// REcmtFile::ReplaceLine
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::ReplaceLine( const TDesC8& aOld, const TDesC8& aNew )
    {
    TUint diff = aNew.Length() - aOld.Length();
    TUint8* data = const_cast< TUint8*> ( aOld.Ptr() );
    TUint pos = data - iData;

    if ( pos > 0 && pos + diff < iMaxLen )
        {
        TPtr8 content( iData, iLen, iMaxLen );
        content.Replace( pos, aOld.Length(), aNew );
        iLen = content.Length();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// REcmtFile::DeleteLine
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::DeleteLine( const TDesC8& aOld )
    {
	TBuf8<1> empty( KEmpty );
    TUint8* data = const_cast< TUint8*> ( aOld.Ptr() );
    TUint pos = data - iData;
    TPtr8 content( iData, iLen, iMaxLen );

    content.Replace( pos, aOld.Length(), empty );
    iLen = content.Length();
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// REcmtFile::InsertLine
// 
// -----------------------------------------------------------------------------
//
TBool REcmtFile::InsertLine( const TDesC8& aLine )
    {
    if ( iLen + aLine.Length() < iMaxLen ) 
    	{
	    TPtr8 content( iData, iLen, iMaxLen );
    	content.Insert( 0, aLine );
    	iLen = content.Length();
    	
    	return ETrue;
    	}
    else 
    	{
    	return EFalse;
    	}
    }

//  End of File  
