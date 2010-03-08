/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hal.h>
#include <charconv.h>

#include "logfile.h"
#include "logfile.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CLogFile::NewL()
// Two-phased constructor.
// Standard Symbian OS construction sequence.
// ----------------------------------------------------------------------------
//
CLogFile* CLogFile::NewL( const TDesC& aFileName, TBool aInitialiseLog )
    {
    CLogFile* self = NewLC( aFileName, aInitialiseLog );
    CleanupStack::Pop();
    return( self );
    }

// ----------------------------------------------------------------------------
// CLogFile::NewLC()
// Two-phased constructor.
// Standard Symbian OS construction sequence.
// ----------------------------------------------------------------------------
//
CLogFile* CLogFile::NewLC( const TDesC& aFileName, TBool aInitialiseLog )
    {
    CLogFile* self = new ( ELeave ) CLogFile();
    CleanupStack::PushL( self );
    self->ConstructL( aFileName, aInitialiseLog );
    return( self );
    }

// ----------------------------------------------------------------------------
// CLogFile::CLogFile()
// Constructor.
// ----------------------------------------------------------------------------
//
CLogFile::CLogFile()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CLogFile::CLogFile()
// Destructor.
// ----------------------------------------------------------------------------
//
CLogFile::~CLogFile()
    {
    iLogFile.Flush();
    iLogFile.Close();
    iSession.Close();

    delete iCharacterConverter;
    iCharacterConverter = NULL;
    }

// ----------------------------------------------------------------------------
// CLogFile::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLogFile::ConstructL( const TDesC& aFileName, TBool aInitialiseLog )
    {
    TInt period;
    User::LeaveIfError( HAL::Get( HALData::ESystemTickPeriod, period ) );

    iLogMillisecsPerTick = period / 1000;

    if ( iLogMillisecsPerTick == 0 )
        {
        iLogMillisecsPerTick = 1;
        }


    iSession.Connect();

    if ( aInitialiseLog )
        {
        User::LeaveIfError( iLogFile.Replace( iSession,
                                              aFileName,
                                              EFileShareExclusive ) );
        }
    else
        {
        TInt err = iLogFile.Open( iSession, aFileName,
                                  EFileShareExclusive | EFileWrite );

        switch ( err )
            {
            case KErrNone: // Opened ok, so seek to end of file
                {
                TInt position = 0;
                User::LeaveIfError( iLogFile.Seek( ESeekEnd, position ) );
                }
                break;

            case KErrNotFound: // File doesn't exist, so create it
                User::LeaveIfError( iLogFile.Create( iSession,
                                                     aFileName,
                                                     EFileShareExclusive |
                                                     EFileWrite ) );
                break;

            default: // Unexepected error
                User::Leave( err );
                break;
            }
        }

    // Create character converter
    iCharacterConverter = CCnvCharacterSetConverter::NewL();
    CCnvCharacterSetConverter::TAvailability iConverterAvailability;
    iConverterAvailability = iCharacterConverter->PrepareToConvertToOrFromL(
                             KCharacterSetIdentifierAscii, iSession );
    }

// ----------------------------------------------------------------------------
// CLogFile::LogTime()
// Append a timestamp to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::LogTime()
    {
    StartWrite();
    LogTimeInternal();
    EndWrite();
    }

// ----------------------------------------------------------------------------
// CLogFile::Log()
// Append text to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::Log( const TDesC8& aText )
    {
    StartWrite();
    LogTextInternal( aText );
    EndWrite();
    }

// ----------------------------------------------------------------------------
// CLogFile::Log()
// Append text to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::Log( const TDesC& aText )
    {
    StartWrite();

    for ( TInt i = 0; i < aText.Length(); i++ )
        {
        if ( aText.Mid(i).Find( KCrLf ) == 0 )
            {
            LogNewline();
            i++;
            }
        else if ( iConverterAvailability ==
                  CCnvCharacterSetConverter::EAvailable )
            {
            // Convert character from unicode
            TBuf<1> unicodeBuffer;
            TBuf8<10> asciiBuffer;

            unicodeBuffer.Append(aText[i]);
            TInt status = iCharacterConverter->ConvertFromUnicode(
                                               asciiBuffer, unicodeBuffer );

            if ( status >= 0 )
                {
                LogTextInternal( asciiBuffer );
                }
            }
        else // character converter not available
            {
                TBuf8<1> asciiBuffer;
                asciiBuffer.Append( static_cast<TUint8>( aText[i] ) );
                LogTextInternal( asciiBuffer );
            }
        }

    EndWrite();
    }

// ----------------------------------------------------------------------------
// CLogFile::Log()
// Append the byte to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::Log( TUint8 aByte )
    {
    StartWrite();
    LogByteInternal( aByte );
    EndWrite();
    }

// ----------------------------------------------------------------------------
// CLogFile::Log()
// Append the integer to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::Log( TUint aNumber )
    {
    StartWrite();
    LogIntInternal( aNumber );
    EndWrite();
    }

// ----------------------------------------------------------------------------
// CLogFile::LogBytes()
// Append the bytes to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::LogBytes( const TDesC8& aBuffer )
    {
    StartWrite();

    for ( TInt i = 0; i < aBuffer.Length(); i++ )
        {
        LogByteInternal( aBuffer[i] );
        }

    EndWrite();
    }

// ----------------------------------------------------------------------------
// CLogFile::LogTimeInternal()
// Internal function to log time.
// ----------------------------------------------------------------------------
//
void CLogFile::LogTimeInternal()
    {
    TBuf8<50> text;
    TInt timeInMillisecs = User::TickCount() * iLogMillisecsPerTick;
    TInt secs = timeInMillisecs / 1000;
    TInt millisecs = timeInMillisecs % 1000;
    text.Num( secs );
    text.Append( '.' );
    Write( text );
    text.Num( millisecs );

    while ( text.Length() < KNumberOfDecimalPlaces )
        {
        text.Insert( 0, _L8( "0" ) );
        }

    text.Append( '-' );
    Write( text );
    }

// ----------------------------------------------------------------------------
// CLogFile::LogTextInternal()
// Internal function to log text.
// ----------------------------------------------------------------------------
//
void CLogFile::LogTextInternal( const TDesC8& aText )
    {
    TPtrC8 tail( aText.Ptr(), aText.Length() );

    TInt newLinePosition = tail.Find( KCrLf8 );
    while ( newLinePosition != KErrNotFound )
        {
        if ( newLinePosition > 0 )
            {
            Write( tail.Left( newLinePosition ) );
            tail.Set( aText.Ptr() + newLinePosition, tail.Length() -
                                                     newLinePosition );
            }
        LogNewline();
        tail.Set( aText.Ptr() + KCrLf8.iTypeLength, tail.Length() -
                                                    KCrLf8.iTypeLength );

        newLinePosition = tail.Find( KCrLf8 );
        }

    //  No more newlines left so print remainder
    Write( tail );
    }

// ----------------------------------------------------------------------------
// CLogFile::LogByteInternal()
// internal function to log a byte.
// ----------------------------------------------------------------------------
//
void CLogFile::LogByteInternal( TUint8 aByte )
    {
    if ( ( aByte >= KAsciiStart ) && ( aByte < KAsciiEnd ) )
        {

        // Display as ASCII char
        TBuf8<1> str;
        str.Append( aByte );
        Write( str );
        }
    else
        {
        // Display as hex number
        TBuf8<4> str;
        str.Append( KHexCharLeft );
        str.AppendNum( ( TUint )aByte, EHex );
        str.Append( KHexCharRight );
        Write( str );
        }
    }

// ----------------------------------------------------------------------------
// CLogFile::LogIntInternal()
// Internal function to log an integer.
// ----------------------------------------------------------------------------
//
void CLogFile::LogIntInternal( TUint aNumber )
    {
    // Display as ASCII char
    TBuf8<20> str;
    str.Append( KHexCharLeft );
    str.AppendNum( aNumber, EHex );
    str.Append( KHexCharRight );
    Write( str );
    }

// ----------------------------------------------------------------------------
// CLogFile::LogNewline()
// Start a newline in the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::LogNewline()
    {
    Write( KCrLf8 );

    if ( iAutoTimestamp )
        {
        LogTimeInternal();
        }
    }

// ----------------------------------------------------------------------------
// CLogFile::StartWrite()
// Perform any initial operation before the main log operation.
// ----------------------------------------------------------------------------
//
void CLogFile::StartWrite()
    {
    ASSERT( iCheckNestDepth == 0 );
    iCheckNestDepth++;

    if ( iAutoNewline )
        {
        LogNewline();
        }
    }

// ----------------------------------------------------------------------------
// CLogFile::EndWrite()
// Perform any tidying up operations after the main log operation.
// ----------------------------------------------------------------------------
//
void CLogFile::EndWrite()
    {
    if ( iAutoFlush )
        {
        iLogFile.Flush();
        }

    iCheckNestDepth--;
    ASSERT( iCheckNestDepth == 0 );
    }

// ----------------------------------------------------------------------------
// CLogFile::Write()
// Do the actual writing, and associated error checking.
// ----------------------------------------------------------------------------
//
void CLogFile::Write( const TDesC8& aText )
    {

    if ( iLogFile.Write( aText ) != KErrNone )
        {
        //  As the framework may be trapping User::Panic we need to
        //  produce the panic at a lower level.
        RThread().Panic( KLogFilePanic, TLogFileWriteFailed );
        }
    }

// ----------------------------------------------------------------------------
// CLogFile::SetAutoFlush()
// Set AutoFlush on.
// ----------------------------------------------------------------------------
//
void CLogFile::SetAutoFlush( TBool aOn )
    {
    iAutoFlush = aOn;
    }

// ----------------------------------------------------------------------------
// CLogFile::SetAutoTimeStamp()
// Set AutoTimeStamp on.
// ----------------------------------------------------------------------------
//
void CLogFile::SetAutoTimeStamp( TBool aOn )
    {
    iAutoTimestamp = aOn;
    }

// ----------------------------------------------------------------------------
// CLogFile::SetAutoNewline()
// Set AutoNewline on.
// ----------------------------------------------------------------------------
//
void CLogFile::SetAutoNewline( TBool aOn )
    {
    iAutoNewline = aOn;
    }

// ----------------------------------------------------------------------------
// CLogFile::StaticLogL()
// Static option to append text to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::StaticLogL( const TDesC& aFileName, const TDesC8& aText )
    {
    CLogFile* logFile = NewLC( aFileName, EFalse );
    logFile->Log( aText );
    CleanupStack::Pop( logFile );
    delete logFile;
    }

// ----------------------------------------------------------------------------
// CLogFile::StaticLogL()
// Static option to append text to the log file.
// ----------------------------------------------------------------------------
//
void CLogFile::StaticLogL( const TDesC& aFileName, const TDesC& aText )
    {
    CLogFile* logFile = NewLC( aFileName, EFalse );
    logFile->Log( aText );
    CleanupStack::Pop( logFile );
    delete logFile;
    }

// End of File
